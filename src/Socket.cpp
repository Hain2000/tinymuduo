//
// Created by Hain_official on 2023/7/26.
//
#include "Socket.h"
#include "Logger.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <strings.h>

Socket::~Socket() {
    close(sockfd_);
}

void Socket::bindAddress(const InetAddress &localaddr) {
    if (::bind(sockfd_, (sockaddr *)localaddr.getSockAddr(), sizeof(sockaddr_in)) != 0) {
        LOG_FATAL("bind sockfd:%d fail \n", sockfd_);
    }
}

void Socket::listen() {
    if (::listen(sockfd_, 1024) != 0) {
        LOG_FATAL("listen sockfd:%d fail \n", sockfd_);
    }
}

int Socket::accept(InetAddress *peeradr) {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    bzero(&addr, sizeof(addr));
    int connfd = ::accept4(sockfd_, (sockaddr *)&addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0) {
        peeradr->setSockaddr(addr);
    }
    return connfd;
}


void Socket::shutdownWrite() {
    // shutdown用来关闭套接字的读(SHUT_RD)、写(SHUT_WR)或读写(SHUT_RDWR)。
    if (::shutdown(sockfd_, SHUT_WR) < 0) {
        LOG_FATAL("shutdownWrite error");
    }
}


// setsockopt 设置套接字选项（socket option）的系统调用函数。
// 它可以用来设置套接字的各种属性和选项，以影响套接字的行为和性能。


void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    // IPPROTO_TCP级别
    // TCP_NODELAY 禁用Nagle算法，以减少延迟，提高实时性。
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    // SOL_SOCKET级别
    // 当设置了SO_REUSEADDR选项后，即使之前的连接仍然处于TIME_WAIT状态，
    // 也可以重新绑定同一个地址和端口，从而允许多个套接字绑定到相同的本地地址。
    // SO_REUSEADDR选项对于服务器应用程序非常有用，
    // 特别是在快速重启或者频繁启动和关闭服务器时。
    // 它可以提供更灵活的端口绑定，减少因为处于TIME_WAIT状态而无法立即重新绑定的情况。
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}

void Socket::setReusePort(bool on) {
    int optval = on ? 1 : 0;
    // SO_REUSEPORT 用于设置套接字的地址重用功能。它允许多个套接字绑定到同一个端口上，并且每个套接字都可以独立地接收连接。
    // 用于当多个进程或线程需要同时监听同一个端口，并且希望每个进程或线程都能够独立地接收连接时。
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval);
}

void Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    // 当设置了SO_KEEPALIVE选项后，操作系统会定期发送心跳包（keep-alive探测报文）来检测连接的存活性。
    // TCP的keep-alive机制可以用于检测网络连接是否仍然有效，以及及时发现连接断开的情况。
    // 它适用于那些长时间闲置的连接，以保持连接的稳定性，并及时释放不再活动的连接。
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof optval);
}
// 物理层 链路层 网络层 传输层 [会话层 表示层] 应用层