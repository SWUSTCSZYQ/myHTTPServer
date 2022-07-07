//
// Created by zyq on 7/6/22.
//

#ifndef MYHTTPSERVER_TRIE_H
#define MYHTTPSERVER_TRIE_H

#include <string>
#include <vector>
#include <unordered_map>

struct node{
    std::string pattern;        // 待匹配路由，例如 /p/:lang
    std::string part;           // 路由中的一部分，例如 :lang
    std::vector<node*>children;  // 子节点，例如 [doc, tutorial, intro]
    bool isWild;                // 是否精确匹配，part 含有 : 或 * 时为true

};

class Trie {
public:
    void insert(std::string pattern, std::vector<std::string>& parts, int height);
    Trie* search(std::vector<std::string>&parts, int height);

private:
    // 第一个匹配成功的节点，用于插入
    Trie* matchChild(std::string part);
    // 所有匹配成功的节点，用于查找
    std::vector<Trie*>matchChildren(std::string part);
private:
    std::unordered_map<std::string, node*>ump; // GET POST
};


#endif //MYHTTPSERVER_TRIE_H
