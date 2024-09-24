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
                std::cout << read_idx << std::endl;
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

bool Http_request::write()
{
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

    std::string request_json;

    if (method == "POST") {
        // 提取请求体部分
        std::string headers_end = "\r\n\r\n";
        size_t headers_end_pos = request_post.find(headers_end);
        request_json = request_post.substr(headers_end_pos + headers_end.length(), request_post.size()-headers_end_pos + headers_end.length());
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
        if (Http::waitQueue.size() == 2) {
            Room* room = new Room();
            for (int i = 0; i < 2; i++) {
                std::string tmp = Http::waitQueue.front();
                Http::waitQueue.pop();
                Player* player = new Player(tmp);
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

        // 解析 JSON 字符串
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(request_json, root);

        if (!parsingSuccessful) {
            std::cout << "Failed to parse JSON." << std::endl;
        }

        // 提取数据
        const Json::Value& skillIDsValue = root["skillIDs"];
        int level = root["level"].asInt();
        int maxHealth = root["maxHealth"].asInt();
        int expNums = root["expNums"].asInt();
        int gold = root["gold"].asInt();

        std::list<int> skillIds;
        // 输出数据
        std::cout << "Skill IDs: ";
        for (const auto& id : skillIDsValue) {
            skillIds.push_back(id.asInt());
        }

        std::cout << "Level: " << level << std::endl;
        std::cout << "Max Health: " << maxHealth << std::endl;
        std::cout << "Experience Numbers: " << expNums << std::endl;
        std::cout << "Gold: " << gold << std::endl;

        Room::roomLists[Http::userRoomMap[userId]]->updatePlayer(userId, std::move(skillIds), maxHealth, expNums);
        // Room::roomLists[Http::userRoomMap[userId]]->playerMap[userId]->setInfo(std::move(skillIds), maxHealth);

        // std::string query = url.substr(query_pos + 1);
        // std::map<std::string, std::string> queryParams = parse_query_params(query);

        // std::string id = queryParams["userid"];

        // if(Http::userRoomMap.find(id) != userRoomMap.end()) {
        //     response_data = "true";
        // } else {
        //     response_data = "false";
        // }
    } else if (path == "batRes?") {    // 战斗结果
        std::string query = url.substr(query_pos + 1);
        std::map<std::string, std::string> queryParams = parse_query_params(query);

        std::string id = queryParams["userId"];

        if(Http::userRoomMap.find(id) != userRoomMap.end()) {
            response_data = "true";
        } else {
            response_data = "false";
        }
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
    }

}

void Http_request::process_write() {
    //  构建 HTTP 响应头
    response_post =
        "HTTP/1.1 200 OK\r\n"
        "Server: CustomServer\r\n"
        "Content-Length: " + std::to_string(response_data.size()) + "\r\n" // 计算响应体长度
        "Content-Type: text/plain\r\n" // 设置 Content-Type 为 text/plain
        "Connection: keep-alive\r\n" // 关闭连接
        "\r\n"
        + response_data; // 响应体内容
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