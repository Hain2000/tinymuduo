//
// Created by Hain_official on 2023/7/21.
//
#include <unistd.h>
#include <cstring>
#include "poller/EpollPoller.h"
#include "Logger.h"
#include "Channel.h"

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EpollPoller::EpollPoller(EventLoop *loop) :
    Poller(loop),
    epoolfd_(::epoll_create1(EPOLL_CLOEXEC)), // 注意epoll_create1(EPOLL_CLOEXEC)
    events_(kInitEventListSize) {
    if (epoolfd_ < 0) {
        LOG_FATAL("epoll_create error: %d \n", errno);
    }
}

EpollPoller::~EpollPoller() noexcept {
    close(epoolfd_);
}

Timestamp EpollPoller::poll(int timeoutMs, ChannelList *activeChannels) {
    LOG_INFO("func = %s  fd total count: %lu\n", __FUNCTION__ , channels_.size());

    int numEvents = epoll_wait(epoolfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());
    if (numEvents > 0) {
        LOG_INFO("%d events happened \n", numEvents);
        fillActiveChannels(numEvents, activeChannels);

        // 扩容
        if (numEvents == events_.size()) {
            events_.resize(events_.size() * 2);
        }

    } else if (numEvents == 0) {
        LOG_DEBUG("%s timeout! \n", __FUNCTION__ );
    } else {
        if (saveErrno != EINTR) {
            errno = saveErrno;
            LOG_FATAL("EpollPoller::poll() err!");
        }
    }
    return now;
}

/*
 *                EventLoop
 *   ChannelList              Poller
 *                        map<fd, channel *>
 *
 */
void EpollPoller::updateChannel(Channel *channel) {
    const int index = channel->index();
    LOG_INFO("func = %s fd = %d events = %d index = %d \n", __FUNCTION__, channel->Getfd(), channel->Getevents(), index);

    if (index == kNew || index == kDeleted) {
        if (index == kNew) {
            int fd = channel->Getfd();
            channels_[fd] = channel;
        }

        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);

    } else { // channel已经在poller上注册了

        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }

    }
}

void EpollPoller::removeChannel(Channel *channel) {
    int fd = channel->Getfd();
    channels_.erase(fd);

    LOG_INFO("func = %s fd = %d\n", __FUNCTION__, channel->Getfd());

    int index = channel->index();
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const {
    for (int i = 0; i < numEvents; i++) {
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->setRetevents(events_[i].events);
        activeChannels->emplace_back(channel);
    }
}

void EpollPoller::update(int operation, Channel *channel) {
    epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->Getevents();
    event.data.ptr = channel;
    int fd = channel->Getfd();
    event.data.fd = fd;
    if (epoll_ctl(epoolfd_, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            LOG_FATAL("epoll_ctl del error: %d \n", errno);
        }
    }
}

