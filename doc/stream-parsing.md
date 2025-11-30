# Stream Parsing Guide

This guide covers parsing NMEA data from continuous streams (UART, files, network) where sentences may arrive incomplete or fragmented.

## Table of Contents

1. [Understanding the Problem](#understanding-the-problem)
2. [Stream Buffering Patterns](#stream-buffering-patterns)
3. [Implementation Examples](#implementation-examples)
4. [Edge Cases](#edge-cases)
5. [Best Practices](#best-practices)

## Understanding the Problem

### Why Stream Parsing is Needed

NMEA devices send continuous data streams. Unlike file parsing where complete sentences are available, stream parsing must handle:

1. **Partial sentences** - Data arrives byte-by-byte or in chunks
2. **Multiple sentences** - Buffer may contain several complete sentences
3. **Sentence boundaries** - Need to detect start ('$') and end ('\n')
4. **Buffer overflows** - Need to handle malformed or excessively long data
5. **Synchronization** - Recovering from data loss or corruption

### NMEA Sentence Structure

```
$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n
^                                                                 ^^
|                                                                 ||
Start                                                             End
```

- Starts with `$` or `!`
- Fields separated by `,`
- Ends with optional `*` + checksum
- Terminated with `\r\n` (CR+LF)
- Maximum length: 82 characters (NMEA standard)

## Stream Buffering Patterns

### Pattern 1: Line-Based Buffering (Recommended)

Best for most use cases. Buffer incoming bytes until a complete line is received.

```c
typedef struct {
    char buffer[128];      // Sentence buffer (>82 bytes for safety)
    size_t pos;           // Current position
    bool overflow;        // Overflow flag
} line_buffer_t;

void line_buffer_init(line_buffer_t *lb) {
    lb->pos = 0;
    lb->overflow = false;
}

bool line_buffer_add_byte(line_buffer_t *lb, char byte) {
    // Check for overflow
    if (lb->pos >= sizeof(lb->buffer) - 1) {
        lb->overflow = true;
        lb->pos = 0;  // Reset on overflow
        return false;
    }

    // Add byte to buffer
    lb->buffer[lb->pos++] = byte;

    // Check for complete sentence
    if (byte == '\n') {
        lb->buffer[lb->pos] = '\0';  // Null-terminate
        return true;  // Sentence complete
    }

    return false;  // Sentence incomplete
}

// Usage
line_buffer_t lb;
line_buffer_init(&lb);

void on_uart_byte_received(char byte) {
    if (line_buffer_add_byte(&lb, byte)) {
        // Parse complete sentence
        char parse_buffer[256];
        nmea_parse(&ctx, lb.buffer, lb.pos, parse_buffer, sizeof(parse_buffer));

        // Reset for next sentence
        line_buffer_init(&lb);
    }
}
```

### Pattern 2: Circular Buffer

Best for high-throughput applications or when data arrives in bursts.

```c
#define CIRC_BUF_SIZE 512

typedef struct {
    char buffer[CIRC_BUF_SIZE];
    volatile size_t head;  // Write position
    volatile size_t tail;  // Read position
} circular_buffer_t;

void circ_buf_init(circular_buffer_t *cb) {
    cb->head = 0;
    cb->tail = 0;
}

bool circ_buf_put(circular_buffer_t *cb, char byte) {
    size_t next_head = (cb->head + 1) % CIRC_BUF_SIZE;

    if (next_head == cb->tail) {
        return false;  // Buffer full
    }

    cb->buffer[cb->head] = byte;
    cb->head = next_head;
    return true;
}

bool circ_buf_get(circular_buffer_t *cb, char *byte) {
    if (cb->head == cb->tail) {
        return false;  // Buffer empty
    }

    *byte = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % CIRC_BUF_SIZE;
    return true;
}

size_t circ_buf_available(circular_buffer_t *cb) {
    if (cb->head >= cb->tail) {
        return cb->head - cb->tail;
    } else {
        return CIRC_BUF_SIZE - cb->tail + cb->head;
    }
}

// Usage: ISR writes to buffer
void uart_isr(void) {
    char byte = UART_DATA_REG;
    circ_buf_put(&uart_buffer, byte);
}

// Main loop reads and processes
void process_stream(void) {
    static char sentence[128];
    static size_t pos = 0;

    char byte;
    while (circ_buf_get(&uart_buffer, &byte)) {
        sentence[pos++] = byte;

        if (byte == '\n' || pos >= sizeof(sentence) - 1) {
            sentence[pos] = '\0';
            char parse_buffer[256];
            nmea_parse(&ctx, sentence, pos, parse_buffer, sizeof(parse_buffer));
            pos = 0;
        }
    }
}
```

### Pattern 3: DMA Double Buffering

Best for high-speed UART with DMA support.

```c
#define DMA_BUF_SIZE 256

uint8_t dma_buffer_0[DMA_BUF_SIZE];
uint8_t dma_buffer_1[DMA_BUF_SIZE];
static uint8_t *active_buffer = dma_buffer_0;
static uint16_t last_pos = 0;

void process_dma_buffer(uint8_t *buffer, uint16_t start, uint16_t end) {
    static char sentence[128];
    static size_t sentence_pos = 0;

    for (uint16_t i = start; i < end; i++) {
        char byte = buffer[i];

        if (sentence_pos < sizeof(sentence) - 1) {
            sentence[sentence_pos++] = byte;
        }

        if (byte == '\n') {
            sentence[sentence_pos] = '\0';
            char parse_buffer[256];
            nmea_parse(&ctx, sentence, sentence_pos,
                      parse_buffer, sizeof(parse_buffer));
            sentence_pos = 0;
        }
    }
}

// DMA half-transfer callback
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
    process_dma_buffer(active_buffer, 0, DMA_BUF_SIZE / 2);
}

// DMA complete callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    process_dma_buffer(active_buffer, DMA_BUF_SIZE / 2, DMA_BUF_SIZE);
}
```

### Pattern 4: RTOS Queue-Based

Best for RTOS environments with dedicated parser task.

```c
#include "FreeRTOS.h"
#include "queue.h"

#define QUEUE_LENGTH 10
#define SENTENCE_MAX_LEN 128

typedef struct {
    char data[SENTENCE_MAX_LEN];
    size_t length;
} nmea_sentence_t;

QueueHandle_t nmea_queue;

void uart_init(void) {
    // Create queue
    nmea_queue = xQueueCreate(QUEUE_LENGTH, sizeof(nmea_sentence_t));
}

// UART ISR or callback
void uart_rx_callback(void) {
    static char sentence[SENTENCE_MAX_LEN];
    static size_t pos = 0;

    char byte = uart_read_byte();

    if (pos < sizeof(sentence) - 1) {
        sentence[pos++] = byte;
    }

    if (byte == '\n') {
        nmea_sentence_t msg;
        memcpy(msg.data, sentence, pos);
        msg.length = pos;

        // Send to queue (from ISR)
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(nmea_queue, &msg, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        pos = 0;
    }
}

// Parser task
void nmea_parser_task(void *pvParameters) {
    nmea_context_t ctx;
    nmea_sentence_t sentence;
    char parse_buffer[256];

    // Initialize parser
    nmea_config_t config = { /* ... */ };
    nmea_init(&ctx, &config);

    while (1) {
        // Wait for sentence from queue
        if (xQueueReceive(nmea_queue, &sentence, portMAX_DELAY)) {
            nmea_parse(&ctx, sentence.data, sentence.length,
                      parse_buffer, sizeof(parse_buffer));

            // Process parsed data
            nmea_gnss_state_t gnss;
            if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
                // Use GPS data
            }
        }
    }
}
```

## Implementation Examples

### Complete Byte-by-Byte Parser

```c
typedef struct {
    char sentence_buf[128];
    size_t pos;
    bool synced;
    uint32_t error_count;
    uint32_t sentence_count;
} nmea_stream_parser_t;

void nmea_stream_init(nmea_stream_parser_t *parser) {
    parser->pos = 0;
    parser->synced = false;
    parser->error_count = 0;
    parser->sentence_count = 0;
}

nmea_result_t nmea_stream_process_byte(
    nmea_stream_parser_t *stream,
    nmea_context_t *ctx,
    char byte,
    char *parse_buffer,
    size_t parse_buffer_size
) {
    // Check for sentence start
    if (byte == '$' || byte == '!') {
        stream->pos = 0;
        stream->synced = true;
    }

    // If not synced, ignore bytes until we find start
    if (!stream->synced) {
        return NMEA_ERROR_INVALID_SENTENCE;
    }

    // Add byte to buffer
    if (stream->pos < sizeof(stream->sentence_buf) - 1) {
        stream->sentence_buf[stream->pos++] = byte;
    } else {
        // Buffer overflow - reset and resync
        stream->pos = 0;
        stream->synced = false;
        stream->error_count++;
        return NMEA_ERROR_BUFFER_TOO_SMALL;
    }

    // Check for sentence end
    if (byte == '\n') {
        stream->sentence_buf[stream->pos] = '\0';

        // Parse complete sentence
        nmea_result_t result = nmea_parse(
            ctx,
            stream->sentence_buf,
            stream->pos,
            parse_buffer,
            parse_buffer_size
        );

        if (result == NMEA_OK) {
            stream->sentence_count++;
        } else {
            stream->error_count++;
        }

        // Reset for next sentence
        stream->pos = 0;
        stream->synced = false;

        return result;
    }

    return NMEA_ERROR_NO_DATA;  // Sentence incomplete
}
```

### Block Processing (Chunk-Based)

For when data arrives in blocks (e.g., from file or network):

```c
void nmea_process_block(
    nmea_context_t *ctx,
    const char *data,
    size_t length
) {
    static char sentence[128];
    static size_t sentence_pos = 0;
    char parse_buffer[256];

    for (size_t i = 0; i < length; i++) {
        char byte = data[i];

        // Start of new sentence
        if (byte == '$' || byte == '!') {
            sentence_pos = 0;
        }

        // Add to sentence buffer
        if (sentence_pos < sizeof(sentence) - 1) {
            sentence[sentence_pos++] = byte;
        }

        // End of sentence
        if (byte == '\n') {
            sentence[sentence_pos] = '\0';

            nmea_parse(ctx, sentence, sentence_pos,
                      parse_buffer, sizeof(parse_buffer));

            sentence_pos = 0;
        }
    }
}
```

## Edge Cases

### 1. Synchronization Loss

Handle corrupted data or noise:

```c
void handle_sync_loss(nmea_stream_parser_t *stream) {
    // Reset parser state
    stream->pos = 0;
    stream->synced = false;

    // Wait for next '$' to resync
}

// Detect sync loss
if (consecutive_errors > 5) {
    handle_sync_loss(&stream);
}
```

### 2. Buffer Overflow

```c
if (stream->pos >= sizeof(stream->sentence_buf) - 1) {
    // Overflow detected - discard and resync
    log_error("Buffer overflow at position %zu", stream->pos);
    stream->pos = 0;
    stream->synced = false;
}
```

### 3. Incomplete Sentences at End of Block

```c
typedef struct {
    char remainder[128];
    size_t remainder_len;
} stream_state_t;

void process_block_with_remainder(
    stream_state_t *state,
    const char *data,
    size_t len
) {
    // Combine remainder from last block with new data
    char combined[256];
    size_t combined_len = 0;

    if (state->remainder_len > 0) {
        memcpy(combined, state->remainder, state->remainder_len);
        combined_len = state->remainder_len;
    }

    memcpy(combined + combined_len, data, len);
    combined_len += len;

    // Process combined data
    size_t last_newline = 0;
    for (size_t i = 0; i < combined_len; i++) {
        if (combined[i] == '\n') {
            last_newline = i + 1;
        }
    }

    // Process complete sentences
    if (last_newline > 0) {
        process_sentences(combined, last_newline);
    }

    // Save remainder
    state->remainder_len = combined_len - last_newline;
    if (state->remainder_len > 0) {
        memcpy(state->remainder, combined + last_newline, state->remainder_len);
    }
}
```

### 4. Rapid Data Arrival

```c
// Rate limiting / backpressure
#define MAX_SENTENCES_PER_SECOND 100

static uint32_t last_second = 0;
static uint32_t sentences_this_second = 0;

bool should_process_sentence(void) {
    uint32_t current_second = get_system_time_seconds();

    if (current_second != last_second) {
        last_second = current_second;
        sentences_this_second = 0;
    }

    if (sentences_this_second >= MAX_SENTENCES_PER_SECOND) {
        return false;  // Drop sentence
    }

    sentences_this_second++;
    return true;
}
```

## Best Practices

### 1. Always Validate Sentence Start

```c
// Good: Check for sentence start
if (buffer[0] == '$' || buffer[0] == '!') {
    nmea_parse(&ctx, buffer, len, parse_buf, sizeof(parse_buf));
}

// Bad: Parse without checking
nmea_parse(&ctx, buffer, len, parse_buf, sizeof(parse_buf));
```

### 2. Use Timeouts for Incomplete Sentences

```c
#define SENTENCE_TIMEOUT_MS 1000

typedef struct {
    char buffer[128];
    size_t pos;
    uint32_t last_byte_time;
} timed_buffer_t;

void check_timeout(timed_buffer_t *tb) {
    uint32_t now = get_milliseconds();

    if (tb->pos > 0 && (now - tb->last_byte_time) > SENTENCE_TIMEOUT_MS) {
        // Timeout - discard incomplete sentence
        tb->pos = 0;
    }
}
```

### 3. Monitor Buffer Health

```c
typedef struct {
    uint32_t sentences_parsed;
    uint32_t checksum_errors;
    uint32_t buffer_overflows;
    uint32_t sync_losses;
} stream_stats_t;

void print_stats(stream_stats_t *stats) {
    float error_rate = (float)stats->checksum_errors / stats->sentences_parsed * 100.0f;
    printf("Sentences: %lu, Errors: %.2f%%\n",
           stats->sentences_parsed, error_rate);
}
```

### 4. Use Appropriate Buffer Sizes

```c
// NMEA max: 82 bytes
// Add margin for safety
#define SENTENCE_BUFFER_SIZE 128

// Parse buffer should be at least nmea_get_required_buffer_size()
#define PARSE_BUFFER_SIZE 256
```

## Performance Considerations

### Typical Data Rates

| Device Type | Baud Rate | Sentences/sec | Bytes/sec |
|-------------|-----------|---------------|-----------|
| Standard GPS | 4800 | 5-10 | ~400-800 |
| Modern GPS | 9600 | 5-15 | ~500-1200 |
| High-rate GPS | 38400 | 20-50 | ~2000-5000 |

### Buffer Sizing

```c
// For 9600 baud GPS (worst case: 1200 bytes/sec)
// With 100ms processing interval:
#define STREAM_BUFFER_SIZE 256  // 120 bytes/100ms + margin

// For 115200 baud multi-device (14400 bytes/sec)
// With 100ms processing interval:
#define STREAM_BUFFER_SIZE 2048  // 1440 bytes/100ms + margin
```

## Related Documentation

- [Serial Port Integration](serial-port-integration.md) - UART-specific details
- [Error Handling](error-handling.md) - Handling parse errors
- [Integration Guide](integration-guide.md) - General integration patterns

## Examples

- `examples/serial_stream/` - Complete stream parsing examples
- `examples/simple_parser.c` - Basic sentence-by-sentence parsing
