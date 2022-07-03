//
// Created by zyq on 7/1/22.
//

#ifndef MYHTTPSERVER_ASYNCLOGGING_H
#define MYHTTPSERVER_ASYNCLOGGING_H

#include <functional>
#include <string>
#include <vector>
#include <memory>
#include "LogStream.h"
#include "nonCopyable.h"
#include "CountDownLatch.h"
#include "Thread.h"

class AsyncLogging {
public:
    AsyncLogging(const std::string baseName, int flushInterval = 2);
    ~AsyncLogging()
    {
        if(running_)stop();
    }
    void append(const char* logLine, int len);

    void start()
    {
        running_ = true;
        thread_.start();
        latch_.wait();
    }
    void stop()
    {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private:
    void threadFunc();
    typedef FixedBuffer<kLargeBuffer>Buffer;
    typedef std::vector<std::shared_ptr<Buffer>>BufferVector;
    typedef std::shared_ptr<Buffer>BufferPtr;
    const int flushInterval_;
    bool running_;
    std::string baseName_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    BufferPtr currentBuffer;
    BufferPtr nextBuffer;
    BufferVector buffers_;
    CountDownLatch latch_;
};


#endif //MYHTTPSERVER_ASYNCLOGGING_H
