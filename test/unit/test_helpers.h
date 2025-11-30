/**
 * @file test_helpers.h
 * @brief Common test utilities and helpers for NMEA parser unit tests
 */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <gtest/gtest.h>
#include <nmea.h>

#include <cmath>
#include <cstring>

/**
 * @brief Test fixture base class for NMEA parser tests
 */
class NmeaTestBase : public ::testing::Test {
 protected:
  nmea_context_t ctx;
  char buffer[256];
  nmea_config_t config;

  void SetUp() override {
    memset(&ctx, 0, sizeof(ctx));
    memset(buffer, 0, sizeof(buffer));
    memset(&config, 0, sizeof(config));

    // Configure parser with all modules enabled
    config.enabled_modules = 0xFFFFFFFF;  // Enable all modules
    config.validate_checksums = true;
    config.error_callback = nullptr;
    config.error_callback_user_data = nullptr;

    ASSERT_EQ(nmea_init(&ctx, &config), NMEA_OK);
  }

  void TearDown() override { nmea_cleanup(&ctx); }

  /**
   * @brief Helper to parse a sentence and check result
   */
  nmea_result_t parse(const char* sentence) {
    return nmea_parse(&ctx, sentence, strlen(sentence), buffer, sizeof(buffer));
  }

  /**
   * @brief Helper to compare floats with tolerance
   */
  bool float_equals(float a, float b, float epsilon = 0.0001f) {
    return std::fabs(a - b) < epsilon;
  }

  uint8_t get_checksum(const char* sentence) {
    const char* start = sentence;
    // Skip leading start character for NMEA ('$', '!' for AIS)
    if (*start == '$' || *start == '!') start++;

    uint8_t checksum = 0;
    for (size_t i = 0; start[i] != '\0' && start[i] != '*'; i++) {
      checksum ^= (uint8_t)start[i];
    }
    return checksum;
  }

  /**
   * @brief Helper to create a sentence with checksum appended
   * @param sentence_without_checksum The sentence without checksum (e.g.,
   * "$GPGGA,123519,...")
   * @param output_buffer Buffer to write complete sentence to (must be large
   * enough)
   * @return Pointer to output_buffer
   */
  const char* make_sentence(const char* sentence_without_checksum,
                            char* output_buffer) {
    uint8_t cs = get_checksum(sentence_without_checksum);
    snprintf(output_buffer, 256, "%s*%02X", sentence_without_checksum, cs);
    return output_buffer;
  }
};

/**
 * @brief Macro for testing float equality with custom message
 */
#define EXPECT_FLOAT_EQ_MSG(val1, val2, msg)                                \
  EXPECT_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<float>, \
                      val1, val2)                                           \
      << msg

#endif /* TEST_HELPERS_H */
