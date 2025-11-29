/**
 * @file test_heading.cpp
 * @brief Unit tests for Heading module parsers
 */

#include <modules/nmea_heading.h>

#include "test_helpers.h"

class NmeaHeadingTest : public NmeaTestBase {};

/* ========================================================================== */
/*                              HDG TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaHeadingTest, HDG_ValidSentence) {
  char sentence[256];
  make_sentence("$HCHDG,98.3,0.0,E,12.6,W", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  // Check magnetic sensor heading
  EXPECT_TRUE(float_equals(heading.heading_magnetic_sensor, 98.3f));

  // Check deviation
  EXPECT_TRUE(float_equals(heading.magnetic_deviation, 0.0f));
  EXPECT_EQ(heading.deviation_direction, 'E');

  // Check variation
  EXPECT_TRUE(float_equals(heading.magnetic_variation, -12.6f));
  EXPECT_EQ(heading.variation_direction, 'W');

  EXPECT_TRUE(heading.hdg_valid);
}

TEST_F(NmeaHeadingTest, HDG_EmptyDeviationVariation) {
  // Deviation and variation null when unknown
  char sentence[256];
  make_sentence("$HCHDG,101.1,,,,", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  EXPECT_TRUE(float_equals(heading.heading_magnetic_sensor, 101.1f));
  EXPECT_TRUE(heading.hdg_valid);
}

/* ========================================================================== */
/*                              HDT TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaHeadingTest, HDT_ValidSentence) {
  char sentence[256];
  make_sentence("$HEHDT,274.07,T", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  EXPECT_TRUE(float_equals(heading.heading_true, 274.07f));
  EXPECT_TRUE(heading.hdt_valid);
}

/* ========================================================================== */
/*                              ROT TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaHeadingTest, ROT_ValidSentence) {
  char sentence[256];
  make_sentence("$TIROT,35.6,A", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  EXPECT_TRUE(float_equals(heading.rate_of_turn, 35.6f));
  EXPECT_EQ(heading.rot_status, 'A');
  EXPECT_TRUE(heading.rot_valid);
}

TEST_F(NmeaHeadingTest, ROT_NegativeTurn) {
  // Negative = port turn
  char sentence[256];
  make_sentence("$TIROT,-12.5,A", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  EXPECT_TRUE(float_equals(heading.rate_of_turn, -12.5f));
  EXPECT_EQ(heading.rot_status, 'A');
  EXPECT_TRUE(heading.rot_valid);
}

/* ========================================================================== */
/*                              VTG TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaHeadingTest, VTG_ValidSentence) {
  char sentence[256];
  make_sentence("$GPVTG,54.7,T,34.4,M,5.5,N,10.2,K,A", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  // Check course
  EXPECT_TRUE(float_equals(heading.course_true, 54.7f));
  EXPECT_TRUE(float_equals(heading.course_magnetic, 34.4f));

  // Check speed
  EXPECT_TRUE(float_equals(heading.speed_knots, 5.5f));
  EXPECT_TRUE(float_equals(heading.speed_kmh, 10.2f));

  // Check mode
  EXPECT_EQ(heading.vtg_mode, 'A');
  EXPECT_TRUE(heading.vtg_valid);
}

TEST_F(NmeaHeadingTest, VTG_DifferentialMode) {
  char sentence[256];
  make_sentence("$GPVTG,140.5,T,120.3,M,3.2,N,5.9,K,D", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  EXPECT_EQ(heading.vtg_mode, 'D');
  EXPECT_TRUE(heading.vtg_valid);
}

/* ========================================================================== */
/*                              VDR TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaHeadingTest, VDR_ValidSentence) {
  char sentence[256];
  make_sentence("$IIVDR,10.5,T,12.4,M,2.3,N", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  EXPECT_TRUE(float_equals(heading.current_direction_true, 10.5f));
  EXPECT_TRUE(float_equals(heading.current_direction_magnetic, 12.4f));
  EXPECT_TRUE(float_equals(heading.current_speed_knots, 2.3f));
  EXPECT_TRUE(heading.vdr_valid);
}

/* ========================================================================== */
/*                              OSD TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaHeadingTest, OSD_ValidSentence) {
  char sentence[256];
  make_sentence("$IIOSD,230.1,A,180.5,M,12.5,M,5.2,2.1,N", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  // Check heading
  EXPECT_TRUE(float_equals(heading.osd_heading_true, 230.1f));
  EXPECT_EQ(heading.osd_heading_status, 'A');

  // Check course
  EXPECT_TRUE(float_equals(heading.vessel_course_true, 180.5f));
  EXPECT_EQ(heading.course_reference, 'M');

  // Check speed
  EXPECT_TRUE(float_equals(heading.vessel_speed, 12.5f));
  EXPECT_EQ(heading.speed_reference, 'M');
  EXPECT_EQ(heading.speed_units, 'N');

  // Check set and drift
  EXPECT_TRUE(float_equals(heading.vessel_set_true, 5.2f));
  EXPECT_TRUE(float_equals(heading.vessel_drift, 2.1f));

  EXPECT_TRUE(heading.osd_valid);
}

/* ========================================================================== */
/*                              HMR TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaHeadingTest, HMR_ValidSentence) {
  char sentence[256];
  make_sentence("$IIHMR,SNS1,SNS2,5.0,3.2,A,180.5,A,M,2.1,E,185.3,A,T,0.0,E,15.2,W", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  // Check sensor IDs
  EXPECT_STREQ(heading.sensor1_id, "SNS1");
  EXPECT_STREQ(heading.sensor2_id, "SNS2");

  // Check differences
  EXPECT_TRUE(float_equals(heading.set_difference, 5.0f));
  EXPECT_TRUE(float_equals(heading.actual_difference, 3.2f));
  EXPECT_EQ(heading.difference_warning, 'A');

  // Check sensor 1
  EXPECT_TRUE(float_equals(heading.sensor1_heading, 180.5f));
  EXPECT_EQ(heading.sensor1_status, 'A');
  EXPECT_EQ(heading.sensor1_type, 'M');
  EXPECT_TRUE(float_equals(heading.sensor1_deviation, 2.1f));
  EXPECT_EQ(heading.sensor1_deviation_dir, 'E');

  // Check sensor 2
  EXPECT_TRUE(float_equals(heading.sensor2_heading, 185.3f));
  EXPECT_EQ(heading.sensor2_status, 'A');
  EXPECT_EQ(heading.sensor2_type, 'T');
  EXPECT_TRUE(float_equals(heading.sensor2_deviation, 0.0f));
  EXPECT_EQ(heading.sensor2_deviation_dir, 'E');

  // Check variation
  EXPECT_TRUE(float_equals(heading.hmr_variation, -15.2f));
  EXPECT_EQ(heading.hmr_variation_dir, 'W');

  EXPECT_TRUE(heading.hmr_valid);
}

/* ========================================================================== */
/*                              HTC TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaHeadingTest, HTC_ValidSentence) {
  char sentence[256];
  make_sentence("$IIHTC,A,10.5,R,H,R,35.0,5.0,2.5,15.2,270.5,1.0,90.3,T,A,A,A,268.7", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_heading_state_t heading;
  EXPECT_EQ(nmea_get_heading_data(&ctx, &heading), NMEA_OK);

  // Check override and rudder
  EXPECT_EQ(heading.htc_override, 'A');
  EXPECT_TRUE(float_equals(heading.commanded_rudder_angle, 10.5f));
  EXPECT_EQ(heading.rudder_direction, 'R');

  // Check modes
  EXPECT_EQ(heading.steering_mode, 'H');
  EXPECT_EQ(heading.turn_mode, 'R');

  // Check limits
  EXPECT_TRUE(float_equals(heading.rudder_limit, 35.0f));
  EXPECT_TRUE(float_equals(heading.off_heading_limit, 5.0f));

  // Check commands
  EXPECT_TRUE(float_equals(heading.commanded_radius, 2.5f));
  EXPECT_TRUE(float_equals(heading.commanded_rate_of_turn, 15.2f));
  EXPECT_TRUE(float_equals(heading.commanded_heading, 270.5f));
  EXPECT_TRUE(float_equals(heading.off_track_limit, 1.0f));
  EXPECT_TRUE(float_equals(heading.commanded_track, 90.3f));

  // Check reference and status
  EXPECT_EQ(heading.heading_reference, 'T');
  EXPECT_EQ(heading.rudder_status, 'A');
  EXPECT_EQ(heading.off_heading_status, 'A');
  EXPECT_EQ(heading.off_track_status, 'A');

  EXPECT_TRUE(float_equals(heading.vessel_heading, 268.7f));
  EXPECT_TRUE(heading.htc_valid);
}
