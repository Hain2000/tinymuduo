//
// Created by Hain_official on 2023/8/22.
//
#pragma once

#include "TcpConnection.h"
#include "TcpServer.h"
#include "Buffer.h"
#include <memory>
#include <functional>

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;

using MessageCallback = std::function<void(const TcpConnectionPtr &, Buffer *, Timestamp)>;


