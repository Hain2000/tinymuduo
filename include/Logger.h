//
// Created by Hain_official on 2023/7/16.
//
#pragma once
#include "nocopyable.h"
#include <string>
#include <stdio.h>


// LOG_INFO("%s %d", arg1, arg2)
#define LOG_INFO(logmsgFormat, args...) \
    do {                            \
        Logger &logger = Logger::Instance(); \
        logger.SetLogLevel(INFO);   \
        char buf[1024] = {0};       \
        snprintf(buf, 1024, logmsgFormat, ##args); \
        logger.Log(buf);                           \
    } while(0)

#define LOG_FATAL(logmsgFormat, args...) \
    do {                            \
        Logger &logger = Logger::Instance(); \
        logger.SetLogLevel(INFO);   \
        char buf[1024] = {0};       \
        snprintf(buf, 1024, logmsgFormat, ##args); \
        logger.Log(buf);             \
        exit(-1);                    \
    } while(0)

#ifdef MUDEBUG
    #define LOG_DEBUG(logmsgFormat, ...) \
        do {                            \
            Logger &logger = Logger::Instance(); \
            logger.SetLogLevel(INFO);   \
            char buf[1024] = {0};       \
            snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS); \
            logger.Log(buf);                                \
        } while(0)
#else
    #define LOG_DEBUG(logmsgFormat, ...)
#endif


// 日志级别  INFO  ERROR  FATAL
enum LogLevel {
    INFO,     // 普通信息
    ERROR,    // 错误信息
    FATAL,    // core信息
    DEBUG,    // 调试信息
};

class Logger : nocopyable {
public:
    static Logger &Instance();

    void SetLogLevel(int level);

    void Log(std::string msg);

private:
    int logLevel_;
    Logger() {}
};
