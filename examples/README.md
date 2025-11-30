# Examples

This directory contains working examples demonstrating various use cases and integration patterns for the NMEA-0183 parser library.

## Available Examples

### Getting Started Examples

#### [gps_tracker/](gps_tracker/)
**Difficulty:** Beginner
**Purpose:** Basic GPS position tracking

Demonstrates GPS position, speed, altitude, and time parsing with minimal code.

**Topics covered:**
- Parser initialization (GNSS module only)
- Retrieving latitude/longitude
- Reading speed, course, altitude
- Fix quality and satellite count

**Memory:** ~200 bytes RAM, ~6 KB flash

---

#### [marine_instruments/](marine_instruments/)
**Difficulty:** Intermediate
**Purpose:** Marine equipment integration

Integration with legacy marine instruments: depth sounders, wind instruments, compass, speed logs, temperature sensors.

**Topics covered:**
- Multi-module configuration (Sensor + Heading)
- Depth sounder parsing (DBT, DPT)
- Wind instrument parsing (MWD, MWV)
- Compass/heading parsing (HDT, HDG)
- Rate of turn (ROT)
- Speed through water (VHW, VBW)

**Memory:** ~600 bytes RAM, ~15 KB flash

---

#### [simple_parser.c](simple_parser.c)
**Difficulty:** Beginner
**Purpose:** Single-file example

Basic demonstration of initialization and parsing.

---

#### [sensor_test.c](sensor_test.c)
**Difficulty:** Beginner
**Purpose:** Sensor module testing

Demonstrates parsing of various sensor sentence types.

---

### Serial Port / Stream Parsing

#### [serial_stream/](serial_stream/)
**Difficulty:** Intermediate
**Purpose:** Serial port integration

**Topics covered:**
- UART/serial port configuration (POSIX)
- Byte-by-byte buffering
- Stream parsing
- Error statistics

**Files:**
- `serial_stream_posix.c` - Linux/macOS/POSIX implementation
- `README.md` - Build and usage instructions

---

### Multi-Instance Parsing

#### [multi_instance/](multi_instance/)
**Difficulty:** Intermediate
**Purpose:** Multiple parser contexts

**Topics covered:**
- Creating multiple parser contexts
- Per-context configuration
- Parsing from multiple data sources
- Data quality comparison
- GPS redundancy patterns

**Use cases:**
- Multiple GPS devices
- Separate GNSS and sensor data streams
- Redundant systems

**Files:**
- `multi_instance.c` - Example with 3 independent contexts
- `README.md` - Documentation

---

### Memory Optimization

#### [minimal_memory/](minimal_memory/)
**Difficulty:** Intermediate
**Purpose:** RAM and flash optimization

**Topics covered:**
- Minimal configuration
- Static allocation
- Memory measurement
- Configuration comparisons
- Optimization techniques

**Use cases:**
- Resource-constrained microcontrollers
- Memory footprint minimization
- Configuration analysis

**Files:**
- `minimal_memory.c` - Memory optimization examples
- `README.md` - Optimization guide

---

### Error Handling

#### [comprehensive_error_handling/](comprehensive_error_handling/)
**Difficulty:** Advanced
**Purpose:** Error handling patterns

**Topics covered:**
- Error callbacks with categorization
- Error statistics and monitoring
- Recovery strategies
- Health monitoring
- Graceful degradation
- Rate-limited logging

**Use cases:**
- Applications requiring error recovery
- Data quality monitoring
- Detailed error tracking

**Files:**
- `error_handling.c` - Complete error handling patterns
- `README.md` - Error handling guide

---

## Quick Start

### Build All Examples

```bash
# Configure and build
cmake --preset conf-examples
cmake --build --preset build-examples

# Run examples from build directory
./out/build/conf-examples/examples/gps_tracker
./out/build/conf-examples/examples/marine_instruments
```

### Build Individual Example (Manual)

```bash
gcc -I./inc examples/gps_tracker/gps_tracker.c -L./out/build/conf-examples/lib -lnmea -o gps_tracker
./gps_tracker
```

## Examples by Use Case

### New to the library
Start with [gps_tracker/](gps_tracker/) for basic GPS parsing

### GPS position tracking
[gps_tracker/](gps_tracker/) - Position, speed, altitude, time

### Marine instrument integration
[marine_instruments/](marine_instruments/) - Depth, wind, compass, sensors

### Serial/UART connection
[serial_stream/](serial_stream/) - Serial port integration (POSIX)

### Multiple GPS devices
[multi_instance/](multi_instance/) - Redundant GPS systems

### Memory-constrained systems
[minimal_memory/](minimal_memory/) - RAM/flash optimization

### Error handling patterns
[error_handling/](error_handling/) - Error recovery and monitoring

### Sensor parsing
[sensor_test.c](sensor_test.c) - Sensor module examples

## Platform-Specific Examples

### Linux / Raspberry Pi / macOS
- [serial_stream/serial_stream_posix.c](serial_stream/serial_stream_posix.c)

### STM32
- See [doc/serial-port-integration.md](../doc/serial-port-integration.md#stm32-hal)

### ESP32
- See [doc/serial-port-integration.md](../doc/serial-port-integration.md#esp32-esp-idf)

### Arduino
- See [doc/serial-port-integration.md](../doc/serial-port-integration.md#arduino)

## Related Documentation

- [Integration Guide](../doc/integration-guide.md) - Complete setup guide
- [Serial Port Integration](../doc/serial-port-integration.md) - UART specifics
- [Stream Parsing](../doc/stream-parsing.md) - Buffering and parsing
- [Memory Optimization](../doc/memory-optimization.md) - Reducing memory usage
- [Error Handling](../doc/error-handling.md) - Error handling patterns
- [FAQ](../doc/faq.md) - Common questions

## Complexity Progression

**Beginner:**
- simple_parser.c
- sensor_test.c
- gps_tracker/

**Intermediate:**
- marine_instruments/
- serial_stream/
- multi_instance/
- minimal_memory/

**Advanced:**
- comprehensive_error_handling/

## Contributing Examples

Have a useful example? Contributions are welcome!

Examples should:
- Be well-documented with comments
- Include a README.md explaining purpose and usage
- Demonstrate a specific use case or pattern
- Be buildable with standard tools
- Follow the library's coding style

## Getting Help

- Check the [FAQ](../doc/faq.md)
- Read the relevant documentation in [doc/](../doc/)
- File an issue: https://github.com/FinOrr/embedded-nmea-0183/issues
