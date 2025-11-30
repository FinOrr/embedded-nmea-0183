/**
 * @file test_radar.cpp
 * @brief Unit tests for Radar module parsers (RSD, TTM, TLL, TLB, TTD)
 */

#include <modules/nmea_radar.h>

#include "test_helpers.h"

class NmeaRadarTest : public NmeaTestBase {};

/* ========================================================================== */
/*                               RSD TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaRadarTest, RSD_ValidSentence) {
  // Example from source comment: cursor range (field 9), cursor bearing (field
  // 10)
  char sentence[256];
  make_sentence(
      "$RARSD,10.5,45.0,5.0,30.0,20.0,90.0,10.0,60.0,15.0,120.0,24.0,N,H",
      sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_radar_state_t radar;
  EXPECT_EQ(nmea_get_radar_data(&ctx, &radar), NMEA_OK);

  EXPECT_TRUE(float_equals(radar.target_distance_nm, 15.0f));
  EXPECT_TRUE(float_equals(radar.target_bearing, 120.0f));
}

/* ========================================================================== */
/*                               TTM TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaRadarTest, TTM_ValidTracking) {
  // Fields: number, distance, bearing, ref, speed, course, ref, dCPA, tCPA,
  // units, name, status, refTarget, time, acquisition
  char sentence[256];
  make_sentence(
      "$RATTM,01,3.5,45.2,T,12.3,270.0,T,0.5,5.0,N,TARGET1,T,R,120530.00,A",
      sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_radar_state_t radar;
  EXPECT_EQ(nmea_get_radar_data(&ctx, &radar), NMEA_OK);

  EXPECT_EQ(radar.target_number, 1u);
  EXPECT_TRUE(float_equals(radar.target_distance_nm, 3.5f));
  EXPECT_TRUE(float_equals(radar.target_bearing, 45.2f));
  EXPECT_TRUE(float_equals(radar.target_speed_knots, 12.3f));
  EXPECT_TRUE(float_equals(radar.target_course, 270.0f));
  EXPECT_TRUE(radar.target_valid);
}

TEST_F(NmeaRadarTest, TTM_LostTarget) {
  char sentence[256];
  make_sentence(
      "$RATTM,02,10.0,180.0,T,0.0,0.0,T,0.0,0.0,N,TGT2,L,,120000.00,A",
      sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_radar_state_t radar;
  EXPECT_EQ(nmea_get_radar_data(&ctx, &radar), NMEA_OK);

  EXPECT_EQ(radar.target_number, 2u);
  EXPECT_FALSE(radar.target_valid);
}

/* ========================================================================== */
/*                               TLL TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaRadarTest, TLL_ValidLatitudeAndStatus) {
  // TLL stores latitude into target_position; status T sets target_valid
  char sentence[256];
  make_sentence("$RATLL,01,3723.46,N,12158.32,W,TARGET1,120530.00,T,R",
                sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_radar_state_t radar;
  EXPECT_EQ(nmea_get_radar_data(&ctx, &radar), NMEA_OK);

  EXPECT_EQ(radar.target_number, 1u);
  EXPECT_TRUE(radar.target_position.valid);
  // 37 degrees 23.46 minutes => 37 + 23.46/60
  float expected_lat = 37.0f + (23.46f / 60.0f);
  EXPECT_TRUE(float_equals(radar.target_position.degrees, expected_lat));
  EXPECT_TRUE(radar.target_valid);
}

TEST_F(NmeaRadarTest, TLL_LostStatus) {
  char sentence[256];
  // Ensure minimum fields (10) are present with empty placeholders
  make_sentence("$RATLL,05,0000.00,N,,,,L,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_radar_state_t radar;
  EXPECT_EQ(nmea_get_radar_data(&ctx, &radar), NMEA_OK);

  EXPECT_EQ(radar.target_number, 5u);
  EXPECT_FALSE(radar.target_valid);
}

/* ========================================================================== */
/*                               TLB TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaRadarTest, TLB_FirstPairNumberCaptured) {
  // Parser captures the first target number if present; labels are ignored
  char sentence[256];
  make_sentence("$RATLB,03,TGT3,07,TGT7,12,TGT12", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_radar_state_t radar;
  EXPECT_EQ(nmea_get_radar_data(&ctx, &radar), NMEA_OK);

  EXPECT_EQ(radar.target_number, 3u);
}

/* ========================================================================== */
/*                               TTD TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaRadarTest, TTD_AcknowledgedStructure) {
  // Parser acknowledges TTD without decoding encapsulated data
  char sentence[256];
  make_sentence("!RATTD,01,01,0,ENCAPSULATED_DATA,0", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
}
