cmake_minimum_required(VERSION 3.14)
project(jsonrepair-cpp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Library source files
set(LIBRARY_SOURCES
    src/jsonrepair.cpp
    src/string_utils.cpp
)

set(LIBRARY_HEADERS
    include/jsonrepair.hpp
    include/string_utils.hpp
    include/jsonrepair_error.hpp
)

# Create library
add_library(jsonrepair STATIC ${LIBRARY_SOURCES} ${LIBRARY_HEADERS})
target_include_directories(jsonrepair PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Examples
if(BUILD_EXAMPLES)
    add_executable(jsonrepair_example examples/basic_example.cpp)
    target_link_libraries(jsonrepair_example jsonrepair)
    
    add_executable(extended_use_cases examples/extended_use_cases.cpp)
    target_link_libraries(extended_use_cases jsonrepair)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS jsonrepair
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${LIBRARY_HEADERS} DESTINATION include/jsonrepair)
