#include "css_parser.h"
#include <vector>
#include <iostream>
#include <map>
#include <string>

struct TestCase {
    std::string input;
    int expected_count;
    std::map<std::string, std::string> expected_values; // Store expected name-value pairs
};

int main() {
    std::vector<TestCase> test_cases = {
        {"color:red;font-size:12px;", 2, {{"color", "red"}, {"font-size", "12px"}}}, //1
        {"  color : red ; font-size : 12px ;  ", 2, {{"color", "red"}, {"font-size", "12px"}}},//2
        {"color:red;", 1, {{"color", "red"}}},//3
        {"color:red;;;font-weight:bold;", 2, {{"color", "red"}, {"font-weight", "bold"}}},//4
        {"color:red; ; ;", 1, {{"color", "red"}}},//5
        {"color:red; font-size:12px", 2, {{"color", "red"}, {"font-size", "12px"}}},//6
        {"margin:10px", 1, {{"margin", "10px"}}},//7
        {"color:; font-weight:bold;", 1, {{"font-weight", "bold"}}},//8
        {":red; color:blue;", 1, {{"color", "blue"}}},//9
        {"color red; font-size:12px;", 1, {{"font-size", "12px"}}}, //10
        {"border: 1px solid red; margin: 10px 20px;", 2, {{"border", "1px solid red"}, {"margin", "10px 20px"}}},//11
        {"COLOR:RED; Font-Size:12Px;", 2, {{"color", "red"}, {"font-size", "12px"}}},//12
        {"background-image: url(http://a.com/b;c.png); color: blue;", 2, {{"background-image", "url(http://a.com/b;c.png)"}, {"color", "blue"}}},//13
        {"list-style: url('data:image/png;base64,123');", 1, {{"list-style", "url('data:image/png;base64,123')"}}} //14
    };

    std::vector<int> passed_indices;
    std::vector<int> failed_indices;

    for (size_t i = 0; i < test_cases.size(); ++i) {
        const auto& test = test_cases[i];
        std::cout << "--- TEST CASE (" << i + 1 << ") ---" << std::endl;
        std::cout << "Input: " << test.input << std::endl;

        auto result = parse_inline_style(test.input);
        bool is_correct = true;

        // 1. Check total count
        if (result.size() != test.expected_count) {
            std::cerr << "[FAIL] Count mismatch. Got: " << result.size() << ", Expected: " << test.expected_count << std::endl;
            is_correct = false;
        }

        // 2. Check each expected value
        for (const auto& [expected_name, expected_val] : test.expected_values) {
            if (result.find(expected_name) == result.end()) {
                std::cerr << "[FAIL] Missing property: " << expected_name << std::endl;
                is_correct = false;
            } else if (result[expected_name] != expected_val) {
                std::cerr << "[FAIL] Value mismatch for '" << expected_name << "'. Got: '" << result[expected_name] << "', Expected: '" << expected_val << "'" << std::endl;
                is_correct = false;
            }
        }

        if (is_correct) {
            std::cout << "[SUCCESS] All properties matched." << std::endl;
            passed_indices.push_back(i + 1);
        } else {
            failed_indices.push_back(i + 1);
        }
        std::cout << std::endl;
    }

    // --- Statistics Summary ---
    std::cout << "========================================" << std::endl;
    std::cout << "TEST SUMMARY" << std::endl;
    std::cout << "Total: " << test_cases.size() << std::endl;
    std::cout << "Passed: " << passed_indices.size() << " [ ";
    for (int idx : passed_indices) std::cout << idx << " ";
    std::cout << "]" << std::endl;

    std::cout << "Failed: " << failed_indices.size() << " [ ";
    for (int idx : failed_indices) std::cout << idx << " ";
    std::cout << "]" << std::endl;
    std::cout << "Success Rate: " << (float)passed_indices.size() / test_cases.size() * 100 << "%" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}