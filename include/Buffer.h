//
// Created by Hain_official on 2023/8/22.
//

#pragma once
#include <vector>
#include <string>

class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize) : buffer_(kCheapPrepend + initialSize),
    readerIndex_(kCheapPrepend), writerIndex_(kCheapPrepend) {}

    size_t readableBytes() const {
        return writerIndex_ - readerIndex_;
    }

    size_t writableBytes() const {
        return buffer_.size() - writerIndex_;
    }

    size_t prependableBytes() const {
        return readerIndex_;
    }

    // 返回缓冲区中可读数据的起始地址
    const char *peek() const {
        return &*buffer_.begin() + readerIndex_;
    }

    void retrieve(size_t len) {
        if (len < readableBytes()) {
            readerIndex_ += len;
        } else {
            readerIndex_ = kCheapPrepend;
            writerIndex_ = kCheapPrepend;
        }
    }

    std::string retrieveAsString(size_t len) {
        std::string res(peek(), len); // vector<char> 转 string
        retrieve(len);
        return res;
    }

    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    void ensureWriteableBytes(size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }
    }

    // 把[data, data+len]内存上的数据，添加到writable缓冲区当中
    void append(const char *data, size_t len) {
        ensureWriteableBytes(len);
        std::copy(data, data + len, buffer_.begin() + writerIndex_);
        writerIndex_ += len;
    }

    // 从fd上读数据
    ssize_t readFd(int fd, int *saveErrno);

    // 向fd上写数据
    ssize_t writeFd(int fd, int *saveErrno);
private:
    // 扩容
    void makeSpace(size_t len) {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(writerIndex_ + len);
        } else {
            size_t readable = readableBytes();
            std::copy(buffer_.begin() + readerIndex_, buffer_.begin() + writerIndex_, buffer_.begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
        }
    }

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};