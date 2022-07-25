//
// Created by zyq on 7/22/22.
//

#ifndef MYHTTPSERVER_1_1_TRIE_H
#define MYHTTPSERVER_1_1_TRIE_H

#include <string>
#include <vector>
#include <unordered_map>

class Trie {
public:
    Trie() = default;
    Trie(std::string& pattern_, std::string& part_, bool isWild_);
    ~Trie();

    void insert(std::string pattern_, std::vector<std::string>&parts, int height);
    Trie* search(std::vector<std::string>&parts, int height);
    std::string getPattern(){return pattern;}

private:
    // 第一个匹配成功的节点，用于插入
    Trie* matchChild(std::string& part_);
    // 所有匹配成功的节点，用于查找
    std::vector<Trie*>matchChildren(std::string& part_);
    bool hasPrefix(std::string& str, std::string pattern_);

private:
    std::string pattern;            // 待匹配路由，例如 /p/:lang
    std::string part;               // 路由中的一部分，例如 :lang
    std::vector<Trie*>children;     // 子节点，例如 [doc, tutorial, intro]
    bool isWild{};                  // 是否精确匹配，part 含有 : 或 * 时为true
};


#endif //MYHTTPSERVER_1_1_TRIE_H
