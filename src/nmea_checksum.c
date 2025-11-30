/**
 * @file nmea_checksum.c
 * @brief NMEA sentence checksum calculation and validation
 *
 * Implements XOR checksum functions for NMEA-0183 sentences.
 */

#include <ctype.h>
#include <stdio.h>

#include "nmea_internal.h"

/**
 * @brief Calculate NMEA sentence checksum
 *
 * The checksum is an XOR of all characters between the starter ('$'/'!') and '*'.
 *
 * @param sentence  NMEA sentence (must start with '$')
 * @param length    Length of sentence
 *
 * @return 8-bit checksum value
 */
uint8_t nmea_calculate_checksum(const char* sentence, size_t length) {
  if (sentence == NULL || length == 0) {
    return 0;
  }

  /* Find start of data (after '$' or '!') */
  const char* start = sentence;
  if (*start == '$' || *start == '!') {
    start++;
  }

  /* Calculate XOR checksum until '*' or end */
  uint8_t checksum = 0;
  for (size_t i = 0; start[i] != '\0' && i < length; i++) {
    if (start[i] == '*') {
      break; /* Stop at checksum delimiter */
    }
    checksum ^= (uint8_t)start[i];
  }

  return checksum;
}

/**
 * @brief Extract checksum from sentence
 *
 * Extracts the two-character hex checksum after the '*' delimiter.
 *
 * @param sentence  NMEA sentence
 * @param length    Length of sentence
 * @param checksum  Output pointer for checksum value
 *
 * @return NMEA_OK if checksum found, error otherwise
 */
nmea_result_t nmea_extract_checksum(const char* sentence, size_t length,
                                    uint8_t* checksum) {
  if (sentence == NULL || checksum == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  /* Find '*' delimiter */
  const char* asterisk = NULL;
  for (size_t i = 0; i < length; i++) {
    if (sentence[i] == '*') {
      asterisk = &sentence[i];
      break;
    }
  }

  if (asterisk == NULL) {
    return NMEA_ERROR_INVALID_SENTENCE;
  }

  /* Check if we have at least 2 hex digits after '*' */
  if ((asterisk - sentence) + 3 > (long)length) {
    return NMEA_ERROR_INVALID_SENTENCE;
  }

  /* Parse two hex digits */
  char hex_str[3] = {asterisk[1], asterisk[2], '\0'};

  if (!isxdigit(hex_str[0]) || !isxdigit(hex_str[1])) {
    return NMEA_ERROR_INVALID_SENTENCE;
  }

  unsigned int value;
  if (sscanf(hex_str, "%2X", &value) != 1) {
    return NMEA_ERROR_INVALID_SENTENCE;
  }

  *checksum = (uint8_t)value;
  return NMEA_OK;
}

/**
 * @brief Validate NMEA sentence checksum
 *
 * Calculates the checksum and compares it to the value in the sentence.
 * If no checksum is present (no '*' delimiter), validation passes since
 * checksums are optional in NMEA-0183.
 *
 * @param sentence  Complete NMEA sentence with checksum
 * @param length    Length of sentence
 *
 * @return true if checksum is valid or absent, false if invalid
 */
bool nmea_validate_checksum(const char* sentence, size_t length) {
  if (sentence == NULL || length == 0) {
    return false;
  }

  /* Extract checksum from sentence */
  uint8_t expected_checksum;
  nmea_result_t extract_result = nmea_extract_checksum(sentence, length, &expected_checksum);

  /* If no checksum present, validation passes (checksums are optional) */
  if (extract_result != NMEA_OK) {
    return true;
  }

  /* Calculate actual checksum */
  uint8_t calculated_checksum = nmea_calculate_checksum(sentence, length);

  /* Compare */
  return (calculated_checksum == expected_checksum);
}
