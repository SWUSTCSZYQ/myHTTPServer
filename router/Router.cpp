//
// Created by zyq on 7/7/22.
//

#include "Router.h"
#include <string>

void Router::addRoute(std::string &method, std::string& pattern, Router::handler HandlerFunc) {
    auto parts = parsePattern(pattern);
    std::string key = method + "-" + pattern;
    if(roots.find(key) == roots.end())
    {
        roots[method] = new Trie();
    }
    roots[method]->insert(pattern, parts, 0);
    handlers[key] = HandlerFunc;
}

std::pair<Trie *, Router::umpss> Router::getRoute(std::string& method, std::string& path) {
    auto searchParts = parsePattern(path);
    umpss params;
    if(roots.find(method) == roots.end())
        return std::pair<Trie*, umpss>(nullptr, params);
    auto n = roots[method]->search(searchParts, 0);
    if(n != nullptr)
    {
        std::string pattern = n->getPattern();
        auto parts = parsePattern(pattern);
        for(int i = 0; i < parts.size(); i++)
        {
            std::string temp = parts[i].substr(1, parts[i].length());
            if(parts[i][0] == ':')
            {
                params[temp] = searchParts[i];
            }
            else if(parts[i][0] == '*' and parts[i].length() > 1)
            {
                params[temp] = join(std::vector<std::string>(searchParts.begin() + i, searchParts.end()), "/");
                break;
            }
        }
        return std::pair<Trie*, umpss>(n, params);
    }
    return std::pair<Trie*, umpss>(nullptr, params);
}

void Router::handle(Context* c) {
    auto res = getRoute(c->getMethod(), c->getPath());
    if(res.first != nullptr)
    {
        c->setParams(res.second);
        std::string key = c->getMethod() + "-" + c->getPath();
        handlers[key](c);
    }
    else
    {
        c->String(404);
    }
}

std::vector<std::string> Router::parsePattern(std::string& pattern) {
    auto vs = split(pattern, '/');
    std::vector<std::string>parts;
    for(auto item : vs)
    {
        if(item != "")
        {
            parts.push_back(item);
            if(item[0] == '*')break;
        }
    }
    return parts;
}

std::string Router::join(std::vector<std::string> arr, std::string str) {
    if(arr.empty())return "";
    std::string ans = "";
    for(int i = 0; i < arr.size(); i++)
    {
        ans += arr[i] + (i != arr.size() - 1 ? str : "");
    }
    return ans;
}

std::vector<std::string> Router::split(std::string s, char ch) {
    std::vector<std::string>ans;
    std::string::size_type pos;
    while((pos = s.find(ch, 0)) != std::string::npos)
    {
        ans.push_back(s.substr(0, pos));
        s = s.substr(pos + 1);
    }
    if(s != "")ans.push_back(s);
    return ans;
}
