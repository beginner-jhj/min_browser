#include "css/apply_style.h"
#include <queue>

void apply_style(std::shared_ptr<Node> node, CSSOM &cssom)
{
    std::queue<std::shared_ptr<Node>> q;
    q.push(node);

    while (!q.empty())
    {
        auto current_node = q.front();
        q.pop();

        auto matched_rules = cssom.matching_rules(current_node);
        for (const auto &rule : matched_rules)
        {
            for (const auto &decl : rule.decelarations)
            {
                current_node->set_style(decl.property, decl.value);
            }
        }

        for(auto child:current_node->get_children()){
            q.push(child);
        }
    }
}