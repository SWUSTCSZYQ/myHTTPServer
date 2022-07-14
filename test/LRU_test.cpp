//
// Created by zyq on 7/13/22.
//

#include <string>
#include <cassert>
#include "LeastRecentlyUsed.h"
using namespace std;

int main()
{
    LeastRecentlyUsed<int, int>lru(5);
    assert(lru.get(1) == 0);
    lru.put(2, 1);
    lru.put(3, 1);
    lru.put(4, 1);
    lru.put(5, 1);
    lru.put(6, 1);
    lru.put(7, 1);
    assert(lru.get(1) == 0);
    lru.get(2);
    lru.put(3, 1);
    assert(lru.get(4) != 0);
}