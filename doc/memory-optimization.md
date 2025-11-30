# Memory Optimization Guide

This guide shows how to minimize RAM and flash usage of the NMEA parser library for resource-constrained embedded systems.

## Table of Contents

1. [Memory Usage Overview](#memory-usage-overview)
2. [Compile-Time Optimization](#compile-time-optimization)
3. [Runtime Optimization](#runtime-optimization)
4. [Optimization Examples](#optimization-examples)
5. [Measuring Memory Usage](#measuring-memory-usage)

## Memory Usage Overview

### RAM Usage

The library uses only stack-allocated memory. No heap allocation occurs.

**Main memory consumers:**

1. **Context structure** (`nmea_context_t`)
   - Size depends on enabled modules
   - GNSS only: ~200 bytes
   - All modules: ~1-2 KB

2. **Working buffer** (user-provided)
   - Minimum: 82 bytes (NMEA max sentence length)
   - Recommended: 128-256 bytes

3. **Stream buffers** (user-provided)
   - For serial/UART parsing
   - Typical: 128-512 bytes

**Query actual sizes:**

```c
size_t ctx_size = nmea_get_context_size();
size_t buf_size = nmea_get_required_buffer_size();
printf("Context: %zu bytes, Buffer: %zu bytes\n", ctx_size, buf_size);
```

### Flash Usage

Flash usage scales with enabled features:

| Configuration | Approximate Flash Size |
|---------------|----------------------|
| GNSS only (3 sentences) | ~5-8 KB |
| GNSS + Sensor | ~8-12 KB |
| All modules, all sentences | ~25-35 KB |

## Compile-Time Optimization

The library uses compile-time configuration to completely exclude unused code.

### Step 1: Identify Required Modules

Determine which sentence types you actually need:

```c
// Example: GPS tracker only needs position data
// Required sentences: GGA (position), RMC (recommended minimum)
// Required modules: GNSS only
```

### Step 2: Configure nmea_config.h

Edit `inc/nmea_config.h` to disable unused features:

#### Disable Entire Modules

```c
/* Enable only what you need */
#define NMEA_MODULE_GNSS_ENABLED        1  // Need GPS
#define NMEA_MODULE_AIS_ENABLED         0  // Don't need AIS
#define NMEA_MODULE_NAVIGATION_ENABLED  0  // Don't need navigation
#define NMEA_MODULE_WAYPOINT_ENABLED    0  // Don't need waypoints
#define NMEA_MODULE_HEADING_ENABLED     0  // Don't need heading
#define NMEA_MODULE_SENSOR_ENABLED      0  // Don't need sensors
#define NMEA_MODULE_RADAR_ENABLED       0  // Don't need radar
#define NMEA_MODULE_SAFETY_ENABLED      0  // Don't need safety/alarms
#define NMEA_MODULE_COMM_ENABLED        0  // Don't need comms
#define NMEA_MODULE_SYSTEM_ENABLED      0  // Don't need system msgs
#define NMEA_MODULE_ATTITUDE_ENABLED    0  // Don't need attitude
#define NMEA_MODULE_MISC_ENABLED        0  // Don't need misc
```

#### Disable Specific Sentences

Even within enabled modules, disable unnecessary sentences:

```c
/* GNSS Module - only enable what you need */
#define NMEA_SENTENCE_GGA_ENABLED 1  // Position fix
#define NMEA_SENTENCE_RMC_ENABLED 1  // Recommended minimum
#define NMEA_SENTENCE_GLL_ENABLED 0  // Geographic position - redundant
#define NMEA_SENTENCE_GSA_ENABLED 0  // DOP and active satellites
#define NMEA_SENTENCE_GSV_ENABLED 0  // Satellites in view - large!
#define NMEA_SENTENCE_VTG_ENABLED 0  // Track and ground speed
#define NMEA_SENTENCE_GNS_ENABLED 0  // GNSS fix data
#define NMEA_SENTENCE_GST_ENABLED 0  // Pseudorange error statistics
#define NMEA_SENTENCE_GRS_ENABLED 0  // GNSS range residuals
#define NMEA_SENTENCE_GBS_ENABLED 0  // GNSS satellite fault detection
```

### Step 3: Rebuild

After modifying configuration:

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel  # Optimize for size
make clean
make
```

### Advanced Compiler Optimizations

**For GCC/Clang:**

```makefile
# Optimize for size
CFLAGS += -Os

# Enable link-time optimization
CFLAGS += -flto

# Remove unused functions
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

# Disable frame pointer (saves a register)
CFLAGS += -fomit-frame-pointer
```

**For ARM Thumb:**

```makefile
# Use Thumb-2 instruction set (smaller code)
CFLAGS += -mthumb

# Optimize for size
CFLAGS += -Os

# Short enum types
CFLAGS += -fshort-enums
```

## Runtime Optimization

### Module Bitmask Configuration

Only enable modules you need at runtime:

```c
// Minimal: GNSS only
nmea_config_t config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS),
    .validate_checksums = true
};

// Multiple modules
nmea_config_t config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS) |
                       (1 << NMEA_MODULE_SENSOR),
    .validate_checksums = true
};
```

### Checksum Validation

Disabling checksum validation saves a small amount of processing:

```c
// Production: validate checksums
config.validate_checksums = true;

// If your data source is trusted (e.g., local simulator)
config.validate_checksums = false;  // Saves ~10-20 CPU cycles per sentence
```

### Buffer Reuse

Reuse buffers instead of allocating multiple:

```c
// Good: One buffer reused
char parse_buffer[256];

while (has_data()) {
    nmea_parse(&ctx, sentence, len, parse_buffer, sizeof(parse_buffer));
}

// Bad: Multiple buffers
void func1(void) {
    char buffer1[256];  // Stack usage adds up
    nmea_parse(&ctx, sentence, len, buffer1, sizeof(buffer1));
    func2();
}

void func2(void) {
    char buffer2[256];  // More stack usage
    nmea_parse(&ctx, sentence, len, buffer2, sizeof(buffer2));
}
```

### Static Context Allocation

Use static allocation instead of stack:

```c
// Good for embedded: Static allocation
static nmea_context_t nmea_ctx;
static char parse_buffer[256];

void init(void) {
    nmea_init(&nmea_ctx, &config);
}

// Less optimal: Stack allocation
void func(void) {
    nmea_context_t ctx;  // ~200-2000 bytes on stack
    char buffer[256];
    nmea_init(&ctx, &config);
}
```

## Optimization Examples

### Example 1: Minimal GPS Tracker

**Requirements:** Only position and time from GPS

**Configuration:**

```c
// In nmea_config.h
#define NMEA_MODULE_GNSS_ENABLED 1

#define NMEA_SENTENCE_GGA_ENABLED 1  // Has position, time, altitude
#define NMEA_SENTENCE_RMC_ENABLED 1  // Has date, speed
#define NMEA_SENTENCE_GLL_ENABLED 0
#define NMEA_SENTENCE_GSA_ENABLED 0
#define NMEA_SENTENCE_GSV_ENABLED 0  // Disable - saves ~2KB flash
// ... all other modules disabled
```

**Runtime:**

```c
nmea_config_t config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS),
    .validate_checksums = true,
    .error_callback = NULL  // No callback saves code size
};

