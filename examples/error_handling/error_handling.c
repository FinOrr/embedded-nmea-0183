/**
 * @file error_handling.c
 * @brief Comprehensive error handling example for NMEA parser
 *
 * This example demonstrates error handling techniques:
 * 1. Error callbacks with categorization
 * 2. Error statistics and monitoring
 * 3. Recovery strategies
 * 4. Health monitoring
 * 5. Graceful degradation
 *
 * Use this as a template for production applications.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "nmea.h"

/* ========================================================================== */
/*                          ERROR TRACKING                                    */
/* ========================================================================== */

/**
 * Detailed error statistics
 */
typedef struct {
    /* Error counts by type */
    uint32_t parameter_errors;
    uint32_t checksum_errors;
    uint32_t syntax_errors;
    uint32_t semantic_errors;
    uint32_t buffer_errors;
    uint32_t config_errors;
    uint32_t state_errors;
    uint32_t unknown_errors;

    /* Parse result counts */
    uint32_t total_sentences;
    uint32_t successful_parses;
    uint32_t unknown_sentences;
    uint32_t disabled_sentences;

    /* Health monitoring */
    uint32_t consecutive_errors;
    uint32_t max_consecutive_errors;
    bool parser_healthy;

    /* Rate limiting */
    uint32_t last_log_time_ms;
    uint32_t log_interval_ms;
} error_tracker_t;

/**
 * Initialize error tracker
 */
void error_tracker_init(error_tracker_t *tracker, uint32_t log_interval_ms) {
    memset(tracker, 0, sizeof(*tracker));
    tracker->max_consecutive_errors = 10;
    tracker->log_interval_ms = log_interval_ms;
    tracker->parser_healthy = true;
}

/**
 * Print error statistics
 */
void error_tracker_print_stats(error_tracker_t *tracker) {
    uint32_t total_errors = tracker->checksum_errors +
                           tracker->syntax_errors +
                           tracker->semantic_errors +
                           tracker->buffer_errors +
                           tracker->config_errors +
                           tracker->state_errors +
                           tracker->unknown_errors;

    printf("\n=== Error Statistics ===\n");
    printf("Total sentences:     %u\n", tracker->total_sentences);
    printf("Successful parses:   %u (%.1f%%)\n",
           tracker->successful_parses,
           tracker->total_sentences > 0 ?
               (float)tracker->successful_parses / tracker->total_sentences * 100.0f : 0.0f);
    printf("Total errors:        %u\n", total_errors);

    if (total_errors > 0) {
        printf("\nError breakdown:\n");
        printf("  Checksum errors:   %u\n", tracker->checksum_errors);
        printf("  Syntax errors:     %u\n", tracker->syntax_errors);
        printf("  Semantic errors:   %u\n", tracker->semantic_errors);
        printf("  Buffer errors:     %u\n", tracker->buffer_errors);
        printf("  Config errors:     %u\n", tracker->config_errors);
        printf("  State errors:      %u\n", tracker->state_errors);
        printf("  Unknown errors:    %u\n", tracker->unknown_errors);
    }

    printf("\nOther:\n");
    printf("  Unknown sentences: %u\n", tracker->unknown_sentences);
    printf("  Disabled sentences:%u\n", tracker->disabled_sentences);

    printf("\nHealth status:       %s\n",
           tracker->parser_healthy ? "HEALTHY" : "UNHEALTHY");
    printf("Consecutive errors:  %u (max: %u)\n",
           tracker->consecutive_errors,
           tracker->max_consecutive_errors);
}

/* ========================================================================== */
/*                          ERROR CALLBACK                                    */
/* ========================================================================== */

/**
 * Global error tracker instance
 */
static error_tracker_t g_error_tracker;

/**
 * Simulated millisecond counter
 */
static uint32_t g_system_time_ms = 0;

uint32_t get_system_time_ms(void) {
    return g_system_time_ms++;
}

/**
 * Comprehensive error callback
 */
void nmea_error_callback(
    nmea_error_type_t error_type,
    nmea_result_t error_code,
    const char *message,
    void *user_data
) {
    error_tracker_t *tracker = (error_tracker_t *)user_data;

    /* Count by type */
    switch (error_type) {
        case NMEA_ERROR_TYPE_PARAMETER:
            tracker->parameter_errors++;
            break;
        case NMEA_ERROR_TYPE_CHECKSUM:
            tracker->checksum_errors++;
            break;
        case NMEA_ERROR_TYPE_SYNTAX:
            tracker->syntax_errors++;
            break;
        case NMEA_ERROR_TYPE_SEMANTIC:
            tracker->semantic_errors++;
            break;
        case NMEA_ERROR_TYPE_BUFFER:
            tracker->buffer_errors++;
            break;
        case NMEA_ERROR_TYPE_CONFIG:
            tracker->config_errors++;
            break;
        case NMEA_ERROR_TYPE_STATE:
            tracker->state_errors++;
            break;
        default:
            tracker->unknown_errors++;
            break;
    }

    /* Rate-limited logging */
    uint32_t now = get_system_time_ms();
    if ((now - tracker->last_log_time_ms) >= tracker->log_interval_ms) {
        fprintf(stderr, "[ERROR] Type: %d, Code: %s, Message: %s\n",
                error_type,
                nmea_get_error_string(error_code),
                message ? message : "N/A");
        tracker->last_log_time_ms = now;
    }

    /* Fatal errors - these indicate programming errors */
    if (error_type == NMEA_ERROR_TYPE_BUFFER ||
        error_type == NMEA_ERROR_TYPE_CONFIG ||
        error_type == NMEA_ERROR_TYPE_PARAMETER) {
        fprintf(stderr, "FATAL ERROR: %s\n", message);
        /* In production, might trigger a reset or enter safe mode */
    }
}

