#include "http_request.h"

const std::string ok_200_title = "OK";
const std::string error_400_title = "Bad Request";
const std::string error_400_form = "Your request has bad syntax or is inherently impossible to staisfy.\n";
const std::string error_403_title = "Forbidden";
const std::string error_403_form = "You do not have permission to get file form this server.\n";
const std::string error_404_title = "Not Found";
const std::string error_404_form = "The requested file was not found on this server.\n";
const std::string error_500_title = "Internal Error";
const std::string error_500_form = "There was an unusual problem serving the request file.\n";

void Http_request::init() {
    read_idx = 0;
    content_length = 0;

    check_state = CHECK_STATE_REQUESTLINE;
    request_post = "";
    response_post = "";
    method = "";
    url = "";
    version = "";
    path = "",
    start_line = 0;
    check_idx = 0;
    write_idx = 0;
    bytes_to_send = 0;
    bytes_have_send = 0;
    sseTag = false;
    webTag = false;
    bas64 = "";


    memset(read_buf, '\0', READ_BUF_SIZE);
    memset(write_buf, '\0', WRITE_BUF_SIZE);
}

void Http_request::init(int sockfd, struct sockaddr_in addr) {
    fd = sockfd;
    address = addr;

    init();
}

bool Http_request::read() {
    while (1) {
        int ret = recv(fd, read_buf + read_idx, READ_BUF_SIZE - read_idx, 0);
        if (ret == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "处理结束" << std::endl;
                std::cout << read_buf << std::endl;
                break;
            }
            return false;
        } else if (ret == 0) {
            std::cout << "连接已经关闭" << std::endl;
            return false;
        }
        read_idx += ret;
    }
    return true;
}

bool Http_request::write() {
    if (!webTag) {
        // 发送 HTTP 响应
        ssize_t bytes_sent = send(fd, response_post.c_str(), response_post.size(), 0);
        if (bytes_sent == -1) {
            std::cerr << "Error sending HTTP response: " << std::strerror(errno) << std::endl;
        } else if (bytes_sent < static_cast<ssize_t>(response_post.size())) {
            std::cerr << "Partial send: Sent " << bytes_sent << " out of " << response_post.size() << " bytes." << std::endl;
        } else {
            std::cout << "HTTP response sent successfully." << std::endl;
        }
        modfd(epollfd, fd, EPOLLIN);
        init();
    } else {
        // websocket
        ssize_t bytes_sent = send(fd, response_post.c_str(), response_post.size(), 0);
        if (bytes_sent == -1) {
            std::cerr << "Error sending HTTP response: " << std::strerror(errno) << std::endl;
        } else if (bytes_sent < static_cast<ssize_t>(response_post.size())) {
            std::cerr << "Partial send: Sent " << bytes_sent << " out of " << response_post.size() << " bytes." << std::endl;
        } else {
            std::cout << "HTTP response sent successfully." << std::endl;
        }

        // char a[8];
        // std::string 
        // a[0] = 0x81;
        // a[1] = 0x05;
        // a[2] = 'H';
        // a[3] = 'e';
        // a[4] = 'L';
        // a[5] = 'L';
        // a[6] = 'O';
        // a[7] = '\0';
        // send(fd, a, 8, 0);
        // modfd(epollfd, fd, EPOLLIN);
        // init();
    }
}

void Http_request::process(MYSQL* mysql) {
    process_read(mysql);
    process_write();
    modfd(epollfd, fd, EPOLLOUT);
    // bool write_ret = handle_write(read_ret);
    // if (!write_ret) {
    //     close_conn();
    // }
    // modfd(epollfd, fd, EPOLLOUT);

    // if (handle_request()) {
    //     std::cout << "请求预处理" << std::endl;
    //     std::string str(read_buf, read_idx); // 包括结束标记
    //     request_post = str;
    //     std::cout << request_post << std::endl;
    //     modfd(epollfd, fd, EPOLLOUT);
    //     std::string text_content = "this is a C++ server";
    //     response_post = "HTTP/1.1 200 OK\r\n"
    //                         "Content-Type: text/plain; charset=UTF-8\r\n"
    //                         "Content-Length: " + std::to_string(text_content.length()) + "\r\n"
    //                         "Connection: keep-alive\r\n"
    //                         "\r\n"
    //                         + text_content;
    //     modfd(epollfd, fd, EPOLLIN);
    //     init();
    // }
}

