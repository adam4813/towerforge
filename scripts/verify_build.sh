#!/bin/bash
# Build verification script for TowerForge
# This script ensures the project builds successfully

set -e  # Exit on any error

echo "=== TowerForge Build Verification Script ==="
echo ""

# Check for required dependencies
echo "Step 1: Checking dependencies..."
if ! command -v cmake &> /dev/null; then
    echo "ERROR: cmake is not installed"
    exit 1
fi

if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ is not installed"
    exit 1
fi

echo "  ✓ CMake found: $(cmake --version | head -n1)"
echo "  ✓ G++ found: $(g++ --version | head -n1)"

# Check for X11 libraries
echo ""
echo "Step 2: Checking X11 libraries..."
if ! dpkg -l | grep -q libx11-dev; then
    echo "WARNING: libx11-dev not found. Install with:"
    echo "  sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev"
fi

# Create build directory
echo ""
echo "Step 3: Setting up build directory..."
cd "$(dirname "$0")/.."
rm -rf build
mkdir -p build
cd build

# Configure with CMake
echo ""
echo "Step 4: Configuring with CMake..."
if [ -f "/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake" ]; then
    cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake
else
    echo "WARNING: vcpkg toolchain not found, using default configuration"
    cmake ..
fi

# Build the project
echo ""
echo "Step 5: Building the project..."
cmake --build . --parallel $(nproc)

# Verify binaries exist
echo ""
echo "Step 6: Verifying built binaries..."
if [ ! -f "bin/towerforge" ]; then
    echo "ERROR: towerforge binary not found"
    exit 1
fi

if [ ! -f "bin/screenshot_app" ]; then
    echo "ERROR: screenshot_app binary not found"
    exit 1
fi

echo "  ✓ towerforge binary created ($(du -h bin/towerforge | cut -f1))"
echo "  ✓ screenshot_app binary created ($(du -h bin/screenshot_app | cut -f1))"

echo ""
echo "=== ✓ Build verification successful! ==="
echo ""
echo "Binaries are located in: $(pwd)/bin/"
echo "  - towerforge: Main game executable"
echo "  - screenshot_app: Screenshot utility"
echo ""