nmea_context_t ctx;
nmea_init(&ctx, &config);

// Process sentences
char buffer[128];  // Minimal size
nmea_parse(&ctx, sentence, len, buffer, sizeof(buffer));

// Get only position
nmea_gnss_state_t gnss;
nmea_get_gnss_data(&ctx, &gnss);

float lat = gnss.latitude.degrees;
float lon = gnss.longitude.degrees;
```

**Result:** ~200 bytes RAM, ~5-6 KB flash

### Example 2: Marine Depth Sounder + GPS

**Requirements:** GPS position + water depth

**Configuration:**

```c
// In nmea_config.h
#define NMEA_MODULE_GNSS_ENABLED   1
#define NMEA_MODULE_SENSOR_ENABLED 1

// GNSS sentences
#define NMEA_SENTENCE_GGA_ENABLED 1
#define NMEA_SENTENCE_RMC_ENABLED 1

// Sensor sentences
#define NMEA_SENTENCE_DBT_ENABLED 1  // Depth below transducer
#define NMEA_SENTENCE_DPT_ENABLED 1  // Depth
// ... disable others
```

**Runtime:**

```c
nmea_config_t config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS) |
                       (1 << NMEA_MODULE_SENSOR),
    .validate_checksums = true
};

// Single buffer for both
static char parse_buffer[256];

