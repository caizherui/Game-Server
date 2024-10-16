#include "server.h"

std::unordered_set<std::string> Http::users;
std::queue<std::string> Http::waitQueue;
std::mutex Http::mutexWait;
int Http::roomId = 0;
std::mutex Http::mutexRoom;
std::unordered_map<std::string, int> Http::userRoomMap;  // 用户和房间之间的关系
std::vector<std::shared_ptr<Room>> Room::roomLists;
std::unordered_map<std::string, int> Battle::webPlayer;  // 用户和房间之间的关系

Server::Server() {
    epoll = new Epoll();
    // sse = Sse::GetInstance();
    // sse->event_bind();
    for (int i = 0; i < MAX_FD; i++) {
        https[i] = new Http_request();
    }
    pool = new ThreadPool<Http>(8);
}

Server::~Server() {

}

void Server::eventListen() {
    // 创建epoll内核事件表
    epoll->create();
    for (int i = 0; i < MAX_FD; i++) {
        https[i]->epollfd = epoll->epollfd;
    }

    //  网络编程基础步骤
    listenfd = socket(PF_INET, SOCK_STREAM, 0);

    //  等待一段时间后，再关闭套接字
    struct linger tmp = {1, 1};
    setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    int flag = 1;
    //  地址重用
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    //  绑定ip地址和端口号
    bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    //  被动监听模式
    listen(listenfd, 5);
    epoll->add(listenfd, false);

    // //  网络编程基础步骤
    // ssefd = socket(PF_INET, SOCK_STREAM, 0);

    // //  等待一段时间后，再关闭套接字
    // struct linger tmp = {1, 1};
    // setsockopt(ssefd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

    // struct sockaddr_in address;
    // memset(&address, 0, sizeof(address));
    // address.sin_family = AF_INET;
    // address.sin_addr.s_addr = htonl(INADDR_ANY);
    // address.sin_port = htons(port);

    // int flag = 1;
    // //  地址重用
    // setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    // //  绑定ip地址和端口号
    // bind(listenfd, (struct sockaddr *)&address, sizeof(address));
    // //  被动监听模式
    // listen(listenfd, 5);
}

void Server::eventLoop() {
    while (1) {
        for (auto &it: Http::users) {
            std::cout << it << std::endl;
        }

        // 监听event内核事件表
        int num = epoll_wait(epoll->epollfd, epoll->events, MAX_EVENT_NUM, -1);

        for (int i = 0; i < num; i++) {
            int sockfd = epoll->events[i].data.fd;

            if (sockfd == listenfd) {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                while (1) { // ET模式，执行所有操作
                    int connfd = accept(listenfd, (struct sockaddr *)&client_address, &client_addrlength);
                    if (connfd < 0) {
                        std::cout << "事件读取完毕" << std::endl;
                        break;
                    }
                    epoll->add(connfd);
                    https[connfd]->init(connfd, client_address);
                }
            } else if (epoll->events[i].events & EPOLLIN) { // http请求 模拟proactor
                std::cout << "http请求加入" << std::endl;
                if (https[sockfd]->read()) {
                    pool->append(https[sockfd]);
                }
            } else if (epoll->events[i].events & EPOLLOUT) {    //  http响应 模拟proactor
                https[sockfd]->write();
            }
        }
    }
}