//
// Created by Hain_official on 2023/7/16.
//
#pragma once
#include "nocopyable.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "thread/EventLoopThreadPool.h"
#include "Callbacks.h"
#include <functional>
#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>


class TcpSever : nocopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    // 对端口可重用
    enum Option {
        kNoReusePort,
        kReusePort,
    };

    TcpSever(EventLoop *loop, const InetAddress &listenAddr,
             const std::string &nameArg ,Option op = kNoReusePort);

    ~TcpSever();

    void setThreadInitCallback(const ThreadInitCallback &cb) { threadInitCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

    // 设置subloop的个数
    void setThreadNum(int numThreads);

    void start();

private:

    void newConnection(int sockfd, const InetAddress &peerAddr);
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &coon);

    using ConnetionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop *loop_;
    const std::string ipPort_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadPool_; // one loop per thread, 一个线程，一个事件循环

    ConnectionCallback connectionCallback_; // 有新连接时回调
    MessageCallback messageCallback_;       // 有读写消息时回调
    WriteCompleteCallback writeCompleteCallback_; // 消息发送完成以后的回调

    ThreadInitCallback threadInitCallback_;
    std::atomic<int> started_;

    int nextConnId_;
    ConnetionMap connetions_; // 保存所有连接
};