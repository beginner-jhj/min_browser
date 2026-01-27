#pragma once
#include "html/token.h"
#include <string>
#include <vector>

std::vector<TOKEN> tokenize(const std::string& html);
std::map<std::string, std::string> parse_attribute(const std::string& to_parse);