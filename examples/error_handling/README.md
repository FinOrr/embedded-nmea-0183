# Error Handling Example

This example demonstrates error handling patterns for NMEA parser applications.

## Purpose

Demonstrates error callbacks, statistics tracking, and recovery strategies.

## Features

- Error callbacks with categorization
- Detailed error statistics
- Health monitoring
- Recovery strategies
- Graceful degradation based on GPS quality
- Rate-limited logging

## Building

```bash
# From project root - build all examples
cmake --preset conf-examples && cmake --build --preset build-examples

# Or build manually
gcc -I./inc examples/error_handling/error_handling.c -L./out/build/conf-examples/lib -lnmea -lm -o error_handling
```

## Running

```bash
./out/build/conf-examples/examples/error_handling
```

## Expected Output

```
NMEA Error Handling Examples
===========================================

========================================
Example 1: Basic Error Handling
========================================

Parsing test sentences...

[1] $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
    [OK] Success

[2] $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*FF
    [ERR] NMEA_ERROR_CHECKSUM_FAILED

[3] $GPGGA,123519,4807.038
    [ERR] NMEA_ERROR_PARSE_FAILED

[4] $GPXXX,123,456,789*00
    [?] Ignored (NMEA_ERROR_UNKNOWN_SENTENCE)

=== Error Statistics ===
Total sentences:     5
Successful parses:   2 (40.0%)
Total errors:        2

Error breakdown:
  Checksum errors:   1
  Semantic errors:   1

Health status:       HEALTHY
Consecutive errors:  0

========================================
Example 2: Error Recovery
========================================

Simulating burst of checksum errors...
Warning: 5 consecutive checksum errors (check connections)
Consecutive errors: 5

Simulating recovery...
Parser healthy: YES
Consecutive errors: 0

========================================
Example 3: Graceful Degradation
========================================

GPS Status:
  Quality: GPS FIX - Standard quality
  HDOP: 0.9 - EXCELLENT
  Satellites: 8 - EXCELLENT

Recommendation: Safe to use position
```

## Key Concepts

### 1. Error Tracking Structure

Track all error types and statistics:

```c
typedef struct {
    uint32_t checksum_errors;
    uint32_t syntax_errors;
    uint32_t semantic_errors;
    uint32_t consecutive_errors;
    bool parser_healthy;
} error_tracker_t;
```

### 2. Error Callback

Get real-time error notifications:

```c
void nmea_error_callback(
    nmea_error_type_t error_type,
    nmea_result_t error_code,
    const char *message,
    void *user_data
) {
    // Log, count, and handle errors
}
```

### 3. Categorized Handling

Different actions for different error types:

```c
switch (result) {
    case NMEA_OK:
        consecutive_errors = 0;  // Reset
        break;

    case NMEA_ERROR_CHECKSUM_FAILED:
        consecutive_errors++;
        // Common, just log
        break;

    case NMEA_ERROR_BUFFER_TOO_SMALL:
        // Fatal - programming error
        break;

    case NMEA_ERROR_UNKNOWN_SENTENCE:
        // Expected - ignore
        break;
}
```

### 4. Health Monitoring

Track parser health:

```c
if (consecutive_errors > MAX_ERRORS) {
    parser_healthy = false;
    trigger_recovery();
}
```

### 5. Graceful Degradation

Use GPS data quality to make decisions:

```c
if (gnss.fix_quality > 0 && gnss.satellites_used >= 4) {
    use_current_position();
} else {
    use_last_known_position();
}
```

## Production Patterns

### Pattern 1: Error Statistics Dashboard

```c
void print_error_dashboard(error_tracker_t *tracker) {
    float success_rate = (float)tracker->successful_parses /
                        tracker->total_sentences * 100.0f;

    printf("Success rate: %.1f%%\n", success_rate);
    printf("Checksum errors: %u\n", tracker->checksum_errors);

    if (success_rate < 80.0f) {
        printf("WARNING: Poor data quality\n");
    }
}
```

### Pattern 2: Automatic Recovery

```c
if (consecutive_checksum_errors > 10) {
    // Reset UART peripheral
    uart_reset();

    // Or power cycle GPS
    gps_power_cycle();

    consecutive_checksum_errors = 0;
}
```

### Pattern 3: Data Quality Assessment

```c
typedef enum {
    GPS_QUALITY_NO_FIX,
    GPS_QUALITY_POOR,
    GPS_QUALITY_GOOD,
    GPS_QUALITY_EXCELLENT
} gps_quality_t;

gps_quality_t assess_quality(nmea_gnss_state_t *gnss) {
    if (gnss->fix_quality == 0) return GPS_QUALITY_NO_FIX;

    if (gnss->dop.valid && gnss->dop.hdop < 1.0 &&
        gnss->satellites_used >= 8) {
        return GPS_QUALITY_EXCELLENT;
    }

    if (gnss->satellites_used >= 6) {
        return GPS_QUALITY_GOOD;
    }

    return GPS_QUALITY_POOR;
}
```

### Pattern 4: Rate-Limited Logging

```c
typedef struct {
    uint32_t last_log_time;
    uint32_t suppressed_count;
} rate_limiter_t;

bool should_log(rate_limiter_t *limiter, uint32_t interval_ms) {
    uint32_t now = get_time_ms();

    if ((now - limiter->last_log_time) >= interval_ms) {
        if (limiter->suppressed_count > 0) {
            printf("(%u errors suppressed)\n",
                   limiter->suppressed_count);
            limiter->suppressed_count = 0;
        }
        limiter->last_log_time = now;
        return true;
    }

    limiter->suppressed_count++;
    return false;
}
```

## Error Types Reference

### Recoverable Errors

These are expected in real-world conditions:

- `NMEA_ERROR_CHECKSUM_FAILED` - Electrical noise, cable issues
- `NMEA_ERROR_INVALID_SENTENCE` - Partial data, sync loss
- `NMEA_ERROR_PARSE_FAILED` - Malformed data

**Action:** Log, count, continue

### Informational

Not actual errors:

- `NMEA_ERROR_UNKNOWN_SENTENCE` - Sentence type not compiled in
- `NMEA_ERROR_SENTENCE_DISABLED` - Sentence disabled in config
- `NMEA_ERROR_NO_DATA` - No data available yet

**Action:** Ignore

### Fatal Errors

Indicate programming errors:

- `NMEA_ERROR_NULL_PARAM` - Null pointer passed
- `NMEA_ERROR_BUFFER_TOO_SMALL` - Buffer insufficient
- `NMEA_ERROR_INVALID_CONFIG` - Bad configuration

**Action:** Fix code, assert in debug builds

## Best Practices

1. **Always check return values**
   ```c
   if (nmea_parse(...) != NMEA_OK) {
       // Handle error
   }
   ```

2. **Use error callbacks for logging**
   ```c
   config.error_callback = my_error_handler;
   ```

3. **Track consecutive errors**
   ```c
   if (consecutive_errors > threshold) {
       take_action();
   }
   ```

4. **Don't flood logs**
   ```c
   if (should_log_error(&limiter, 5000)) {
       log_error(...);
   }
   ```

5. **Monitor parser health**
   ```c
   if (!parser_healthy) {
       attempt_recovery();
   }
   ```

6. **Gracefully degrade**
   ```c
   if (gps_quality < THRESHOLD) {
       use_fallback_data();
   }
   ```

## Related Documentation

- [Stream Parsing](../../doc/stream-parsing.md) - Handling data streams
- [FAQ](../../doc/faq.md) - Common error scenarios
