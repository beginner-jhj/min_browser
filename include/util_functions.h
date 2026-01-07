#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

std::vector<std::string> split(std::string& s,const char delimiter);
void skip_space(size_t&pos, const std::string& str);
void skip_space(size_t&pos, std::string_view str);
void ltrim(std::string& s);
void rtrim(std::string& s);
void trim(std::string& s);
std::string trim_copy(std::string& s);
std::vector<std::string> split_into_words(const std::string& text);