/* ========================================================================== */
/*                          PARSE WITH ERROR HANDLING                         */
/* ========================================================================== */

/**
 * Parse sentence with comprehensive error handling
 */
nmea_result_t parse_with_error_handling(
    nmea_context_t *ctx,
    error_tracker_t *tracker,
    const char *sentence,
    size_t length,
    char *parse_buffer,
    size_t buffer_size
) {
    tracker->total_sentences++;

    /* Parse */
    nmea_result_t result = nmea_parse(ctx, sentence, length,
                                      parse_buffer, buffer_size);

    /* Handle result */
    switch (result) {
        case NMEA_OK:
            tracker->successful_parses++;
            tracker->consecutive_errors = 0;  /* Reset on success */
            tracker->parser_healthy = true;
            break;

        case NMEA_ERROR_CHECKSUM_FAILED:
            /* Checksum errors are common in noisy environments */
            tracker->consecutive_errors++;
            if (tracker->consecutive_errors > 5) {
                fprintf(stderr, "Warning: %u consecutive checksum errors "
                               "(check connections)\n",
                        tracker->consecutive_errors);
            }
            break;

        case NMEA_ERROR_INVALID_SENTENCE:
            /* Might indicate sync loss */
            tracker->consecutive_errors++;
            fprintf(stderr, "Warning: Invalid sentence format\n");
            break;

        case NMEA_ERROR_UNKNOWN_SENTENCE:
            /* Informational - not an error */
            tracker->unknown_sentences++;
            tracker->consecutive_errors = 0;  /* Don't count as error */
            break;

        case NMEA_ERROR_SENTENCE_DISABLED:
            /* Expected - sentence disabled in config */
            tracker->disabled_sentences++;
            tracker->consecutive_errors = 0;  /* Don't count as error */
            break;

        case NMEA_ERROR_BUFFER_TOO_SMALL:
            /* Fatal - programming error */
            fprintf(stderr, "FATAL: Buffer too small (need %zu bytes)\n",
                    nmea_get_required_buffer_size());
            tracker->parser_healthy = false;
            break;

        default:
            /* Other errors */
            tracker->consecutive_errors++;
            fprintf(stderr, "Parse error: %s\n", nmea_get_error_string(result));
            break;
    }

    /* Check health */
    if (tracker->consecutive_errors > tracker->max_consecutive_errors) {
        tracker->parser_healthy = false;
        fprintf(stderr, "ERROR: Parser unhealthy (%u consecutive errors)\n",
                tracker->consecutive_errors);
    }

    return result;
}

/* ========================================================================== */
/*                          EXAMPLES                                          */
/* ========================================================================== */

/**
 * Example 1: Basic error handling with statistics
 */
void example_basic_error_handling(void) {
    printf("\n========================================\n");
    printf("Example 1: Basic Error Handling\n");
    printf("========================================\n");

    nmea_context_t ctx;
    error_tracker_t tracker;
    char parse_buffer[256];

    /* Initialize tracker */
    error_tracker_init(&tracker, 1000);  /* Log max once per second */

    /* Configure parser with error callback */
    nmea_config_t config = {
        .enabled_modules = (1 << NMEA_MODULE_GNSS),
        .validate_checksums = true,
        .error_callback = nmea_error_callback,
        .error_callback_user_data = &tracker
    };

    nmea_init(&ctx, &config);

    /* Test sentences with various errors */
    const char *test_sentences[] = {
        /* Valid */
        "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n",

        /* Checksum error */
        "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*FF\r\n",

        /* Invalid format (missing fields) */
        "$GPGGA,123519,4807.038\r\n",

        /* Unknown sentence type */
        "$GPXXX,123,456,789*00\r\n",

        /* Valid again */
        "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W,A*6A\r\n",

        NULL
    };

    printf("\nParsing test sentences...\n");

    for (int i = 0; test_sentences[i] != NULL; i++) {
        printf("\n[%d] %s", i + 1, test_sentences[i]);

        nmea_result_t result = parse_with_error_handling(
            &ctx, &tracker,
            test_sentences[i], strlen(test_sentences[i]),
            parse_buffer, sizeof(parse_buffer)
        );

        if (result == NMEA_OK) {
            printf("    [OK] Success\n");
        } else if (result == NMEA_ERROR_UNKNOWN_SENTENCE ||
                   result == NMEA_ERROR_SENTENCE_DISABLED) {
            printf("    [?] Ignored (%s)\n", nmea_get_error_string(result));
        } else {
            printf("    [ERR] %s\n", nmea_get_error_string(result));
        }
    }

    /* Print statistics */
    error_tracker_print_stats(&tracker);

    nmea_cleanup(&ctx);
}

