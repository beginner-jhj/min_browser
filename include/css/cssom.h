#pragma once
#include <string>
#include <vector>
#include "css/css_rule.h"
#include "html/node.h"

class CSSOM{
    private:
        std::vector<CssRule> m_rules;

        bool matches(const std::string& selector, std::shared_ptr<Node> node);

    public:
        void add_rule(CssRule rule){
            m_rules.push_back(rule);
        }

        std::vector<CssRule> get_rules() const {
            return m_rules;
        }

        std::vector<CssRule> matching_rules(std::shared_ptr<Node> node);
};