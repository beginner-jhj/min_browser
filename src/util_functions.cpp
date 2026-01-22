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
    while (pos < str.size() && std::isspace(str[pos]))
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

void normalize_whitespace(std::string &s)
{
    trim(s);
    if (s.empty())
        return;
    std::string result;
    bool last_was_space = false;
    for (char c : s)
    {
        if (std::isspace(static_cast<unsigned char>(c)))
        {
            if (!last_was_space)
            {
                result += ' ';
                last_was_space = true;
            }
        }
        else
        {
            result += c;
            last_was_space = false;
        }
    }
    s = result;
}

QString resolve_url(const QString &base_url, const QString &relative_url)
{
    if (relative_url.isEmpty() || base_url.isEmpty())
    {
        return relative_url;
    }
    if (relative_url.startsWith("http://") ||
        relative_url.startsWith("https://") ||
        relative_url.startsWith("file://"))
    {
        return relative_url;
    }

    if (relative_url.startsWith("data:"))
    {
        return relative_url;
    }

    QUrl base(base_url);
    QUrl resolved = base.resolved(QUrl(relative_url));

    return resolved.toString();
}