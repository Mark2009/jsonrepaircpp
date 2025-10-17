#include "jsonrepair.hpp"
#include <iostream>
#include <string>
#include <sstream>

// Simple JSON value structure for demonstration
// In a real application, you'd use a proper JSON library like nlohmann/json
struct SimpleJsonParser {
    static void parse_and_display(const std::string& json_str) {
        std::cout << "Parsing: " << json_str << std::endl;
        // In a real app, you'd parse the JSON here
        // For this example, we just display it
        std::cout << "  (Would parse with a JSON library)" << std::endl;
    }
};

void demonstrate_use_case(const std::string& scenario, const std::string& malformed_json) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Scenario: " << scenario << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Malformed Input:" << std::endl;
    std::cout << malformed_json << std::endl;
    std::cout << "\n--- Repairing ---" << std::endl;
    
    try {
        std::string repaired = jsonrepair::repair(malformed_json);
        std::cout << "Repaired JSON:" << std::endl;
        std::cout << repaired << std::endl;
        
        std::cout << "\n--- Parsing ---" << std::endl;
        SimpleJsonParser::parse_and_display(repaired);
        
        std::cout << "✅ Success!" << std::endl;
        
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "===========================================\n";
    std::cout << "  JSON Repair Library - Use Cases\n";
    std::cout << "===========================================\n";
    
    // Scenario 1: Missing quotes in object keys
    demonstrate_use_case(
        "Missing quotes around keys",
        R"({
    format: "json-v1",
    precision: 0.7,
    max_length: 1000
})"
    );
    
    // Scenario 2: Python-style boolean/null values
    demonstrate_use_case(
        "Python-style boolean and null syntax",
        R"({
    "success": True,
    "data": None,
    "error": False
})"
    );
    
    // Scenario 3: Code block wrapper
    demonstrate_use_case(
        "JSON wrapped in markdown code block",
        R"(```json
{
    "response": "Here is the data you requested",
    "items": [1, 2, 3]
}
```)"
    );
    
    // Scenario 4: Trailing commas
    demonstrate_use_case(
        "Trailing commas in object and array",
        R"({
    "name": "John",
    "age": 30,
    "hobbies": ["reading", "coding",],
})"
    );
    
    // Scenario 5: Missing commas
    demonstrate_use_case(
        "Missing commas between properties",
        R"({
    "firstName": "John"
    "lastName": "Doe"
    "age": 30
})"
    );
    
    // Scenario 6: Comments in JSON
    demonstrate_use_case(
        "Comments embedded in JSON",
        R"({
    /* User profile */
    "name": "Jane",
    "role": "admin", // highest permission level
    "active": true
})"
    );
    
    // Scenario 7: Mixed quote styles
    demonstrate_use_case(
        "Mixed single and double quotes",
        R"({
    "message": 'Hello, World!',
    'status': "ok",
    code: 200
})"
    );
    
    // Scenario 8: Incomplete JSON (truncated)
    demonstrate_use_case(
        "Truncated/incomplete JSON",
        R"({
    "results": [
        {"id": 1, "name": "Item 1"},
        {"id": 2, "name": "Item 2")"
    );
    
    // Scenario 9: String concatenation
    demonstrate_use_case(
        "String concatenation syntax",
        R"({
    "message": "Hello, " + "World!",
    "greeting": "Hi " + "there"
})"
    );
    
    // Scenario 10: Function-like syntax (MongoDB style)
    demonstrate_use_case(
        "MongoDB-like function syntax",
        R"({
    "timestamp": NumberLong(1234567890),
    "count": NumberInt(42)
})"
    );
    
    std::cout << "\n===========================================\n";
    std::cout << "  All scenarios completed!\n";
    std::cout << "===========================================\n";
    
    std::cout << "\nKey Takeaways:\n";
    std::cout << "• jsonrepair handles all common JSON issues\n";
    std::cout << "• Works with markdown-wrapped content\n";
    std::cout << "• Handles Python-style syntax\n";
    std::cout << "• Repairs truncated/incomplete JSON\n";
    std::cout << "• Removes comments and fixes quotes\n";
    std::cout << "• Perfect for production integration!\n";
    
    return 0;
}
