# jsonrepair-cpp API Documentation

## Overview

`jsonrepair-cpp` is a C++17 library for repairing invalid JSON documents. It's particularly useful for handling malformed JSON from external sources, APIs, configuration files, and automated output that might contain syntax errors.

## Namespace

All functionality is contained within the `jsonrepair` namespace.

## Main API

### repair()

```cpp
std::string repair(const std::string& text)
```

Repairs invalid JSON and returns a valid JSON string.

**Parameters:**
- `text` - The invalid JSON string to repair

**Returns:**
- A valid JSON string

**Throws:**
- `JSONRepairError` - If the JSON cannot be repaired

**Example:**
```cpp
#include "jsonrepair.hpp"

std::string invalid = "{name: 'John', age: 30}";
std::string repaired = jsonrepair::repair(invalid);
// Result: {"name": "John", "age": 30}
```

## Exception Handling

### JSONRepairError

```cpp
class JSONRepairError : public std::runtime_error
```

Exception thrown when JSON cannot be repaired.

**Public Methods:**

#### position()
```cpp
size_t position() const noexcept
```
Returns the character position in the input string where the error occurred.

#### message()
```cpp
const std::string& message() const noexcept
```
Returns the error message without position information.

**Example:**
```cpp
try {
    std::string repaired = jsonrepair::repair(invalid_json);
} catch (const jsonrepair::JSONRepairError& e) {
    std::cerr << "Error at position " << e.position() 
              << ": " << e.message() << std::endl;
}
```

## Supported Repairs

The library can fix the following issues:

### 1. Missing Quotes Around Keys
```cpp
// Input:  {name: "John"}
// Output: {"name": "John"}
```

### 2. Single Quotes to Double Quotes
```cpp
// Input:  {'name': 'John'}
// Output: {"name": "John"}
```

### 3. Missing Commas
```cpp
// Input:  {"a":1 "b":2}
// Output: {"a":1, "b":2}

// Input:  [1 2 3]
// Output: [1, 2, 3]
```

### 4. Trailing Commas
```cpp
// Input:  {"a":1, "b":2,}
// Output: {"a":1, "b":2}

// Input:  [1, 2, 3,]
// Output: [1, 2, 3]
```

### 5. Comments
```cpp
// Input:  {/* comment */ "a": 1}
// Output: {"a": 1}

// Input:  {"a": 1, // comment
//          "b": 2}
// Output: {"a": 1, "b": 2}
```

### 6. Python Constants
```cpp
// Input:  {"valid": True, "empty": None, "invalid": False}
// Output: {"valid": true, "empty": null, "invalid": false}
```

### 7. Missing Closing Brackets
```cpp
// Input:  {"items": [1, 2, 3
// Output: {"items": [1, 2, 3]}

// Input:  [1, 2, {"a": 1
// Output: [1, 2, {"a": 1}]
```

### 8. Unquoted Strings
```cpp
// Input:  {name: John}
// Output: {"name": "John"}
```

### 9. Special Quote Characters
```cpp
// Input:  {"name": "John"}  (using special Unicode quotes)
// Output: {"name": "John"}
```

### 10. MongoDB Data Types
```cpp
// Input:  {"id": NumberLong(123)}
// Output: {"id": 123}
```

### 11. JSONP Notation
```cpp
// Input:  callback({"data": "value"});
// Output: {"data": "value"}
```

### 12. String Concatenation
```cpp
// Input:  {"message": "Hello " + "World"}
// Output: {"message": "Hello World"}
```

### 13. Markdown Code Blocks
```cpp
// Input:  ```json
//         {"data": "value"}
//         ```
// Output: {"data": "value"}
```

### 14. Undefined to Null
```cpp
// Input:  {"value": undefined}
// Output: {"value": null}
```

### 15. Numbers with Leading Zeros
```cpp
// Input:  {"code": 00789}
// Output: {"code": "00789"}  (quoted to preserve leading zeros)
```

### 16. Truncated Numbers
```cpp
// Input:  {"value": 3.}
// Output: {"value": 3.0}
```

## Integration Example

### Minimal Example

```cpp
#include "jsonrepair.hpp"
#include <iostream>

int main() {
    try {
        std::string external_output = R"(
            {
                name: "API Response",
                confidence: 0.95,
                items: [1, 2, 3,]
            }
        )";
        
        std::string valid_json = jsonrepair::repair(external_output);
        std::cout << valid_json << std::endl;
        
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Failed to repair JSON: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

### Integration with JSON Parser

```cpp
#include "jsonrepair.hpp"
#include <nlohmann/json.hpp>  // or your preferred JSON library
#include <iostream>

nlohmann::json parse_relaxed_json(const std::string& input) {
    try {
        // First try to repair the JSON
        std::string repaired = jsonrepair::repair(input);
        
        // Then parse it with a standard JSON parser
        return nlohmann::json::parse(repaired);
        
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "JSON repair failed: " << e.what() << std::endl;
        throw;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse failed: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    std::string malformed_input = "{name: 'John', age: 30}";
    
    auto json = parse_relaxed_json(malformed_input);
    
    std::cout << "Name: " << json["name"] << std::endl;
    std::cout << "Age: " << json["age"] << std::endl;
    
    return 0;
}
```

## CMake Integration

### Option 1: Add as Subdirectory

```cmake
# In your CMakeLists.txt
add_subdirectory(external/jsonrepair-cpp)

add_executable(your_app main.cpp)
target_link_libraries(your_app jsonrepair)
```

### Option 2: Install and Find

```cmake
# After installing jsonrepair-cpp
find_library(JSONREPAIR_LIB jsonrepair)
find_path(JSONREPAIR_INCLUDE jsonrepair.hpp)

add_executable(your_app main.cpp)
target_include_directories(your_app PRIVATE ${JSONREPAIR_INCLUDE})
target_link_libraries(your_app ${JSONREPAIR_LIB})
```

## Performance Considerations

- The library operates in a single pass over the input string
- Memory usage is proportional to the size of the input and output
- For very large documents, consider streaming approaches or chunking
- The library is designed for typical JSON document sizes (KBs to low MBs)

## Thread Safety

The `repair()` function is thread-safe as it operates on local state only. Multiple threads can call `repair()` concurrently on different inputs without synchronization.

## Limitations

1. **Buffer Size**: The library keeps the entire input and output in memory
2. **Deeply Nested Structures**: Very deeply nested JSON may cause stack issues
3. **Ambiguous Repairs**: In some cases, the repair might not match the intended JSON structure
4. **Non-JSON Text**: If the input is not JSON-like at all, the repair will fail

## Error Handling Best Practices

```cpp
#include "jsonrepair.hpp"
#include <iostream>
#include <optional>

std::optional<std::string> try_repair_json(const std::string& input) {
    try {
        return jsonrepair::repair(input);
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Repair failed: " << e.message() 
                  << " at position " << e.position() << std::endl;
        
        // Optionally log the surrounding context
        size_t pos = e.position();
        size_t start = (pos > 20) ? pos - 20 : 0;
        size_t end = std::min(pos + 20, input.length());
        std::cerr << "Context: ..." << input.substr(start, end - start) 
                  << "..." << std::endl;
        
        return std::nullopt;
    }
}
```

## Version History

### 1.0.0 (Current)
- Initial C++ port of jsonrepair JavaScript library
- Support for all major repair operations
- Compatible with C++17 and later
