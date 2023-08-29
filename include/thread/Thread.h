//
// Created by Hain_official on 2023/7/25.
//
#pragma once
#include <thread>
#include <functional>
#include <memory>
#include <unistd.h>
#include <atomic>
#include <string>
#include "nocopyable.h"
#include <condition_variable>
#include "EventLoop.h"

class ThreadBase : nocopyable {
public:
    explicit ThreadBase(std::function<void()>, const std::string &name = std::string());
    ~ThreadBase();

    void start();
    void join();

    bool isStart() const { return started_; }
    pid_t Getid() const { return tid_; }
    const std::string& name() const { return name_; }

    static int numCreated() { return numCreated_; }
private:
    void setDefaultName();

    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    std::function<void()> func_;
    std::string name_;
    static std::atomic<int> numCreated_;
};

class EventLoopThread : nocopyable {
public:

    EventLoopThread(const std::function<void(EventLoop *)> &cb, const std::string &name);

    ~EventLoopThread();

    EventLoop *startLoop();
private:
    void threadFunc();

    EventLoop *loop_;
    bool exiting_;
    ThreadBase thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::function<void(EventLoop *)> callback_;
};

