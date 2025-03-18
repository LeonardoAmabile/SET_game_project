cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 17)

# Add source files for the main program (these are required for both the main program and tests)
set(SOURCE_FILES
    src/Table.cpp
    src/Find_SETs.cpp
    src/Find_SETs_brute.cpp
    src/Write_txt.cpp
)

# Add the executable for the main program
add_executable(my_program
    Main.cpp
    ${SOURCE_FILES}  # Link the necessary source files
)

# Include the header files directory
target_include_directories(my_program PRIVATE include)

# Set the output directory for the executable (outside "build/")
set_target_properties(my_program PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Add a test target
add_executable(run_tests tests/test.cpp ${SOURCE_FILES})  # Link the same source files as for the main program

# Include the same header files for the test as for the main program
target_include_directories(run_tests PRIVATE include)

# Ensure that the test is built and executed before the main program
add_dependencies(my_program run_tests)

# Optionally, add a command to run the tests if needed (after they are built)
add_custom_command(TARGET run_tests POST_BUILD
    COMMAND run_tests
    COMMENT "Running tests before building main program"
)
