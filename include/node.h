#pragma once
#include <string>
#include <vector>
#include <memory>


enum class NODE_TYPE{
    ELEMENT,TEXT
};

class Node{
    private:
        NODE_TYPE m_type;
        std::string m_tag_name;
        std::string m_text;

        std::vector<std::shared_ptr<Node>> m_children;


    public:
        Node(NODE_TYPE t, const std::string& content);

        void add_child(std::shared_ptr<Node> child);

        std::string get_tag_name() const;
        std::string get_text_content() const;
        NODE_TYPE get_type() const;

        std::vector<std::shared_ptr<Node>> get_children() const;

};
