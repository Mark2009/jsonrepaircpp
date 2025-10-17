# Contributing to jsonrepair-cpp

## Overview

This is a C++ port of the excellent [jsonrepair](https://github.com/josdejong/jsonrepair) JavaScript library by Jos de Jong. Our goal is to maintain feature parity with the original library while providing a native C++ implementation.

## Project Structure

```
jsonrepaircpp/
├── include/              # Public header files
│   ├── jsonrepair.hpp   # Main API
│   ├── jsonrepair_error.hpp
│   └── string_utils.hpp
├── src/                 # Implementation files
│   ├── jsonrepair.cpp
│   └── string_utils.cpp
├── tests/               # Unit tests
│   └── test_main.cpp
├── examples/            # Example programs
│   └── basic_example.cpp
└── CMakeLists.txt       # Build configuration
```

## Development Setup

### Requirements

- C++17 or later compiler (MSVC, GCC, Clang)
- CMake 3.14 or later
- Git

### Building

```bash
# Clone the repository
git clone <repository-url>
cd jsonrepaircpp

# Build
mkdir build
cd build
cmake ..
cmake --build .

# Run tests
ctest
```

### Windows PowerShell

```powershell
.\build.ps1
```

### Linux/macOS

```bash
./build.sh
```

## Coding Standards

### Style Guidelines

1. **Naming Conventions**:
   - Classes: `PascalCase` (e.g., `JSONRepairError`)
   - Functions: `camelCase` (e.g., `parseValue`)
   - Variables: `snake_case` with trailing underscore for members (e.g., `output_`)
   - Constants: `UPPER_SNAKE_CASE` (e.g., `CODE_SPACE`)

2. **Formatting**:
   - Indentation: 4 spaces (no tabs)
   - Line length: Prefer 100 characters max
   - Braces: Opening brace on same line

3. **Comments**:
   - Use `//` for single-line comments
   - Use `/** ... */` for documentation comments
   - Document all public APIs

### Example

```cpp
/**
 * Parse a JSON value from the input stream.
 * @return true if a value was successfully parsed
 */
bool parseValue() {
    parseWhitespaceAndSkipComments();
    bool processed = parseObject() || parseArray() || parseString();
    parseWhitespaceAndSkipComments();
    return processed;
}
```

## Testing

### Adding Tests

Tests are located in `tests/test_main.cpp`. When adding a new test:

1. Add a test case to the `tests` vector
2. Provide a descriptive name
3. Include both input and expected output
4. Run tests to ensure they pass

Example:

```cpp
{"Your test name",
 "input JSON",
 "expected output",
 false},  // shouldThrow flag
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

Or run the test executable directly:

```bash
./build/Release/jsonrepair_tests  # Windows
./build/jsonrepair_tests           # Linux/macOS
```

## Porting from JavaScript

### Key Differences

1. **String Handling**:
   - JavaScript: Strings are UTF-16
   - C++: We use UTF-8 `std::string`

2. **Character Access**:
   - JavaScript: `text[i]` returns a character
   - C++: `text[i]` returns a char, `text.substr(i, len)` for substrings

3. **Regex**:
   - JavaScript: Built-in regex with `/pattern/`
   - C++: Use `<regex>` library with `std::regex`

4. **Array/Object Methods**:
   - JavaScript: `.slice()`, `.includes()`, etc.
   - C++: `.substr()`, `.find()`, etc.

### Porting Checklist

When porting a feature from the JavaScript version:

- [ ] Understand the JavaScript implementation
- [ ] Translate to C++ idioms
- [ ] Add tests matching the JavaScript test cases
- [ ] Verify edge cases
- [ ] Update documentation

### Example Translation

**JavaScript:**
```javascript
if (text.slice(i, i + 3) === '...') {
    i += 3
    return true
}
```

**C++:**
```cpp
if (i + 3 <= text.length() && text.substr(i, 3) == "...") {
    i += 3;
    return true;
}
```

## Performance Optimization

### Guidelines

1. **Minimize String Copies**: Use references where possible
2. **Reserve Capacity**: Pre-allocate strings when size is known
3. **Avoid Unnecessary Regex**: Use simple string operations when possible
4. **Bounds Checking**: Always check array bounds before access

### Profiling

If you're optimizing performance:

1. Measure before optimizing (use benchmarks)
2. Focus on hot paths identified by profiling
3. Document any performance-critical sections
4. Add benchmarks for regression testing

## Pull Request Process

1. **Fork and Branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make Changes**:
   - Write clear, focused commits
   - Add tests for new functionality
   - Update documentation

3. **Test**:
   ```bash
   cmake --build build
   cd build && ctest
   ```

4. **Commit**:
   ```bash
   git commit -m "Add feature: description"
   ```

5. **Push and Create PR**:
   ```bash
   git push origin feature/your-feature-name
   ```

### Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `test`: Adding tests
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `chore`: Build/tool changes

Example:
```
feat: Add support for MongoDB ObjectId repair

- Recognize ObjectId("...") pattern
- Convert to string representation
- Add tests for ObjectId patterns

Closes #42
```

## Bug Reports

When filing a bug report, include:

1. **Description**: Clear description of the issue
2. **Input**: The JSON string that causes the problem
3. **Expected**: What you expected to happen
4. **Actual**: What actually happened
5. **Environment**: OS, compiler, version
6. **Code**: Minimal code to reproduce

Example:

```markdown
### Description
The library fails to repair JSON with nested arrays

### Input
```json
{items: [[1,2][3,4]]}
```

### Expected
```json
{"items": [[1,2],[3,4]]}
```

### Actual
Exception thrown: "Unexpected character ..."

### Environment
- OS: Windows 11
- Compiler: MSVC 19.44
- Version: 1.0.0
```

## Feature Requests

Feature requests should:

1. Explain the use case
2. Provide examples
3. Reference the JavaScript version if applicable
4. Consider backwards compatibility

## Synchronization with JavaScript Version

We aim to keep parity with the JavaScript library. When the JavaScript version is updated:

1. Review the changelog
2. Identify new features or fixes
3. Port to C++
4. Add tests
5. Update documentation
6. Tag release with matching version

## Questions and Support

- **Issues**: Use GitHub issues for bugs and features
- **Discussions**: Use GitHub discussions for questions
- **Original Library**: Reference the [JavaScript version](https://github.com/josdejong/jsonrepair) for clarification

## License

By contributing, you agree that your contributions will be licensed under the ISC License, matching the original library.

## Acknowledgments

This project is a port of the excellent work by Jos de Jong. We are grateful for the original implementation and comprehensive test suite that make this C++ port possible.
