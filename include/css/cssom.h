#pragma once
#include <string>
#include <vector>
#include "css/css_rule.h"
#include "html/node.h"

class CSSOM{
    private:
        std::vector<CSS_RULE> m_rules;

        bool matches(const std::string& selector, std::shared_ptr<NODE> node);

    public:
        void add_rule(CSS_RULE rule){
            m_rules.push_back(rule);
        }

        std::vector<CSS_RULE> get_rules() const {
            return m_rules;
        }

        std::vector<CSS_RULE> matching_rules(std::shared_ptr<NODE> node);
};