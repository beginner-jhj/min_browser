#include "html/html_tokenizer.h"
#include "util_functions.h"

/**
 * \brief Parses HTML attribute strings into a name-value map.
 *
 * Extracts HTML attributes from a string (e.g., "class=\"button\" id=\"submit\"")
 * into individual name-value pairs. Expects attributes in the format name="value".
 * Skips whitespace and stops at the end of the string.
 *
 * \param to_parse The attribute string to parse.
 * \return A map of attribute names to their values.
 */
std::map<std::string, std::string> parse_attribute(const std::string &to_parse)
{
    std::map<std::string, std::string> attrs;
    size_t pos = 0;
    while (pos < to_parse.size())
    {
        skip_space(pos, to_parse);

        size_t equal_sign_pos = to_parse.find("=", pos);

        std::string attribute_name = to_parse.substr(pos, equal_sign_pos - pos);

        pos = equal_sign_pos + 1;

        skip_space(pos, to_parse);

        char quote = to_parse[pos];
        pos++;
        size_t closing_quote_pos = to_parse.find(quote, pos);
        std::string attribute_value = to_parse.substr(pos, closing_quote_pos - pos);
        pos = closing_quote_pos + 1;

        if (!attribute_value.empty())
        {
            attrs[attribute_name] = attribute_value;
        }
    }

    return attrs;
}

/**
 * \brief Tokenizes HTML source code into a sequence of tokens.
 *
 * Parses HTML text into semantic tokens representing start tags, end tags,
 * and text content. Handles HTML comments, attribute parsing, and whitespace
 * normalization. Throws an exception on malformed HTML (missing closing >).
 *
 * \param html The HTML source code to tokenize.
 * \return A vector of TOKEN objects representing the tokenized HTML.
 * \throws std::runtime_error If HTML syntax is invalid (missing tag closure).
 */
std::vector<TOKEN> tokenize(const std::string &html) // todo: ignoring comments.
{
    std::vector<TOKEN> tokens;

    size_t pos = 0;
    while (pos < html.size())
    {
        if (html[pos] == '<' && pos + 1 < html.size() && html[pos + 1] == '!')
        {
            size_t end_pos = html.find('>', pos);
            if (end_pos != std::string::npos)
            {
                pos = end_pos + 1;
                continue;
            }
        }

        if (html[pos] == '<')
        {
            size_t end_pos = html.find('>', pos);

            if (end_pos != std::string::npos)
            {
                if (html[pos + 1] == '/')
                {
                    tokens.push_back({TOKEN_TYPE::END_TAG, html.substr(pos + 2, end_pos - pos - 2), {}});
                }
                else
                {
                    std::string full_tag = html.substr(pos + 1, end_pos - pos - 1);
                    size_t space_pos = full_tag.find(' ');

                    std::string tag_name;
                    std::map<std::string, std::string> attrs;

                    if (space_pos != std::string::npos)
                    {
                        tag_name = full_tag.substr(0, space_pos);
                        std::string attr_string = full_tag.substr(space_pos + 1);
                        attrs = parse_attribute(attr_string);
                    }
                    else
                    {
                        tag_name = full_tag;
                    }
                    tokens.push_back({TOKEN_TYPE::START_TAG, tag_name, attrs});
                }
                pos = end_pos + 1;
            }
            else
            {
                throw std::runtime_error("INVALIDE HTML: NO CLOSING TAG");
            }
        }
        else
        {
            size_t end_pos = html.find('<', pos);
            if (!html.substr(pos, end_pos - pos).empty())
            {
                std::string text = html.substr(pos, end_pos - pos);
                normalize_whitespace(text);
                if (!text.empty())
                {
                    tokens.push_back({TOKEN_TYPE::TEXT, text});
                }
            }
            pos = end_pos;
        }
    }
    return tokens;
}
