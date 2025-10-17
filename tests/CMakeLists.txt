cmake_minimum_required(VERSION 3.14)

# Enable testing
enable_testing()

# Test executable
add_executable(jsonrepair_tests test_main.cpp)
target_link_libraries(jsonrepair_tests jsonrepair)

# Add the test
add_test(NAME jsonrepair_tests COMMAND jsonrepair_tests)
