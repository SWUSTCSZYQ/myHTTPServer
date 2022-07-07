//
// Created by zyq on 7/6/22.
//

#include "Trie.h"

Trie *Trie::matchChild(std::string part) {
    for(auto child : children)
    {
        if(child->part == part or child->isWild)
        {
            return child;
        }
    }
    return nullptr;
}

std::vector<Trie *> Trie::matchChildren(std::string part) {
    std::vector<Trie*>nodes;
    for(auto child : children)
    {
        if(child->part == part or child->isWild)
            nodes.push_back(child);
    }
    return nodes;
}

void Trie::insert(std::string pattern, std::vector<std::string>& parts, int height) {
    if(part.size() == height)
    {
        this->pattern = pattern;
    }
    auto part = parts[height];
    auto child = matchChild(part);
    if(child == nullptr)
    {
        child =
    }
}

Trie *Trie::search(std::vector<std::string>& parts, int height) {
    return nullptr;
}
