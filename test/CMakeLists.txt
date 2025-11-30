cmake_minimum_required(VERSION 3.20)
project(embedded-nmea-0183 LANGUAGES ASM C CXX)

include(CTest)
include(GoogleTest)

if(ENABLE_FUZZING)
    message("Found fuzzing test directory")
    add_subdirectory(fuzzing)
endif()

if(ENABLE_UNIT_TESTS)
    message("Found unit test directory")
    add_subdirectory(unit)
endif()
