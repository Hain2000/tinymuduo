//
// Created by Hain_official on 2023/7/25.
//
#include "Thread/EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, const std::string &name) :
    baseLoop_(baseLoop), name_(name), started_(false), numThreads_(0), next_(0) {}


EventLoopThreadPool::~EventLoopThreadPool() {}


void EventLoopThreadPool::start(const std::function<void(EventLoop *)> &cb) {
    started_ = true;
    for (int i = 0; i < numThreads_; i++) {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof(buf), "%s%d", name_.c_str() + i);
        EventLoopThread *t = new EventLoopThread(cb, buf);
        threads_.emplace_back(std::unique_ptr<EventLoopThread>(t));
        loops_.emplace_back(t->startLoop());
    }

    // 整个服务端只有一个线程，运行着baseloop
    if (numThreads_ == 0 && cb) {
        cb(baseLoop_);
    }
}

EventLoop *EventLoopThreadPool::getNextLoop() {
    EventLoop *res = baseLoop_;
    if (loops_.size()) {
        res = loops_[next_];
        next_++;
        if (next_ >= loops_.size()) {
            next_ = 0;
        }
    }
    return res;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops() {
    if (loops_.size()) return loops_;
    else return std::vector<EventLoop *>(1, baseLoop_);
}
