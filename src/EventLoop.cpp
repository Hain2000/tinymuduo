//
// Created by Hain_official on 2023/7/20.
//
#include "EventLoop.h"
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include "Logger.h"
#include "Poller.h"
#include "Channel.h"
#include <memory>

// 防止一个线程创建多个EventLoop
__thread EventLoop *t_loopInThisThread = nullptr;

// IO复用接口的超时时间
const int kPollTimeMs = 10000;

// 创建wakeupfd，用来notify唤醒subReactor处理新来的channel
int createEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOG_FATAL("eventfd error: %d\n", errno);
    }
    return evtfd;
}

EventLoop::EventLoop() :
    looping_(false),
    quit_(false),
    callingPendingFunctors_(false),
    threadId_(CurrentThread::tid()),
    poller_(Poller::newDefaultPoller(this)),
    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this, wakeupFd_)),
    currentActiveChannel_(nullptr) {
    LOG_DEBUG("EventLoop created %p in thread %d \n", this, threadId_);
    if (t_loopInThisThread) {
        LOG_FATAL("Another EventLoop %p exists in this thread %d \n", t_loopInThisThread, threadId_);
    } else {
        t_loopInThisThread = this;
    }

    // 设置wakeupfd的事件类型以及事件后回调操作
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    // 每一个eventloop都监听wakeupchannel的EPOLLIN事件了
    wakeupChannel_->enableReading();
}


EventLoop::~EventLoop() {
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_FATAL("EventLoop::handleRead() reads %d bytes instead if 8\n", n);
    }
}

void EventLoop::loop() {
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p starr looping\n", this);

    while (!quit_) {
        activeChannels_.clear();
        // 监听两种fd 一种是client fd 一种是 wakeup fd
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (Channel *channel : activeChannels_) {
            // Poller监听哪些channel发生事件了，然后上报给EventLoop，通知channle处理相应的事件
            channel->handleEvent(pollReturnTime_);
        }
        // 执行当前EventLoop当前事件循环需要处理的回调操作
        /*
         *  IO线程 mainLoop accept fd 发给 channel subLoop
         *  minaLoop 事先注册一个回调cb（给subLoop执行） wakeup subLoop后，执行下面方法,
         *  执行mainLoop注册的操作，可以是多个
         */
        doPeningFunctors();
    }
    LOG_INFO("Event %p stop looping. \n", this);
    looping_ = false;
}


// 退出事件循环 1.loop在自己的线程中调用quit  2.在非loop的线程中，调用loop的quit
/*           mainLoop
 *  ======================== 生产者消费者队列
 *      sub1   sub2   sub3
 */
void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread()) { // 不是在本线程中调用quit
        wakeup();            // 在一个subLoop（worker线程）中，调用mainLoop(IO线程)的quit
    }
}

void EventLoop::runInLoop(EventLoop::Functor cb) {
    if (isInLoopThread()) { // 是当前的线程中
        cb();
    } else { // 需要唤醒loop所在线程，执行cb
        queneInLoop(cb);
    }

}

void EventLoop::queneInLoop(EventLoop::Functor cb) {
    std::unique_lock<std::mutex> lock(mutex_);
    pendingFunctors_.emplace_back(cb);
    lock.unlock();
    // 唤醒相应的需要执行上面回调操作的loop线程时
    // callingPendingFunctors_:当前loop正在执行回调，但是loop又有了新的回调
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

// 向wakeupFd_写一个数据，wakeupChannel就会发生读事件，当前的loop线程就会被唤醒
void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_FATAL("EventLoop::wakeup() write %lu bytes instead of 8", n);
    }
}

void EventLoop::updateChannel(Channel *channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
    return poller_->hasChannel(channel);
}

void EventLoop::doPeningFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor &f : functors) {
        f(); // 执行当前loop需要执行的回调操作
    }
    callingPendingFunctors_ = false;
}