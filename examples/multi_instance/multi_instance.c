/**
 * @file multi_instance.c
 * @brief Example demonstrating multiple independent NMEA parser instances
 *
 * This example shows how to use multiple parser contexts simultaneously,
 * which is useful when:
 * - Parsing data from multiple GPS devices
 * - Separating GNSS and sensor data streams
 * - Processing different NMEA sources independently
 *
 * Demonstrates:
 * 1. Creating multiple parser contexts
 * 2. Configuring each with different modules
 * 3. Parsing different sentence types independently
 * 4. Retrieving data from each context
 */

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "nmea.h"

/**
 * Simulated GPS device 1: Primary navigation GPS
 */
const char* gps1_sentences[] = {
    "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n",
    "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W,A*"
    "6A\r\n",
    "$GPGLL,4807.038,N,01131.000,E,123519,A,A*5C\r\n", NULL};

/**
 * Simulated GPS device 2: Backup GPS (different location)
 */
const char* gps2_sentences[] = {
    "$GPGGA,123520,4808.100,N,01132.500,E,1,07,1.2,550.0,M,47.0,M,,*4D\r\n",
    "$GPRMC,123520,A,4808.100,N,01132.500,E,018.5,090.0,230394,003.0,W,A*"
    "6E\r\n",
    NULL};

/**
 * Simulated sensor device: Depth sounder and wind sensor
 */
const char* sensor_sentences[] = {
    "$SDDBT,12.3,f,3.7,M,2.0,F*3A\r\n",            // Depth
    "$IIMTW,15.5,C*21\r\n",                        // Water temperature
    "$IIMWD,123.4,T,127.8,M,12.5,N,6.4,M*5E\r\n",  // Wind
    NULL};

/**
 * Display GPS data from a context
 */
void display_gps(const char* device_name, nmea_context_t* ctx) {
  nmea_gnss_state_t gnss;

  if (nmea_get_gnss_data(ctx, &gnss) != NMEA_OK) {
    printf("%s: No GPS data\n", device_name);
    return;
  }

  printf("\n=== %s ===\n", device_name);

  if (gnss.latitude.valid && gnss.longitude.valid) {
    printf("Position: %.6f°, %.6f°\n", gnss.latitude.degrees,
           gnss.longitude.degrees);
  }

  if (gnss.altitude_valid) {
    printf("Altitude: %.1f m\n", gnss.altitude_meters);
  }

  if (gnss.speed.valid) {
    printf("Speed: %.1f knots\n", gnss.speed.knots);
  }

  if (gnss.course.valid) {
    printf("Course: %.1f°\n", gnss.course.degrees);
  }

  printf("Fix quality: %d, Satellites: %d\n", gnss.fix_quality,
         gnss.satellites_used);

  if (gnss.dop.valid) {
    printf("HDOP: %.1f\n", gnss.dop.hdop);
  }
}

/**
 * Display sensor data from a context
 */
void display_sensors(const char* device_name, nmea_context_t* ctx) {
  nmea_sensor_state_t sensor;

  if (nmea_get_sensor_data(ctx, &sensor) != NMEA_OK) {
    printf("%s: No sensor data\n", device_name);
    return;
  }

  printf("\n=== %s ===\n", device_name);

  if (sensor.depth_valid) {
    printf("Depth: %.2f m (%.2f ft, %.2f fathoms)\n",
           sensor.depth_below_transducer_meters,
           sensor.depth_below_transducer_feet,
           sensor.depth_below_transducer_fathoms);
  }

  if (sensor.water_temperature_valid) {
    printf("Water temp: %.1f°C\n", sensor.water_temperature_celsius);
  }

  if (sensor.wind_valid) {
    printf("Wind: %.1f° (T) at %.1f knots\n", sensor.wind_direction_true,
           sensor.wind_speed_knots);
  }
}

/**
 * Parse sentences from a simulated device
 */
void parse_device_data(nmea_context_t* ctx, const char** sentences,
                       const char* device_name) {
  char parse_buffer[256];
  int count = 0;

  printf("\nParsing data from %s...\n", device_name);

  for (int i = 0; sentences[i] != NULL; i++) {
    nmea_result_t result = nmea_parse(ctx, sentences[i], strlen(sentences[i]),
                                      parse_buffer, sizeof(parse_buffer));

    if (result == NMEA_OK) {
      printf("  [OK] %s", sentences[i]);
      count++;
    } else if (result == NMEA_ERROR_UNKNOWN_SENTENCE ||
               result == NMEA_ERROR_SENTENCE_DISABLED) {
      // Expected - sentence not enabled for this context
      printf("  [?] %s", sentences[i]);
    } else {
      printf("  [ERR] %s    Error: %s\n", sentences[i],
             nmea_get_error_string(result));
    }
  }

  printf("Parsed %d sentences from %s\n", count, device_name);
}

/**
 * Main program
 */
