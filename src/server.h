#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include "http.h"
#include "threadPool.h"

class Server {
public:
    Server();
    virtual ~Server();

    void init();
    void eventListen();
    void eventLoop();
    Epoll* epoll;
    ThreadPool<Http>* pool;
    Http* https[MAX_FD];

private:
    int listenfd;   //   监听套接字
    int port = 9010;   // 端口号


};

#endif