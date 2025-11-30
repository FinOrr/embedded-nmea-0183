# Serial Port Integration Guide

This guide covers integrating the NMEA parser with serial ports (UART) across different platforms.

## Table of Contents

1. [Overview](#overview)
2. [Common Serial Port Configurations](#common-serial-port-configurations)
3. [Platform-Specific Integration](#platform-specific-integration)
4. [Best Practices](#best-practices)
5. [Troubleshooting](#troubleshooting)

## Overview

NMEA-0183 devices typically output data via RS-232/RS-485 serial connections at standard baud rates. The most common configuration is:

- **Baud rate:** 4800 bps (older devices) or 9600 bps (modern devices)
- **Data bits:** 8
- **Parity:** None
- **Stop bits:** 1
- **Flow control:** None

## Common Serial Port Configurations

### Standard GPS Module

```
Baud: 9600 8N1
Output rate: 1 Hz (1 sentence set per second)
Typical sentences: GGA, RMC, GSA, GSV, GLL, VTG
```

### Marine Electronics

```
Baud: 4800 8N1
Output rate: Varies by device
Multiple talker IDs (GP, II, SD, etc.)
```

### High-Performance GPS

```
Baud: 38400 or 115200 8N1
Output rate: 5-10 Hz
More sentences, higher update rate
```

## Platform-Specific Integration

### Linux / Raspberry Pi / POSIX

#### Opening and Configuring Serial Port

```c
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

int setup_serial_port(const char *device, int baudrate) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("Unable to open serial port");
        return -1;
    }

    struct termios options;
    memset(&options, 0, sizeof(options));

    // Get current options
    tcgetattr(fd, &options);

    // Set baud rate
    speed_t speed;
    switch (baudrate) {
        case 4800:  speed = B4800;  break;
        case 9600:  speed = B9600;  break;
        case 38400: speed = B38400; break;
        case 115200: speed = B115200; break;
        default: speed = B9600; break;
    }
    cfsetispeed(&options, speed);
    cfsetospeed(&options, speed);

    // 8N1 configuration
    options.c_cflag &= ~PARENB;  // No parity
    options.c_cflag &= ~CSTOPB;  // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;      // 8 data bits

    // Enable receiver, ignore modem control lines
    options.c_cflag |= (CLOCAL | CREAD);

    // Raw input mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Raw output mode
    options.c_oflag &= ~OPOST;

    // No flow control
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    // No special character handling
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    // Timeouts
    options.c_cc[VMIN] = 0;   // Non-blocking read
    options.c_cc[VTIME] = 1;  // 0.1 second timeout

    // Apply settings
    tcsetattr(fd, TCSANOW, &options);

    // Flush buffers
    tcflush(fd, TCIOFLUSH);

    return fd;
}
```

#### Reading Data

```c
void read_serial_loop(int fd, nmea_context_t *ctx) {
    char line_buffer[256];
    size_t line_pos = 0;
    char parse_buffer[256];

    while (1) {
        char byte;
        int n = read(fd, &byte, 1);

        if (n > 0) {
            // Add to line buffer
            if (line_pos < sizeof(line_buffer) - 1) {
                line_buffer[line_pos++] = byte;
            }

            // Check for end of sentence
            if (byte == '\n') {
                line_buffer[line_pos] = '\0';

                // Parse complete sentence
                nmea_result_t result = nmea_parse(ctx, line_buffer, line_pos,
                                                  parse_buffer, sizeof(parse_buffer));
                if (result != NMEA_OK) {
                    fprintf(stderr, "Parse error: %s\n", nmea_get_error_string(result));
                }

                // Reset for next sentence
                line_pos = 0;
            }
        } else if (n < 0) {
            perror("Read error");
            break;
        }
    }
}
```

**Complete Example:** `examples/serial_stream/serial_stream_posix.c`

### STM32 (HAL)

#### UART Initialization

```c
/* In main.c or uart setup */
void MX_USART1_UART_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    HAL_UART_Init(&huart1);
}
```

#### Interrupt-Based Reception

```c
/* Global variables */
static char uart_rx_buffer[512];
static volatile uint16_t uart_rx_pos = 0;
static uint8_t uart_rx_byte;

/* In main() after initialization */
void start_uart_reception(void) {
    // Start receiving one byte at a time
    HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);
}

/* Interrupt callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart1) {
        // Add byte to buffer
        uart_rx_buffer[uart_rx_pos++] = uart_rx_byte;

        // Check for complete sentence
        if (uart_rx_byte == '\n' || uart_rx_pos >= sizeof(uart_rx_buffer)) {
            // Set flag for main loop to process
            uart_sentence_ready = 1;
        }

        // Continue receiving
        HAL_UART_Receive_IT(&huart1, &uart_rx_byte, 1);
    }
}

/* In main loop */
void main_loop(void) {
    nmea_context_t ctx;
    char parse_buffer[256];

    while (1) {
        if (uart_sentence_ready) {
            uart_sentence_ready = 0;

            // Parse sentence
            nmea_parse(&ctx, uart_rx_buffer, uart_rx_pos,
                      parse_buffer, sizeof(parse_buffer));

            // Reset buffer
            uart_rx_pos = 0;
        }
    }
}
```

#### DMA-Based Reception

```c
#define RX_BUFFER_SIZE 1024

uint8_t uart_dma_buffer[RX_BUFFER_SIZE];
uint16_t last_pos = 0;

void start_uart_dma(void) {
    // Start DMA in circular mode
    HAL_UART_Receive_DMA(&huart1, uart_dma_buffer, RX_BUFFER_SIZE);
}

void process_uart_dma(nmea_context_t *ctx) {
    // Get current DMA position
    uint16_t curr_pos = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);

    if (curr_pos != last_pos) {
        // Calculate bytes received
        uint16_t bytes_received;
        if (curr_pos > last_pos) {
            bytes_received = curr_pos - last_pos;
        } else {
            bytes_received = RX_BUFFER_SIZE - last_pos + curr_pos;
        }

        // Process new data
        process_nmea_stream(ctx, &uart_dma_buffer[last_pos], bytes_received);

        last_pos = curr_pos;
    }
}
```

### ESP32 (ESP-IDF)

```c
#include "driver/uart.h"

#define UART_NUM UART_NUM_1
#define RX_BUF_SIZE 1024

void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, GPIO_NUM_17, GPIO_NUM_16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}

void uart_rx_task(void *pvParameters) {
    nmea_context_t ctx;
    uint8_t *data = malloc(RX_BUF_SIZE);
    char parse_buffer[256];

    while (1) {
        int len = uart_read_bytes(UART_NUM, data, RX_BUF_SIZE, 100 / portTICK_PERIOD_MS);

        if (len > 0) {
            // Process stream data
            for (int i = 0; i < len; i++) {
                process_nmea_byte(&ctx, data[i], parse_buffer, sizeof(parse_buffer));
            }
        }
    }

    free(data);
}
```

### Arduino

```cpp
#include <nmea.h>

nmea_context_t ctx;
String currentSentence = "";

void setup() {
    // Initialize hardware serial (pins 0/1 on most boards)
    Serial.begin(9600);

    // Or software serial
    // SoftwareSerial gpsSerial(10, 11); // RX, TX
    // gpsSerial.begin(9600);

    // Initialize NMEA parser
    nmea_config_t config = {
        .enabled_modules = (1 << NMEA_MODULE_GNSS),
        .validate_checksums = true
    };
    nmea_init(&ctx, &config);
}

void loop() {
    while (Serial.available()) {
        char c = Serial.read();
        currentSentence += c;

        if (c == '\n') {
            // Parse complete sentence
            char parseBuffer[256];
            nmea_parse(&ctx, currentSentence.c_str(), currentSentence.length(),
                      parseBuffer, sizeof(parseBuffer));

            // Get GPS data
            nmea_gnss_state_t gnss;
            if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
                // Use GPS data
            }

            currentSentence = "";
        }
    }
}
```

### Zephyr RTOS

```c
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_DEVICE_NODE DT_ALIAS(gps_uart)

static const struct device *uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);

void uart_callback(const struct device *dev, struct uart_event *evt, void *user_data) {
    nmea_context_t *ctx = (nmea_context_t *)user_data;

    switch (evt->type) {
    case UART_RX_RDY:
        process_nmea_data(ctx, evt->data.rx.buf + evt->data.rx.offset,
                         evt->data.rx.len);
        break;
    case UART_RX_DISABLED:
        // Re-enable RX
        break;
    }
}

void setup_uart(nmea_context_t *ctx) {
    if (!device_is_ready(uart_dev)) {
        return;
    }

    uart_callback_set(uart_dev, uart_callback, ctx);
}
```

## Best Practices

### 1. Buffer Management

Always use appropriately sized buffers:

```c
// Minimum sentence buffer: 82 bytes (NMEA max)
char sentence_buffer[128];  // Some margin for safety

// Working buffer for parsing
char parse_buffer[256];  // Use nmea_get_required_buffer_size()
```

### 2. Handle Partial Sentences

```c
static char sentence_buffer[128];
static size_t buffer_pos = 0;

void process_serial_byte(char byte) {
    // Buffer overflow protection
    if (buffer_pos >= sizeof(sentence_buffer) - 1) {
        buffer_pos = 0;  // Reset on overflow
    }

    sentence_buffer[buffer_pos++] = byte;

    // Complete sentence received
    if (byte == '\n') {
        sentence_buffer[buffer_pos] = '\0';

        // Only parse if it starts with '$'
        if (sentence_buffer[0] == '$') {
            char parse_buffer[256];
            nmea_parse(&ctx, sentence_buffer, buffer_pos,
                      parse_buffer, sizeof(parse_buffer));
        }

        buffer_pos = 0;
    }
}
```

### 3. Synchronization

If receiving NMEA data in an ISR, use a flag or queue:

```c
volatile bool sentence_ready = false;
char rx_sentence[128];

// In ISR
void uart_isr(void) {
    if (complete_sentence_received) {
        sentence_ready = true;
    }
}

// In main loop
void main_loop(void) {
    if (sentence_ready) {
        sentence_ready = false;
        char parse_buffer[256];
        nmea_parse(&ctx, rx_sentence, strlen(rx_sentence),
                  parse_buffer, sizeof(parse_buffer));
    }
}
```

### 4. Error Recovery

```c
void process_serial_data(char *data, size_t len) {
    nmea_result_t result = nmea_parse(&ctx, data, len, buffer, sizeof(buffer));

    switch (result) {
    case NMEA_OK:
        // Success
        break;

    case NMEA_ERROR_CHECKSUM_FAILED:
        // Log but continue - checksum errors are common
        error_count++;
        break;

    case NMEA_ERROR_INVALID_SENTENCE:
        // Sync may be lost, wait for next sentence
        reset_sentence_buffer();
        break;

    default:
        // Other errors
        log_error(result);
        break;
    }
}
```

## Troubleshooting

### No Data Received

1. Check baud rate matches device
2. Verify TX/RX pins are not swapped
3. Check voltage levels (3.3V vs 5V)
4. Ensure ground is connected

### Garbled Data

1. Verify baud rate configuration
2. Check for electrical noise/interference
3. Verify 8N1 configuration
4. Check cable quality/length

### Intermittent Parsing Errors

1. Check buffer sizes are adequate
2. Verify sentence synchronization logic
3. Look for buffer overflows
4. Check for timing issues in interrupt handlers

### Checksum Failures

1. Electrical noise on the line
2. Baud rate mismatch
3. Cable issues
4. Device sending invalid checksums

**Debug tip:** Temporarily disable checksum validation to isolate the issue:

```c
config.validate_checksums = false;  // For debugging only
```

## Performance Considerations

### UART Baud Rates

At 9600 baud, you receive approximately:
- 960 bytes per second
- 12-15 NMEA sentences per second (typical GPS)

Buffer sizing should account for worst-case burst rates.

### ISR Processing Time

Keep ISR handlers minimal:

```c
// Good: Just set flag
void uart_isr(void) {
    rx_buffer[rx_pos++] = UART_DATA_REG;
    if (UART_DATA_REG == '\n') {
        sentence_ready = true;
    }
}

// Bad: Heavy processing in ISR
void uart_isr(void) {
    // Don't do this!
    nmea_parse(&ctx, ...);
}
```

## Related Documentation

- [Stream Parsing Guide](stream-parsing.md) - Handling continuous data streams
- [Integration Guide](integration-guide.md) - General integration patterns
- [Error Handling](error-handling.md) - Error handling patterns

## Examples

- `examples/serial_stream/serial_stream_posix.c` - Linux/POSIX serial port
- `examples/serial_stream/serial_stream_stm32.c` - STM32 HAL (if available)
