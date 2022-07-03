//
// Created by zyq on 7/1/22.
//

#include "AsyncLogging.h"
#include "LogFile.h"


AsyncLogging::AsyncLogging(const std::string baseName, int flushInterval):
    flushInterval_(flushInterval),
    running_(false),
    baseName_(baseName),
    thread_(std::bind(&AsyncLogging::threadFunc, this), "Logging"),
    mutex_(),
    cond_(mutex_),
    currentBuffer(new Buffer),
    nextBuffer(new Buffer),
    buffers_(),
    latch_(1)
{
    assert(baseName_.length() > 1);
    currentBuffer->bZero();
    nextBuffer->bZero();
    buffers_.reserve(16);
}

void AsyncLogging::append(const char *logLine, int len) {
    MutexLockGuard lock(mutex_);
    if(currentBuffer->avail() > len)
    {
        currentBuffer->append(logLine, len);
    }
    else
    {
        buffers_.push_back(currentBuffer);
        currentBuffer.reset();
        if(nextBuffer)
        {
            currentBuffer = std::move(nextBuffer);
        }
        else
        {
            currentBuffer.reset(new Buffer);
        }
        currentBuffer->append(logLine, len);
        cond_.notify();
    }
}

void AsyncLogging::threadFunc() {
    assert(running_ == true);
    latch_.countDown();
    LogFile output(baseName_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bZero();
    newBuffer2->bZero();
    BufferVector  buffersToWrite;
    buffersToWrite.reserve(16);
    while(running_)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {
            MutexLockGuard lock(mutex_);
            if(buffers_.empty())
            {
                cond_.waitForSeconds(flushInterval_);
            }
            buffers_.push_back(currentBuffer);
            currentBuffer.reset();

            currentBuffer = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuffer)
            {
                nextBuffer = std::move(newBuffer2);
            }
        }
        assert(!buffersToWrite.empty());
        ///日志量太大了 丢弃掉一部分
        if(buffersToWrite.size() > 25)
        {
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for(size_t i = 0; i < buffersToWrite.size(); ++i)
        {
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }
        if (buffersToWrite.size() > 2)
        {
            ///丢掉非空的缓冲区，避免trash
            buffersToWrite.resize(2);
        }

        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }
        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}
