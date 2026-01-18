#include "css/cssom.h"
#include "util_functions.h"
#include <algorithm>

std::vector<CssRule> CSSOM::matching_rules(std::shared_ptr<Node> node)
{
    std::vector<CssRule> matched;

    for (auto &rule : m_rules)
    {
        rule.selector.erase(std::remove_if(rule.selector.begin(), rule.selector.end(), [](unsigned char c){
            return std::isspace(c);
        }), rule.selector.end());
        auto multiple_selectors = split(rule.selector, ',');
        for(auto selector:multiple_selectors){
            if(matches(selector, node)){
                matched.push_back(rule);
            }
        }
    }

    return matched;
}

bool CSSOM::matches(const std::string &selector, std::shared_ptr<Node> node)
{
    
    if (selector[0] == '.')
    {
        std::string class_value = node->get_attribute("class");
        std::vector<std::string> classes = split(class_value, ' ');

        for (const auto &class_name : classes)
        {
            if (class_name == selector.substr(1))
            {
                return true;
            }
        }

        return false;
    }

    else if (selector[0] == '#')
    {
        return node->get_attribute("id") == selector.substr(1);
    }

    else
    {
        return node->get_tag_name() == selector;
    }
}