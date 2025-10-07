cmake_minimum_required(VERSION 3.20)

project(towerforge VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Specify the vcpkg toolchain if not already set
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Main executable
set(SOURCE_FILES
    src/main.cpp
)

add_executable(towerforge ${SOURCE_FILES})

# Subdirectories for future modules (uncomment as needed)
# add_subdirectory(src/core)
# add_subdirectory(src/rendering)
# add_subdirectory(src/simulation)
# add_subdirectory(third_party)
