#include "css/css_parser.h"
#include "util_functions.h"
#include <iostream>
#include <cctype>
#include <queue>
#include <QDebug>

std::unordered_map<std::string, std::string> parse_inline_style(std::string_view style_string)
{
    std::unordered_map<std::string, std::string> result;
    if (style_string.empty())
        return result;
    std::string name, value;
    bool is_value = false;
    int paren_depth = 0;
    bool is_in_quote = false;
    size_t pos = 0;

    while (pos < style_string.size())
    {
        char current = style_string[pos];

        if (current == ':' && !is_value)
        {
            is_value = true;
        }
        else if (current == ';' && paren_depth == 0 && !is_in_quote)
        {
            if (!name.empty() && !value.empty())
            {
                result[name] = trim_copy(value);
            }
            name.clear();
            value.clear();
            is_value = false;
        }
        else if (std::isspace(current) && !is_value)
        {
            ++pos;
            continue;
        }
        else
        {
            if (!is_value)
            {
                name += std::tolower(static_cast<unsigned char>(current));
            }
            else
            {
                if (current == '(')
                {
                    ++paren_depth;
                }

                else if (current == ')')
                {
                    --paren_depth;
                }

                if (current == '"' || current == '\'')
                {
                    is_in_quote = !is_in_quote;
                }
                value += paren_depth > 0 || is_in_quote ? current : std::tolower(static_cast<unsigned char>(current));
            }
        }

        ++pos;
    }

    if (!name.empty() && !value.empty())
    {
        result[name] = trim_copy(value);
    }

    return result;
}

std::string extract_stylesheets(std::shared_ptr<Node> dom)
{
    std::string css;
    std::queue<std::shared_ptr<Node>> q;

    q.push(dom);

    while (!q.empty())
    {
        auto node = q.front();
        q.pop();

        if (node->get_tag_name() == "style")
        {
            for (const auto &child : node->get_children())
            {
                if (child->get_type() == NODE_TYPE::TEXT)
                {
                    css += child->get_text_content();
                }
            }
            css += '\n';
        }

        for (const auto &child : node->get_children())
        {
            q.push(child);
        }
    }

    return css;
}

CSSOM create_cssom(const std::string &css)
{
    CSSOM cssom;
    for (auto &rule : parse_css(css))
    {
        cssom.add_rule(rule);
    }
    return cssom;
}

std::vector<CssRule> parse_css(const std::string &css)
{
    size_t pos = 0;
    std::string selector;
    std::unordered_map<std::string, std::string> style;
    std::vector<CssRule> result;
    while (pos < css.size())
    {
        skip_space(pos, css);

        if (pos + 1 < css.size() && css[pos] == '/' && css[pos + 1] == '*')
        {
            pos += 2;

            while (pos + 1 < css.size())
            {
                if (css[pos] == '*' && css[pos + 1] == '/')
                {
                    pos += 2;
                    break;
                }
                ++pos;
            }
            continue;
        }

        if (pos + 1 < css.size() && css[pos] == '/' && css[pos + 1] == '/')
        {
            while (pos < css.size() && css[pos] != '\n')
            {
                ++pos;
            }
            ++pos;
            continue;
        }

        size_t block_start_pos = css.find('{', pos);
        size_t block_end_pos = css.find('}', pos);
        if (block_start_pos != std::string::npos)
        {
            selector = css.substr(pos, block_start_pos - pos);
            trim(selector);
            style = parse_inline_style(css.substr(block_start_pos + 1, block_end_pos - block_start_pos - 1));
            CssRule rule(selector);
            for (const auto &[property, value] : style)
            {
                rule.declarations.push_back({property, value});
            }
            result.push_back(rule);

            if (block_end_pos != std::string::npos)
            {
                pos = block_end_pos + 1;
            }
            else
            {
                break;
            }
        }

        else if (block_start_pos == std::string::npos && block_end_pos != std::string::npos)
        {
            pos = block_end_pos + 1;
        }

        else
        {
            ++pos;
        }
    }
    return result;
}