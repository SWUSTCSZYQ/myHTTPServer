//
// Created by zyq on 7/6/22.
//

#include "Trie.h"

Trie *Trie::matchChild(std::string& part_) {
    for(auto child : this->children)
    {
        if(child->part == part_ or child->isWild)
        {
            return child;
        }
    }
    return nullptr;
}

std::vector<Trie *> Trie::matchChildren(std::string& part_) {
    std::vector<Trie*>nodes;
    for(auto child : this->children)
    {
        if(child->part == part_ or child->isWild)
            nodes.push_back(child);
    }
    return nodes;
}

void Trie::insert(std::string pattern_, std::vector<std::string>& parts, int height) {
    if(parts.size() == height)
    {
        this->pattern = pattern_;
    }
    auto part_ = parts[height];
    auto child = matchChild(part_);
    if(child == nullptr)
    {
        child = new Trie(pattern_, part_, part_[0] == ':' or part_[0] == '*');
        this->children.push_back(child);
    }

}

Trie *Trie::search(std::vector<std::string>& parts, int height) {
    if(parts.size() == height or hasPrefix(this->part, std::string("*")))
    {
        if(this->pattern.empty())return nullptr;
        return this;
    }
    auto part_ = parts[height];
    auto children_ = matchChildren(part_);
    for(auto child : children_)
    {
        auto result = child->search(parts, height + 1);
        if(result != nullptr)return result;
    }
    return nullptr;
}

Trie::Trie(std::string& pattern_, std::string& part_, bool isWild_)
    :pattern(pattern_),
    part(part_),
    children(std::vector<Trie*>()),
    isWild(isWild_)
    {}

Trie::~Trie() {

}

bool Trie::hasPrefix(std::string &str, std::string pattern) {
    return str.length() >= pattern.length() and str.substr(0, pattern.length()) == pattern;
}
