/**
 * @file minimal_memory.c
 * @brief Example demonstrating minimal memory configuration
 *
 * This example shows how to minimize RAM and flash usage for
 * extremely resource-constrained embedded systems.
 *
 * Demonstrates:
 * 1. Minimal module/sentence configuration
 * 2. Static memory allocation
 * 3. Memory usage measurement
 * 4. Comparing different configurations
 *
 * NOTE: To see the full effect, you must modify inc/nmea_config.h
 * to disable unused modules and sentences, then rebuild the library.
 */

#include <stdio.h>
#include <string.h>

#include "nmea.h"

/**
 * Memory usage tracking
 */
typedef struct {
  size_t context_size;
  size_t buffer_size;
  size_t total_stack_usage;
} memory_usage_t;

/**
 * Calculate memory usage for current configuration
 */
void measure_memory_usage(memory_usage_t* usage) {
  usage->context_size = nmea_get_context_size();
  usage->buffer_size = nmea_get_required_buffer_size();
  usage->total_stack_usage = usage->context_size + usage->buffer_size;
}

/**
 * Display memory usage
 */
void print_memory_usage(const char* config_name, memory_usage_t* usage) {
  printf("\n=== %s ===\n", config_name);
  printf("Context size:       %5zu bytes\n", usage->context_size);
  printf("Required buffer:    %5zu bytes\n", usage->buffer_size);
  printf("Total stack usage:  %5zu bytes\n", usage->total_stack_usage);
}

/**
 * Example 1: Absolute minimal - GNSS module, only GGA sentence
 *
 * Configuration in nmea_config.h:
 *   #define NMEA_MODULE_GNSS_ENABLED 1
 *   #define NMEA_SENTENCE_GGA_ENABLED 1
 *   #define NMEA_SENTENCE_RMC_ENABLED 0
 *   #define NMEA_SENTENCE_GLL_ENABLED 0
 *   (all other modules/sentences disabled)
 */
void example_minimal_gps(void) {
  printf("\n========================================\n");
  printf("Example 1: Minimal GPS Configuration\n");
  printf("========================================\n");

  /* Static allocation - no stack/heap overhead */
  static nmea_context_t ctx;
  static char parse_buffer[256];

  /* Minimal configuration */
  nmea_config_t config = {
      .enabled_modules = (1 << NMEA_MODULE_GNSS),
      .validate_checksums = true,
      .error_callback = NULL /* No callback saves code size */
  };

  /* Measure memory */
  memory_usage_t usage;
  measure_memory_usage(&usage);
  print_memory_usage("Minimal GPS (GGA only)", &usage);

  /* Initialize */
  nmea_result_t result = nmea_init(&ctx, &config);
  if (result != NMEA_OK) {
    fprintf(stderr, "Init failed: %s\n", nmea_get_error_string(result));
    return;
  }

  /* Parse a GGA sentence */
  const char* gga =
      "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";

  printf("\nParsing GGA sentence...\n");
  result =
      nmea_parse(&ctx, gga, strlen(gga), parse_buffer, sizeof(parse_buffer));

  if (result == NMEA_OK) {
    printf("[OK] Parse successful\n");

    nmea_gnss_state_t gnss;
    if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
      if (gnss.latitude.valid && gnss.longitude.valid) {
        printf("Position: %.6f°, %.6f°\n", gnss.latitude.degrees,
               gnss.longitude.degrees);
      }
      if (gnss.altitude_valid) {
        printf("Altitude: %.1f m\n", gnss.altitude_meters);
      }
      printf("Satellites: %d\n", gnss.satellites_used);
    }
  } else {
    printf("[ERR] Parse failed: %s\n", nmea_get_error_string(result));
  }

  /* Cleanup */
  nmea_cleanup(&ctx);

  printf("\nFlash estimate: ~5-6 KB (with only GGA enabled)\n");
}

/**
 * Example 2: Basic GPS tracker - GGA + RMC
 *
 * Configuration:
 *   #define NMEA_SENTENCE_GGA_ENABLED 1  // Position, altitude
 *   #define NMEA_SENTENCE_RMC_ENABLED 1  // Speed, course, date
 */
void example_basic_tracker(void) {
  printf("\n========================================\n");
  printf("Example 2: Basic GPS Tracker\n");
  printf("========================================\n");

  static nmea_context_t ctx;
  static char parse_buffer[256];

  nmea_config_t config = {.enabled_modules = (1 << NMEA_MODULE_GNSS),
                          .validate_checksums = true,
                          .error_callback = NULL};

  memory_usage_t usage;
  measure_memory_usage(&usage);
  print_memory_usage("Basic Tracker (GGA + RMC)", &usage);

  nmea_init(&ctx, &config);

  /* Parse both GGA and RMC */
  const char* gga =
      "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";
  const char* rmc =
      "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230325,003.1,W,A*"
      "0D\r\n";

  printf("\nParsing GGA and RMC sentences...\n");

  if (nmea_parse(&ctx, gga, strlen(gga), parse_buffer, sizeof(parse_buffer)) ==
      NMEA_OK) {
    printf("[OK] GGA parsed\n");
  }

  if (nmea_parse(&ctx, rmc, strlen(rmc), parse_buffer, sizeof(parse_buffer)) ==
      NMEA_OK) {
    printf("[OK] RMC parsed\n");
  }

  nmea_gnss_state_t gnss;
  if (nmea_get_gnss_data(&ctx, &gnss) == NMEA_OK) {
    printf("\nData from both sentences:\n");
    printf("Position: %.6f°, %.6f°\n", gnss.latitude.degrees,
           gnss.longitude.degrees);

    if (gnss.speed.valid) {
      printf("Speed: %.1f knots\n", gnss.speed.knots);
    }

    if (gnss.course.valid) {
      printf("Course: %.1f°\n", gnss.course.degrees);
    }

    if (gnss.utc_date.valid) {
      printf("Date: %04d-%02d-%02d\n", gnss.utc_date.year, gnss.utc_date.month,
             gnss.utc_date.day);
    }
  }

  nmea_cleanup(&ctx);

  printf("\nFlash estimate: ~6-8 KB (with GGA + RMC enabled)\n");
}

