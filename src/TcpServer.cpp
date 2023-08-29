//
// Created by Hain_official on 2023/7/20.
//
#include "TcpServer.h"
#include "Logger.h"

EventLoop *CHECK_NOTNULL(EventLoop *loop) {
    if (loop == nullptr) {
        LOG_FATAL("%s:%s:%d mainLoop is null!", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}


TcpSever::TcpSever(EventLoop *loop, const InetAddress &listenAddr,
    const std::string &nameArg, TcpSever::Option op) :
    loop_(CHECK_NOTNULL(loop)),
    ipPort_(listenAddr.toIpPort()),
    name_(nameArg),
    acceptor_(new Acceptor(loop, listenAddr, op == kReusePort)),
    threadPool_(new EventLoopThreadPool(loop, name_)),
    connectionCallback_(),
    messageCallback_(),
    nextConnId_(1),
    started_(0) {
    // 有新用户连接时，会执行TcpSever::newConnection回调
    acceptor_->setNewConnectionCallback(std::bind(&TcpSever::newConnetion, this, std::placeholders::_1, std::placeholders::_2));
}


TcpSever::~TcpSever() {
    for (auto &item : connetions_) {
        TcpConnectionPtr conn(item.second);
        item.second.reset();

    }
}

void TcpSever::start() {
    if (started_++ == 0) { // 防止一个TcpServer对象被start多次
        threadPool_->start(threadInitCallback_); // 启动底层的loop线程池
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get())); // listen并且把acceptChannel注册到Poller上
    }
}

void TcpSever::newConnection(int sockfd, const InetAddress &peerAddr) {

}

void TcpSever::removeConnection(const TcpConnectionPtr &conn) {

}

void TcpSever::removeConnectionInLoop(const TcpConnectionPtr &coon) {

}

void TcpSever::setThreadNum(int numThreads) {
    threadPool_->setThreadNum(numThreads);
}