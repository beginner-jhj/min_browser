#pragma once
#include <unordered_map>
#include <string_view>
#include "html/node.h"
#include "css/cssom.h"
#include "util_functions.h"

std::string extract_stylesheets(std::shared_ptr<Node> dom);
std::unordered_map<std::string, std::string> parse_inline_style(std::string_view style_string);

std::vector<CssRule> parse_css(const std::string& css);
CSSOM create_cssom(const std::string& css);