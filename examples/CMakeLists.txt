# ============================================================================
# NMEA Parser Examples
# ============================================================================

# Common compile options for all examples
set(EXAMPLE_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wpedantic
)

# ============================================================================
# GPS Tracker Example (Beginner)
# ============================================================================
add_executable(gps_tracker
    gps_tracker/gps_tracker.c
)

target_link_libraries(gps_tracker PRIVATE nmea)
target_compile_options(gps_tracker PRIVATE ${EXAMPLE_COMPILE_OPTIONS})

# ============================================================================
# Marine Instruments Example (Intermediate)
# ============================================================================
add_executable(marine_instruments
    marine_instruments/marine_instruments.c
)

target_link_libraries(marine_instruments PRIVATE nmea)
target_compile_options(marine_instruments PRIVATE ${EXAMPLE_COMPILE_OPTIONS})

# ============================================================================
# Serial Stream Example (Intermediate - POSIX only)
# ============================================================================
if(UNIX)
    add_executable(serial_stream
        serial_stream/serial_stream_posix.c
    )

    target_link_libraries(serial_stream PRIVATE nmea)
    target_compile_options(serial_stream PRIVATE ${EXAMPLE_COMPILE_OPTIONS})
endif()

# ============================================================================
# Multi-Instance Example (Intermediate)
# ============================================================================
add_executable(multi_instance
    multi_instance/multi_instance.c
)

target_link_libraries(multi_instance PRIVATE nmea m)
target_compile_options(multi_instance PRIVATE ${EXAMPLE_COMPILE_OPTIONS})

# ============================================================================
# Minimal Memory Example (Intermediate)
# ============================================================================
add_executable(minimal_memory
    minimal_memory/minimal_memory.c
)

target_link_libraries(minimal_memory PRIVATE nmea)
target_compile_options(minimal_memory PRIVATE ${EXAMPLE_COMPILE_OPTIONS})

# ============================================================================
# Error Handling Example (Advanced)
# ============================================================================
add_executable(error_handling
    error_handling/error_handling.c
)

target_link_libraries(error_handling PRIVATE nmea)
target_compile_options(error_handling PRIVATE ${EXAMPLE_COMPILE_OPTIONS})

# ============================================================================
# Set output directory for all examples
# ============================================================================
set_target_properties(
    gps_tracker
    marine_instruments
    multi_instance
    minimal_memory
    error_handling
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

if(UNIX)
    set_target_properties(
        serial_stream
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
    )
endif()
