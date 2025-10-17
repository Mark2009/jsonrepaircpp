# Quick Start Guide

## Building the Project

### Windows (Visual Studio)

1. Open PowerShell in the project directory
2. Run the build script:
   ```powershell
   .\build.ps1
   ```

Or manually:
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make
```

## Running Examples

After building, run the example:

### Windows
```powershell
.\build\Release\jsonrepair_example.exe
```

### Linux/macOS
```bash
./build/jsonrepair_example
```

## Running Tests

### Windows
```powershell
cd build
ctest -C Release
# Or run directly:
.\Release\jsonrepair_tests.exe
```

### Linux/macOS
```bash
cd build
ctest
# Or run directly:
./jsonrepair_tests
```

## Using in Your Project

### Include the library

```cpp
#include "jsonrepair.hpp"
```

### Basic usage

```cpp
#include "jsonrepair.hpp"
#include <iostream>

int main() {
    try {
        std::string invalid_json = "{name: 'John', age: 30}";
        std::string repaired = jsonrepair::repair(invalid_json);
        std::cout << repaired << std::endl;
        // Output: {"name": "John", "age": 30}
    } catch (const jsonrepair::JSONRepairError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
```

## Common Use Cases

### Missing quotes
```cpp
// Input:  {name: John, type: person}
// Output: {"name": "John", "type": "person"}
```

### Python-style booleans
```cpp
// Input:  {"valid": True, "empty": None}
// Output: {"valid": true, "empty": null}
```

### Comments in JSON
```cpp
// Input:  {"a": 1, /* comment */ "b": 2}
// Output: {"a": 1, "b": 2}
```

### Markdown code blocks
```cpp
// Input:  ```json\n{"data": "value"}\n```
// Output: {"data": "value"}
```

### Missing closing brackets
```cpp
// Input:  {"items": [1, 2, 3
// Output: {"items": [1, 2, 3]}
```

## API Reference

### Main Function

```cpp
std::string jsonrepair::repair(const std::string& text)
```

Repairs invalid JSON and returns a valid JSON string.

**Parameters:**
- `text`: The invalid JSON string to repair

**Returns:**
- A valid JSON string

**Throws:**
- `jsonrepair::JSONRepairError`: If the JSON cannot be repaired

### Exception Class

```cpp
class jsonrepair::JSONRepairError : public std::runtime_error
```

**Methods:**
- `size_t position() const`: Returns the position where the error occurred
- `const std::string& message() const`: Returns the error message without position info
