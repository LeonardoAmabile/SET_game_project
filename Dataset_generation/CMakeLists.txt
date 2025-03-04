cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)

enable_testing()  # Enable testing framework

# Add the test executable
add_executable(test_program tests/test.cpp src/Table.cpp src/Find_SETs.cpp src/Find_SETs_brute)
target_include_directories(test_program PRIVATE include)

# Register test
add_test(NAME RunTests COMMAND test_program)

# Ensure tests must pass before compiling my_program
add_executable(my_program Main.cpp src/Table.cpp src/Find_SETs.cpp src/Find_SETs_brute.cpp src/Write_txt.cpp)
target_include_directories(my_program PRIVATE include)

# Make my_program depend on passing tests
add_dependencies(my_program test_program)
add_custom_target(run_tests COMMAND ctest --output-on-failure DEPENDS test_program)
add_dependencies(my_program run_tests)

# Set output directory
set_target_properties(my_program PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
