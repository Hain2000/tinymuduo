//
// Created by Hain_official on 2023/7/20.
//
#pragma once
#include "nocopyable.h"
#include <functional>
#include <memory>
#include "Timestamp.h"

class EventLoop;
class Timestamp;


class Channel : nocopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTime);

    void setReadCallback(ReadEventCallback cb) {
        readCallback_ = std::move(cb);
    }

    void setWriteCallback(EventCallback cb) {
        writeCallback_ = std::move(cb);
    }

    void setCloseCallback(EventCallback cb) {
        closeCallback_ = std::move(cb);
    }

    void setErrorCallback(EventCallback cb) {
        errorCallback_ = std::move(cb);
    }

    // 防止当channel被手动remove掉，channel还在执行回调操作
    void tie(const std::shared_ptr<void> &);

    int Getfd() const { return fd_; }
    int Getevents() const { return events_; }

    void setRetevents(int ret_event) { ret_events_ = ret_event; }

    void enableReading() {
        events_ |= kReadEvent;
        // Update();
    }

    void disableReading() {
        events_ &= ~kReadEvent;
        update();
    }

    void enableWriting() {
        events_ |= kWriteEvent;
        update();
    }

    void disableWriting() {
        events_ &= ~kWriteEvent;
        update();
    }

    void disableAll() {
        events_ = kNoneEvent;
        update();
    }

    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isReading() const { return events_ & kReadEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }

    int index() const { return index_; }
    void setIndex(int idx) { index_ = idx; }

    EventLoop *ownerLoop() { return loop_; }

    void remove();

private:

    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_; // 事件循环
    int fd_;          // Poller监听的对象
    int events_;      // fd感兴趣的事件
    int ret_events_;  // poller返回具体发生的事件
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};