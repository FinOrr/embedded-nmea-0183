# Documentation Index

This guide will help you find the right documentation.

## Getting Started

**New to the library?** Start here:

1. Read the main [README.md](../README.md) for an overview
2. Follow the [Integration Guide](integration-guide.md) for step-by-step setup
3. Check out the [examples](../examples/) for working code

## Documentation by Topic

### Integration & Setup

- **[Integration Guide](integration-guide.md)** - Complete integration guide with build system examples
  - CMake, Makefile, PlatformIO, Arduino
  - Platform-specific patterns (STM32, ESP32, Raspberry Pi)
  - Quick start examples

- **[Serial Port Integration](serial-port-integration.md)** - Connecting to UART/serial ports
  - POSIX/Linux configuration
  - STM32 HAL (interrupt & DMA)
  - ESP32 ESP-IDF
  - Arduino
  - Zephyr RTOS

- **[Stream Parsing](stream-parsing.md)** - Handling continuous NMEA data streams
  - Byte-by-byte buffering
  - Circular buffers
  - DMA double buffering
  - RTOS queue-based patterns
  - Edge cases and best practices

### Optimization

- **[Memory Optimization](memory-optimization.md)** - Minimizing RAM and flash usage
  - Compile-time configuration
  - Disabling unused modules/sentences
  - Static vs dynamic allocation
  - Compiler optimization flags
  - Memory measurement techniques

### Error Handling

- **[Error Handling](error-handling.md)** - Error handling patterns
  - Error types and classification
  - Error callbacks
  - Recovery strategies
  - Health monitoring
  - Production patterns

### Reference

- **[Supported Sentences](supported-sentences.md)** - List of all supported NMEA sentence types
  - 100+ sentence types across 12 modules

- **[FAQ](faq.md)** - Frequently asked questions
  - Common integration issues
  - Troubleshooting
  - Configuration questions
  - Platform-specific tips

## Documentation by Use Case

### "I want to add GPS to my microcontroller project"

1. [Integration Guide](integration-guide.md) - Learn how to integrate
2. [Serial Port Integration](serial-port-integration.md) - Connect to GPS module
3. [Stream Parsing](stream-parsing.md) - Handle incoming data
4. **Example:** [serial_stream](../examples/serial_stream/)

### "I need to minimize memory usage"

1. [Memory Optimization](memory-optimization.md) - Configuration guide
2. **Example:** [minimal_memory](../examples/minimal_memory/)

### "I need error handling"

1. [Error Handling](error-handling.md) - Error handling patterns
2. [FAQ](faq.md) - Common problems
3. **Example:** [comprehensive_error_handling](../examples/comprehensive_error_handling/)

### "I have multiple GPS devices"

1. [Integration Guide](integration-guide.md#pattern-2-multiple-data-sources) - Multi-instance pattern
2. **Example:** [multi_instance](../examples/multi_instance/)

### "I'm getting parse errors"

1. [FAQ](faq.md#troubleshooting) - Common issues
2. [Error Handling](error-handling.md) - Understanding errors
3. [Serial Port Integration](serial-port-integration.md#troubleshooting) - Connection issues

## Examples by Complexity

### Beginner
- [simple_parser.c](../examples/simple_parser.c) - Basic parsing
- [sensor_test.c](../examples/sensor_test.c) - Sensor module

### Intermediate
- [serial_stream](../examples/serial_stream/) - Real-world serial port parsing
- [multi_instance](../examples/multi_instance/) - Multiple parser contexts

### Advanced
- [minimal_memory](../examples/minimal_memory/) - Memory optimization techniques
- [comprehensive_error_handling](../examples/comprehensive_error_handling/) - Error handling patterns

## Quick Reference

### Memory Usage

| Configuration       | RAM        | Flash   |
|---------------------|------------|---------|
| GNSS only (minimal) | ~200 bytes | ~5-6 KB |
| GNSS + Sensor       | ~600 bytes | ~15 KB  |
| All modules         | ~1.8 KB    | ~30 KB  |

### Common Baud Rates

| Device Type   | Baud Rate |
|---------------|-----------|
| Older GPS     | 4800      |
| Modern GPS    | 9600      |
| High-rate GPS | 38400+    |

### Buffer Sizes

| Buffer          | Minimum   | Recommended |
|-----------------|-----------|-------------|
| Sentence buffer | 82 bytes  | 128 bytes   |
| Parse buffer    | 256 bytes | 256 bytes   |

## Platform-Specific Quick Links

### STM32
- [Serial Port Integration - STM32 HAL](serial-port-integration.md#stm32-hal)
- [Integration Guide - STM32](integration-guide.md#stm32-halll)

### ESP32
- [Serial Port Integration - ESP32](serial-port-integration.md#esp32-esp-idf)
- [Integration Guide - ESP32](integration-guide.md#esp32-esp-idf)

### Raspberry Pi / Linux
- [Serial Port Integration - POSIX](serial-port-integration.md#linux--raspberry-pi--posix)
- [Example: serial_stream_posix.c](../examples/serial_stream/serial_stream_posix.c)

### Arduino
- [Serial Port Integration - Arduino](serial-port-integration.md#arduino)
- [Integration Guide - Arduino IDE](integration-guide.md#arduino-ide)

## API Reference

For detailed API documentation, see the header files:

- [nmea.h](../inc/nmea.h) - Main API
- [nmea_types.h](../inc/nmea_types.h) - Type definitions
- [nmea_config.h](../inc/nmea_config.h) - Configuration

Module-specific headers:
- [nmea_gnss.h](../inc/modules/nmea_gnss.h) - GPS/GNSS
- [nmea_sensor.h](../inc/modules/nmea_sensor.h) - Sensors
- [nmea_ais.h](../inc/modules/nmea_ais.h) - AIS
- [nmea_heading.h](../inc/modules/nmea_heading.h) - Heading/compass
- [nmea_navigation.h](../inc/modules/nmea_navigation.h) - Navigation

## Contributing

Found an issue or want to contribute?

- Report bugs: <https://github.com/FinOrr/embedded-nmea-0183/issues>
- Documentation improvements welcome!
- See [LICENSE](../LICENSE) for terms

## Version History

See the main [README.md](../README.md) for version information.
