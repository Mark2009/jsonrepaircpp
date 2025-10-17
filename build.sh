#!/bin/bash
# Build script for Linux/macOS
# Usage: ./build.sh

set -e

echo "Building jsonrepair-cpp..."

# Create build directory
if [ -d "build" ]; then
    echo "Cleaning existing build directory..."
    rm -rf build
fi

mkdir build
cd build

# Configure with CMake
echo ""
echo "Configuring with CMake..."
cmake ..

# Build
echo ""
echo "Building..."
cmake --build .

# Run tests
echo ""
echo "Running tests..."
ctest --output-on-failure

# Run example
echo ""
echo "Running example..."
echo "========================================================================"
./jsonrepair_example
echo "========================================================================"

cd ..

echo ""
echo "Build completed successfully!"
echo "Binaries are in: ./build/"