int main(void) {
  printf("NMEA Multi-Instance Parser Example\n");
  printf("===================================\n\n");

  /* Create three independent parser contexts */
  nmea_context_t gps1_ctx, gps2_ctx, sensor_ctx;

  /* Configure GPS device 1 - Primary navigation */
  nmea_config_t gps1_config = {.enabled_modules = (1 << NMEA_MODULE_GNSS),
                               .validate_checksums = true,
                               .error_callback = NULL};

  /* Configure GPS device 2 - Backup GPS */
  nmea_config_t gps2_config = {.enabled_modules = (1 << NMEA_MODULE_GNSS),
                               .validate_checksums = true,
                               .error_callback = NULL};

  /* Configure sensor device - Only sensors, no GNSS */
  nmea_config_t sensor_config = {
      .enabled_modules = (1 << NMEA_MODULE_SENSOR),
      .validate_checksums = false,  // Sensors might not have checksums
      .error_callback = NULL};

  /* Initialize all contexts */
  printf("Initializing parser contexts...\n");

  if (nmea_init(&gps1_ctx, &gps1_config) != NMEA_OK) {
    fprintf(stderr, "Failed to init GPS1 context\n");
    return 1;
  }
  printf("  GPS1 context initialized (context size: %zu bytes)\n",
         nmea_get_context_size());

  if (nmea_init(&gps2_ctx, &gps2_config) != NMEA_OK) {
    fprintf(stderr, "Failed to init GPS2 context\n");
    return 1;
  }
  printf("  GPS2 context initialized\n");

  if (nmea_init(&sensor_ctx, &sensor_config) != NMEA_OK) {
    fprintf(stderr, "Failed to init sensor context\n");
    return 1;
  }
  printf("  Sensor context initialized\n");

  /* Parse data from each device */
  parse_device_data(&gps1_ctx, gps1_sentences, "GPS Device 1");
  parse_device_data(&gps2_ctx, gps2_sentences, "GPS Device 2");
  parse_device_data(&sensor_ctx, sensor_sentences, "Sensor Device");

  /* Display data from each device */
  display_gps("GPS Device 1 (Primary)", &gps1_ctx);
  display_gps("GPS Device 2 (Backup)", &gps2_ctx);
  display_sensors("Sensor Device", &sensor_ctx);

  /* Compare GPS positions */
  printf("\n=== Position Comparison ===\n");

  nmea_gnss_state_t gps1_data, gps2_data;

  if (nmea_get_gnss_data(&gps1_ctx, &gps1_data) == NMEA_OK &&
      nmea_get_gnss_data(&gps2_ctx, &gps2_data) == NMEA_OK) {
    if (gps1_data.latitude.valid && gps2_data.latitude.valid) {
      float lat_diff = gps1_data.latitude.degrees - gps2_data.latitude.degrees;
      float lon_diff =
          gps1_data.longitude.degrees - gps2_data.longitude.degrees;

      printf("GPS1: %.6f°, %.6f°\n", gps1_data.latitude.degrees,
             gps1_data.longitude.degrees);
      printf("GPS2: %.6f°, %.6f°\n", gps2_data.latitude.degrees,
             gps2_data.longitude.degrees);
      printf("Difference: %.6f° lat, %.6f° lon\n", lat_diff, lon_diff);

      // Rough distance calculation (simplified)
      float dist_meters =
          sqrtf(lat_diff * lat_diff + lon_diff * lon_diff) * 111000.0f;
      printf("Approximate separation: %.1f meters\n", dist_meters);
    }
  }

  /* Use case: Choose best GPS based on quality */
  printf("\n=== GPS Selection ===\n");

  nmea_context_t* best_gps = NULL;
  const char* best_gps_name = NULL;

  if (gps1_data.fix_quality > 0 && gps2_data.fix_quality > 0) {
    // Both have fix - choose by HDOP (lower is better)
    if (gps1_data.dop.valid && gps2_data.dop.valid) {
      if (gps1_data.dop.hdop <= gps2_data.dop.hdop) {
        best_gps = &gps1_ctx;
        best_gps_name = "GPS1";
      } else {
        best_gps = &gps2_ctx;
        best_gps_name = "GPS2";
      }
      printf("Selected %s (HDOP: %.1f vs %.1f)\n", best_gps_name,
             gps1_data.dop.hdop, gps2_data.dop.hdop);
    }
  } else if (gps1_data.fix_quality > 0) {
    best_gps = &gps1_ctx;
    best_gps_name = "GPS1";
    printf("Selected GPS1 (only GPS with fix)\n");
  } else if (gps2_data.fix_quality > 0) {
    best_gps = &gps2_ctx;
    best_gps_name = "GPS2";
    printf("Selected GPS2 (only GPS with fix)\n");
  } else {
    printf("No GPS has valid fix\n");
  }

  /* Cleanup all contexts */
  printf("\nCleaning up...\n");
  nmea_cleanup(&gps1_ctx);
  nmea_cleanup(&gps2_ctx);
  nmea_cleanup(&sensor_ctx);

  printf("Done\n");
  return 0;
}
