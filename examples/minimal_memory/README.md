# Minimal Memory Example

This example demonstrates how to minimize RAM and flash usage for resource-constrained embedded systems.

## Purpose

Shows memory optimization techniques for systems with limited resources (e.g., 8-bit AVR, small ARM Cortex-M0/M0+).

## Features

- Minimal module/sentence configuration
- Static memory allocation
- Memory usage measurement
- Configuration comparisons
- Optimization tips

## Building

```bash
# From project root - build all examples
cmake --preset conf-examples && cmake --build --preset build-examples

# Or build manually
gcc -I./inc examples/minimal_memory/minimal_memory.c -L./out/build/conf-examples/lib -lnmea -o minimal_memory
```

## Running

```bash
./out/build/conf-examples/examples/minimal_memory
```

## Expected Output

```text
NMEA Minimal Memory Configuration Examples
===========================================

========================================
Example 1: Minimal GPS Configuration
========================================

=== Minimal GPS (GGA only) ===
Context size:        216 bytes
Required buffer:     256 bytes
Total stack usage:   472 bytes

Parsing GGA sentence...
[OK] Parse successful
Position: 48.117300°, 11.516670°
Altitude: 545.4 m
Satellites: 8

Flash estimate: ~5-6 KB (with only GGA enabled)

========================================
Example 2: Basic GPS Tracker
========================================

=== Basic Tracker (GGA + RMC) ===
Context size:        216 bytes
Required buffer:     256 bytes
Total stack usage:   472 bytes

Parsing GGA and RMC sentences...
[OK] GGA parsed
[OK] RMC parsed

Data from both sentences:
Position: 48.117300°, 11.516670°
Speed: 22.4 knots
Course: 84.4°
Date: 2025-03-23

Flash estimate: ~6-8 KB (with GGA + RMC enabled)
```

## Achieving Minimal Memory

### Step 1: Edit Configuration

Edit `inc/nmea_config.h`:

```c
/* Disable all modules except GNSS */
#define NMEA_MODULE_GNSS_ENABLED        1
#define NMEA_MODULE_AIS_ENABLED         0
#define NMEA_MODULE_NAVIGATION_ENABLED  0
#define NMEA_MODULE_WAYPOINT_ENABLED    0
#define NMEA_MODULE_HEADING_ENABLED     0
#define NMEA_MODULE_SENSOR_ENABLED      0
#define NMEA_MODULE_RADAR_ENABLED       0
#define NMEA_MODULE_SAFETY_ENABLED      0
#define NMEA_MODULE_COMM_ENABLED        0
#define NMEA_MODULE_SYSTEM_ENABLED      0
#define NMEA_MODULE_ATTITUDE_ENABLED    0
#define NMEA_MODULE_MISC_ENABLED        0

/* Within GNSS module, enable only needed sentences */
#define NMEA_SENTENCE_GGA_ENABLED 1  // Position
#define NMEA_SENTENCE_RMC_ENABLED 1  // Speed, date
#define NMEA_SENTENCE_GLL_ENABLED 0  // Disable
#define NMEA_SENTENCE_GSA_ENABLED 0  // Disable
#define NMEA_SENTENCE_GSV_ENABLED 0  // Disable (saves ~2 KB)
#define NMEA_SENTENCE_VTG_ENABLED 0  // Disable
// ... etc
```

### Step 2: Rebuild Library

```bash
cd build
make clean
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
make
```

### Step 3: Compiler Optimizations

```makefile
# Optimize for size
CFLAGS += -Os

# Link-time optimization
CFLAGS += -flto

# Remove unused code
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

# For ARM
CFLAGS += -mthumb  # Use Thumb-2 instructions
```

## Memory Usage Comparison

| Configuration | RAM (bytes) | Flash (KB) |
|---------------|-------------|------------|
| GGA only      | ~200        | 5-6        |
| GGA + RMC     | ~220        | 6-8        |
| GNSS (all)    | ~350        | 12-15      |
| GNSS + Sensor | ~600        | 15-20      |
| All modules   | ~1800       | 30-35      |

## Code Techniques

### 1. Static Allocation

```c
// Good: Static allocation (BSS section)
static nmea_context_t ctx;
static char parse_buffer[256];

// Avoid: Stack allocation in functions
void parse_data(void) {
    nmea_context_t ctx;  // 200+ bytes on stack
    char buffer[256];    // Additional stack usage
    // ...
}
```

### 2. Buffer Reuse

```c
// Good: Reuse buffers
static char parse_buffer[256];

void process_sentence1(void) {
    nmea_parse(&ctx, s1, len, parse_buffer, sizeof(parse_buffer));
}

void process_sentence2(void) {
    nmea_parse(&ctx, s2, len, parse_buffer, sizeof(parse_buffer));
}
```

### 3. Minimal Configuration

```c
// Good: Only enable what you need
config.enabled_modules = (1 << NMEA_MODULE_GNSS);

// Avoid: Enabling everything
config.enabled_modules = 0xFFFFFFFF;  // Wastes 1-1.5 KB RAM
```

## Measuring Memory

### Runtime Queries

```c
printf("Context: %zu bytes\n", nmea_get_context_size());
printf("Buffer:  %zu bytes\n", nmea_get_required_buffer_size());
```

### Compiler Tools

```bash
# Show memory usage
arm-none-eabi-size -A libnmea.a

# Detailed breakdown
arm-none-eabi-nm --size-sort --radix=d libnmea.a

# Generate map file
LDFLAGS += -Wl,-Map=output.map
```

## Target Platform Examples

### ATmega328P (Arduino Uno)
- RAM: 2 KB
- Flash: 32 KB
- Recommendation: GNSS only, 2-3 sentences (~300 bytes RAM, ~8 KB flash)

### STM32F103C8 (Blue Pill)
- RAM: 20 KB
- Flash: 64 KB
- Recommendation: GNSS + Sensor (~600 bytes RAM, ~15 KB flash)

### ESP32
- RAM: 520 KB
- Flash: 4 MB
- Recommendation: All modules if needed (~2 KB RAM, ~35 KB flash)

## Related Documentation

- [Memory Optimization Guide](../../doc/memory-optimization.md)
- [Integration Guide](../../doc/integration-guide.md)
- [FAQ](../../doc/faq.md)
