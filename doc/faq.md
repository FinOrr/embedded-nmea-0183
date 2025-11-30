# Frequently Asked Questions (FAQ)

Common questions and answers about integrating and using the NMEA-0183 parser library.

## General Questions

### Q: What platforms are supported?

**A:** The library is platform-agnostic C code designed for embedded systems. It works on:
- Microcontrollers (ARM Cortex-M, AVR, PIC, RISC-V, etc.)
- Single-board computers (Raspberry Pi, BeagleBone, etc.)
- Desktop systems (Linux, Windows, macOS)
- RTOS environments (FreeRTOS, Zephyr, ThreadX, etc.)

The only requirements are a C99-compliant compiler and standard C library.

### Q: Does this library allocate memory from the heap?

**A:** No. The library uses **zero heap allocation**. All memory is either:
1. Stack-allocated (context structure, buffers)
2. Static/global (if you choose to allocate that way)
3. Caller-provided (buffers passed to functions)

This makes it safe for hard real-time and safety-critical applications.

### Q: How much memory does the library use?

**A:** It depends on your configuration:

**RAM:**
- Minimal (GNSS only, few sentences): ~200-300 bytes
- Typical (GNSS + sensors): ~500-800 bytes
- All modules enabled: ~1-2 KB

**Flash:**
- Minimal (GNSS only, few sentences): ~5-8 KB
- Typical (GNSS + sensors): ~8-15 KB
- All modules enabled: ~25-35 KB

Use `nmea_get_context_size()` to query the actual size for your configuration.

### Q: Can I use this library in a commercial product?

**A:** Yes. The library is MIT licensed, which allows commercial use. See the LICENSE file for details.

## Integration Questions

### Q: How do I integrate this with my UART/serial port?

**A:** See [Serial Port Integration](serial-port-integration.md) for platform-specific examples. Basic pattern:

```c
// 1. Setup UART (9600 baud, 8N1)
// 2. Read bytes into line buffer
// 3. When '\n' received, parse complete sentence
nmea_parse(&ctx, sentence, len, buffer, sizeof(buffer));
```

### Q: How do I handle data that arrives byte-by-byte from UART?

**A:** Use line buffering or stream parsing. See [Stream Parsing Guide](stream-parsing.md). Basic example:

```c
static char line_buf[128];
static size_t pos = 0;

void on_uart_byte(char byte) {
    line_buf[pos++] = byte;
    if (byte == '\n') {
        char parse_buf[256];
        nmea_parse(&ctx, line_buf, pos, parse_buf, sizeof(parse_buf));
        pos = 0;
    }
}
```

### Q: Can I use multiple parser instances (multiple GPS devices)?

**A:** Yes! Create separate contexts for each device:

```c
nmea_context_t gps1_ctx, gps2_ctx;
nmea_init(&gps1_ctx, &config);
nmea_init(&gps2_ctx, &config);

// Parse from different devices
nmea_parse(&gps1_ctx, gps1_sentence, ...);
nmea_parse(&gps2_ctx, gps2_sentence, ...);
```

See `examples/multi_instance/` for a complete example.

### Q: Is the library thread-safe?

**A:** No. Each context is independent but not internally synchronized. You must:
- Use separate contexts for separate threads, OR
- Protect shared contexts with mutexes/locks, OR
- Call parser functions from only one thread

### Q: Can I use this in an interrupt service routine (ISR)?

**A:** **Not recommended** for the main parsing function (`nmea_parse`), as it may take too long. Instead:

**Good approach:**
```c
// In ISR: Just buffer the data
void uart_isr(void) {
    char byte = UART_DATA_REG;
    buffer[pos++] = byte;
    if (byte == '\n') {
        sentence_ready = true;  // Set flag
    }
}

// In main loop: Parse
if (sentence_ready) {
    nmea_parse(&ctx, buffer, pos, parse_buf, sizeof(parse_buf));
    sentence_ready = false;
}
```

## Configuration Questions

### Q: How do I reduce memory usage?

**A:** Edit `inc/nmea_config.h` to disable unused modules and sentences:

```c
// Disable entire modules you don't need
#define NMEA_MODULE_AIS_ENABLED 0
#define NMEA_MODULE_RADAR_ENABLED 0

// Disable specific sentences
#define NMEA_SENTENCE_GSV_ENABLED 0  // Satellites in view
```

Rebuild the library after changes. See [Memory Optimization Guide](memory-optimization.md).

### Q: Which modules/sentences should I enable?

**A:** Enable only what you need:

