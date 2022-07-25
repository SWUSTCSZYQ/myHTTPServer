//
// Created by zyq on 7/22/22.
//

#include "Trie.h"

Trie::Trie(std::string &pattern_, std::string &part_, bool isWild_)
    :pattern(pattern_),
    part(part_),
    children(),
    isWild(isWild_)
{}

Trie::~Trie() {
    for(auto child : children)
        delete child;
}

void Trie::insert(std::string pattern_, std::vector<std::string> &parts, int height) {
    if(height == parts.size())
    {
        this->pattern = pattern_;
        return;
    }
    std::string part_ = parts[height];
    auto child = this->matchChild(part_);
    if(child == nullptr)
    {
        child = new Trie(pattern_, part_, part_[0] == ':' or part_[0] == '*');
        this->children.push_back(child);
    }
    child->insert(pattern_, parts, height + 1);

}

Trie *Trie::search(std::vector<std::string> &parts, int height) {
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

Trie *Trie::matchChild(std::string &part_) {
    for(auto child : children)
    {
        if(child->isWild or part_ == child->part)
            return child;
    }
    return nullptr;
}

std::vector<Trie *> Trie::matchChildren(std::string &part_) {
    std::vector<Trie*>nodes;
    for(auto child : children)
    {
        if(part_ == child->part or child->isWild)
            nodes.push_back(child);
    }
    return nodes;
}

bool Trie::hasPrefix(std::string &str, std::string pattern_) {
    return str.length() >= pattern_.length() and str.substr(0, pattern_.length()) == pattern_;
}
