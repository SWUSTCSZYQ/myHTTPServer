//
// Created by zyq on 7/13/22.
//

#include "LeastRecentlyUsed.h"

static const int CAPACITY = 1001;

template<typename K, typename V>
LeastRecentlyUsed<K, V>::LinkList::LinkList(K key, V value)
    :key_(key),
    value_(value),
    prev(nullptr),
    next(nullptr)
{}

template<typename K, typename V>
LeastRecentlyUsed<K, V>::LeastRecentlyUsed(int capacity)
        :capacity_(capacity),
         count_(0),
         head(new LinkList(K(), V())),
         tail(new LinkList(K(), V()))
{
             head->next = tail;
             tail->prev = head;
             if(capacity_ <= 0)
             {
                 capacity_ = CAPACITY;
             }
}

template<typename K, typename V>
LeastRecentlyUsed<K, V>::~LeastRecentlyUsed()
{
    delete head;
    delete tail;
}

template<typename K, typename V>
void LeastRecentlyUsed<K, V>::deleteFromList(LeastRecentlyUsed::LinkList *node) {
    node->next->prev = node->prev;
    node->prev->next = node->next;
    node->prev = std::nullopt;
    node->next = std::nullopt;
}

template<typename K, typename V>
void LeastRecentlyUsed<K, V>::insertToHead(LeastRecentlyUsed::LinkList *node) {
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

template<typename K, typename V>
void LeastRecentlyUsed<K, V>::put(K key, V value) {
    if(ump.count(key))
    {
        ump[key]->value_ = value;
        get(key);
    }
    else
    {
        LinkList *node = new LinkList(key, value);
        ump[key] = node;
        insertToHead(node);
        ++count_;
        if(count_ > capacity_)
        {
            ump.erase(tail->prev->key_);
            deleteFromList(tail->prev);
            --count_;
        }
    }
}

template<typename K, typename V>
V LeastRecentlyUsed<K, V>::get(K key) {
    if(!ump.count(key))return V();
    V value = ump[key]->value_;
    deleteFromList(ump[key]);
    insertToHead(ump[key]);
    return value;
}

template<typename K, typename V>
void LeastRecentlyUsed<K, V>::deleteKey(K key) {
    deleteFromList(ump[key]);
    ump.erase(key);
    --count_;
}
