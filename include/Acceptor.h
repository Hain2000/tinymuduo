//
// Created by Hain_official on 2023/8/19.
//
#pragma once
#include "nocopyable.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"

#include <functional>

class Acceptor : nocopyable {
public:
    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const std::function<void(int sockfd, const InetAddress &)> &cb) {
        newConnectionCallback_ = cb;
    }

    bool listening() const { return listening_; }
    void listen();

private:
    void handleRead();

    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    std::function<void(int sockfd, const InetAddress &)> newConnectionCallback_;
    bool listening_;
};