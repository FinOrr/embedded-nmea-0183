# embedded-nmea-0183
## Overview
C library for parsing NMEA-0183 sentences on embedded systems. Zero heap allocation, multi-instance contexts, compile-time module selection to minimize binary size.

**Use this if you need to:** parse GPS/GNSS, AIS, radar, navigation, or sensor data from NMEA-0183 devices on microcontrollers or resource-constrained systems.

**Don't use this if:** you need NMEA-2000, need dynamic configuration, or want automatic sentence type detection without compile-time configuration.

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![GitHub top language](https://img.shields.io/github/languages/top/FinOrr/embedded-nmea-0183.svg)](https://github.com/FinOrr/embedded-nmea-0183)
[![GitHub issues](https://img.shields.io/github/issues/FinOrr/embedded-nmea-0183.svg)](https://github.com/FinOrr/embedded-nmea-0183/issues)
[![GitHub pull requests](https://img.shields.io/github/issues-pr/FinOrr/embedded-nmea-0183.svg)](https://github.com/FinOrr/embedded-nmea-0183/pulls)

## Features

- **Zero heap allocation** - All state in caller-provided context, working buffer for tokenization
- **Multi-instance support** - Multiple independent parser contexts
- **Modular** - Enable only the modules you need (GNSS, AIS, Navigation, Heading, Sensor, Radar, Safety, Comm, System, Attitude, Waypoint, Misc)
- **Compile-time configuration** - Disable unused sentence types to reduce binary size
- **~100+ sentence types** - See [supported-sentences.md](doc/supported-sentences.md)
- **Static library** - Link with CMake

## Quick Start

```c
#include "nmea.h"

// 1. Create context and config
nmea_context_t ctx;
nmea_config_t config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS) | (1 << NMEA_MODULE_SENSOR),
    .error_callback = NULL,
    .validate_checksums = true
};

// 2. Initialize
if (nmea_init(&ctx, &config) != NMEA_OK) {
    return -1;
}

// 3. Parse sentence (requires working buffer)
const char *sentence = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";
char buffer[128];  // Min 82 bytes required

if (nmea_parse(&ctx, sentence, strlen(sentence), buffer, sizeof(buffer)) == NMEA_OK) {
    // 4. Access parsed data
    nmea_gnss_state_t gnss;
    if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
        printf("Lat: %.6f, Lon: %.6f\n", gnss.latitude.degrees, gnss.longitude.degrees);
    }
}

// 5. Cleanup when done
nmea_cleanup(&ctx);
```

## Examples & Use Cases

Working examples demonstrating common integration scenarios:

### [GPS Tracker](examples/gps_tracker/)
Basic GPS tracking: position, speed, altitude, time.

- Data: Latitude, longitude, altitude, speed, course, UTC time/date, fix quality
- Memory: ~200 bytes RAM, ~6 KB flash
- Complexity: Beginner

```c
nmea_parse(&ctx, sentence, len, buffer, buf_size);
nmea_gnss_state_t gps;
nmea_get_gnss_data(&ctx, &gps);
printf("Position: %.6f, %.6f\n", gps.latitude.degrees, gps.longitude.degrees);
```

### [Marine Instruments](examples/marine_instruments/)
Legacy marine equipment integration: depth sounders, wind instruments, compass, speed logs, temperature sensors.

- Data: Water depth, wind speed/direction, heading, speed through water, ROT, water temperature
- Memory: ~600 bytes RAM, ~15 KB flash
- Complexity: Intermediate

```c
nmea_sensor_state_t sensors;
nmea_get_sensor_data(&ctx, &sensors);
printf("Depth: %.1f m, Wind: %.1f knots\n",
       sensors.depth_below_transducer_meters,
       sensors.wind_speed_knots);
```

### [Serial Stream Parsing](examples/serial_stream/)
UART/serial port integration for Linux/POSIX systems. Includes buffering, error recovery, and statistics.

- Complexity: Intermediate

### [Multiple GPS Devices](examples/multi_instance/)
Multiple independent parser contexts for redundant GPS systems or separate data streams.

- Complexity: Intermediate

### [Minimal Memory](examples/minimal_memory/)
Memory optimization for resource-constrained microcontrollers.

- Complexity: Intermediate

### [Error Handling](examples/error_handling/)
Error handling with callbacks, statistics, recovery strategies, and health monitoring.

- Complexity: Advanced

**All examples include:**
- Complete, buildable code
- Detailed documentation
- Real-world use cases
- Platform-specific notes

See [examples/](examples/) for complete list and [doc/](doc/) for integration guides.

## Building

```bash
mkdir build && cd build
cmake ..
make
make install  # Installs to system or CMAKE_INSTALL_PREFIX
```

Link against `libnmea.a` and include `nmea.h`.

## Configuration

Modules and sentences can be disabled at compile-time in `inc/nmea_config.h`:

```c
// Disable entire modules
#define NMEA_MODULE_AIS_ENABLED 0
#define NMEA_MODULE_RADAR_ENABLED 0

// Disable specific sentences
#define NMEA_SENTENCE_GGA_ENABLED 1  // Keep GPS position
#define NMEA_SENTENCE_RMC_ENABLED 1  // Keep recommended minimum
#define NMEA_SENTENCE_GSV_ENABLED 0  // Don't need satellites in view
```

See `inc/nmea_config.h` for all configuration options.

## Modules

Each module groups related sentence types:

- **GNSS** - GPS/GLONASS/Galileo position, velocity, time, satellites (GGA, RMC, GLL, GSA, GSV, etc.)
- **AIS** - Automatic Identification System messages (VDM, VDO, ABM, BBM)
- **Navigation** - Cross-track error, waypoint arrival, bearing/distance to waypoint (APB, BOD, BWC, RMB, etc.)
- **Heading** - True/magnetic heading, attitude, rate of turn (HDT, HDG, ROT, etc.)
- **Sensor** - Depth, water temperature, wind, pressure (DBT, DPT, MTW, MWV, etc.)
- **Radar** - Radar status and targets (TTM, TLL, etc.)
- **Safety** - Alarms and safety-related messages (ALA, ALR, etc.)
- **Comm** - Communication state and DSC (DSC, DSE, etc.)
- **System** - System state and text messages (TXT, VER, etc.)
- **Attitude** - Pitch, roll, heave (HMR, HSS, etc.)
- **Waypoint** - Waypoint/route management (WPL, RTE, etc.)
- **Misc** - Everything else

## API

Core functions:
- `nmea_init(ctx, config)` - Initialize parser context
- `nmea_parse(ctx, sentence, len, buffer, buf_size)` - Parse one sentence
- `nmea_cleanup(ctx)` - Reset context
- `nmea_get_gnss_data(ctx, out)` - Retrieve parsed GNSS data
- `nmea_get_ais_data(ctx, out)` - Retrieve parsed AIS data
- *(similar accessors for other modules)*

See `inc/nmea.h` for full API documentation.

## Testing

```bash
cd build
ctest
```

Unit tests are in `test/unit/`, built with Google Test.

## Memory Usage

Context size depends on enabled modules. Typical configurations:

- GNSS only: ~200 bytes
- GNSS + Sensor: ~300 bytes
- All modules: ~1-2 KB

Use `nmea_get_context_size()` to query actual size for your configuration.

Buffer requirement: 82 bytes minimum for working buffer (use `nmea_get_required_buffer_size()`).

## Error Handling

All functions return `nmea_result_t`:

```c
nmea_result_t result = nmea_parse(&ctx, sentence, len, buf, buf_size);
if (result != NMEA_OK) {
    printf("Error: %s\n", nmea_get_error_string(result));
}
```

Optional error callback for real-time notification:

```c
void my_error_handler(nmea_error_type_t type, nmea_result_t code,
                      const char *message, void *user_data) {
    fprintf(stderr, "NMEA error: %s\n", message ? message : nmea_get_error_string(code));
}

nmea_config_t config = {
    .error_callback = my_error_handler,
    .error_callback_user_data = NULL,
    // ...
};
```

## Documentation

Complete guides and references in the [`doc/`](doc/) directory:

**Getting Started:**
- [Integration Guide](doc/integration-guide.md) - Step-by-step setup for CMake, Makefile, Arduino, PlatformIO
- [Serial Port Integration](doc/serial-port-integration.md) - UART/serial connection examples (STM32, ESP32, Linux, Arduino)
- [FAQ](doc/faq.md) - Common questions and troubleshooting

**Advanced Topics:**
- [Stream Parsing](doc/stream-parsing.md) - Buffering strategies for continuous data
- [Memory Optimization](doc/memory-optimization.md) - Minimize RAM/flash usage
- [Error Handling](doc/error-handling.md) - Error recovery patterns

**Reference:**
- [Supported Sentences](doc/supported-sentences.md) - Complete list of 100+ sentence types
- [Documentation Index](doc/) - Browse all documentation

**Examples:**
- [GPS Tracker](examples/gps_tracker/) - Simple GPS position tracking
- [Marine Instruments](examples/marine_instruments/) - Depth, wind, compass, temperature
- [Serial Stream](examples/serial_stream/) - Real serial port integration
- [Multi-Instance](examples/multi_instance/) - Multiple GPS devices
- [Minimal Memory](examples/minimal_memory/) - Memory optimization
- [Error Handling](examples/error_handling/) - Error handling patterns
- [All Examples](examples/) - Browse all examples

## License

MIT License - See [LICENSE](LICENSE)

## Support

- **Documentation:** [doc/](doc/) and [examples/](examples/)
- **Issues:** https://github.com/FinOrr/embedded-nmea-0183/issues
- **Questions:** Check [FAQ](doc/faq.md) first
