# Integration Guide

This guide helps you integrate the NMEA-0183 parser library into your embedded project.

## Table of Contents

1. [Quick Integration](#quick-integration)
2. [Build System Integration](#build-system-integration)
3. [Common Integration Patterns](#common-integration-patterns)
4. [Platform-Specific Considerations](#platform-specific-considerations)
5. [Next Steps](#next-steps)

## Quick Integration

### Step 1: Build and Install the Library

```bash
mkdir build && cd build
cmake ..
make
make install  # May require sudo
```

Or specify a custom install prefix:

```bash
cmake -DCMAKE_INSTALL_PREFIX=/path/to/your/project/libs ..
make install
```

### Step 2: Include in Your Project

**Using CMake:**

```cmake
# Find the library
find_library(NMEA_LIB nmea PATHS /path/to/install/lib)
find_path(NMEA_INCLUDE nmea.h PATHS /path/to/install/include)

# Add to your target
target_link_libraries(your_target ${NMEA_LIB})
target_include_directories(your_target PRIVATE ${NMEA_INCLUDE})
```

**Using Makefile:**

```makefile
CFLAGS += -I/path/to/install/include
LDFLAGS += -L/path/to/install/lib -lnmea
```

**Manual Integration:**

Copy the library files to your project:
```bash
cp build/libnmea.a your_project/libs/
cp -r inc/ your_project/include/nmea/
```

### Step 3: Basic Usage

```c
#include "nmea.h"

nmea_context_t ctx;
nmea_config_t config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS),
    .validate_checksums = true,
    .error_callback = NULL
};

// Initialize
nmea_init(&ctx, &config);

// Parse sentence
char buffer[256];
const char *sentence = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";
nmea_parse(&ctx, sentence, strlen(sentence), buffer, sizeof(buffer));

// Get data
nmea_gnss_state_t gnss;
if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
    printf("Lat: %.6f, Lon: %.6f\n", gnss.latitude.degrees, gnss.longitude.degrees);
}

// Cleanup
nmea_cleanup(&ctx);
```

## Build System Integration

### CMake (Recommended)

**Option 1: Install system-wide**

```cmake
find_package(NMEA QUIET)
if(NOT NMEA_FOUND)
    find_library(NMEA_LIB nmea)
    find_path(NMEA_INCLUDE nmea.h)
endif()

target_link_libraries(your_target ${NMEA_LIB})
target_include_directories(your_target PRIVATE ${NMEA_INCLUDE})
```

**Option 2: Submodule in your project**

```cmake
# Add as subdirectory
add_subdirectory(external/embedded-nmea-0183)

# Link to your target
target_link_libraries(your_target nmea)
```

### Makefile

```makefile
# Configuration
NMEA_DIR = /path/to/embedded-nmea-0183
NMEA_LIB = $(NMEA_DIR)/build/libnmea.a
NMEA_INC = $(NMEA_DIR)/inc

# Compiler flags
CFLAGS += -I$(NMEA_INC)
LDFLAGS += $(NMEA_LIB)

# Your target
your_program: main.o $(NMEA_LIB)
	$(CC) -o $@ $^ $(LDFLAGS)

# Build NMEA library if needed
$(NMEA_LIB):
	cd $(NMEA_DIR) && mkdir -p build && cd build && cmake .. && make
```

### PlatformIO (Arduino/ESP32/STM32)

Create `platformio.ini`:

```ini
[env:your_board]
platform = your_platform
board = your_board
framework = arduino

lib_deps =
    file:///path/to/embedded-nmea-0183

build_flags =
    -I path/to/embedded-nmea-0183/inc
```

### Arduino IDE

1. Copy the library to your Arduino libraries folder:
   ```bash
   cp -r embedded-nmea-0183 ~/Arduino/libraries/
   ```

2. Restart Arduino IDE

3. Include in your sketch:
   ```cpp
   #include <nmea.h>
   ```

### Bare Metal / Custom Toolchain

```makefile
# Toolchain configuration
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
CFLAGS = -mcpu=cortex-m4 -mthumb -Os

# NMEA library sources
NMEA_DIR = lib/embedded-nmea-0183
NMEA_SRCS = $(wildcard $(NMEA_DIR)/src/*.c) \
            $(wildcard $(NMEA_DIR)/src/modules/*.c)
NMEA_OBJS = $(NMEA_SRCS:.c=.o)

# Build NMEA library
libnmea.a: $(NMEA_OBJS)
	$(AR) rcs $@ $^

# Your application
your_app: main.o libnmea.a
	$(CC) $(CFLAGS) -o $@ $^
```

## Common Integration Patterns

### Pattern 1: Single GPS Device

Most common use case - reading GPS coordinates from a UART-connected GPS module.

```c
nmea_config_t config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS),
    .validate_checksums = true,
    .error_callback = my_error_handler
};
```

**See:** `examples/simple_parser.c`

### Pattern 2: Multiple Data Sources

Parsing data from multiple NMEA devices (GPS + depth sounder + wind sensor).

```c
// Create separate contexts for each device
nmea_context_t gps_ctx, sensor_ctx;

nmea_config_t gps_config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS)
};

nmea_config_t sensor_config = {
    .enabled_modules = (1 << NMEA_MODULE_SENSOR)
};

nmea_init(&gps_ctx, &gps_config);
nmea_init(&sensor_ctx, &sensor_config);
```

**See:** `examples/multi_instance/`

### Pattern 3: Stream Parsing

Handling incomplete NMEA sentences from a continuous data stream.

```c
char stream_buffer[512];
size_t buffer_pos = 0;

// On data received
void uart_rx_handler(char byte) {
    stream_buffer[buffer_pos++] = byte;

    // Check for complete sentence
    if (byte == '\n') {
        parse_sentence(stream_buffer, buffer_pos);
        buffer_pos = 0;
    }
}
```

**See:** `examples/serial_stream/` and `doc/stream-parsing.md`

### Pattern 4: Memory-Constrained Systems

Minimizing memory usage for deeply embedded systems.

```c
// Configure only what you need in inc/nmea_config.h
#define NMEA_MODULE_GNSS_ENABLED 1
#define NMEA_MODULE_AIS_ENABLED 0
#define NMEA_MODULE_SENSOR_ENABLED 0
// ... disable unused modules

#define NMEA_SENTENCE_GGA_ENABLED 1  // Keep
#define NMEA_SENTENCE_GSV_ENABLED 0  // Disable satellites in view
```

**See:** `examples/minimal_memory/` and `doc/memory-optimization.md`

## Platform-Specific Considerations

### STM32 (HAL/LL)

```c
// UART receive interrupt
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    static char rx_buffer[128];
    static uint8_t rx_index = 0;

    uint8_t byte;
    HAL_UART_Receive_IT(huart, &byte, 1);

    rx_buffer[rx_index++] = byte;

    if (byte == '\n') {
        parse_nmea_sentence(rx_buffer, rx_index);
        rx_index = 0;
    }
}
```

**See:** `doc/serial-port-integration.md`

### ESP32 (ESP-IDF)

```c
// UART task
void uart_rx_task(void *pvParameters) {
    uart_event_t event;
    uint8_t *data = malloc(1024);

    while (1) {
        if (xQueueReceive(uart_queue, &event, portMAX_DELAY)) {
            if (event.type == UART_DATA) {
                int len = uart_read_bytes(UART_NUM, data, event.size, 100);
                process_nmea_stream(data, len);
            }
        }
    }
}
```

**See:** `doc/rtos-integration.md`

### Raspberry Pi / Linux

```c
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
struct termios options;
tcgetattr(fd, &options);
cfsetispeed(&options, B9600);
options.c_cflag |= (CLOCAL | CREAD);
tcsetattr(fd, TCSANOW, &options);

// Read loop
char buffer[256];
int n = read(fd, buffer, sizeof(buffer));
```

**See:** `examples/serial_stream/serial_stream_posix.c`

### FreeRTOS

```c
void nmea_parser_task(void *pvParameters) {
    nmea_context_t ctx;
    // ... initialize

    while (1) {
        // Wait for data from queue
        uart_data_t data;
        if (xQueueReceive(uart_queue, &data, portMAX_DELAY)) {
            process_nmea_data(&ctx, &data);
        }
    }
}

// Create task
xTaskCreate(nmea_parser_task, "NMEA", 2048, NULL, 5, NULL);
```

**See:** `doc/rtos-integration.md`

## Next Steps

1. **Choose your integration pattern** from the examples above
2. **Configure modules** in `inc/nmea_config.h` to minimize memory usage
3. **Implement error handling** - see `doc/error-handling.md`
4. **Handle stream parsing** - see `doc/stream-parsing.md`
5. **Optimize memory usage** - see `doc/memory-optimization.md`

## Related Documentation

- [Stream Parsing Guide](stream-parsing.md) - Handling continuous NMEA data streams
- [Serial Port Integration](serial-port-integration.md) - UART/serial port specifics
- [Memory Optimization](memory-optimization.md) - Reducing RAM/flash usage
- [Error Handling](error-handling.md) - Error handling patterns
- [RTOS Integration](rtos-integration.md) - Using with FreeRTOS, Zephyr, etc.
- [FAQ](faq.md) - Frequently asked questions

## Examples

- `examples/simple_parser.c` - Basic usage
- `examples/sensor_test.c` - Sensor module parsing
- `examples/serial_stream/` - Serial port stream parsing
- `examples/multi_instance/` - Multiple parser contexts
- `examples/minimal_memory/` - Minimal memory configuration
- `examples/comprehensive_error_handling/` - Advanced error handling
