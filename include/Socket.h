//
// Created by Hain_official on 2023/7/26.
//
#pragma once

#include "nocopyable.h"
#include "InetAddress.h"

class Socket : nocopyable {
public:
    explicit Socket(int sockfd) : sockfd_(sockfd) {}
    ~Socket();

    int fd() const { return sockfd_; }
    void bindAddress(const InetAddress &localaddr);
    void listen();
    int accept(InetAddress *peeradr);

    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int sockfd_;
};
