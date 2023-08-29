//
// Created by Hain_official on 2023/7/15.
//
#pragma once

class nocopyable {
public:
    nocopyable(const nocopyable &) = delete;
    void operator=(const nocopyable &) = delete;
protected:
    nocopyable() = default;
    ~nocopyable() = default;
};
