# Error Handling Guide

Error handling patterns for NMEA parser applications.

## Table of Contents

1. [Error Types](#error-types)
2. [Error Handling Strategies](#error-handling-strategies)
3. [Error Callbacks](#error-callbacks)
4. [Recovery Strategies](#recovery-strategies)
5. [Common Patterns](#common-patterns)

## Error Types

### Result Codes

All parser functions return `nmea_result_t`:

```c
typedef enum {
    NMEA_OK                     = 0,    // Success
    NMEA_ERROR_NULL_PARAM       = -1,   // Null parameter
    NMEA_ERROR_INVALID_CONTEXT  = -2,   // Context not initialized
    NMEA_ERROR_INVALID_CONFIG   = -3,   // Bad configuration
    NMEA_ERROR_BUFFER_TOO_SMALL = -4,   // Buffer insufficient
    NMEA_ERROR_INVALID_SENTENCE = -5,   // Malformed sentence
    NMEA_ERROR_CHECKSUM_FAILED  = -6,   // Checksum mismatch
    NMEA_ERROR_UNKNOWN_SENTENCE = -7,   // Unknown sentence type
    NMEA_ERROR_SENTENCE_DISABLED = -8,  // Sentence disabled
    NMEA_ERROR_MODULE_DISABLED  = -9,   // Module disabled
    NMEA_ERROR_TOO_MANY_FIELDS  = -10,  // Too many fields
    NMEA_ERROR_TOO_FEW_FIELDS   = -11,  // Too few fields
    NMEA_ERROR_PARSE_FAILED     = -12,  // Generic parse error
    NMEA_ERROR_NO_DATA          = -13,  // No data available
    NMEA_ERROR_ALREADY_INIT     = -14,  // Already initialized
    NMEA_ERROR_NOT_INIT         = -15,  // Not initialized
} nmea_result_t;
```

### Error Classification

| Error Type                     | Severity      | Action               |
|--------------------------------|---------------|----------------------|
| `NMEA_ERROR_NULL_PARAM`        | Fatal         | Fix code             |
| `NMEA_ERROR_INVALID_CONTEXT`   | Fatal         | Fix code             |
| `NMEA_ERROR_INVALID_CONFIG`    | Fatal         | Fix configuration    |
| `NMEA_ERROR_BUFFER_TOO_SMALL`  | Fatal         | Increase buffer size |
| `NMEA_ERROR_CHECKSUM_FAILED`   | Recoverable   | Log and continue     |
| `NMEA_ERROR_INVALID_SENTENCE`  | Recoverable   | Resync stream        |
| `NMEA_ERROR_UNKNOWN_SENTENCE`  | Informational | Ignore               |
| `NMEA_ERROR_SENTENCE_DISABLED` | Informational | Expected             |
| `NMEA_ERROR_PARSE_FAILED`      | Recoverable   | Log and continue     |
| `NMEA_ERROR_NO_DATA`           | Informational | No action needed     |

## Error Handling Strategies

### Strategy 1: Basic Error Checking

Minimum viable error handling:

```c
nmea_result_t result = nmea_parse(&ctx, sentence, len, buffer, sizeof(buffer));

if (result != NMEA_OK) {
    // Log error
    fprintf(stderr, "Parse error: %s\n", nmea_get_error_string(result));

    // Continue processing - don't crash on bad data
    return;
}

// Use parsed data
nmea_gnss_state_t gnss;
if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
    process_gps_data(&gnss);
}
```

### Strategy 2: Categorized Error Handling

Different actions for different error types:

```c
nmea_result_t result = nmea_parse(&ctx, sentence, len, buffer, sizeof(buffer));

switch (result) {
    case NMEA_OK:
        // Success - process data
        process_parsed_data(&ctx);
        break;

    case NMEA_ERROR_CHECKSUM_FAILED:
        // Common in noisy environments - log but continue
        stats.checksum_errors++;
        if (stats.checksum_errors % 100 == 0) {
            log_warning("Checksum errors: %u", stats.checksum_errors);
        }
        break;

    case NMEA_ERROR_UNKNOWN_SENTENCE:
    case NMEA_ERROR_SENTENCE_DISABLED:
        // Expected - ignore silently
        break;

    case NMEA_ERROR_INVALID_SENTENCE:
        // Possible sync loss - may need to resync
        stats.invalid_sentences++;
        if (stats.invalid_sentences > 10) {
            resync_stream_parser();
        }
        break;

    case NMEA_ERROR_BUFFER_TOO_SMALL:
        // Fatal - fix your code
        log_error("Buffer too small! Need %zu bytes", nmea_get_required_buffer_size());
        assert(0);
        break;

    default:
        // Other errors - log and continue
        log_error("Parse error: %s (%d)", nmea_get_error_string(result), result);
        break;
}
```

### Strategy 3: Error Callbacks

Real-time error notification:

```c
void nmea_error_callback(
    nmea_error_type_t error_type,
    nmea_result_t error_code,
    const char *message,
    void *user_data
) {
    error_stats_t *stats = (error_stats_t *)user_data;

    // Categorize error
    switch (error_type) {
        case NMEA_ERROR_TYPE_CHECKSUM:
            stats->checksum_errors++;
            break;

        case NMEA_ERROR_TYPE_SYNTAX:
            stats->syntax_errors++;
            break;

        case NMEA_ERROR_TYPE_SEMANTIC:
            stats->semantic_errors++;
            break;

        default:
            stats->other_errors++;
            break;
    }

    // Log if excessive
    uint32_t total_errors = stats->checksum_errors +
                           stats->syntax_errors +
                           stats->semantic_errors +
                           stats->other_errors;

    if (total_errors % 100 == 0) {
        log_warning("Total errors: %u (checksum: %u, syntax: %u, semantic: %u)",
                   total_errors,
                   stats->checksum_errors,
                   stats->syntax_errors,
                   stats->semantic_errors);
    }
}

// Setup
error_stats_t error_stats = {0};

nmea_config_t config = {
    .enabled_modules = (1 << NMEA_MODULE_GNSS),
    .error_callback = nmea_error_callback,
    .error_callback_user_data = &error_stats,
    .validate_checksums = true
};
```

### Strategy 4: Rate-Limited Logging

Prevent log flooding:

```c
typedef struct {
    uint32_t last_log_time;
    uint32_t suppressed_count;
} rate_limiter_t;

bool should_log_error(rate_limiter_t *limiter, uint32_t min_interval_ms) {
    uint32_t now = get_milliseconds();

    if ((now - limiter->last_log_time) >= min_interval_ms) {
        if (limiter->suppressed_count > 0) {
            log_info("(%u errors suppressed)", limiter->suppressed_count);
            limiter->suppressed_count = 0;
        }
        limiter->last_log_time = now;
        return true;
    }

    limiter->suppressed_count++;
    return false;
}

// Usage
static rate_limiter_t checksum_limiter = {0};

if (result == NMEA_ERROR_CHECKSUM_FAILED) {
    if (should_log_error(&checksum_limiter, 5000)) {  // Max once per 5 seconds
        log_warning("Checksum error in sentence: %s", sentence);
    }
}
```

## Error Callbacks

### Callback Types

```c
typedef enum {
    NMEA_ERROR_TYPE_PARAMETER,   // Invalid function parameter
    NMEA_ERROR_TYPE_CHECKSUM,    // Checksum validation error
    NMEA_ERROR_TYPE_SYNTAX,      // Sentence syntax error
    NMEA_ERROR_TYPE_SEMANTIC,    // Field parsing error
    NMEA_ERROR_TYPE_BUFFER,      // Buffer management error
    NMEA_ERROR_TYPE_CONFIG,      // Configuration error
    NMEA_ERROR_TYPE_STATE,       // State machine error
    NMEA_ERROR_TYPE_UNKNOWN,     // Unknown error
} nmea_error_type_t;
```

### Complete Callback Example

```c
typedef struct {
    uint32_t parameter_errors;
    uint32_t checksum_errors;
    uint32_t syntax_errors;
    uint32_t semantic_errors;
    uint32_t buffer_errors;
    uint32_t config_errors;
    uint32_t state_errors;
    uint32_t unknown_errors;

    // Rate limiting
    uint32_t last_log_time;
    uint32_t log_interval_ms;

    // Optional: log callback
    void (*log_func)(const char *fmt, ...);
} nmea_error_tracking_t;

void comprehensive_error_callback(
    nmea_error_type_t error_type,
    nmea_result_t error_code,
    const char *message,
    void *user_data
) {
    nmea_error_tracking_t *tracker = (nmea_error_tracking_t *)user_data;

    // Count by type
    switch (error_type) {
        case NMEA_ERROR_TYPE_PARAMETER:  tracker->parameter_errors++; break;
        case NMEA_ERROR_TYPE_CHECKSUM:   tracker->checksum_errors++;  break;
        case NMEA_ERROR_TYPE_SYNTAX:     tracker->syntax_errors++;    break;
        case NMEA_ERROR_TYPE_SEMANTIC:   tracker->semantic_errors++;  break;
        case NMEA_ERROR_TYPE_BUFFER:     tracker->buffer_errors++;    break;
        case NMEA_ERROR_TYPE_CONFIG:     tracker->config_errors++;    break;
        case NMEA_ERROR_TYPE_STATE:      tracker->state_errors++;     break;
        default:                         tracker->unknown_errors++;    break;
    }

    // Rate-limited logging
    uint32_t now = get_milliseconds();
    if ((now - tracker->last_log_time) >= tracker->log_interval_ms) {
        if (tracker->log_func) {
            tracker->log_func("NMEA Error [%d]: %s - %s",
                            error_type,
                            nmea_get_error_string(error_code),
                            message ? message : "N/A");
        }
        tracker->last_log_time = now;
    }

    // Fatal errors - take immediate action
    if (error_type == NMEA_ERROR_TYPE_BUFFER ||
        error_type == NMEA_ERROR_TYPE_CONFIG) {
        // These indicate programming errors
        if (tracker->log_func) {
            tracker->log_func("FATAL: %s", message);
        }
        // Could trigger a reset or enter safe mode
    }
}
```

## Recovery Strategies

### 1. Checksum Errors

**Cause:** Electrical noise, baud rate mismatch, cable issues

**Recovery:**

```c
static uint32_t consecutive_checksum_errors = 0;

if (result == NMEA_ERROR_CHECKSUM_FAILED) {
    consecutive_checksum_errors++;

    // If too many errors, something is seriously wrong
    if (consecutive_checksum_errors > 10) {
        log_error("Too many checksum errors - check connection");

        // Possible actions:
        // 1. Reset UART peripheral
        // 2. Toggle GPS power
        // 3. Alert user
        // 4. Switch to backup GPS

        consecutive_checksum_errors = 0;
    }
} else {
    // Reset counter on success
    consecutive_checksum_errors = 0;
}
```

### 2. Sync Loss (Invalid Sentences)

**Cause:** Corrupted data, partial sentence, sync loss

**Recovery:**

```c
typedef struct {
    char buffer[128];
    size_t pos;
    bool synced;
} stream_parser_t;

void handle_invalid_sentence(stream_parser_t *parser) {
    // Reset parser state
    parser->pos = 0;
    parser->synced = false;

    log_debug("Stream sync lost - resyncing");
}

void process_byte(stream_parser_t *parser, char byte) {
    // Look for sentence start
    if (byte == '$' || byte == '!') {
        parser->pos = 0;
        parser->synced = true;
    }

    if (!parser->synced) {
        return;  // Ignore until synced
    }

    // Add byte to buffer
    parser->buffer[parser->pos++] = byte;

    // Parse on sentence end
    if (byte == '\n') {
        nmea_result_t result = nmea_parse(&ctx, parser->buffer, parser->pos,
                                          parse_buf, sizeof(parse_buf));

        if (result == NMEA_ERROR_INVALID_SENTENCE) {
            handle_invalid_sentence(parser);
        }

        parser->pos = 0;
        parser->synced = false;
    }
}
```

### 3. No Data / Stale Data

**Cause:** GPS has no fix, device disconnected, data stopped

**Detection:**

```c
typedef struct {
    uint32_t last_valid_data_time;
    uint32_t timeout_ms;
    bool data_valid;
} data_watchdog_t;

void update_watchdog(data_watchdog_t *wd, nmea_result_t result) {
    if (result == NMEA_OK) {
        wd->last_valid_data_time = get_milliseconds();
        wd->data_valid = true;
    } else {
        uint32_t now = get_milliseconds();
        if ((now - wd->last_valid_data_time) > wd->timeout_ms) {
            wd->data_valid = false;

            // Take action
            log_warning("No valid GPS data for %u ms", wd->timeout_ms);

            // Possible actions:
            // - Show "No GPS" indicator
            // - Use last known position
            // - Switch to dead reckoning
            // - Alert user
        }
    }
}
```

### 4. Buffer Overflow

**Cause:** Programming error (buffer too small)

**Prevention:**

```c
// Always use adequate buffer sizes
#define SENTENCE_BUF_SIZE 128  // > 82 (NMEA max)
#define PARSE_BUF_SIZE    256  // >= nmea_get_required_buffer_size()

char sentence_buf[SENTENCE_BUF_SIZE];
char parse_buf[PARSE_BUF_SIZE];

// Verify at compile time
_Static_assert(SENTENCE_BUF_SIZE >= 82, "Sentence buffer too small");
_Static_assert(PARSE_BUF_SIZE >= 256, "Parse buffer too small");

// Runtime check
size_t required = nmea_get_required_buffer_size();
if (sizeof(parse_buf) < required) {
    log_error("Parse buffer too small: have %zu, need %zu",
              sizeof(parse_buf), required);
    assert(0);
}
```

## Common Patterns

### Pattern 1: Comprehensive Error Handler

```c
typedef struct {
    // Counters
    uint32_t total_sentences;
    uint32_t successful_parses;
    uint32_t checksum_errors;
    uint32_t invalid_sentences;
    uint32_t unknown_sentences;
    uint32_t other_errors;

    // Thresholds
    uint32_t max_consecutive_errors;
    uint32_t consecutive_errors;

    // Status
    bool parser_healthy;
    uint32_t last_success_time;
} nmea_health_monitor_t;

nmea_result_t parse_with_monitoring(
    nmea_health_monitor_t *monitor,
    nmea_context_t *ctx,
    const char *sentence,
    size_t len
) {
    monitor->total_sentences++;

    char parse_buffer[256];
    nmea_result_t result = nmea_parse(ctx, sentence, len,
                                      parse_buffer, sizeof(parse_buffer));

    // Update statistics
    switch (result) {
        case NMEA_OK:
            monitor->successful_parses++;
            monitor->consecutive_errors = 0;
            monitor->last_success_time = get_milliseconds();
            monitor->parser_healthy = true;
            break;

        case NMEA_ERROR_CHECKSUM_FAILED:
            monitor->checksum_errors++;
            monitor->consecutive_errors++;
            break;

        case NMEA_ERROR_INVALID_SENTENCE:
            monitor->invalid_sentences++;
            monitor->consecutive_errors++;
            break;

        case NMEA_ERROR_UNKNOWN_SENTENCE:
        case NMEA_ERROR_SENTENCE_DISABLED:
            monitor->unknown_sentences++;
            // Don't count as error
            break;

        default:
            monitor->other_errors++;
            monitor->consecutive_errors++;
            break;
    }

    // Check health
    if (monitor->consecutive_errors > monitor->max_consecutive_errors) {
        monitor->parser_healthy = false;
        log_error("Parser unhealthy: %u consecutive errors",
                  monitor->consecutive_errors);

        // Take recovery action
        recover_parser(ctx);
        monitor->consecutive_errors = 0;
    }

    return result;
}
```

### Pattern 2: Graceful Degradation

```c
typedef enum {
    GPS_QUALITY_EXCELLENT,  // Recent, valid fix
    GPS_QUALITY_GOOD,       // Slightly stale but valid
    GPS_QUALITY_DEGRADED,   // Old data, use with caution
    GPS_QUALITY_NO_FIX,     // No valid data
} gps_quality_t;

typedef struct {
    nmea_gnss_state_t data;
    uint32_t last_update_time;
    bool valid;
    gps_quality_t quality;
} gps_state_t;

gps_quality_t evaluate_gps_quality(gps_state_t *state) {
    if (!state->valid) {
        return GPS_QUALITY_NO_FIX;
    }

    uint32_t age_ms = get_milliseconds() - state->last_update_time;

    if (age_ms < 1000) {
        return GPS_QUALITY_EXCELLENT;
    } else if (age_ms < 5000) {
        return GPS_QUALITY_GOOD;
    } else if (age_ms < 30000) {
        return GPS_QUALITY_DEGRADED;
    } else {
        return GPS_QUALITY_NO_FIX;
    }
}

void use_gps_data(gps_state_t *state) {
    gps_quality_t quality = evaluate_gps_quality(state);

    switch (quality) {
        case GPS_QUALITY_EXCELLENT:
        case GPS_QUALITY_GOOD:
            // Use current position
            navigate_to(state->data.latitude.degrees,
                       state->data.longitude.degrees);
            break;

        case GPS_QUALITY_DEGRADED:
            // Use with warning
            show_warning("GPS data is stale");
            navigate_to(state->data.latitude.degrees,
                       state->data.longitude.degrees);
            break;

        case GPS_QUALITY_NO_FIX:
            // Fallback behavior
            show_error("No GPS fix");
            use_dead_reckoning();
            break;
    }
}
```

## Related Documentation

- [Stream Parsing Guide](stream-parsing.md) - Handling data streams
- [Integration Guide](integration-guide.md) - General integration
- [FAQ](faq.md) - Common problems and solutions

## Examples

- `examples/comprehensive_error_handling/` - Complete error handling example
- `examples/simple_parser.c` - Basic error handling