**For GPS tracking:**
```c
#define NMEA_MODULE_GNSS_ENABLED 1
#define NMEA_SENTENCE_GGA_ENABLED 1  // Position
#define NMEA_SENTENCE_RMC_ENABLED 1  // Speed, date
// Disable others
```

**For marine navigation:**
```c
#define NMEA_MODULE_GNSS_ENABLED 1
#define NMEA_MODULE_SENSOR_ENABLED 1  // Depth, wind
#define NMEA_MODULE_HEADING_ENABLED 1  // Compass
```

### Q: How do I know what sentences my device sends?

**A:** Use a terminal program (minicom, PuTTY, screen) to view raw UART output:

```bash
# Linux
screen /dev/ttyUSB0 9600

# macOS
screen /dev/cu.usbserial 9600
```

Or temporarily enable all modules and check which sentences parse successfully.

## Error Handling Questions

### Q: Why am I getting checksum errors?

**A:** Common causes:
1. **Electrical noise** - Check cables, add filtering caps
2. **Baud rate mismatch** - Verify both sides use same baud rate
3. **Cable quality** - Use shielded cables for long runs
4. **Voltage mismatch** - Check 3.3V vs 5V logic levels

**Temporary debug:**
```c
config.validate_checksums = false;  // For testing only!
```

If errors stop, it's a hardware/electrical issue.

### Q: Why am I getting NMEA_ERROR_UNKNOWN_SENTENCE?

