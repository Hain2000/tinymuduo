//
// Created by Hain_official on 2023/7/20.
//
#include "Channel.h"
#include <sys/epoll.h>
#include "EventLoop.h"
#include "Logger.h"


const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd) :
    loop_(loop),
    fd_(fd),
    events_(0),
    ret_events_(0),
    index_(-1),
    tied_(false) {}



Channel::~Channel() {

}

void Channel::tie(const std::shared_ptr<void> &ptr) {
    tie_ = ptr;
    tied_ = true;
}

// fd得到poller通知以后，处理事件
void Channel::handleEvent(Timestamp receiveTime) {
    if (tied_) {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) {
            handleEventWithGuard(receiveTime);
        } else {
            handleEventWithGuard(receiveTime);
        }
    }

}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
    LOG_INFO("channel handleEvent ret_events:%d", ret_events_);

    if ((ret_events_ & EPOLLHUP) && !(ret_events_ & EPOLLIN)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    if (ret_events_ & EPOLLERR) {
        if (errorCallback_) {
            errorCallback_();
        }
    }

    if (ret_events_ & (EPOLLIN | EPOLLPRI)) {
        if (readCallback_) {
            readCallback_(receiveTime);
        }
    }

    if (ret_events_ & EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }

}

void Channel::update() {
    // 通过channel所属的EventLoop，调用Poller方法，注册fd的events事件
    loop_->updateChannel(this);
}

// 删掉当前的channel
void Channel::remove() {
    loop_->removeChannel(this);
}