void Http_request::process_read(MYSQL *mysql) {
    std::string str(read_buf);
    request_post = std::move(str);
    size_t first_newline = request_post.find("\r\n");
    std::string request_line = request_post.substr(0, first_newline);
    std::regex request_pattern(R"(^(GET|POST)\s+([^ \r\n]+)\s+HTTP/(\d\.\d)$)");
    std::smatch request_match;
    std::cout << "请求行部分：" << request_line << std::endl;

    if (std::regex_search(request_line, request_match, request_pattern)) {
        method = request_match[1];
        url = request_match[2];
        version = request_match[3];
        std::cout << "Method: " << method << std::endl;
        std::cout << "URL: " << url << std::endl;
        std::cout << "Version: " << version << std::endl;
    } else {
        std::cout << "Invalid request line format." << std::endl;
    }

    //  解析查询参数
    size_t query_pos = url.find('?');
    path = url.substr(1, query_pos);

    if (path == "getUser?") {    //  获取用户信息
        std::string query = url.substr(query_pos + 1);
        std::map<std::string, std::string> queryParams = parse_query_params(query);

        std::string username = queryParams["username"];
        std::string password = queryParams["password"];


        std::string mysqlquery = "select * from users where username = '" + username + "' and password = '" + password + "'";
        mysql_query(mysql, mysqlquery.c_str());
        MYSQL_RES* res = mysql_store_result(mysql);
        unsigned int num_fields = mysql_num_fields(res);
        MYSQL_ROW row;

        
        row = mysql_fetch_row(res);
        dataParse::User user;
        if (row) {
            user.set_id(row[0]);
            user.set_name(row[1]);
            Http::users.insert(row[0]);
        } else {
            std::string mysqlquery = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "')";
            mysql_query(mysql, mysqlquery.c_str());
            unsigned long lastInsertId = mysql_insert_id(mysql);
            user.set_id(std::to_string(lastInsertId));
            user.set_name(username);
            Http::users.insert(std::to_string(lastInsertId));
        }
        user.SerializeToString(&response_data);
        mysql_free_result(res); // 释放结果集
    } else if (path == "waitExp?") {    //  等待历练
        std::string query = url.substr(query_pos + 1);
        std::map<std::string, std::string> queryParams = parse_query_params(query);

        std::string id = queryParams["userId"];

        std::unique_lock<std::mutex> lk(Http::mutexWait);
        std::unique_lock<std::mutex> lkR(Http::mutexRoom);
        Http::waitQueue.push(id);
        if (Http::waitQueue.size() == 5) {
            std::shared_ptr<Room> room = std::make_shared<Room>();
            for (int i = 0; i < 5; i++) {
                std::string tmp = Http::waitQueue.front();
                Http::waitQueue.pop();
                std::shared_ptr<Player> player = std::make_shared<Player>(tmp);
                room->add(player);
                Http::userRoomMap[tmp] = Http::roomId;
            }
            Room::roomLists.push_back(room);
            Http::roomId++;
            //  方案一：C++服务器进行客户端广播，发送消息
            //  方案二：C++客户端向服务器发起轮询定时请求
        }
        response_data = "add wait queue";

        lk.unlock();
        lkR.unlock();

    } else if (path == "askStart?") {    // 询问是否可以开始历练  
        std::string query = url.substr(query_pos + 1);
        std::map<std::string, std::string> queryParams = parse_query_params(query);

        std::string id = queryParams["userId"];

        if(Http::userRoomMap.find(id) != Http::userRoomMap.end()) {
            response_data = "true";
        } else {
            response_data = "false";
        }
    } else if (path == "updatePlayer?") {    // 用户购买技能之后的状态，提交给C++服务器 
        std::string query = url.substr(query_pos + 1);
        std::map<std::string, std::string> queryParams = parse_query_params(query);

        std::string userId = queryParams["userId"];
        std::string skillIds = queryParams["skillIds"];
        int maxHealth = atoi(queryParams["maxHealth"].c_str());
        int expNums = atoi(queryParams["expNums"].c_str());

        std::list<int> skills;
        int si = skillIds.size()/5;
        std::cout << "传入的技能" << skillIds << std::endl;
        for (int i = 0; i < si; i++) {
            skills.push_back(atoi(skillIds.substr(i*5, 4).c_str()));
            std::cout << atoi(skillIds.substr(i*5, 4).c_str()) << std::endl;
        }

        Room::roomLists[Http::userRoomMap[userId]]->updatePlayer(userId, std::move(skills), maxHealth, expNums);
        std::cout << "存活用户人数" << Room::roomLists[Http::userRoomMap[userId]]->surviveList.size() << std::endl;
    } else if (path == "readyBattle?") {    // 准备战斗 
        std::string query = url.substr(query_pos + 1);
        std::map<std::string, std::string> queryParams = parse_query_params(query);

        std::string id = queryParams["userId"];

        std::cout << "准备好的用户人数:" << Room::roomLists[Http::userRoomMap[id]]->readyNum << std::endl;
        std::cout << "存活用户人数:" << Room::roomLists[Http::userRoomMap[id]]->surviveList.size() << std::endl;

        if (Room::roomLists[Http::userRoomMap[id]]->surviveList.size() == 1) {  //  胜利结算
            response_data = "you are win";
        } else {
            if(Room::roomLists[Http::userRoomMap[id]]->readyFlag) {
                Json::Value root;
                Json::Value com;
                Json::FastWriter writer;

                // 创建一个 JSON 数组来存储技能ID
                Json::Value skillsArray(Json::arrayValue);

                std::cout << "存活用户人数" << Room::roomLists[Http::userRoomMap[id]]->surviveList.size() << std::endl;
                // 向 skillsArray 中添加技能ID
                for (auto id : Room::roomLists[Http::userRoomMap[id]]->getOppoSkills(id)) {
                    std::cout << id << std::endl;
                    skillsArray.append(id);
                }
                std::cout << "对手技能填充完毕" << std::endl;  
                std::cout << Room::roomLists[Http::userRoomMap[id]]->getOppoHealth(id) << std::endl;
                com["maxHealth"] = Room::roomLists[Http::userRoomMap[id]]->getOppoHealth(id);
                com["skillIDs"] = skillsArray;
                root.append(com);

                response_data = writer.write(root);
            } else {
                response_data = "No Ready Battle";
            }
        }
    } else if (path == "battle?") {    // 战斗阶段，服务器建立SSE连接
        std::string query = url.substr(query_pos + 1);
        std::map<std::string, std::string> queryParams = parse_query_params(query);

        std::string id = queryParams["userId"];

        std::cout << "战斗玩家数量" << Room::roomLists[Http::userRoomMap[id]]->battleNum << std::endl;
        std::cout << "已统计玩家数量" << Room::roomLists[Http::userRoomMap[id]]->webReuse.size() << std::endl;

        if  (Room::roomLists[Http::userRoomMap[id]]->webReuse.find(id) == Room::roomLists[Http::userRoomMap[id]]->webReuse.end()) {
            Room::roomLists[Http::userRoomMap[id]]->webReuse.insert(id);
            Room::roomLists[Http::userRoomMap[id]]->joinBattle();
        }

        // 分割请求字符串，去除第一行（请求行）
        Battle::webPlayer[id] = fd;
        size_t first_newline = request_post.find("Sec-WebSocket-Key: ");
        std::string headers_part = request_post.substr(first_newline);
        std::cout << headers_part << std::endl;
        size_t colon_pos = headers_part.find(':');
        std::string key = headers_part.substr(colon_pos + 2);
        // 找到最后一个非空白字符的位置
        size_t end = key.find_last_not_of(" \t\n\r\f\v");
        key = key.substr(0, end+1);

        key = key+"258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        std::cout << key << std::endl;
        char* keyc = const_cast<char*>(key.c_str());

        char WSkey[64];
        memset(WSkey, 0, sizeof(WSkey));
        memcpy(WSkey, keyc, strlen(keyc));
        std::cout << WSkey << std::endl;

        char SHA1_data[21];
        memset(SHA1_data, 0, sizeof(SHA1_data));

        SHA1((uchar*)&WSkey, strlen(WSkey), (uchar*)&SHA1_data);
        char Sec_A[32];
        BIO *b64, *bio;
        BUF_MEM *bptr = NULL;
        size_t size = 0;
    
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new(BIO_s_mem());
        bio = BIO_push(b64, bio);
    
        BIO_write(bio, SHA1_data, strlen(SHA1_data));
        BIO_flush(bio);
    
        BIO_get_mem_ptr(bio, &bptr);
        memcpy(Sec_A, bptr->data, bptr->length);
        Sec_A[bptr->length] = '\0';
        size = bptr->length;
    
        BIO_free_all(bio);
        std::cout << "Base64编码：" << Sec_A << std::endl;
        bas64 = Sec_A;
        // bas64 = Sec_Accept;
        // std::cout << "Base64编码：" << bas64 << std::endl;

        webTag = true;


        // unsigned char hash[SHA_DIGEST_LENGTH];
        // SHA1_CTX sha1;
        // SHA1_Init(&sha1);
        // SHA1_Update(&sha1, key.c_str(), key.length());
        // SHA1_Final(hash, &sha1);

        // std::stringstream hexStream;
        // hexStream << std::hex << std::setfill('0');
        // for (size_t i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        //     hexStream << std::setw(2) << static_cast<int>(hash[i]);
        // }
        // key = hexStream.str();
        // std::cout << key << std::endl;

    

        // Sse* sse = new Sse();
        // std::thread sse_thread(sse->sse_client, (void *)sse, fd);
        // sse_thread.detach();

    } else if (path == "updateState?") {    // 更新状态
        std::string query = url.substr(query_pos + 1);
        std::map<std::string, std::string> queryParams = parse_query_params(query);

        std::string id = queryParams["userId"];

        if(Http::userRoomMap.find(id) != userRoomMap.end()) {
            response_data = "true";
        } else {
            response_data = "false";
        }
    } else if (path == "disOppo?") {    // 分配对手
        std::string query = url.substr(query_pos + 1);
        std::map<std::string, std::string> queryParams = parse_query_params(query);

        std::string id = queryParams["userId"];

        if(Http::userRoomMap.find(id) != userRoomMap.end()) {
            response_data = "true";
        } else {
            response_data = "false";
        }
    } else if (path == "sse?") {    // sse事件推送机制
        std::string query = url.substr(query_pos + 1);
        std::map<std::string, std::string> queryParams = parse_query_params(query);

        std::string id = queryParams["userId"];

        if(Http::userRoomMap.find(id) != userRoomMap.end()) {
            response_data = "true";
        } else {
            response_data = "false";
        }
    }

}

