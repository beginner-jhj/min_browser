#include "html_tokenizer.h"

std::vector<Token> tokenize(const std::string &html)
{
    std::vector<Token> tokens;

    size_t pos = 0;
    while (pos < html.size())
    {
        if (html[pos] == '<')
        {
            size_t end_pos = html.find('>', pos);

            if (end_pos != std::string::npos)
            {
                if (html[pos + 1] == '/')
                {
                    tokens.push_back({TOKEN_TYPE::END_TAG, html.substr(pos + 2, end_pos - pos - 2)});
                }
                else
                {
                    tokens.push_back({TOKEN_TYPE::START_TAG, html.substr(pos + 1, end_pos - pos - 1)});
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
                tokens.push_back({TOKEN_TYPE::TEXT, html.substr(pos, end_pos - pos)});
            }
            pos = end_pos;
        }
    }
    return tokens;
}
