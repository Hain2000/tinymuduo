//
// Created by Hain_official on 2023/7/21.
//
#include "Poller.h"
#include "Channel.h"

Poller::Poller(EventLoop *loop) : ownerLoop_(loop) {}

bool Poller::hasChannel(Channel *channel) const {
    auto it = channels_.find(channel->Getfd());
    return it != channels_.end() && it->second == channel;
}
