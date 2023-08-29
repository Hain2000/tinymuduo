//
// Created by Hain_official on 2023/7/21.
//
#pragma once
#include "Poller.h"
#include <vector>
#include <sys/epoll.h>
#include "Timestamp.h"

/*
 * epoll_create
 * epoll_ctl    add/mod/del
 * epoll_wait
 */

class EpollPoller : public Poller {
public:
    EpollPoller(EventLoop *loop);
    ~EpollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    using EventList = std::vector<epoll_event>;

    // 填充活跃的连接
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    // 更新channel通道
    void update(int operation, Channel *channel);

    static const int kInitEventListSize = 16;

    int epoolfd_;
    EventList events_;
};