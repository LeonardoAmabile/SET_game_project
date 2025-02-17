cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)

# Add source files
add_executable(my_program Main.cpp src/Table.cpp src/Find_SETs.cpp src/Find_SETs_brute.cpp src/Write_txt.cpp)

# Include the header files directory
target_include_directories(my_program PRIVATE include)

# Set the output directory for the executable (outside "build/")
set_target_properties(my_program PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