void Http_request::process_write() {
    if (!webTag) {
        //  构建 HTTP 响应头
        response_post =
            "HTTP/1.1 200 OK\r\n"
            "Server: CustomServer\r\n"
            "Content-Length: " + std::to_string(response_data.size()) + "\r\n" // 计算响应体长度
            "Content-Type: text/plain\r\n" // 设置 Content-Type 为 text/plain
            "Connection: keep-alive\r\n" // 关闭连接
            "\r\n"
            + response_data; // 响应体内容
        
        std::cout << "响应内容" << response_data << std::endl;
        response_data = "";  //将响应字符串置空，为下次响应作准备
    } else {
        std::cout << "开始执行websocket" << std::endl;
        //  构建 HTTP 响应头
        response_post =
            "HTTP/1.1 101 Switching Protocols\r\n" \
                "Connection: Upgrade\r\n" \
                "Server:beetle websocket server\r\n" \
                "Upgrade: websocket\r\n" \
                "Access-Control-Allow-Credentials:true\r\n" \
                "Access-Control-Allow-Headers:content-type\r\n" \
                "Sec-WebSocket-Accept:" + bas64 +"\r\n" \
                "\r\n";

        // memset(Sec_Accept, '\0', 32);
        std::cout << response_post << std::endl;
    }
    // 构建 HTTP 响应字符串
    // std::stringstream response_stream;

    // response_post = "HTTP/1.1 200 OK\r\n"
    //             "Content-Type: application/x-protobuf\r\n"
    //             "Content-Length: " + response_data.size() + "\r\n"
    //             "\r\n"
    //             + response_data; // 发送数据

    // response_post = response_stream.str();
}

