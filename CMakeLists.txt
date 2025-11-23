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

# ============================================================================
# NMEA Parser Library
# ============================================================================

# Core source files
set(NMEA_CORE_SOURCES
    src/nmea_core.c
    src/nmea_tokenizer.c
    src/nmea_checksum.c
    src/nmea_error.c
    src/nmea_helpers.c
)

# Module source files (conditionally compiled based on nmea_config.h)
set(NMEA_MODULE_SOURCES
    src/modules/nmea_gnss.c
    src/modules/nmea_sensor.c
    src/modules/nmea_heading.c
    src/modules/nmea_navigation.c
    src/modules/nmea_waypoint.c
    src/modules/nmea_ais.c
    # Additional modules will be added as they are implemented
    # src/modules/nmea_radar.c
    # src/modules/nmea_safety.c
    # src/modules/nmea_comm.c
    # src/modules/nmea_system.c
    # src/modules/nmea_attitude.c
    # src/modules/nmea_misc.c
)

# Create static library
add_library(nmea STATIC
    ${NMEA_CORE_SOURCES}
    ${NMEA_MODULE_SOURCES}
)

# Include directories
target_include_directories(nmea PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_include_directories(nmea PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compiler options
target_compile_options(nmea PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    $<$<CONFIG:Debug>:-g3 -O0>
    $<$<CONFIG:Release>:-O2>
)

# Install targets
install(TARGETS nmea
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY inc/
    DESTINATION include/nmea
    FILES_MATCHING PATTERN "*.h"
)

# ============================================================================
# Testing and Fuzzing
# ============================================================================

add_subdirectory(test)

if (ENABLE_FUZZING)
    add_compile_options(-fsanitize=fuzzer,address,undefined)
    add_link_options(-fsanitize=fuzzer,address,undefined)
endif()

if (UNIT_TEST_ENABLED)
    add_compile_definitions(UNIT_TEST=1)
endif()

