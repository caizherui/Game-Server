#ifndef HTTP_H
#define HTTP_H

#include <regex>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <map>
#include <mysql/mysql.h>
#include <string>
#include <fstream>
#include <condition_variable>
#include <mutex>
#include <json/json.h>  // 使用JsonCpp库来解析JSON
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <list>
#include "epoll.h"
#include "sql_pool.h"
#include "room.h"
#include "proto/dataParse.pb.h"

#define READ_BUF_SIZE 2048
#define WRITE_BUF_SIZE 1024
#define MAX_FD 65536
#define FILENAME_LEN 200

class Http {
public:
    enum HTTP_CODE {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };

    enum CHECK_STATE {
        CHECK_STATE_REQUESTLINE = 0,
        CHECK_STATE_HEADER,
        CHECK_STATE_CONTENT
    };

    enum LINE_STATUS {
        LINE_OK = 0,
        LINE_BAD,
        LINE_OPEN
    };

    Http() {};
    virtual ~Http() {};
    virtual void init() {};
    virtual void init(int, struct sockaddr_in) {};
    virtual void process(MYSQL *) {};
    virtual bool read() {};    //  处理http请求的预处理操作
    virtual bool write() {};    //  处理http响应的后处理操作

    void modfd(int epollfd, int fd, int ev) {   //  让文件描述符重新生效
        epoll_event event;
        event.data.fd = fd;
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
    }

    int fd;
    int epollfd;
    static std::unordered_set<std::string> users;  // 在线用户集合
    static std::queue<std::string> waitQueue;   //  等待用户队列
    static std::mutex mutexWait;   //  等待队列的锁
    static std::mutex mutexRoom;   //  对局房间的锁
    static int roomId; // 房间的id号
    static std::unordered_map<std::string, int> userRoomMap;  // 用户id和房间之间的关系

};


#endif