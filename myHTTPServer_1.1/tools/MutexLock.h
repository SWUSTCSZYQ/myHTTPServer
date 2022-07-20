//
// Created by zyq on 7/1/22.
//

#ifndef MYHTTPSERVER_MUTEXLOCK_H
#define MYHTTPSERVER_MUTEXLOCK_H

#include <pthread.h>
#include <cstdio>
#include "nonCopyable.h"


class MutexLock: nonCopyable {
public:
    MutexLock()
    {
        pthread_mutex_init(&mutex, nullptr);
    }
    ~MutexLock()
    {
        pthread_mutex_lock(&mutex);
        pthread_mutex_destroy(&mutex);
    }

    void lock()
    {
        pthread_mutex_lock(&mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_t* get()
    {
        return &mutex;
    }

private:
    pthread_mutex_t mutex;

};

class MutexLockGuard : nonCopyable{
public:
    explicit MutexLockGuard(MutexLock &_mutex):mutex(_mutex)
    {
        mutex.lock();
    }

    ~MutexLockGuard()
    {
        mutex.unlock();
    }
private:
    MutexLock &mutex;
};


#endif //MYHTTPSERVER_MUTEXLOCK_H