/**
 * Example 3: Memory comparison - Show impact of different configs
 */
void example_memory_comparison(void) {
  printf("\n========================================\n");
  printf("Example 3: Memory Comparison\n");
  printf("========================================\n");

  memory_usage_t usage;

  /* Note: These measurements show the current configuration.
   * To see different sizes, you must modify nmea_config.h and rebuild. */

  measure_memory_usage(&usage);

  printf("\nCurrent configuration:\n");
  printf("  Context: %zu bytes\n", usage.context_size);
  printf("  Buffer:  %zu bytes\n", usage.buffer_size);
  printf("  Total:   %zu bytes\n", usage.total_stack_usage);

  printf("\nTypical configurations:\n");
  printf("  GNSS only (1-2 sentences):  ~200-250 bytes\n");
  printf("  GNSS only (all sentences):  ~300-400 bytes\n");
  printf("  GNSS + Sensor:              ~500-800 bytes\n");
  printf("  GNSS + Sensor + Heading:    ~800-1200 bytes\n");
  printf("  All modules:                ~1500-2000 bytes\n");

  printf("\nFlash usage (typical):\n");
  printf("  GNSS only (1-2 sentences):  ~5-7 KB\n");
  printf("  GNSS only (all sentences):  ~10-15 KB\n");
  printf("  GNSS + Sensor + Heading:    ~15-20 KB\n");
  printf("  All modules:                ~25-35 KB\n");

  printf("\nOptimization tips:\n");
  printf("  1. Disable unused modules in nmea_config.h\n");
  printf("  2. Disable unused sentences within modules\n");
  printf("  3. Use -Os compiler flag for size optimization\n");
  printf("  4. Enable -flto (link-time optimization)\n");
  printf("  5. Use -Wl,--gc-sections to remove unused code\n");
}

/**
 * Example 4: Ultra-minimal buffer usage
 */
void example_minimal_buffers(void) {
  printf("\n========================================\n");
  printf("Example 4: Minimal Buffer Usage\n");
  printf("========================================\n");

/* Absolute minimum buffer sizes */
#define MIN_SENTENCE_BUF 82 /* NMEA max sentence length */
#define MIN_PARSE_BUF 256   /* Library requirement */

  static nmea_context_t ctx;
  static char parse_buffer[MIN_PARSE_BUF];
  char sentence_buffer[MIN_SENTENCE_BUF];

  printf("Minimum buffer sizes:\n");
  printf("  Sentence buffer: %d bytes (NMEA max)\n", MIN_SENTENCE_BUF);
  printf("  Parse buffer:    %d bytes (library requirement)\n", MIN_PARSE_BUF);
  printf("  Total buffers:   %d bytes\n", MIN_SENTENCE_BUF + MIN_PARSE_BUF);

  nmea_config_t config = {.enabled_modules = (1 << NMEA_MODULE_GNSS),
                          .validate_checksums = true};

  nmea_init(&ctx, &config);

  /* Use the minimal buffers */
  const char* short_sentence =
      "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";

  /* Copy to local buffer (in real app, this would come from UART) */
  size_t len = strlen(short_sentence);
  if (len < MIN_SENTENCE_BUF) {
    memcpy(sentence_buffer, short_sentence, len + 1);

    nmea_result_t result = nmea_parse(&ctx, sentence_buffer, len, parse_buffer,
                                      sizeof(parse_buffer));

    if (result == NMEA_OK) {
      printf("[OK] Successfully parsed with minimal buffers\n");
    } else {
      printf("[ERR] Parse failed: %s\n", nmea_get_error_string(result));
    }
  }

  nmea_cleanup(&ctx);
}

/**
 * Main program
 */
int main(void) {
  printf("NMEA Minimal Memory Configuration Examples\n");
  printf("===========================================\n");
  printf("\nNOTE: To achieve the minimum sizes shown, you must:\n");
  printf("  1. Edit inc/nmea_config.h\n");
  printf("  2. Disable unused modules and sentences\n");
  printf("  3. Rebuild the library\n");

  /* Run examples */
  example_minimal_gps();
  example_basic_tracker();
  example_memory_comparison();
  example_minimal_buffers();

  printf("\n===========================================\n");
  printf("For detailed optimization instructions, see:\n");
  printf("  doc/memory-optimization.md\n");
  printf("===========================================\n");

  return 0;
}
