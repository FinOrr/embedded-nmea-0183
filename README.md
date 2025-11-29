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

## License

MIT License - See [LICENSE](LICENSE)

## Support

File issues at: https://github.com/FinOrr/embedded-nmea-0183/issues

See [doc/supported-sentences.md](doc/supported-sentences.md) for complete sentence list.
