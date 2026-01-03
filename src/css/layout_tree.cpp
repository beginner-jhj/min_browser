#include "css/layout_tree.h"

void create_layout_tree(std::shared_ptr<Node> node){
    std::queue<std::shared_ptr<Node>> q;
    auto parent_style = node->get_all_styles();

    q.push(node);

    while(!q.empty()){
        std::shared_ptr<Node> current_node = q.front();
        auto current_style =  current_node->get_all_styles();
        q.pop();


    }
}