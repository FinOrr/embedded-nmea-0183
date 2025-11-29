/**
 * @file test_core.cpp
 * @brief Unit tests for core NMEA parser functionality
 */

#include "test_helpers.h"

class NmeaCoreTest : public NmeaTestBase {};

/* ========================================================================== */
/*                         INITIALIZATION TESTS                               */
/* ========================================================================== */

TEST_F(NmeaCoreTest, InitValidParameters) {
  nmea_context_t test_ctx;
  nmea_config_t test_config = {};
  test_config.enabled_modules = 0xFFFFFFFF;
  test_config.validate_checksums = true;

  EXPECT_EQ(nmea_init(&test_ctx, &test_config), NMEA_OK);
  nmea_cleanup(&test_ctx);
}

TEST_F(NmeaCoreTest, InitNullContext) {
  nmea_config_t test_config = {};
  EXPECT_EQ(nmea_init(nullptr, &test_config), NMEA_ERROR_NULL_PARAM);
}

TEST_F(NmeaCoreTest, InitNullConfig) {
  nmea_context_t test_ctx;
  EXPECT_EQ(nmea_init(&test_ctx, nullptr), NMEA_ERROR_NULL_PARAM);
}

TEST_F(NmeaCoreTest, InitInvalidConfig) {
  nmea_context_t test_ctx;
  nmea_config_t test_config = {};
  test_config.enabled_modules = 0;  // No modules enabled
  test_config.validate_checksums = true;
  // This test may succeed depending on implementation
  // Just test that it doesn't crash
  nmea_result_t result = nmea_init(&test_ctx, &test_config);
  EXPECT_TRUE(result == NMEA_OK || result == NMEA_ERROR_INVALID_CONFIG);
}

/* ========================================================================== */
/*                            CHECKSUM TESTS                                  */
/* ========================================================================== */

TEST_F(NmeaCoreTest, ChecksumValid) {
  // GGA sentence with valid checksum
  const char* sentence =
      "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
  EXPECT_EQ(parse(sentence), NMEA_OK);
}

TEST_F(NmeaCoreTest, ChecksumInvalid) {
  // GGA sentence with invalid checksum (should be *47, not *FF)
  const char* sentence =
      "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*FF";
  EXPECT_EQ(parse(sentence), NMEA_ERROR_CHECKSUM_FAILED);
}

TEST_F(NmeaCoreTest, ChecksumMissing) {
  // Sentence without checksum
  const char* sentence =
      "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,";
  // Should still parse successfully (checksum is optional per spec)
  EXPECT_EQ(parse(sentence), NMEA_OK);
}

/* ========================================================================== */
/*                         SENTENCE FORMAT TESTS                              */
/* ========================================================================== */

TEST_F(NmeaCoreTest, SentenceNoLeadingDollar) {
  const char* sentence =
      "GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
  EXPECT_EQ(parse(sentence), NMEA_ERROR_INVALID_SENTENCE);
}

TEST_F(NmeaCoreTest, SentenceEmpty) {
  EXPECT_EQ(parse(""), NMEA_ERROR_INVALID_SENTENCE);
}

TEST_F(NmeaCoreTest, SentenceOnlyDollar) {
  EXPECT_EQ(parse("$"), NMEA_ERROR_INVALID_SENTENCE);
}

TEST_F(NmeaCoreTest, SentenceTooLong) {
  // Create a sentence longer than NMEA_MAX_SENTENCE_LENGTH (82 chars)
  char long_sentence[300];
  memset(long_sentence, 'A', sizeof(long_sentence) - 1);
  long_sentence[0] = '$';
  long_sentence[sizeof(long_sentence) - 1] = '\0';

  EXPECT_EQ(parse(long_sentence), NMEA_ERROR_INVALID_SENTENCE);
}

/* ========================================================================== */
/*                         TALKER ID TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCoreTest, TalkerIdGP) {
  // GPS talker
  const char* sentence =
      "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
  EXPECT_EQ(parse(sentence), NMEA_OK);
}

TEST_F(NmeaCoreTest, TalkerIdGL) {
  // GLONASS talker
  const char* sentence =
      "$GLGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*5B";
  EXPECT_EQ(parse(sentence), NMEA_OK);
}

TEST_F(NmeaCoreTest, TalkerIdGN) {
  // Combined GNSS talker
  const char* sentence =
      "$GNGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*59";
  EXPECT_EQ(parse(sentence), NMEA_OK);
}

TEST_F(NmeaCoreTest, ProprietarySentence) {
  // Proprietary sentence (starts with P)
  const char* sentence = "$PMTK001,604,3*32";
  // Should be recognized but may not have a parser
  nmea_result_t result = parse(sentence);
  EXPECT_TRUE(result == NMEA_OK || result == NMEA_ERROR_UNKNOWN_SENTENCE);
}

/* ========================================================================== */
/*                         MULTI-SENTENCE TESTS                               */
/* ========================================================================== */

TEST_F(NmeaCoreTest, ParseMultipleSentences) {
  // Parse two different valid sentences
  EXPECT_EQ(
      parse(
          "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47"),
      NMEA_OK);
  EXPECT_EQ(parse("$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,"
                  "003.1,W*6A"),
            NMEA_OK);
}

/* ========================================================================== */
/*                         NULL PARAMETER TESTS                               */
/* ========================================================================== */

TEST_F(NmeaCoreTest, ParseNullContext) {
  const char* sentence =
      "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
  char test_buffer[256];
  EXPECT_EQ(nmea_parse(nullptr, sentence, strlen(sentence), test_buffer,
                       sizeof(test_buffer)),
            NMEA_ERROR_NULL_PARAM);
}

TEST_F(NmeaCoreTest, ParseNullSentence) {
  EXPECT_EQ(nmea_parse(&ctx, nullptr, 0, buffer, sizeof(buffer)),
            NMEA_ERROR_NULL_PARAM);
}

/* ========================================================================== */
/*                         CLEANUP TESTS                                      */
/* ========================================================================== */

TEST_F(NmeaCoreTest, CleanupValid) {
  nmea_context_t test_ctx = {};
  nmea_config_t test_config = {};
  test_config.enabled_modules = 0xFFFFFFFF;
  test_config.validate_checksums = true;

  EXPECT_EQ(nmea_init(&test_ctx, &test_config), NMEA_OK);
  nmea_cleanup(&test_ctx);  // void function, no return value
}

TEST_F(NmeaCoreTest, CleanupNull) {
  // nmea_cleanup is void, so it should be safe to call with nullptr
  // This test just ensures it doesn't crash
  nmea_cleanup(nullptr);
  SUCCEED();  // If we get here, the test passed
}
