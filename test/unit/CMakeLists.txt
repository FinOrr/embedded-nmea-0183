cmake_minimum_required(VERSION 3.20)
project(unit-tests LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Add the unit test executable
add_executable(unit_tests
    main.cpp # Used by GoogleTest to link tests

)

# Link to GTest
target_link_libraries(unit_tests PRIVATE gtest)

# Add the unit tests to CTest
add_test(NAME unit_tests COMMAND unit_tests)
