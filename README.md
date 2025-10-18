# jsonrepair-cpp

A C++ port of the [jsonrepair](https://github.com/josdejong/jsonrepair) JavaScript library for repairing invalid JSON documents by Claude Sonnet 4.5, plus a few additional repairs capabilities.

## Features

The following issues can be fixed:

- Add missing quotes around keys
- Add missing escape characters
- Add missing commas
- Add missing commas between object properties (e.g., `}  {` → `}, {`)
- Add missing closing brackets
- Repair truncated JSON
- Replace single quotes with double quotes
- Replace special quote characters like `"..."` with regular double quotes
- Replace special whitespace characters with regular spaces
- Replace Python constants `None`, `True`, and `False` with `null`, `true`, and `false`
- Strip trailing commas
- Strip comments like `/* ... */` and `// ...`
- Strip JSONP notation like `callback({ ... })`
- Strip MongoDB data types like `NumberLong(2)`
- Concatenate strings like `"long text" + "more text"`
- Handle unquoted strings
- Fix wrong bracket types (e.g., `}` instead of `],` when closing array properties in objects)

## Requirements

- C++17 or later
- CMake 3.14 or later

## Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

```cpp
#include "jsonrepair.hpp"
#include <iostream>

int main() {
    try {
        // Invalid JSON: missing quotes around keys, using single quotes
        std::string invalid_json = "{name: 'John', age: 30}";
        
        // Repair the JSON
        std::string repaired = jsonrepair::repair(invalid_json);
        
        std::cout << "Repaired JSON: " << repaired << std::endl;
        // Output: {"name": "John", "age": 30}
        
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
```

### Advanced Examples

#### Fix Wrong Bracket Type
```cpp
// Invalid: } instead of ], when closing array in object
std::string json = R"({
  "items": [
    {"id": 1},
    {"id": 2}
  }
  "count": 2
})";

std::string repaired = jsonrepair::repair(json);
// Result: {"items": [{"id": 1}, {"id": 2}], "count": 2}
```

#### Fix Missing Commas Between Properties
```cpp
// Invalid: missing comma after array close
std::string json = R"({
  "first": [1, 2, 3]
  "second": [4, 5, 6]
})";

std::string repaired = jsonrepair::repair(json);
// Result: {"first": [1, 2, 3], "second": [4, 5, 6]}
```

#### Fix Missing Commas Between Objects
```cpp
// Invalid: missing comma between objects in array
std::string json = R"([
  {"name": "Alice"}
  {"name": "Bob"}
])";

std::string repaired = jsonrepair::repair(json);
// Result: [{"name": "Alice"}, {"name": "Bob"}]
```

## License

See LICENSE file for details.


