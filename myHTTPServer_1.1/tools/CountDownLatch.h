//
// Created by zyq on 7/1/22.
//

#ifndef MYHTTPSERVER_COUNTDOWNLATCH_H
#define MYHTTPSERVER_COUNTDOWNLATCH_H

#include "Condition.h"
#include "MutexLock.h"
#include "nonCopyable.h"


// CountDownLatch的主要作用是确保Thread中传进去的func真的启动了以后
// 外层的start才返回
class CountDownLatch : nonCopyable{
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();
private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};


#endif //MYHTTPSERVER_COUNTDOWNLATCH_H
