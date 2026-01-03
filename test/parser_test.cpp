#include <iostream>
#include <cassert>
#include "html/html_parser.h"
#include "html/html_tokenizer.h"

int main()
{
    // Test 1
    std::string html1 = "<p>Hello</p>";
    auto tokens1 = tokenize(html1);
    auto tree1 = parse(tokens1);

    if (!tree1)
    {
        std::cerr << "Test 1 FAILED: tree is null" << std::endl;
        return 1; // 실패
    }

    if (tree1->get_tag_name() != "p")
    {
        std::cerr << "Test 1 FAILED: expected <p>, got <"
                  << tree1->get_tag_name() << ">" << std::endl;
        return 1;
    }

    std::cout << "Test 1 PASSED" << std::endl;

    // Test 2
    std::string html2 = "<div><p>Nested</p></div>";
    auto tree2 = parse(tokenize(html2));

    if (tree2->get_children().size() != 1)
    {
        std::cerr << "Test 2 FAILED: expected 1 child" << std::endl;
        return 1;
    }

    std::cout << "Test 2 PASSED" << std::endl;

    std::string html3 = "<a href=\"https://example.com\">link</a>";
    auto tokens3 = tokenize(html3);
    auto tree3 = parse(tokens3);

    for (const auto &token : tokens3)
    {
        std::cout << static_cast<int>(token.type) << "Value: " << token.value << std::endl;
        for (const auto &[name, value] : token.attributes)
        {
            std::cout << "Name: " << name << "," << "Value: " << value << std::endl;
        }
    }

    if (tree3->get_attribute("href") != "https://example.com")
    {
        std::cerr << "Test 3 FAILED: href attribute" << std::endl;
        return 1;
    }

    std::cout << "Test 3 PASSED" << std::endl;

    std::string html = "<style>ABC</style>";
    auto tokens = tokenize(html);
    auto tree = parse(tokens);

    // DOM 확인
    std::cout << "Root: " << tree->get_tag_name() << std::endl;
    std::cout << "Children: " << tree->get_children().size() << std::endl;

    if (tree->get_children().size() > 0)
    {
        auto child = tree->get_children()[0];
        std::cout << "First child type: " << (int)child->get_type() << std::endl;
        std::cout << "First child text: [" << child->get_text_content() << "]" << std::endl;
    }

    std::cout << "All tests PASSED!" << std::endl;
    return 0; // 성공
}