//
// Created by zyq on 7/1/22.
//

#ifndef MYHTTPSERVER_CONDITION_H
#define MYHTTPSERVER_CONDITION_H

#include <cerrno>
#include <pthread.h>
#include <ctime>
#include <cstdint>
#include "MutexLock.h"
#include "nonCopyable.h"

class Condition : nonCopyable {
public:
    explicit Condition(MutexLock &_mutex): mutex(_mutex)
    {
        pthread_cond_init(&cond, nullptr);
    }
    ~Condition()
    {
        pthread_cond_destroy(&cond);
    }

    void wait()
    {
        pthread_cond_wait(&cond, mutex.get());
    }

    void notify()
    {
        pthread_cond_signal(&cond);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&cond);
    }

    bool waitForSeconds(int seconds)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cond, mutex.get(), &abstime);
    }
private:
    MutexLock &mutex;
    pthread_cond_t cond;
};


#endif //MYHTTPSERVER_CONDITION_H
