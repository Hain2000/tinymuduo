//
// Created by Hain_official on 2023/7/21.
//
#include "Poller.h"
#include "poller/EpollPoller.h"

Poller *Poller::newDefaultPoller(EventLoop *loop) {
    if (::getenv("MUDUO_USE_POLL")) {
        return nullptr;                    // poll
    } else {
        return new EpollPoller(loop);      // epoll
    }
}