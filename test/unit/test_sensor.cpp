/**
 * @file test_sensor.cpp
 * @brief Unit tests for Sensor module parsers
 */

#include <modules/nmea_sensor.h>

#include "test_helpers.h"

class NmeaSensorTest : public NmeaTestBase {};

/* ========================================================================== */
/*                              DBT TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaSensorTest, DBT_ValidSentence) {
  const char* sentence = "$SDDBT,12.3,f,3.7,M,2.0,F*30";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check depth values
  EXPECT_TRUE(float_equals(sensor.depth_below_transducer_feet, 12.3f));
  EXPECT_TRUE(float_equals(sensor.depth_below_transducer_meters, 3.7f));
  EXPECT_TRUE(float_equals(sensor.depth_below_transducer_fathoms, 2.0f));

  // Check valid flag
  EXPECT_TRUE(sensor.depth_valid);
}

TEST_F(NmeaSensorTest, DBT_EmptyFields) {
  const char* sentence = "$SDDBT,,,,,*69";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);
  EXPECT_FALSE(sensor.depth_valid);
}

/* ========================================================================== */
/*                              DPT TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaSensorTest, DPT_ValidSentence) {
  const char* sentence = "$SDDPT,5.5,0.5,*7E";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check depth
  EXPECT_TRUE(float_equals(sensor.depth_meters, 5.5f));

  // Check offset
  EXPECT_TRUE(float_equals(sensor.depth_offset_meters, 0.5f));

  // Check valid flag
  EXPECT_TRUE(sensor.depth_valid);
}

/* ========================================================================== */
/*                              MTW TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaSensorTest, MTW_ValidSentence) {
  const char* sentence = "$YXMTW,15.5,C*13";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check temperature (MTW reports in Celsius)
  EXPECT_TRUE(float_equals(sensor.water_temperature_celsius, 15.5f));

  // Check valid flag
  EXPECT_TRUE(sensor.water_temperature_valid);
}

/* ========================================================================== */
/*                              MWD TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaSensorTest, MWD_ValidSentence) {
  const char* sentence = "$WIMWD,215.5,T,220.3,M,12.5,N,6.4,M*6E";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check wind direction
  EXPECT_TRUE(float_equals(sensor.wind_direction_true, 215.5f));
  EXPECT_TRUE(float_equals(sensor.wind_direction_magnetic, 220.3f));

  // Check wind speed
  EXPECT_TRUE(float_equals(sensor.wind_speed_knots, 12.5f));
  EXPECT_TRUE(float_equals(sensor.wind_speed_mps, 6.4f));

  // Check valid flag
  EXPECT_TRUE(sensor.wind_valid);
}

/* ========================================================================== */
/*                              MWV TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaSensorTest, MWV_ValidSentence) {
  const char* sentence = "$WIMWV,045.0,R,12.5,N,A*14";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check reference
  EXPECT_EQ(sensor.wind_reference, 'R');  // Relative

  // Check speed (in knots)
  EXPECT_TRUE(float_equals(sensor.wind_speed_knots, 12.5f));

  // Check valid flag
  EXPECT_TRUE(sensor.wind_valid);
}

TEST_F(NmeaSensorTest, MWV_TrueWind) {
  const char* sentence = "$WIMWV,215.0,T,8.5,M,A*2D";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check reference is True
  EXPECT_EQ(sensor.wind_reference, 'T');

  // Check speed in m/s
  EXPECT_TRUE(float_equals(sensor.wind_speed_mps, 8.5f));
}

/* ========================================================================== */
/*                              VBW TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaSensorTest, VBW_ValidSentence) {
  const char* sentence = "$VDVBW,5.2,0.3,A,1.5,0.2,A*53";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check water speed (longitudinal component becomes speed_water_knots)
  EXPECT_TRUE(float_equals(sensor.speed_water_knots, 5.2f));

  // Check ground speed (longitudinal component becomes speed_ground_knots)
  EXPECT_TRUE(float_equals(sensor.speed_ground_knots, 1.5f));

  // Check valid flag
  EXPECT_TRUE(sensor.speed_valid);
}

/* ========================================================================== */
/*                              VHW TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaSensorTest, VHW_ValidSentence) {
  const char* sentence = "$VDVHW,215.5,T,220.3,M,5.5,N,10.2,K*74";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check heading
  EXPECT_TRUE(float_equals(sensor.heading_true, 215.5f));
  EXPECT_TRUE(float_equals(sensor.heading_magnetic, 220.3f));

  // Check speed
  EXPECT_TRUE(float_equals(sensor.water_speed_knots, 5.5f));

  // Check valid flag
  EXPECT_TRUE(sensor.water_speed_heading_valid);
}

/* ========================================================================== */
/*                              VLW TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaSensorTest, VLW_ValidSentence) {
  // Full format: total_water, water_since_reset, total_ground,
  // ground_since_reset
  const char* sentence = "$VDVLW,1234.5,N,123.4,N,987.6,N,98.7,N*5C";

  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check water distance since reset (field 3)
  EXPECT_TRUE(float_equals(sensor.distance_water_nm, 123.4f));

  // Check ground distance since reset (field 7)
  EXPECT_TRUE(float_equals(sensor.distance_ground_nm, 98.7f));

  // Check valid flag
  EXPECT_TRUE(sensor.speed_valid);
}

TEST_F(NmeaSensorTest, VLW_PartialSentence) {
  // Only water distance fields provided
  const char* sentence = "$VDVLW,12.3,N,56.7,N,,,,*5B";

  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check water distance since reset (field 3)
  EXPECT_TRUE(float_equals(sensor.distance_water_nm, 56.7f));

  // Ground distance not provided, so value should be 0 (from context init)
  EXPECT_TRUE(float_equals(sensor.distance_ground_nm, 0.0f));

  // Check valid flag (set based on water distance)
  EXPECT_TRUE(sensor.speed_valid);
}

/* ========================================================================== */
/*                              VPW TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaSensorTest, VPW_ValidSentence) {
  const char* sentence = "$VDVPW,5.5,N,3.2,M*41";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_sensor_state_t sensor;
  EXPECT_EQ(nmea_get_sensor_data(&ctx, &sensor), NMEA_OK);

  // Check speed knots
  EXPECT_TRUE(float_equals(sensor.speed_water_knots, 5.5f));

  // Check valid flag
  EXPECT_TRUE(sensor.speed_valid);
}
