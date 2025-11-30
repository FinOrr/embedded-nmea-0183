cmake_minimum_required(VERSION 3.20)
project(unit-tests LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Build local NMEA library sources (avoid linking to a system 'nmea' with mismatched API)
file(GLOB nmea_sources CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/modules/*.c
)

# Only create the nmea target if it does not already exist (avoid duplicate target error)
if(NOT TARGET nmea)
    add_library(nmea STATIC ${nmea_sources})
endif()
target_include_directories(nmea PUBLIC
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
)

# Add the unit test executable
add_executable(unit_tests
    main.cpp
    test_ais.cpp
    test_attitude.cpp
    test_core.cpp
    test_gnss.cpp
    test_sensor.cpp
    test_heading.cpp
    test_navigation.cpp
    test_misc.cpp
)

# Include directories for tests
target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link to GTest and local NMEA library
target_link_libraries(unit_tests PRIVATE gtest nmea)

# Add the unit tests to CTest
add_test(NAME unit_tests COMMAND unit_tests)