void process_sentence(const char *sentence, size_t len) {
    nmea_parse(&ctx, sentence, len, parse_buffer, sizeof(parse_buffer));

    // Check both modules
    nmea_gnss_state_t gnss;
    if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
        update_gps_display(gnss.latitude.degrees, gnss.longitude.degrees);
    }

    nmea_sensor_state_t sensor;
    if (nmea_get_sensor_data(&ctx, &sensor) == NMEA_OK) {
        if (sensor.depth_valid) {
            update_depth_display(sensor.depth_meters);
        }
    }
}
```

**Result:** ~350 bytes RAM, ~8-10 KB flash

### Example 3: Ultra-Minimal (Hard-Coded)

For extreme constraints, bypass module system:

```c
// Only parse GGA sentences manually
typedef struct {
    float latitude;
    float longitude;
    uint8_t fix_quality;
} minimal_gps_t;

bool parse_gga_minimal(const char *sentence, minimal_gps_t *gps) {
    // Verify it's GGA
    if (strncmp(sentence + 3, "GGA", 3) != 0) {
        return false;
    }

    // Simple manual parsing (no validation)
    float lat_deg, lon_deg;
    char lat_dir, lon_dir;
    int fix;

    sscanf(sentence,
           "$%*2cGGA,%*f,%f,%c,%f,%c,%d",
           &lat_deg, &lat_dir, &lon_deg, &lon_dir, &fix);

    // Convert DDMM.MMMM to decimal degrees
    gps->latitude = convert_to_decimal(lat_deg, lat_dir);
    gps->longitude = convert_to_decimal(lon_deg, lon_dir);
    gps->fix_quality = fix;

    return true;
}
```

**Result:** ~50 bytes RAM, ~1-2 KB flash (but no validation, error handling, or flexibility)

## Measuring Memory Usage

### RAM Usage

**Check stack usage:**

```c
// GCC: Use stack painting
uint32_t get_stack_usage(void) {
    extern uint32_t _stack_start, _stack_end;
    uint32_t *p = &_stack_start;
    uint32_t count = 0;

    // Count unpainted (used) stack
    while (p < &_stack_end && *p != 0xA5A5A5A5) {
        count++;
        p++;
    }

    return count * sizeof(uint32_t);
}
```

**Runtime queries:**

```c
printf("Context size: %zu bytes\n", nmea_get_context_size());
printf("Required buffer: %zu bytes\n", nmea_get_required_buffer_size());
```

### Flash Usage

**GCC size command:**

```bash
arm-none-eabi-size your_app.elf

# Output:
#    text    data     bss     dec     hex filename
#   12345     100    2048   14493    3897 your_app.elf
#
# text = flash usage (code)
# data + bss = RAM usage (static)
```

**Detailed analysis:**

```bash
# Show size of each object file
arm-none-eabi-size -A libnmea.a

# Show size by section
arm-none-eabi-nm --size-sort --radix=d libnmea.a
```

**Map file analysis:**

```makefile
LDFLAGS += -Wl,-Map=output.map

# Then analyze output.map to see which functions consume most space
```

### Profiling Memory Impact

Create test builds with different configurations:

```bash
# Baseline: All modules
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
make
arm-none-eabi-size libnmea.a > size_all.txt

# Test: GNSS only
# Edit nmea_config.h to disable modules
make clean && make
arm-none-eabi-size libnmea.a > size_gnss.txt

# Compare
diff size_all.txt size_gnss.txt
```

## Memory Budget Planning

### Example Budget for STM32F103 (64KB Flash, 20KB RAM)

```
Application:        30 KB flash, 10 KB RAM
NMEA Library:        8 KB flash,  1 KB RAM
  - Context:                      ~300 bytes
  - Parse buffer:                 ~256 bytes
  - Stream buffer:                ~512 bytes
RTOS:               10 KB flash,  4 KB RAM
Communications:      8 KB flash,  2 KB RAM
Remaining:           8 KB flash,  3 KB RAM (margin)
```

### Optimization Checklist

- [ ] Disabled all unused modules in `nmea_config.h`
- [ ] Disabled unused sentences within enabled modules
- [ ] Used `-Os` compiler flag
- [ ] Enabled link-time optimization (`-flto`)
- [ ] Enabled dead code elimination (`--gc-sections`)
- [ ] Used static allocation for context and buffers
- [ ] Reused parse buffers across calls
- [ ] Measured actual RAM/flash usage
- [ ] Profiled with different configurations

## Related Documentation

- [Integration Guide](integration-guide.md) - General integration patterns
- [FAQ](faq.md) - Common questions about configuration

## Examples

- `examples/minimal_memory/` - Minimal configuration example with measurements
