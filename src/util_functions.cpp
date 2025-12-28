#include "util_functions.h"

std::vector<std::string> &split(std::string &s, const char delimiter)
{
    std::vector<std::string> result;

    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter))
    {
        result.push_back(token);
    }

    return result;
}

void skip_space(size_t &pos, const std::string &str)
{
    while (pos < str.size() && str[pos] == ' ')
    {
        ++pos;
    }
}

void skip_space(size_t &pos, std::string_view str)
{
    while (pos < str.size() && str[pos] == ' ')
    {
        ++pos;
    }
}

void ltrim(std::string& s){
    s.erase(s.begin(),std::find_if(s.begin(), s.end(), [](unsigned char c){
        return !std::isspace(c);
    }));
}

void rtrim(std::string& s){
    s.erase(std::find_if(s.rbegin(), s.rend(),[](unsigned char c){
        return !std::isspace(c);
    }).base(), s.end());
}

void trim(std::string& s){
    ltrim(s);
    rtrim(s);
}

std::string trim_copy(std::string& s){
    ltrim(s);
    rtrim(s);
    return s;
}