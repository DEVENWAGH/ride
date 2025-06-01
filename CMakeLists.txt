cmake_minimum_required(VERSION 3.10)
project(RideEasy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all source files
file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

# Add executable with all source files
add_executable(rideeasy ${SOURCES})

# Set include directories
target_include_directories(rideeasy PRIVATE .)

# Compiler-specific options
if(MSVC)
    target_compile_options(rideeasy PRIVATE /W4)
else()
    target_compile_options(rideeasy PRIVATE -Wall -Wextra -Wpedantic)
endif()

# For Windows, ensure console output is visible
if(WIN32)
    set_target_properties(rideeasy PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()
