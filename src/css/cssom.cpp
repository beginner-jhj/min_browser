#include "css/cssom.h"
#include "util_functions.h"
#include <algorithm>

/**
 * \brief Finds all CSS rules that match a given DOM node.
 *
 * Iterates through all CSS rules in the CSSOM and tests each selector against
 * the provided node. Handles multiple selectors per rule (comma-separated).
 * Returns all matching rules in priority order.
 *
 * \param node The DOM node to match against CSS selectors.
 * \return A vector of CSS_RULE objects that match the node.
 */
std::vector<CSS_RULE> CSSOM::matching_rules(std::shared_ptr<NODE> node)
{
    std::vector<CSS_RULE> matched;

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

/**
 * \brief Tests if a CSS selector matches a DOM node.
 *
 * Supports three selector types: element selectors (tag names),
 * class selectors (dot notation), and ID selectors (hash notation).
 * Performs case-sensitive matching for element and ID selectors.
 *
 * \param selector The CSS selector to test (e.g., "div", ".classname", "#idname").
 * \param node The DOM node to test against.
 * \return True if the selector matches the node, false otherwise.
 */
bool CSSOM::matches(const std::string &selector, std::shared_ptr<NODE> node)
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