/**
 * Example 2: Recovery from errors
 */
void example_error_recovery(void) {
    printf("\n========================================\n");
    printf("Example 2: Error Recovery\n");
    printf("========================================\n");

    nmea_context_t ctx;
    error_tracker_t tracker;
    char parse_buffer[256];

    error_tracker_init(&tracker, 1000);

    nmea_config_t config = {
        .enabled_modules = (1 << NMEA_MODULE_GNSS),
        .validate_checksums = true,
        .error_callback = nmea_error_callback,
        .error_callback_user_data = &tracker
    };

    nmea_init(&ctx, &config);

    /* Simulate burst of checksum errors followed by recovery */
    printf("\nSimulating burst of checksum errors...\n");

    const char *bad_sentence = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*FF\r\n";
    const char *good_sentence = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";

    /* 5 bad sentences */
    for (int i = 0; i < 5; i++) {
        parse_with_error_handling(&ctx, &tracker, bad_sentence, strlen(bad_sentence),
                                  parse_buffer, sizeof(parse_buffer));
    }

    printf("\nConsecutive errors: %u\n", tracker.consecutive_errors);

    /* Recovery with good sentences */
    printf("\nSimulating recovery...\n");

    for (int i = 0; i < 3; i++) {
        parse_with_error_handling(&ctx, &tracker, good_sentence, strlen(good_sentence),
                                  parse_buffer, sizeof(parse_buffer));
    }

    printf("Parser healthy: %s\n", tracker.parser_healthy ? "YES" : "NO");
    printf("Consecutive errors: %u\n", tracker.consecutive_errors);

    error_tracker_print_stats(&tracker);

    nmea_cleanup(&ctx);
}

/**
 * Example 3: Graceful degradation
 */
void example_graceful_degradation(void) {
    printf("\n========================================\n");
    printf("Example 3: Graceful Degradation\n");
    printf("========================================\n");

    nmea_context_t ctx;
    error_tracker_t tracker;
    char parse_buffer[256];

    error_tracker_init(&tracker, 1000);

    nmea_config_t config = {
        .enabled_modules = (1 << NMEA_MODULE_GNSS),
        .validate_checksums = true,
        .error_callback = NULL  /* No callback for this example */
    };

    nmea_init(&ctx, &config);

    /* Valid GPS data */
    const char *valid_gga = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";

    parse_with_error_handling(&ctx, &tracker, valid_gga, strlen(valid_gga),
                              parse_buffer, sizeof(parse_buffer));

    /* Get GPS data */
    nmea_gnss_state_t gnss;
    if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
        printf("\nGPS Status:\n");

        /* Evaluate data quality */
        if (gnss.fix_quality == 0) {
            printf("  Quality: NO FIX - Don't use position\n");
        } else if (gnss.fix_quality == 1) {
            printf("  Quality: GPS FIX - Standard quality\n");
        } else if (gnss.fix_quality == 2) {
            printf("  Quality: DGPS FIX - Good quality\n");
        } else {
            printf("  Quality: HIGH PRECISION FIX\n");
        }

        /* Check HDOP */
        if (gnss.dop.valid) {
            if (gnss.dop.hdop < 1.0) {
                printf("  HDOP: %.1f - EXCELLENT\n", gnss.dop.hdop);
            } else if (gnss.dop.hdop < 2.0) {
                printf("  HDOP: %.1f - GOOD\n", gnss.dop.hdop);
            } else if (gnss.dop.hdop < 5.0) {
                printf("  HDOP: %.1f - MODERATE\n", gnss.dop.hdop);
            } else {
                printf("  HDOP: %.1f - POOR (use with caution)\n", gnss.dop.hdop);
            }
        }

        /* Check satellite count */
        if (gnss.satellites_used < 4) {
            printf("  Satellites: %d - INSUFFICIENT (need 4+)\n",
                   gnss.satellites_used);
        } else if (gnss.satellites_used < 6) {
            printf("  Satellites: %d - MINIMAL\n", gnss.satellites_used);
        } else if (gnss.satellites_used < 8) {
            printf("  Satellites: %d - GOOD\n", gnss.satellites_used);
        } else {
            printf("  Satellites: %d - EXCELLENT\n", gnss.satellites_used);
        }

        /* Decision */
        printf("\nRecommendation: ");
        if (gnss.fix_quality > 0 && gnss.satellites_used >= 4) {
            printf("Safe to use position\n");
        } else {
            printf("Use last known position or wait for better fix\n");
        }
    }

    nmea_cleanup(&ctx);
}

/**
 * Main program
 */
int main(void) {
    printf("Comprehensive NMEA Error Handling Examples\n");
    printf("===========================================\n");

    /* Run examples */
    example_basic_error_handling();
    example_error_recovery();
    example_graceful_degradation();

    printf("\n===========================================\n");
    printf("For detailed error handling guide, see:\n");
    printf("  doc/error-handling.md\n");
    printf("===========================================\n");

    return 0;
}
