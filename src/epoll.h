#ifndef EPOLL_H
#define EPOLL_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#define MAX_EVENT_NUM 10000

class Epoll{
public:
    Epoll();
    virtual ~Epoll();

    void create();
    void add(int, bool shot = true);

    epoll_event events[MAX_EVENT_NUM];
    int epollfd;

private:
    void setnonblocking(int);
};

#endif
