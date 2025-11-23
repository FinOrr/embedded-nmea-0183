/**
 * @file nmea_helpers.c
 * @brief Parsing helper functions
 *
 * Common utilities for parsing NMEA field values.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "nmea_internal.h"

/**
 * @brief Parse a float value from token
 */
nmea_result_t nmea_parse_float(const char* token, float* value) {
  if (value == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  if (nmea_is_empty_token(token)) {
    *value = 0.0f;
    return NMEA_ERROR_NO_DATA;
  }

  char* endptr;
  *value = strtof(token, &endptr);

  if (endptr == token || *endptr != '\0') {
    return NMEA_ERROR_PARSE_FAILED;
  }

  return NMEA_OK;
}

/**
 * @brief Parse an integer value from token
 */
nmea_result_t nmea_parse_int(const char* token, int* value) {
  if (value == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  if (nmea_is_empty_token(token)) {
    *value = 0;
    return NMEA_ERROR_NO_DATA;
  }

  char* endptr;
  long result = strtol(token, &endptr, 10);

  if (endptr == token || *endptr != '\0') {
    return NMEA_ERROR_PARSE_FAILED;
  }

  *value = (int)result;
  return NMEA_OK;
}

/**
 * @brief Parse a coordinate (latitude/longitude) from NMEA format
 *
 * NMEA format: DDMM.MMMM or DDDMM.MMMM
 * Converts to decimal degrees.
 *
 * @param coord_token     Coordinate token (e.g., "4807.038")
 * @param direction_token Direction token (e.g., "N", "S", "E", "W")
 * @param coordinate      Output coordinate structure
 */
nmea_result_t nmea_parse_coordinate(const char* coord_token,
                                    const char* direction_token,
                                    nmea_coordinate_t* coordinate) {
  if (coordinate == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  /* Initialize as invalid */
  coordinate->degrees = 0.0;
  coordinate->valid = false;

  /* Check for empty tokens */
  if (nmea_is_empty_token(coord_token) ||
      nmea_is_empty_token(direction_token)) {
    return NMEA_ERROR_NO_DATA;
  }

  /* Parse coordinate value */
  float value;
  if (nmea_parse_float(coord_token, &value) != NMEA_OK) {
    return NMEA_ERROR_PARSE_FAILED;
  }

  /* Determine if latitude (DDMM.MMMM) or longitude (DDDMM.MMMM) */
  /* Latitude: degrees are first 2 digits, longitude: first 3 digits */
  (void)(value >=
         10000.0f); /* Unused - format is determined by field position */

  /* Extract degrees and minutes */
  int degrees = (int)(value / 100.0f);
  float minutes = value - (degrees * 100.0f);

  /* Convert to decimal degrees */
  float decimal_degrees = degrees + (minutes / 60.0f);

  /* Apply direction (S and W are negative) */
  char direction = direction_token[0];
  if (direction == 'S' || direction == 's' || direction == 'W' ||
      direction == 'w') {
    decimal_degrees = -decimal_degrees;
  } else if (direction != 'N' && direction != 'n' && direction != 'E' &&
             direction != 'e') {
    return NMEA_ERROR_PARSE_FAILED;
  }

  coordinate->degrees = decimal_degrees;
  coordinate->valid = true;

  return NMEA_OK;
}

/**
 * @brief Parse time from NMEA format
 *
 * NMEA format: HHMMSS or HHMMSS.SSS
 *
 * @param token  Time token
 * @param time   Output time structure
 */
nmea_result_t nmea_parse_time(const char* token, nmea_time_t* time) {
  if (time == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  /* Initialize as invalid */
  time->hour = 0;
  time->minute = 0;
  time->second = 0;
  time->millisecond = 0;
  time->valid = false;

  if (nmea_is_empty_token(token)) {
    return NMEA_ERROR_NO_DATA;
  }

  /* Parse as float to handle fractional seconds */
  float value;
  if (nmea_parse_float(token, &value) != NMEA_OK) {
    return NMEA_ERROR_PARSE_FAILED;
  }

  /* Extract hours, minutes, seconds */
  int total_seconds = (int)value;
  float fractional_seconds = value - total_seconds;

  time->hour = total_seconds / 10000;
  time->minute = (total_seconds / 100) % 100;
  time->second = total_seconds % 100;
  time->millisecond = (uint16_t)(fractional_seconds * 1000.0f);

  /* Validate ranges */
  if (time->hour > 23 || time->minute > 59 || time->second > 59) {
    return NMEA_ERROR_PARSE_FAILED;
  }

  time->valid = true;
  return NMEA_OK;
}

/**
 * @brief Parse date from NMEA format
 *
 * NMEA format: DDMMYY
 *
 * @param token  Date token
 * @param date   Output date structure
 */
nmea_result_t nmea_parse_date(const char* token, nmea_date_t* date) {
  if (date == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  /* Initialize as invalid */
  date->year = 0;
  date->month = 0;
  date->day = 0;
  date->valid = false;

  if (nmea_is_empty_token(token)) {
    return NMEA_ERROR_NO_DATA;
  }

  /* Parse as integer */
  int value;
  if (nmea_parse_int(token, &value) != NMEA_OK) {
    return NMEA_ERROR_PARSE_FAILED;
  }

  /* Extract day, month, year */
  date->day = value / 10000;
  date->month = (value / 100) % 100;
  int year_2digit = value % 100;

  /* Convert 2-digit year to 4-digit (assume 2000-2099 range) */
  date->year = 2000 + year_2digit;

  /* Validate ranges */
  if (date->day < 1 || date->day > 31 || date->month < 1 || date->month > 12) {
    return NMEA_ERROR_PARSE_FAILED;
  }

  date->valid = true;
  return NMEA_OK;
}

/**
 * @brief Get library version string
 */
const char* nmea_get_version(void) {
  static char version[16];
  snprintf(version, sizeof(version), "%d.%d.%d", NMEA_VERSION_MAJOR,
           NMEA_VERSION_MINOR, NMEA_VERSION_PATCH);
  return version;
}

/**
 * @brief Get required buffer size for parsing
 */
size_t nmea_get_required_buffer_size(void) { return NMEA_MAX_SENTENCE_LENGTH; }

/**
 * @brief Get size of parser context structure
 */
size_t nmea_get_context_size(void) { return sizeof(nmea_context_t); }
