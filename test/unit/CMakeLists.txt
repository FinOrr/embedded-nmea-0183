cmake_minimum_required(VERSION 3.20)
project(unit-tests LANGUAGES C CXX)

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Build local NMEA library sources (avoid linking to a system 'nmea' with mismatched API)
# Resolve project root (two levels up from test/unit -> project root)
get_filename_component(PROJECT_ROOT "${CMAKE_SOURCE_DIR}/../.." ABSOLUTE)

file(GLOB_RECURSE nmea_sources CONFIGURE_DEPENDS
    ${PROJECT_ROOT}/src/*.c
    ${PROJECT_ROOT}/src/*.cpp
)

# Only create the nmea target if it does not already exist (avoid duplicate target error)
if(NOT TARGET nmea)
    if(nmea_sources)
        add_library(nmea STATIC ${nmea_sources})
    else()
        message(FATAL_ERROR "Failed to locate NMEA sources under ${PROJECT_ROOT}/src")
    endif()
endif()
target_include_directories(nmea PUBLIC
    ${PROJECT_ROOT}/inc
    ${PROJECT_ROOT}/src
)

# Add the unit test executable
add_executable(unit_tests
    main.cpp
    test_ais.cpp
    test_attitude.cpp
    test_comm.cpp
    test_core.cpp
    test_gnss.cpp
    test_heading.cpp
    test_misc.cpp
    test_navigation.cpp
    test_radar.cpp
    test_safety.cpp
    test_sensor.cpp
    test_system.cpp
    test_waypoint.cpp
)

# Include directories for tests
target_include_directories(unit_tests PRIVATE
    ${PROJECT_ROOT}/inc
    ${PROJECT_ROOT}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link to GTest and local NMEA library
target_link_libraries(unit_tests PRIVATE gtest nmea)

# Add the unit tests to CTest
add_test(NAME unit_tests COMMAND unit_tests)
