//
// Created by Hain_official on 2023/7/16.
//

#include "Logger.h"
#include <iostream>
#include "Timestamp.h"
Logger &Logger::Instance() {
    static Logger logger;
    return logger;
}

void Logger::SetLogLevel(int level) {
    logLevel_ = level;
}

// 写日志 [级别信息] time : msg
void Logger::Log(std::string msg) {
    switch (logLevel_) {
        case INFO:
            std::cout << "[INFO]";
            break;
        case ERROR:
            std::cout << "[ERROR]";
            break;
        case FATAL:
            std::cout << "[FATAL]";
            break;
        case DEBUG:
            std::cout << "DEBUG";
        default:
            break;
    }

    std::cout << Timestamp::now().toString() <<  " : " << "" << msg << std::endl;
}