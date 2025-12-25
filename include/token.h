#pragma once
#include <string>

enum class TOKEN_TYPE{
    START_TAG,END_TAG,TEXT
};

struct Token{
    TOKEN_TYPE type;
    std::string value;
};