//
// Created by zyq on 7/1/22.
//

#ifndef MYHTTPSERVER_NONCOPYABLE_H
#define MYHTTPSERVER_NONCOPYABLE_H


class nonCopyable {
protected:
    nonCopyable() = default;
    ~nonCopyable() = default;

public:
    nonCopyable(const nonCopyable&) = delete;
    const nonCopyable& operator=(const nonCopyable&) = delete;
};


#endif //MYHTTPSERVER_NONCOPYABLE_H
