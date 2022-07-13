//
// Created by zyq on 7/13/22.
//

#ifndef MYHTTPSERVER_LEASTRECENTLYUSED_H
#define MYHTTPSERVER_LEASTRECENTLYUSED_H

#include<unordered_map>



/// KV类型应该提供无参的构造函数
template<typename K, typename V>
class LeastRecentlyUsed {
public:
    struct LinkList{
        K key_;
        V value_;
        LinkList *prev, *next;
        LinkList(K key, V value);
    };
public:
    explicit LeastRecentlyUsed(int capacity);
    ~LeastRecentlyUsed();
    void put(K key, V value);
    V get(K key);
    void deleteKey(K key);

private:
    void deleteFromList(LinkList* node);
    void insertToHead(LinkList* node);

private:
    int capacity_;
    int count_;
    LinkList *head, *tail;
    std::unordered_map<K, LinkList*>ump;
};




#endif //MYHTTPSERVER_LEASTRECENTLYUSED_H
