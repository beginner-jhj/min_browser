#pragma once
#include <map>
#include <string_view>

std::map<std::string, std::string> parse_inline_style(std::string_view style_string);