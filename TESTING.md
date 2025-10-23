# Testing Guide for TowerForge

This document provides information about the comprehensive test suite for TowerForge.

## Test Overview

The project includes three types of tests prioritized as follows:

1. **Integration Tests** (Highest Priority) - Test interactions between major components
2. **End-to-End (E2E) Tests** (Second Priority) - Test complete workflows
3. **Unit Tests** (Lowest Priority) - Test unique or complex logic only

## Test Structure

```
tests/
├── integration/          # Integration tests
│   ├── test_tower_grid_integration.cpp
│   ├── test_facility_manager_integration.cpp
│   ├── test_ecs_world_integration.cpp
│   ├── test_save_load_integration.cpp
│   ├── test_achievement_manager_integration.cpp
│   └── test_lua_mod_manager_integration.cpp
├── e2e/                  # End-to-end tests
│   ├── test_game_initialization_e2e.cpp
│   ├── test_facility_placement_workflow_e2e.cpp
│   └── test_save_load_workflow_e2e.cpp
└── unit/                 # Unit tests
    ├── test_user_preferences_unit.cpp
    ├── test_command_history_unit.cpp
    └── test_accessibility_settings_unit.cpp
```

## Prerequisites

Before running tests, ensure you have:

1. CMake 3.20 or newer
2. A C++20-capable compiler (GCC 10+, Clang 10+, or MSVC 2019+)
3. vcpkg installed and bootstrapped
4. On Linux: X11 development libraries installed

```bash
# On Ubuntu/Debian
sudo apt-get install -y build-essential cmake pkg-config \
  libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
  libgl1-mesa-dev libglu1-mesa-dev xvfb
```

## Building Tests

### Initial Setup

1. Clone and bootstrap vcpkg (if not already done):
```bash
git clone https://github.com/microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh  # or .bat on Windows
```

2. Configure the project with the test CMake preset:
```bash
export VCPKG_ROOT=/path/to/vcpkg
cmake --preset test
```

3. Build all tests:
```bash
cmake --build --preset test-debug --parallel $(nproc)
```

On Windows (cmd.exe):
```cmd
cmake --build --preset test-debug --parallel %NUMBER_OF_PROCESSORS%
```

### Building Specific Tests

You can build individual test targets:

```bash
cmake --build --preset test-debug --target test_tower_grid_integration
cmake --build --preset test-debug --target test_user_preferences_unit
cmake --build --preset test-debug --target test_game_initialization_e2e
```

## Running Tests

### Run All Tests

Using CTest from the tests subdirectory:
```bash
cd build/test/tests
ctest -C Debug --output-on-failure
```

Or run tests in parallel:
```bash
cd build/test/tests
ctest -C Debug -j$(nproc) --output-on-failure
```

Alternatively, use the test preset:
```bash
ctest --preset test-debug
```

### Run Individual Tests

Each test executable can be run directly:

```bash
# From the project root
./build/test/bin/Debug/test_tower_grid_integration
./build/test/bin/Debug/test_facility_manager_integration
./build/test/bin/Debug/test_user_preferences_unit
```

### Run Tests by Category

Run only integration tests:
```bash
cd build/test/tests
ctest -C Debug -R ".*_integration" --output-on-failure
```

Run only E2E tests:
```bash
cd build/test/tests
ctest -C Debug -R ".*_e2e" --output-on-failure
```

Run only unit tests:
```bash
cd build/test/tests
ctest -C Debug -R ".*_unit" --output-on-failure
```

### Using GTest Filters

You can run specific test cases within a test executable:

```bash
# Run only tests matching a pattern
./build/test/bin/Debug/test_tower_grid_integration --gtest_filter="*FloorExpansion*"

# Run all tests except those matching a pattern
./build/test/bin/Debug/test_tower_grid_integration --gtest_filter="-*Removal*"

# List all tests without running them
./build/test/bin/Debug/test_tower_grid_integration --gtest_list_tests
```

## Test Coverage

### Integration Tests

Integration tests verify that multiple components work together correctly:

- **TowerGrid Integration**: Floor/column management, facility placement, spatial queries
- **FacilityManager Integration**: Facility creation, placement on grid, removal, maintenance operations
- **ECSWorld Integration**: ECS initialization, entity creation, subsystem interactions
- **SaveLoadManager Integration**: Save/load operations, slot management, autosave functionality
- **AchievementManager Integration**: Achievement tracking, unlocking, progress management
- **LuaModManager Integration**: Mod loading, custom content registration

### E2E Tests

E2E tests verify complete workflows from start to finish:

- **Game Initialization**: Complete game startup sequence, system initialization
- **Facility Placement Workflow**: Building floors, placing facilities, expanding the tower
- **Save/Load Workflow**: Saving game state, loading into fresh world, continuing play

### Unit Tests

Unit tests focus on complex or unique logic:

- **UserPreferences**: Preferences persistence, validation, state management
- **CommandHistory**: Undo/redo functionality, command stack management
- **AccessibilitySettings**: Accessibility configuration and validation

## Test Status

As of the latest build:

- **Tests build successfully** - All compilation and linking issues resolved
- **4 out of 12 tests passing** (33% pass rate)
- **8 tests have logic issues** that need fixing:
  - Some tests make incorrect assumptions about API behavior
  - Some tests have logic bugs (e.g., expecting specific values that don't match implementation)

The test infrastructure is complete and functional. Remaining work involves fixing test logic to match actual API behavior, not fixing the build system.

## Writing New Tests

### Test Structure

Follow this structure for new tests:

```cpp
#include <gtest/gtest.h>
#include "core/your_component.hpp"

using namespace TowerForge::Core;

class YourComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test fixtures
    }

    void TearDown() override {
        // Clean up after tests
    }

    // Test fixtures
};

TEST_F(YourComponentTest, DescriptiveTestName) {
    // Arrange
    // Act
    // Assert
    EXPECT_EQ(actual, expected);
}
```

### Best Practices

1. **One assertion concept per test**: Each test should verify one specific behavior
2. **Clear test names**: Use descriptive names that explain what is being tested
3. **Arrange-Act-Assert**: Structure tests with clear setup, execution, and verification
4. **Clean up resources**: Always clean up temporary files, saves, or state
5. **Independent tests**: Tests should not depend on each other
6. **Meaningful assertions**: Use appropriate EXPECT_* macros (EXPECT_EQ, EXPECT_TRUE, etc.)

## Continuous Integration

Tests are automatically run in CI on:
- Push to main or develop branches
- Pull request creation or updates

CI configuration is in `.github/workflows/build.yml`.

## Troubleshooting

### Tests fail to build

1. Ensure vcpkg is properly bootstrapped
2. Verify all dependencies are installed
3. Check that you're using C++20 compatible compiler
4. Use the test preset: `cmake --preset test`
5. Try cleaning and rebuilding: `cmake --build --preset test-debug --clean-first`

### Tests fail to run

1. Check that all required system libraries are installed (especially on Linux)
2. Verify file permissions on test executables
3. For save/load tests, ensure write permissions in the test directory

### Performance issues

1. Run tests in parallel: `ctest -j$(nproc)`
2. Run only necessary tests during development
3. Use test filters to focus on relevant tests

## Contact

For questions or issues with the test suite, please create an issue on the GitHub repository.
