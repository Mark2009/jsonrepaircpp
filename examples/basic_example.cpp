#include "jsonrepair.hpp"
#include <iostream>
#include <string>

int main() {
    // Example 1: Missing quotes around keys and using single quotes
    std::cout << "Example 1: Basic repair" << std::endl;
    try {
        std::string invalid = "{name: 'John', age: 30}";
        std::string repaired = jsonrepair::repair(invalid);
        std::cout << "Input:    " << invalid << std::endl;
        std::cout << "Repaired: " << repaired << std::endl;
        std::cout << std::endl;
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Example 2: Missing commas
    std::cout << "Example 2: Missing commas" << std::endl;
    try {
        std::string invalid = "{\"a\":1 \"b\":2}";
        std::string repaired = jsonrepair::repair(invalid);
        std::cout << "Input:    " << invalid << std::endl;
        std::cout << "Repaired: " << repaired << std::endl;
        std::cout << std::endl;
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Example 3: Trailing commas
    std::cout << "Example 3: Trailing commas" << std::endl;
    try {
        std::string invalid = "{\"a\":1,\"b\":2,}";
        std::string repaired = jsonrepair::repair(invalid);
        std::cout << "Input:    " << invalid << std::endl;
        std::cout << "Repaired: " << repaired << std::endl;
        std::cout << std::endl;
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Example 4: Comments
    std::cout << "Example 4: Comments" << std::endl;
    try {
        std::string invalid = "{/* comment */\"a\":1,// line comment\n\"b\":2}";
        std::string repaired = jsonrepair::repair(invalid);
        std::cout << "Input:    " << invalid << std::endl;
        std::cout << "Repaired: " << repaired << std::endl;
        std::cout << std::endl;
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Example 5: Python keywords
    std::cout << "Example 5: Python keywords" << std::endl;
    try {
        std::string invalid = "{\"valid\":True,\"empty\":None,\"invalid\":False}";
        std::string repaired = jsonrepair::repair(invalid);
        std::cout << "Input:    " << invalid << std::endl;
        std::cout << "Repaired: " << repaired << std::endl;
        std::cout << std::endl;
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Example 6: Missing closing bracket
    std::cout << "Example 6: Missing closing bracket" << std::endl;
    try {
        std::string invalid = "{\"items\":[1,2,3";
        std::string repaired = jsonrepair::repair(invalid);
        std::cout << "Input:    " << invalid << std::endl;
        std::cout << "Repaired: " << repaired << std::endl;
        std::cout << std::endl;
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Example 7: Markdown-wrapped JSON with mixed quotes
    std::cout << "Example 7: Markdown code block with malformed JSON" << std::endl;
    try {
        std::string invalid = "```json\n{name: \"Data Service\", status: 'active', score: 0.95}\n```";
        std::string repaired = jsonrepair::repair(invalid);
        std::cout << "Input:    " << invalid << std::endl;
        std::cout << "Repaired: " << repaired << std::endl;
        std::cout << std::endl;
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Example 8: Concatenated strings
    std::cout << "Example 8: Concatenated strings" << std::endl;
    try {
        std::string invalid = "{\"message\":\"Hello \" + \"World\"}";
        std::string repaired = jsonrepair::repair(invalid);
        std::cout << "Input:    " << invalid << std::endl;
        std::cout << "Repaired: " << repaired << std::endl;
        std::cout << std::endl;
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
