#include "epoll.h"

Epoll::Epoll() {

}

Epoll::~Epoll() {

}

void Epoll::create() {
    epollfd = epoll_create(5);
}

void Epoll::add(int fd, bool shot) {
    epoll_event event;
    event.data.fd = fd;

    if (shot == true)
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLONESHOT;
    else
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

void Epoll::setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
}