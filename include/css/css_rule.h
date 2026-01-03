#pragma once
#include <string>
#include <vector>

struct Decelaration
{
    std::string property;
    std::string value;
};

struct CssRule
{
    std::string selector;
    std::vector<Decelaration> decelarations;

    CssRule(std::string& s):selector(s){}
};
