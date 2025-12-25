#include <iostream>
#include <cassert>
#include "parser.h"
#include "html_tokenizer.h"

int main() {
    // Test 1
    std::string html1 = "<p>Hello</p>";
    auto tokens1 = tokenize(html1);
    auto tree1 = parse(tokens1);
    
    if (!tree1) {
        std::cerr << "Test 1 FAILED: tree is null" << std::endl;
        return 1;  // 실패
    }
    
    if (tree1->get_tag_name() != "p") {
        std::cerr << "Test 1 FAILED: expected <p>, got <" 
                  << tree1->get_tag_name() << ">" << std::endl;
        return 1;
    }
    
    std::cout << "Test 1 PASSED" << std::endl;
    
    // Test 2
    std::string html2 = "<div><p>Nested</p></div>";
    auto tree2 = parse(tokenize(html2));
    
    if (tree2->get_children().size() != 1) {
        std::cerr << "Test 2 FAILED: expected 1 child" << std::endl;
        return 1;
    }
    
    std::cout << "Test 2 PASSED" << std::endl;
    
    std::cout << "All tests PASSED!" << std::endl;
    return 0;  // 성공
}