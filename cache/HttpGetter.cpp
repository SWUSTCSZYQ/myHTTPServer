//
// Created by zyq on 7/17/22.
//

#include "HttpGetter.h"

std::string HttpGetter::get(std::string group, std::string key) {
    ///eg:http://localhost:8080/cache/
    std::string targetUrl = baseURL + group + "/" + key;

    ///todo:通过socket从targetURL处获取到结果
    std::string ans = "";

    return ans;
}

HttpGetter::HttpGetter(std::string& url) : baseURL(url)
{
}
