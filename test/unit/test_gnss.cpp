/**
 * @file test_gnss.cpp
 * @brief Unit tests for GNSS module parsers
 */

#include <modules/nmea_gnss.h>

#include "test_helpers.h"

class NmeaGnssTest : public NmeaTestBase {};

/* ========================================================================== */
/*                              GGA TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaGnssTest, GGA_ValidSentence) {
  const char* sentence =
      "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);

  // Check time
  EXPECT_EQ(gnss.utc_time.hour, 12);
  EXPECT_EQ(gnss.utc_time.minute, 35);
  EXPECT_EQ(gnss.utc_time.second, 19);

  // Check latitude (48°07.038'N = 48.1173°)
  EXPECT_TRUE(float_equals(gnss.latitude.degrees, 48.1173, 0.0001));
  EXPECT_TRUE(gnss.latitude.valid);

  // Check longitude (11°31.000'E = 11.51667°)
  EXPECT_TRUE(float_equals(gnss.longitude.degrees, 11.51667, 0.0001));
  EXPECT_TRUE(gnss.longitude.valid);

  // Check quality
  EXPECT_EQ(gnss.fix_quality, NMEA_QUALITY_GPS);

  // Check satellite count
  EXPECT_EQ(gnss.satellites_used, 8);

  // Check HDOP
  EXPECT_TRUE(float_equals(gnss.dop.hdop, 0.9f));
  EXPECT_TRUE(gnss.dop.valid);

  // Check altitude
  EXPECT_TRUE(float_equals(gnss.altitude_meters, 545.4f));
  EXPECT_TRUE(gnss.altitude_valid);

  // Check geoidal separation
  EXPECT_TRUE(float_equals(gnss.geoid_separation_meters, 46.9f));
  EXPECT_TRUE(gnss.geoid_separation_valid);
}

TEST_F(NmeaGnssTest, GGA_EmptyFields) {
  const char* sentence = "$GPGGA,,,,,,,,,,,,,*7A";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);
}

TEST_F(NmeaGnssTest, GGA_InvalidChecksum) {
  const char* sentence =
      "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*FF";
  EXPECT_EQ(parse(sentence), NMEA_ERROR_CHECKSUM_FAILED);
}

/* ========================================================================== */
/*                              RMC TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaGnssTest, RMC_ValidSentence) {
  const char* sentence =
      "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);

  // Check time
  EXPECT_EQ(gnss.utc_time.hour, 12);
  EXPECT_EQ(gnss.utc_time.minute, 35);
  EXPECT_EQ(gnss.utc_time.second, 19);

  // Check status
  EXPECT_EQ(gnss.status, 'A');
  EXPECT_TRUE(gnss.data_valid);

  // Check latitude
  EXPECT_TRUE(float_equals(gnss.latitude.degrees, 48.1173, 0.0001));

  // Check longitude
  EXPECT_TRUE(float_equals(gnss.longitude.degrees, 11.51667, 0.0001));

  // Check speed (knots)
  EXPECT_TRUE(float_equals(gnss.speed.knots, 22.4f));
  EXPECT_TRUE(gnss.speed.valid);

  // Check course
  EXPECT_TRUE(float_equals(gnss.course.degrees, 84.4f));
  EXPECT_TRUE(gnss.course.valid);

  // Check date
  EXPECT_EQ(gnss.utc_date.day, 23);
  EXPECT_EQ(gnss.utc_date.month, 3);
  EXPECT_EQ(gnss.utc_date.year, 2094);  // Full year

  // Check magnetic variation (West is negative)
  EXPECT_TRUE(float_equals(gnss.magnetic_variation, -3.1f));
  EXPECT_EQ(gnss.magnetic_variation_direction, 'W');
  EXPECT_TRUE(gnss.magnetic_variation_valid);
}

TEST_F(NmeaGnssTest, RMC_InvalidStatus) {
  const char* sentence = "$GPRMC,123519,V,,,,,,,230394,,*33";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);
  EXPECT_EQ(gnss.status, 'V');  // Void/invalid
  EXPECT_FALSE(gnss.data_valid);
}

/* ========================================================================== */
/*                              GLL TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaGnssTest, GLL_ValidSentence) {
  const char* sentence = "$GPGLL,4807.038,N,01131.000,E,123519,A,A*48";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);

  // Check latitude
  EXPECT_TRUE(float_equals(gnss.latitude.degrees, 48.1173, 0.0001));

  // Check longitude
  EXPECT_TRUE(float_equals(gnss.longitude.degrees, 11.51667, 0.0001));

  // Check time
  EXPECT_EQ(gnss.utc_time.hour, 12);
  EXPECT_EQ(gnss.utc_time.minute, 35);
  EXPECT_EQ(gnss.utc_time.second, 19);

  // Check status
  EXPECT_EQ(gnss.status, 'A');

  // Check mode
  EXPECT_EQ(gnss.mode_indicator, 'A');
}

/* ========================================================================== */
/*                              GNS TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaGnssTest, GNS_ValidSentence) {
  const char* sentence =
      "$GPGNS,123519,4807.038,N,01131.000,E,AA,08,0.9,545.4,46.9,,,V*17";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);

  // Check time
  EXPECT_EQ(gnss.utc_time.hour, 12);
  EXPECT_EQ(gnss.utc_time.minute, 35);
  EXPECT_EQ(gnss.utc_time.second, 19);

  // Check position
  EXPECT_TRUE(float_equals(gnss.latitude.degrees, 48.1173, 0.0001));
  EXPECT_TRUE(float_equals(gnss.longitude.degrees, 11.51667, 0.0001));

  // Check satellite count
  EXPECT_EQ(gnss.satellites_used, 8);

  // Check HDOP
  EXPECT_TRUE(float_equals(gnss.dop.hdop, 0.9f));
}

/* ========================================================================== */
/*                              GSA TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaGnssTest, GSA_ValidSentence) {
  const char* sentence = "$GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);

  // Check mode
  EXPECT_EQ(gnss.fix_selection, NMEA_FIX_SELECTION_AUTO);

  // Check fix type
  EXPECT_EQ(gnss.fix_mode, NMEA_FIX_MODE_3D);

  // Check satellite PRNs (stored sequentially, empty fields skipped)
  EXPECT_EQ(gnss.satellite_prns[0], 4);
  EXPECT_EQ(gnss.satellite_prns[1], 5);
  EXPECT_EQ(gnss.satellite_prns[2], 9);
  EXPECT_EQ(gnss.satellite_prns[3], 12);
  EXPECT_EQ(gnss.satellite_prns[4], 24);
  EXPECT_EQ(gnss.satellites_used, 5);

  // Check DOP values
  EXPECT_TRUE(float_equals(gnss.dop.pdop, 2.5f));
  EXPECT_TRUE(float_equals(gnss.dop.hdop, 1.3f));
  EXPECT_TRUE(float_equals(gnss.dop.vdop, 2.1f));
  EXPECT_TRUE(gnss.dop.valid);
}

/* ========================================================================== */
/*                              GSV TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaGnssTest, GSV_ValidSentence) {
  const char* sentence =
      "$GPGSV,3,1,11,03,03,111,00,04,15,270,00,06,01,010,00,13,06,292,00*74";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);

  // Check total satellites in view
  EXPECT_EQ(gnss.satellites_in_view, 11);

  // Check first satellite info
  EXPECT_EQ(gnss.satellites[0].prn, 3);
  EXPECT_EQ(gnss.satellites[0].elevation, 3);
  EXPECT_EQ(gnss.satellites[0].azimuth, 111);
  EXPECT_EQ(gnss.satellites[0].snr, 0);
  EXPECT_TRUE(gnss.satellites[0].valid);
}

/* ========================================================================== */
/*                              GST TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaGnssTest, GST_ValidSentence) {
  const char* sentence = "$GPGST,123519,0.2,1.5,1.2,45.0,1.0,1.5,2.0*47";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);

  // Check time
  EXPECT_EQ(gnss.utc_time.hour, 12);
  EXPECT_EQ(gnss.utc_time.minute, 35);
  EXPECT_EQ(gnss.utc_time.second, 19);

  // Check RMS deviation
  EXPECT_TRUE(float_equals(gnss.error_stats.rms_std_dev, 0.2f));

  // Check standard deviations
  EXPECT_TRUE(float_equals(gnss.error_stats.std_dev_major, 1.5f));
  EXPECT_TRUE(float_equals(gnss.error_stats.std_dev_minor, 1.2f));
  EXPECT_TRUE(float_equals(gnss.error_stats.orientation, 45.0f));
  EXPECT_TRUE(float_equals(gnss.error_stats.std_dev_lat, 1.0f));
  EXPECT_TRUE(float_equals(gnss.error_stats.std_dev_lon, 1.5f));
  EXPECT_TRUE(float_equals(gnss.error_stats.std_dev_alt, 2.0f));

  // Check valid flag
  EXPECT_TRUE(gnss.error_stats.valid);
}

/* ========================================================================== */
/*                              GBS TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaGnssTest, GBS_ValidSentence) {
  const char* sentence = "$GPGBS,123519,1.2,1.5,2.0,03,0.5,0.8,1.2*44";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);

  // Check time
  EXPECT_EQ(gnss.utc_time.hour, 12);
  EXPECT_EQ(gnss.utc_time.minute, 35);
  EXPECT_EQ(gnss.utc_time.second, 19);

  // Check expected errors
  EXPECT_TRUE(float_equals(gnss.expected_error_lat, 1.2f));
  EXPECT_TRUE(float_equals(gnss.expected_error_lon, 1.5f));
  EXPECT_TRUE(float_equals(gnss.expected_error_alt, 2.0f));

  // Check failed satellite
  EXPECT_EQ(gnss.failed_satellite_id, 3);

  // Check valid flag
  EXPECT_TRUE(gnss.gbs_valid);
}

/* ========================================================================== */
/*                          COORDINATE PARSING TESTS                          */
/* ========================================================================== */

TEST_F(NmeaGnssTest, CoordinateSouth) {
  const char* sentence = "$GPGLL,3350.100,S,15110.200,E,123519,A,A*55";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);

  // South latitude should be negative
  EXPECT_TRUE(float_equals(gnss.latitude.degrees, -33.835, 0.001));
}

TEST_F(NmeaGnssTest, CoordinateWest) {
  const char* sentence = "$GPGLL,3350.100,N,12210.200,W,123519,A,A*5E";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_gnss_state_t gnss;
  EXPECT_EQ(nmea_get_gnss_data(&ctx, &gnss), NMEA_OK);

  // West longitude should be negative
  EXPECT_TRUE(float_equals(gnss.longitude.degrees, -122.17, 0.001));
}
