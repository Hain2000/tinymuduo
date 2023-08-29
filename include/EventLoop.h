//
// Created by Hain_official on 2023/7/20.
//
#pragma once
#include <functional>
#include <vector>
#include <atomic>
#include "Timestamp.h"
#include <memory>
#include <mutex>
#include "thread/CurrentThread.h"

class Channel;
class Poller;
// 事件循环类， Channel   Poller(epoll的抽象)

class EventLoop {
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void loop(); // 开启事件循环
    void quit(); // 退出事件循环

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    void runInLoop(Functor cb); // 在loop中执行cb
    void queneInLoop(Functor cb); // 把cb放入队列中

    void wakeup(); // 用来唤醒loop所在的线程

    // Poller的方法
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    // 判断EventLoop对象是否在自己的线程里面
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
private:

    void handleRead(); // wake up
    void doPeningFunctors(); // 执行回调

    using ChannelList = std::vector<Channel *>;

    std::atomic<bool> looping_; // 原子操作,通过CAS实现
    std::atomic<bool> quit_;    // 推出loop循环
    std::atomic<bool> callingPendingFunctors_; // 标识当前loop是否需要执行的回调操作
    pid_t threadId_;       // 记录当前loop所在线程的id
    Timestamp pollReturnTime_; // poller返回发生事件的channels的时间点
    std::unique_ptr<Poller> poller_;

    int wakeupFd_; // 当mainLoop获取一个新用户的channel，通过轮询算法选择一个subloop
    std::unique_ptr<Channel> wakeupChannel_;
    ChannelList activeChannels_;
    Channel *currentActiveChannel_;
    std::vector<Functor> pendingFunctors_; // loop需要的回调操作
    std::mutex mutex_;

};
