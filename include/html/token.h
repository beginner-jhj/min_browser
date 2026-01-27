#pragma once
#include <string>
#include <map>

enum class TOKEN_TYPE{
    START_TAG,END_TAG,TEXT
};

struct TOKEN{
    TOKEN_TYPE type;
    std::string value;
    std::map<std::string, std::string> attributes;
};