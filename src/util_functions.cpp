#include "util_functions.h"

std::vector<std::string> split(std::string &s, const char delimiter)
{
    std::vector<std::string> result;

    std::stringstream ss(s);
    std::string token;

    while (std::getline(ss, token, delimiter))
    {
        result.push_back(token);
    }

    return result;
}

void skip_space(size_t &pos, const std::string &str)
{
    while (pos < str.size() && std::isspace(str[pos])) // isspace is awesome,
    /*
    - `' '`
- `\n`
- `\t`
- `\r`
isspace can find above things!
    */
    {
        ++pos;
    }
}

void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c)
                                    { return !std::isspace(c); }));
}

void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c)
                         { return !std::isspace(c); })
                .base(),
            s.end());
}

void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

std::string trim_copy(std::string &s)
{
    ltrim(s);
    rtrim(s);
    return s;
}

std::vector<std::string> split_into_words(const std::string &text)
{
    std::string current_word;
    std::vector<std::string> result;

    for (const char c : text)
    {
        if (std::isspace(c))
        {
            if (!current_word.empty())
            {
                result.push_back(current_word);
                current_word.clear();
            }
            result.push_back(std::string(1, c));
        }
        else
        {
            current_word += c;
        }
    }

    if (!current_word.empty())
    {
        result.push_back(current_word);
    }

    return result;
}