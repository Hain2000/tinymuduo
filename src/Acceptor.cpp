//
// Created by Hain_official on 2023/8/19.
//
#include "Acceptor.h"
#include "Logger.h"
#include "InetAddress.h"

static int createNoblocking() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (sockfd < 0) {
        LOG_FATAL("%s:%s:%d listen socket create err:%d \n", __FILE__, __FUNCTION__, __LINE__, 0);
    }
    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport) :
    loop_(loop), acceptSocket_(createNoblocking()), acceptChannel_(loop, acceptSocket_.fd()), listening_(false) {
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(true);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}

void Acceptor::listen() {
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}


void Acceptor::handleRead() {
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr); // 找一个文件，填入peerAddr
    if (connfd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddr); // 轮询找到subLoop，唤醒，分发当前的新客户端的Channel
        } else {
            ::close(connfd);
        }
    } else {
        LOG_FATAL("%s:%s:%d accept err:%d \n", __FILE__, __FUNCTION__, __LINE__, errno);
        if (errno == EMFILE) {
            LOG_FATAL("%s:%s:%d sockfd reached limit! \n", __FILE__, __FUNCTION__, __LINE__);
        }
    }
}