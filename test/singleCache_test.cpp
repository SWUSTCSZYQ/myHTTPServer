//
// Created by zyq on 7/14/22.
//

#include "CacheControl.h"
#include <string>
#include <unordered_map>
#include <iostream>
using namespace std;

string func(string key)
{
    unordered_map<string, string>db{{"Tom", "630"}, {"jack", "589"}, {"sam", "567"}};
    if(!db.count(key))return "";
    cout << "find in db" << endl;
    return db[key];
}

int main()
{
    unordered_map<string, string>db{{"Tom", "630"}, {"jack", "589"}, {"sam", "567"}};
    CacheControl *cache = new CacheControl();
    auto fun = &func;
    cache->addGroup("scores", 1001, fun);
    auto group = cache->getGroup("scores");
    for(auto iter = db.begin(); iter != db.end(); ++iter)
    {
        //在缓存为空的情况下，能够通过回调函数获取到源数据。
        cout << group->get(iter->first) << endl;
    }
    //在缓存已经存在的情况下，是否直接从缓存中获取
    for(auto iter = db.begin(); iter != db.end(); ++iter)
    {

        cout << group->get(iter->first) << endl;
    }
}