#pragma once
#include <string>
#include <vector>

struct DECLARATION
{
    std::string property;
    std::string value;
};

struct CSS_RULE
{
    std::string selector;
    std::vector<DECLARATION> declarations;

    CSS_RULE(std::string& s):selector(s){}
};
