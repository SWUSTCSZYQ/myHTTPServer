//
// Created by zyq on 7/13/22.
//

#ifndef MYHTTPSERVER_BUFFER_H
#define MYHTTPSERVER_BUFFER_H

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;


///用来表示缓存值, 只读
template <typename T, int SIZE>
class Buffer {
public:
    Buffer();
    Buffer(T* data);
    ~Buffer();
    int length(){return len;}
    bool empty(){return len == 0;}

private:
    int len;
    T data_[SIZE];
    char* cur_;
};


#endif //MYHTTPSERVER_BUFFER_H
