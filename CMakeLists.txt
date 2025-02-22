cmake_minimum_required(VERSION 3.20)

project (embedded-nmea-0183)

enable_language(ASM C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()

set(CTEST_OUTPUT_ON_FAILURE TRUE)
set(CTEST_CUSTOM_REPORT_JUNIT TRUE)

add_subdirectory(test)

if (ENABLE_FUZZING)
    add_compile_options(-fsanitize=fuzzer,address,undefined)
    add_link_options(-fsanitize=fuzzer,address,undefined)
endif()

if (UNIT_TEST_ENABLED)
    add_compile_definitions(UNIT_TEST=1)
endif()

