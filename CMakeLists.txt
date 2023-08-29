cmake_minimum_required(VERSION 3.16)
project(tinymuduo)
include_directories(include)
set(CMAKE_CXX_STANDARD 17)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

aux_source_directory(. SRC_LIST)

# 编译生成动态库
add_library(tinymuduo SHARED ${SRC_LIST} src/InetAddress.cpp src/TcpServer.cpp include/EventLoop.h src/EventLoop.cpp include/Channel.h src/Channel.cpp include/TcpServer.h src/Loggic.cpp include/Poller.h src/Poller.cpp src/DefaultPoller.cpp include/poller/EpollPoller.h src/poller/EpollPoller.cpp src/thread/CurrentThread.cpp include/thread/CurrentThread.h include/thread/Thread.h src/thread/Thread.cpp include/thread/EventLoopThreadPool.h src/thread/EventLoopThreadPool.cpp include/Socket.h src/Socket.cpp
        include/Acceptor.h
        src/Acceptor.cpp
        include/TcpConnection.h
        include/TcpConnection.h
        src/TcpConnection.cpp
        src/TcpConnection.cpp
        include/Buffer.h
        src/Buffer.cpp
        src/Buffer.cpp
        include/Callbacks.h)