**A:** The sentence type is not compiled in. Either:
1. Enable the sentence in `nmea_config.h`, or
2. Ignore this error (it's informational)

Example:
```c
if (result == NMEA_ERROR_UNKNOWN_SENTENCE) {
    // Ignore - not all sentences are needed
    return;
}
```

### Q: What does NMEA_ERROR_SENTENCE_DISABLED mean?

**A:** The sentence type exists but you disabled it at runtime:

```c
// You enabled the module
config.enabled_modules = (1 << NMEA_MODULE_GNSS);

// But the sentence is disabled in nmea_config.h
#define NMEA_SENTENCE_GLL_ENABLED 0
```

This is expected behavior - you can ignore this error.

### Q: How do I handle lost/corrupted data?

**A:** See [Error Handling Guide](error-handling.md). Basic strategy:

```c
if (result == NMEA_ERROR_CHECKSUM_FAILED) {
    checksum_error_count++;
    return;  // Skip this sentence
}

if (result == NMEA_ERROR_INVALID_SENTENCE) {
    resync_stream();  // Might have lost synchronization
    return;
}
```

## Parsing Questions

### Q: How do I get the latest GPS position?

**A:**

```c
nmea_gnss_state_t gnss;
if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
    if (gnss.latitude.valid && gnss.longitude.valid) {
        float lat = gnss.latitude.degrees;
        float lon = gnss.longitude.degrees;
        printf("Position: %.6f, %.6f\n", lat, lon);
    }
}
```

### Q: How do I know if the GPS has a valid fix?

**A:** Check the fix quality field:

```c
nmea_gnss_state_t gnss;
nmea_get_gnss_data(&ctx, &gnss);

if (gnss.fix_quality > 0) {
    // Valid fix
    // fix_quality: 0=invalid, 1=GPS, 2=DGPS, 4=RTK, etc.
} else {
    // No fix
}
```

### Q: Why is the data not updating?

**A:** Possible causes:
1. GPS has no fix (check `fix_quality`)
2. Not parsing the right sentences (GGA/RMC contain position)
3. Module not enabled in configuration
4. Data validity flags are false

**Debug:**
```c
nmea_result_t result = nmea_parse(&ctx, sentence, len, buf, sizeof(buf));
printf("Parse result: %s\n", nmea_get_error_string(result));

nmea_gnss_state_t gnss;
if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
    printf("Lat valid: %d, Lon valid: %d\n",
           gnss.latitude.valid, gnss.longitude.valid);
}
```

### Q: How do I get the timestamp from GPS?

**A:**

```c
nmea_gnss_state_t gnss;
if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
    if (gnss.utc_time.valid) {
        printf("Time: %02d:%02d:%02d UTC\n",
               gnss.utc_time.hour,
               gnss.utc_time.minute,
               gnss.utc_time.second);
    }

    if (gnss.utc_date.valid) {
        printf("Date: %04d-%02d-%02d\n",
               gnss.utc_date.year,
               gnss.utc_date.month,
               gnss.utc_date.day);
    }
}
```

### Q: How do I convert lat/lon to other formats?

**A:** The library provides decimal degrees. Convert as needed:

```c
// Decimal degrees to degrees/minutes/seconds
void decimal_to_dms(float decimal, int *deg, int *min, float *sec) {
    *deg = (int)decimal;
    float remainder = (decimal - *deg) * 60.0;
    *min = (int)remainder;
    *sec = (remainder - *min) * 60.0;
}

// Decimal degrees to degrees and decimal minutes
void decimal_to_dm(float decimal, int *deg, float *min) {
    *deg = (int)decimal;
    *min = (decimal - *deg) * 60.0;
}
```

## Performance Questions

### Q: How fast can I parse sentences?

**A:** The parser can typically handle:
- Simple sentences (GGA, RMC): ~50-100 µs per sentence (on ARM Cortex-M4 @ 168MHz)
- Complex sentences (GSV): ~100-200 µs
- Most GPS output rates (1-10 Hz) are easily supported

### Q: Can this handle high-rate GPS (10 Hz, 20 Hz)?

**A:** Yes, but consider:
- Buffer sizing for burst data
- Processing time in your main loop
- Use DMA for UART reception if possible

See [Stream Parsing Guide](stream-parsing.md) for buffering strategies.

### Q: Does checksum validation impact performance?

**A:** Minimally (~10-20 CPU cycles per sentence). Only disable for debugging, not production.

## Troubleshooting

### Q: Compiler error: "nmea_config.h: No such file or directory"

**A:** Include paths not set correctly. Add the include directory:

```makefile
CFLAGS += -I/path/to/embedded-nmea-0183/inc
```

### Q: Linker error: "undefined reference to nmea_init"

**A:** Library not linked. Add to link flags:

```makefile
LDFLAGS += -L/path/to/build -lnmea
```

### Q: Getting garbage data / wrong values

**A:** Check:
1. Baud rate matches device
2. Checksum validation is enabled (catches bad data)
3. TX/RX pins not swapped
4. UART parity/stop bits configured correctly (should be 8N1)

### Q: Parser seems to skip sentences

**A:** Possible causes:
1. Sentence type disabled in configuration
2. Module not enabled at runtime
3. Buffer overflow (sentences too long)

Enable error callback to see what's happening:

```c
void error_cb(nmea_error_type_t type, nmea_result_t code,
              const char *msg, void *user_data) {
    printf("Error: %s\n", msg);
}

config.error_callback = error_cb;
```

### Q: Building fails with "multiple definition" errors

**A:** Check if you're including `.c` files instead of `.h` files. Only include `nmea.h`:

```c
#include "nmea.h"  // Correct

// Don't do this:
// #include "src/nmea.c"  // Wrong!
```

## Advanced Questions

### Q: Can I parse NMEA-2000 messages?

**A:** No. This library is specifically for NMEA-0183 (text-based). NMEA-2000 uses a different binary protocol (CAN bus).

### Q: Can I parse AIS messages?

**A:** Yes, if the AIS module is enabled:

```c
#define NMEA_MODULE_AIS_ENABLED 1
```

AIS messages are encapsulated in NMEA-0183 format (VDM, VDO, ABM, BBM sentences).

### Q: How do I implement a custom sentence parser?

**A:** The library doesn't currently support runtime-registered custom parsers. You would need to:
1. Modify the library source code to add your sentence type, or
2. Parse custom sentences separately before/after the main parser

### Q: Can I modify the library?

**A:** Yes! It's MIT licensed. Fork it, modify it, use it commercially. Contributions back to the project are welcome.

## Getting Help

### Q: Where do I report bugs?

**A:** File issues at: https://github.com/FinOrr/embedded-nmea-0183/issues

Include:
- Platform/MCU
- Compiler version
- NMEA sentence that fails
- Error messages/logs

### Q: How do I get support?

**A:**
1. Check this FAQ
2. Read the documentation in `doc/`
3. Look at examples in `examples/`
4. File an issue on GitHub

### Q: Can I contribute?

**A:** Yes! Contributions are welcome:
- Bug fixes
- New sentence types
- Documentation improvements
- Examples for new platforms

## Related Documentation

- [Integration Guide](integration-guide.md) - Getting started
- [Serial Port Integration](serial-port-integration.md) - UART/serial specifics
- [Stream Parsing](stream-parsing.md) - Handling continuous data
- [Memory Optimization](memory-optimization.md) - Reducing memory usage
- [Error Handling](error-handling.md) - Error handling patterns
- [Supported Sentences](supported-sentences.md) - List of all sentence types
