# C++ vs JavaScript Implementation Comparison

## Feature Parity

This C++ implementation aims to provide feature parity with the [jsonrepair JavaScript library](https://github.com/josdejong/jsonrepair).

### Supported Features ✅

| Feature | JavaScript | C++ | Notes |
|---------|-----------|-----|-------|
| Add missing quotes around keys | ✅ | ✅ | Full support |
| Replace single quotes with double quotes | ✅ | ✅ | Full support |
| Add missing commas | ✅ | ✅ | Full support |
| Remove trailing commas | ✅ | ✅ | Full support |
| Add missing closing brackets | ✅ | ✅ | Full support |
| Strip comments (`/* */` and `//`) | ✅ | ✅ | Full support |
| Strip markdown code blocks | ✅ | ✅ | Full support |
| Python constants (True, False, None) | ✅ | ✅ | Full support |
| MongoDB data types | ✅ | ✅ | Full support |
| JSONP notation | ✅ | ✅ | Full support |
| String concatenation (`+`) | ✅ | ✅ | Full support |
| Unquoted strings | ✅ | ✅ | Full support |
| Special quote characters | ✅ | ✅ | Full support |
| Special whitespace characters | ✅ | ✅ | Full support |
| Undefined to null | ✅ | ✅ | Full support |
| Numbers with leading zeros | ✅ | ✅ | Full support |
| Truncated JSON repair | ✅ | ✅ | Full support |
| Escape character handling | ✅ | ✅ | Full support |
| Unicode escape sequences | ✅ | ✅ | Full support |

### Not Yet Implemented ⚠️

| Feature | Status | Priority | Notes |
|---------|--------|----------|-------|
| Streaming API | ❌ | Low | JavaScript has streaming support for Node.js |
| Newline delimited JSON (NDJSON) | ⚠️ | Medium | Partially supported |

## API Comparison

### JavaScript API

```javascript
import { jsonrepair } from 'jsonrepair'

try {
  const json = "{name: 'John'}"
  const repaired = jsonrepair(json)
  console.log(repaired)
} catch (err) {
  console.error(err)
}
```

### C++ API

```cpp
#include "jsonrepair.hpp"

try {
  std::string json = "{name: 'John'}";
  std::string repaired = jsonrepair::repair(json);
  std::cout << repaired << std::endl;
} catch (const jsonrepair::JSONRepairError& e) {
  std::cerr << e.what() << std::endl;
}
```

## Implementation Differences

### 1. String Handling

**JavaScript:**
```javascript
if (text.slice(i, i + 3) === '...') {
  i += 3
}
```

**C++:**
```cpp
if (i + 3 <= text.length() && text.substr(i, 3) == "...") {
  i += 3;
}
```

**Key Difference:** C++ requires explicit bounds checking to avoid undefined behavior.

### 2. Character Types

**JavaScript:**
- Uses UTF-16 encoding internally
- Single `string` type

**C++:**
- Uses UTF-8 encoding with `std::string`
- Separate `char` and `std::string` types
- Multi-byte UTF-8 sequences for Unicode characters

### 3. Regular Expressions

**JavaScript:**
```javascript
const regex = /^[0-9A-Fa-f]$/
return regex.test(char)
```

**C++:**
```cpp
return (c >= '0' && c <= '9') || 
       (c >= 'A' && c <= 'F') || 
       (c >= 'a' && c <= 'f');
```

**Key Difference:** C++ uses character range comparisons instead of regex for better performance.

### 4. Error Handling

**JavaScript:**
```javascript
throw new JSONRepairError(message, position)
```

**C++:**
```cpp
throw JSONRepairError(message, position);
```

**Key Difference:** C++ uses exceptions derived from `std::runtime_error` for better integration with standard C++ error handling.

### 5. Memory Management

**JavaScript:**
- Automatic garbage collection
- No explicit memory management needed

**C++:**
- Manual memory management (but using RAII)
- Stack allocation for most objects
- `std::string` handles its own memory

## Performance Comparison

| Aspect | JavaScript | C++ |
|--------|-----------|-----|
| Startup Time | Slower (JIT compilation) | Faster (native code) |
| Execution Speed | Fast (optimized JIT) | Very Fast (compiled) |
| Memory Usage | Higher (GC overhead) | Lower (precise allocation) |
| Small Inputs (<1KB) | Comparable | Slightly faster |
| Large Inputs (>1MB) | Good | Better |

## Use Case Recommendations

### Choose JavaScript When:
- ✅ Running in Node.js or browser
- ✅ Need streaming support for very large files
- ✅ Already using JavaScript ecosystem
- ✅ Need npm package distribution

### Choose C++ When:
- ✅ Building native desktop applications
- ✅ Embedding in C/C++ applications
- ✅ Need minimal dependencies
- ✅ Performance is critical
- ✅ Memory constraints
- ✅ Handling malformed JSON in C++ applications

## Example: External API Response Handling

### JavaScript
```javascript
import { jsonrepair } from 'jsonrepair'

const response = await fetch('https://api.example.com/data')
const responseText = await response.text()

const validJson = jsonrepair(responseText)
const data = JSON.parse(validJson)
```

### C++
```cpp
#include "jsonrepair.hpp"
#include <nlohmann/json.hpp>

// Get external API output (from your HTTP client)
std::string api_output = get_api_response();

// Repair the JSON
std::string valid_json = jsonrepair::repair(api_output);

// Parse with standard JSON library
auto data = nlohmann::json::parse(valid_json);
```

## Testing Approach

Both implementations use similar test suites to ensure consistent behavior.

### JavaScript Test Example
```javascript
test('repair unquoted keys', () => {
  expect(jsonrepair("{name: 'John'}"))
    .toBe('{"name": "John"}')
})
```

### C++ Test Example
```cpp
{"Unquoted keys",
 "{name: 'John'}",
 R"({"name": "John"})",
 false}
```

## Limitations

### JavaScript Version
- Streaming API only available in Node.js
- Larger memory footprint
- Slower startup time

### C++ Version
- No streaming API yet
- Requires compilation for each platform
- More complex build setup
- Manual memory safety (though mitigated by RAII)

## Future Roadmap

### Planned Features
1. ⏳ Streaming API for large files
2. ⏳ Better NDJSON support
3. ⏳ Performance optimizations
4. ⏳ Additional test cases

### Maintaining Parity
We actively monitor the JavaScript library for updates and port new features as they're released.

## Version Alignment

| JavaScript | C++ | Notes |
|-----------|-----|-------|
| 3.x | 1.0 | Core features ported |
| Streaming | Planned | Future implementation |

## Contributing

To help maintain parity between implementations:

1. When the JavaScript version adds a feature, create an issue
2. Implement the feature in C++
3. Add equivalent test cases
4. Update documentation

See [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## Credits

This C++ implementation is based on the excellent work by [Jos de Jong](https://github.com/josdejong) on the original [jsonrepair JavaScript library](https://github.com/josdejong/jsonrepair).

## License

Both implementations use the ISC License, ensuring compatibility and permissive usage.