// bool Http_request::afpre() {
//     std::cout << response_post << std::endl;
//     int ret = send(fd, response_post.c_str(), response_post.size(), 0);
//     if (ret <= 0) {
//         std::cout << "http响应发送失败" << std::endl;
//     } else 
//         std::cout << "http响应发送成功" << std::endl;
//     modfd(epollfd, fd, EPOLLIN);

//     if (bytes_to_send == 0) {
//         modfd(epollfd, fd, EPOLLIN);
//         init();
//         return true;
//     }

//     while (1) {
//         int ret = writev(fd, iv, iv_count);

//         if (ret < 0) {
//             if (errno == EAGAIN) {
//                 modfd(epollfd, fd, EPOLLOUT);
//                 return true;
//             }
//             // unmap();
//             return false;
//         }

//         bytes_have_send += ret;
//         bytes_to_send -= ret;
//         if (bytes_have_send >= iv[0].iov_len) {
//             iv[0].iov_len = 0;
//             iv[1].iov_base = file_address + (bytes_have_send - write_idx);
//             iv[1].iov_len = bytes_to_send;
//         }
//         else
//         {
//             iv[0].iov_base = write_buf + bytes_have_send;
//             iv[0].iov_len = iv[0].iov_len - bytes_have_send;
//         }

//         if (bytes_to_send <= 0)
//         {
//             // unmap();
//             modfd(epollfd, fd, EPOLLIN);
//             init();
//             return true;
//         }
//     }
// }

// bool Http_request::handle_request() {
//     if (parse_line(request_post.substr(0, request_post.find("\r\n")), method, url, version)) {
//         // 解析查询参数
//         size_t query_pos = url.find('?');
//         std::string query = url.substr(query_pos + 1);
//         std::map<std::string, std::string> queryParams = parse_query_params(query);

//         std::cout << "Query Parameters:" << std::endl;
//         for (const auto& param : queryParams) {
//             std::cout << param.first << ": " << param.second << std::endl;
//         }
//         return true;
//     }
//     return false;
// }

// 解析查询参数
std::map<std::string, std::string> Http_request::parse_query_params(const std::string& query)
{
    std::map<std::string, std::string> queryParams;
    std::istringstream queryStream(query);
    std::string param;

    while (std::getline(queryStream, param, '&')) {
        size_t equalsPos = param.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = param.substr(0, equalsPos);
            std::string value = param.substr(equalsPos + 1);
            queryParams[key] = value;
        }
    }

    return queryParams;
}