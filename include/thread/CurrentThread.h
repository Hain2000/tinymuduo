//
// Created by Hain_official on 2023/7/22.
//
#pragma once
#include <unistd.h>

namespace CurrentThread {
    // _thread变量每一个线程有一份独立实体，各个线程的值互不干扰。
    extern __thread int t_cachedTid;

    void cacheTid();

    inline int tid() {
        if (__builtin_expect(t_cachedTid == 0, 0)) {
            cacheTid();
        }
        return t_cachedTid;
    }
}