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

void Http_request::process() {
    process_read();
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

void Http_request::process_read() {
    // LINE_STATUS line_status = LINE_OK;
    // HTTP_CODE ret = NO_REQUEST;
    // char *text = 0;

    // while ((check_state == CHECK_STATE_CONTENT && line_status == LINE_OK) || ((line_status = parse_line()) == LINE_OK)) {
    //     text = get_line();
    //     start_line = check_idx;
    //     switch (check_state) {
    //     case CHECK_STATE_REQUESTLINE: {
    //         ret = parse_request_line(text);
    //         if (ret == BAD_REQUEST)
    //             return BAD_REQUEST;
    //         break;
    //     }
    //     case CHECK_STATE_HEADER: {
    //         ret = parse_headers(text);
    //         if (ret == BAD_REQUEST)
    //             return BAD_REQUEST;
    //         else if (ret == GET_REQUEST)
    //         {
    //             return do_request();
    //         }
    //         break;
    //     }
    //     // case CHECK_STATE_CONTENT: {
    //     //     ret = parse_content(text);
    //     //     if (ret == GET_REQUEST)
    //     //         return do_request();
    //     //     line_status = LINE_OPEN;
    //     //     break;
    //     // }
    //     default:
    //         return INTERNAL_ERROR;
    //     }
    // }
    // return NO_REQUEST;
    std::string str(read_buf);
    request_post = std::move(str);
    std::string request_line = request_post.substr(0, request_post.find("\r\n"));
    std::regex request_pattern(R"(^(GET|POST)\s+([^ \r\n]+)\s+HTTP/(\d\.\d)$)");
    std::smatch request_match;

    if (std::regex_search(request_line, request_match, request_pattern)) {
        method = request_match[1];
        url = request_match[2];
        version = request_match[3];
        std::cout << "Method: " << method << std::endl;
        std::cout << "URI: " << url << std::endl;
        std::cout << "Version: " << version << std::endl;
    } else {
        std::cout << "Invalid request line format." << std::endl;
    }

    // 解析查询参数
    size_t query_pos = url.find('?');
    std::string query = url.substr(query_pos + 1);
    std::map<std::string, std::string> queryParams = parse_query_params(query);

    std::cout << "Query Parameters:" << std::endl;
    for (const auto& param : queryParams) {
        std::cout << param.first << ": " << param.second << std::endl;
    }
}

void Http_request::process_write() {
    // 构建 HTTP 响应头
    response_post =
        "HTTP/1.1 200 OK\r\n"
        "Server: CustomServer\r\n"
        "Content-Length: " + std::to_string(strlen("Hello, World!")) + "\r\n" // 计算响应体长度
        "Content-Type: text/plain\r\n" // 设置 Content-Type 为 text/plain
        "Connection: close\r\n" // 关闭连接
        "\r\n"
        "Hello, World!"; // 响应体内容
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