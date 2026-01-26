#include "html/html_parser.h"
#include "css/css_parser.h"
#include "util_functions.h"
#include <set>

/*
"initial" mode - Before anything exists

If you get TEXT → create <html>, switch to "before html"


"before html" mode - No <html> yet

If you get TEXT → create <html>, insert it, switch to "before head"


"before head" mode - <html> exists but no <head>

If you get TEXT → create <head>, insert it, switch to "in head"


"in head" mode - Inside <head>

If you get TEXT that's whitespace → ignore
If you get TEXT that's not whitespace → pop out of head, create <body>, switch to "in body"


"in body" mode - Inside <body>

If you get TEXT → insert it normally

*/

std::shared_ptr<Node> parse(const std::vector<Token> &tokens)
{
    std::set<std::string> void_elements = {"meta", "link", "img", "br", "hr", "input"};
    std::vector<std::shared_ptr<Node>> stack;
    std::shared_ptr<Node> root = nullptr;
    for (auto &token : tokens)
    {
        if (token.type == TOKEN_TYPE::TEXT)
        {
            std::string text = token.value;
            text.erase(0, text.find_first_not_of(" \t\n\r"));
            text.erase(text.find_last_not_of(" \t\n\r") + 1);

            if (text.empty())
            {
                continue;
            }
        }
        if (token.type == TOKEN_TYPE::TEXT && stack.empty())
        {
            std::shared_ptr<Node> html = std::make_shared<Node>(NODE_TYPE::ELEMENT, "html");
            std::shared_ptr<Node> body = std::make_shared<Node>(NODE_TYPE::ELEMENT, "body");

            std::shared_ptr<Node> text_node = create_node(token);

            body->add_child(text_node);
            html->add_child(body);

            root = html;

            stack.push_back(html);
            stack.push_back(body);

            continue;
        }

        if (token.type == TOKEN_TYPE::START_TAG)
        {
            bool is_void_token = void_elements.find(token.value) != void_elements.end();
            std::shared_ptr<Node> new_node = create_node(token);
            if (!stack.empty())
            {
                stack.back()->add_child(new_node);
            }
            else
            {
                root = new_node;
            }
            if(!is_void_token){
                stack.push_back(new_node);
            }
            continue;
        }

        else if (token.type == TOKEN_TYPE::TEXT)
        {
            if (!stack.empty())
            {
                std::shared_ptr<Node> parent = stack.back();
                parent->add_child(create_node(token));
            }
            continue;
        }

        else
        {
            stack.pop_back();
        }
    }

    return root;
}

std::shared_ptr<Node> create_node(const Token &token)
{
    if (token.type == TOKEN_TYPE::TEXT)
    {
        return std::make_shared<Node>(NODE_TYPE::TEXT, token.value);
    }

    auto node = std::make_shared<Node>(NODE_TYPE::ELEMENT, token.value);

    if (!token.attributes.empty())
    {
        for (auto [name, value] : token.attributes)
        {
            node->set_attribute(name, value);
        }
    }
    return node;
}