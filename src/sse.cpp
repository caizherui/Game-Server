#include "sse.h"

const int SSE_PORT = 8080;

Sse* Sse::instance = nullptr;

// // 存储已连接的客户端及其标识符
// std::map<int, std::string> clients;
// std::mutex clients_mutex;

// // 存储待发送的事件
// std::vector<std::pair<std::string, std::string>> events;
// std::mutex events_mutex;

// // 推送事件
// void push_event(const std::string& clientId, const std::string& event) {
//     std::lock_guard<std::mutex> lock(events_mutex);
//     events.push_back({clientId, event});
// }

// 发送 SSE 事件
void send_events(int client_fd) {
    // std::lock_guard<std::mutex> lock(events_mutex);
    std::string sse_event = "event: message\ndata: 12\n\n";
    send(client_fd, sse_event.c_str(), sse_event.size(), 0);
    // events.erase(std::remove_if(events.begin(), events.end(),
    //                             [&clients, client_fd](const auto& e) {
    //                                 return e.first.empty() || e.first == clients[client_fd];
    //                             }),
    // events.end());
}

// 处理客户端连接
void Sse::sse_client(void* arg, int client_fd) {
    Sse* sse = static_cast<Sse*>(arg);
    // 发送 SSE 事件
    std::string response = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/event-stream\r\n"
                            "Cache-Control: no-cache\r\n"
                            "Connection: keep-alive\r\n"
                            "Access-Control-Allow-Origin: *\r\n"
                            "\r\n";
    send(client_fd, response.c_str(), response.size(), 0);
    Room* room = new Room();
    room->test();

    // 发送已有的事件
    // send_events(client_fd);

    // 添加客户端到列表
    // std::lock_guard<std::mutex> lock(clients_mutex);
    // clients[client_fd] = "client_" + std::to_string(client_fd);

    // 监听新的事件并发送
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 模拟延时
        std::cout << "发送事件" << std::endl;
        std::string sse_event = "event: message\ndata: 12\n\n";
        send(client_fd, sse_event.c_str(), sse_event.size(), 0);
        // sse->send_events(client_fd);
    }
    close(client_fd);
}

void Sse::event_bind() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    // 创建 socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // 设置地址重用选项
    int optval = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        std::cerr << "Failed to set socket options." << std::endl;
        close(server_fd);
    }

    // 绑定地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SSE_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // 监听连接
    listen(server_fd, 5);

    std::thread sse_thread(event_listen, (void *)this, server_fd);
    sse_thread.detach();
}

void Sse::event_listen(void* arg, int server_fd) {
    Sse* sse = static_cast<Sse*>(arg);

    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    while (true) {
        // 接受客户端连接
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == -1) {
            std::cerr << "Failed to accept client." << std::endl;
            continue;
        } else {
            std::cout << "客户端发起sse请求，服务器准备事件分发" << std::endl;
        }

        // 创建线程处理客户端请求
        std::thread sse_thread(sse_client, (void *)sse, client_fd);
        sse_thread.detach();
    }
    close(server_fd);
}