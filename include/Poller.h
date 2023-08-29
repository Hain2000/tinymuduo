//
// Created by Hain_official on 2023/7/21.
//
#pragma once
#include "nocopyable.h"
#include <vector>
#include <unordered_map>
#include "Channel.h"
#include "Timestamp.h"

class Channel;
class EventLoop;

// 多路事件分发器的核心IO复用模块
class Poller : nocopyable {
public:
    using ChannelList = std::vector<Channel *>;

    Poller(EventLoop *loop);
    virtual ~Poller();

    // 给所有IO复用保留统一的接口
    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;

    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    // 判断参数channel是否在当前Poller当中
    bool hasChannel(Channel *channel) const;

    // EventLoop可以通过该接口获取默认的IO复用实现
    static Poller *newDefaultPoller(EventLoop *loop);

protected:
    // [sockfd, sockfd所属的Channel]
    using ChannelMap = std::unordered_map<int, Channel *>;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_; // 定义Poller所属的事件循环EventLoop

};