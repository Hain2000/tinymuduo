//
// Created by Hain_official on 2023/7/25.
//
#include "Thread/Thread.h"
#include <semaphore.h>
#include "Thread/CurrentThread.h"

ThreadBase::ThreadBase(std::function<void()> func, const std::string &name) :
    started_(false), joined_(false), tid_(0), func_(std::move(func)), name_(name) {
    numCreated_.store(0);
    setDefaultName();
}

ThreadBase::~ThreadBase() {
    if (started_ && !joined_) {
        thread_->detach();
    }
}

void ThreadBase::start() {
    started_ = true;
    sem_t sem; // 使用信号量控制线程数量
    sem_init(&sem, 0, 0); // 局部信号量
    thread_ = std::shared_ptr<std::thread>(new std::thread([&]() {
        tid_ = CurrentThread::tid();
        sem_post(&sem); //信号量+1 时原子操作
        func_();
    }));

    sem_wait(&sem); //
}

void ThreadBase::join() {
    joined_ = true;
    thread_->join();
}

void ThreadBase::setDefaultName() {
    int num = ++numCreated_;
    if (name_.empty()) {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "thread%d", num);
        name_ = buf;
    }
}

EventLoopThread::EventLoopThread(const std::function<void(EventLoop *)> &cb, const std::string &name = std::string()) :
    loop_(nullptr),
    exiting_(false),
    thread_(std::bind(&EventLoopThread::threadFunc, this), name),
    callback_(cb) {}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_ != nullptr) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop *EventLoopThread::startLoop() {
    thread_.start();
    EventLoop *loop = nullptr;
    std::unique_lock<std::mutex> lock(mutex_);
    while (loop_ == nullptr) {
        cv_.wait(lock);
    }
    loop = loop_;
    lock.unlock();
    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop; //  创建一个独立的eventloop，和上面的线程是一一对应的，one loop per thread
    if (callback_) {
        callback_(&loop);
    }
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = &loop;
    cv_.notify_one();
    lock.unlock();
    loop.loop();
    lock.lock();
    loop_ = nullptr;
}