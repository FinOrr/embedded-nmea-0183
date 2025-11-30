/**
 * @file test_attitude.cpp
 * @brief Unit tests for Attitude and Motion module
 */

#include <modules/nmea_attitude.h>

#include "test_helpers.h"

class NmeaAttitudeTest : public NmeaTestBase {};

/* ========================================================================== */
/*                              HRM TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaAttitudeTest, HRM_ValidSentence) {
  char sentence[128];
  make_sentence("$HEHRM,12.5,5.2,15.3,13.7,A,16.8,15.1,121519,23,03", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check heel angle
  EXPECT_TRUE(float_equals(att.hrm_heel_angle, 12.5f));

  // Check roll period
  EXPECT_TRUE(float_equals(att.hrm_roll_period, 5.2f));

  // Check roll amplitudes
  EXPECT_TRUE(float_equals(att.hrm_roll_amplitude_port, 15.3f));
  EXPECT_TRUE(float_equals(att.hrm_roll_amplitude_starboard, 13.7f));

  // Check status
  EXPECT_EQ(att.hrm_status, 'A');

  // Check peak hold values
  EXPECT_TRUE(float_equals(att.hrm_peak_hold_port, 16.8f));
  EXPECT_TRUE(float_equals(att.hrm_peak_hold_starboard, 15.1f));

  // Check peak reset time
  EXPECT_EQ(att.hrm_peak_reset_time.hour, 12);
  EXPECT_EQ(att.hrm_peak_reset_time.minute, 15);
  EXPECT_EQ(att.hrm_peak_reset_time.second, 19);

  // Check peak reset date
  EXPECT_EQ(att.hrm_peak_reset_day, 23);
  EXPECT_EQ(att.hrm_peak_reset_month, 3);

  // Check valid flag
  EXPECT_TRUE(att.hrm_valid);
}

TEST_F(NmeaAttitudeTest, HRM_MinimalFields) {
  char sentence[128];
  make_sentence("$HEHRM,5.0,,,,,,,,,", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check heel angle is parsed
  EXPECT_TRUE(float_equals(att.hrm_heel_angle, 5.0f));

  // Should still be valid even with minimal fields
  EXPECT_TRUE(att.hrm_valid);
}

TEST_F(NmeaAttitudeTest, HRM_InvalidStatus) {
  char sentence[128];
  make_sentence("$HEHRM,10.5,6.0,12.0,11.5,V,15.0,14.0,120000,15,06", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Status should be 'V' (invalid)
  EXPECT_EQ(att.hrm_status, 'V');

  // But sentence is still parsed (valid flag relates to sentence parsing, not
  // data status)
  EXPECT_TRUE(att.hrm_valid);
}

TEST_F(NmeaAttitudeTest, HRM_EmptyFields) {
  char sentence[128];
  make_sentence("$HEHRM,,,,,,,,,,,", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Should be marked valid even with all empty fields (sentence was
  // successfully parsed)
  EXPECT_TRUE(att.hrm_valid);
}

TEST_F(NmeaAttitudeTest, HRM_HeelAngleRange) {
  nmea_attitude_state_t att;
  char sentence[128];

  // Test positive heel angle (starboard)
  make_sentence("$HEHRM,15.0,5.0,10.0,10.0,A,12.0,12.0,120000,01,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_TRUE(float_equals(att.hrm_heel_angle, 15.0f));

  // Test negative heel angle (port)
  make_sentence("$HEHRM,-10.5,5.0,10.0,10.0,A,12.0,12.0,120000,01,01",
                sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_TRUE(float_equals(att.hrm_heel_angle, -10.5f));
}

TEST_F(NmeaAttitudeTest, HRM_RollAmplitudes) {
  char sentence[128];
  make_sentence("$HEHRM,5.0,4.5,20.5,18.3,A,25.0,22.0,120000,15,03", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check both port and starboard amplitudes
  EXPECT_TRUE(float_equals(att.hrm_roll_amplitude_port, 20.5f));
  EXPECT_TRUE(float_equals(att.hrm_roll_amplitude_starboard, 18.3f));
}

TEST_F(NmeaAttitudeTest, HRM_PeakHoldValues) {
  char sentence[128];
  make_sentence("$HEHRM,8.0,5.0,12.0,11.0,A,22.5,21.3,120000,10,05", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check peak hold values
  EXPECT_TRUE(float_equals(att.hrm_peak_hold_port, 22.5f));
  EXPECT_TRUE(float_equals(att.hrm_peak_hold_starboard, 21.3f));
}

TEST_F(NmeaAttitudeTest, HRM_TimeAndDate) {
  char sentence[128];
  make_sentence("$HEHRM,5.0,4.0,10.0,10.0,A,15.0,15.0,235959,31,12", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check peak reset time (23:59:59)
  EXPECT_EQ(att.hrm_peak_reset_time.hour, 23);
  EXPECT_EQ(att.hrm_peak_reset_time.minute, 59);
  EXPECT_EQ(att.hrm_peak_reset_time.second, 59);

  // Check peak reset date (31st of December)
  EXPECT_EQ(att.hrm_peak_reset_day, 31);
  EXPECT_EQ(att.hrm_peak_reset_month, 12);
}

/* ========================================================================== */
/*                              PRC TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaAttitudeTest, PRC_ValidSentence) {
  char sentence[128];
  make_sentence("$HEPRC,45.5,A,1200.0,P,85.0,P,B,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check lever demand
  EXPECT_TRUE(float_equals(att.prc_lever_demand, 45.5f));

  // Check lever status
  EXPECT_EQ(att.prc_lever_status, 'A');

  // Check RPM demand and mode
  EXPECT_TRUE(float_equals(att.prc_rpm_demand, 1200.0f));
  EXPECT_EQ(att.prc_rpm_mode, 'P');

  // Check pitch demand and mode
  EXPECT_TRUE(float_equals(att.prc_pitch_demand, 85.0f));
  EXPECT_EQ(att.prc_pitch_mode, 'P');

  // Check operating location
  EXPECT_EQ(att.prc_location, 'B');  // Bridge

  // Check engine number
  EXPECT_EQ(att.prc_engine_number, 1);

  // Check valid flag
  EXPECT_TRUE(att.prc_valid);
}

TEST_F(NmeaAttitudeTest, PRC_LeverDemand) {
  nmea_attitude_state_t att;
  char sentence[128];

  // Test positive lever demand (ahead)
  make_sentence("$HEPRC,75.0,A,1500.0,R,90.0,D,B,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_TRUE(float_equals(att.prc_lever_demand, 75.0f));

  // Test negative lever demand (astern)
  make_sentence("$HEPRC,-50.0,A,800.0,R,45.0,D,B,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_TRUE(float_equals(att.prc_lever_demand, -50.0f));

  // Test zero (stop)
  make_sentence("$HEPRC,0.0,A,0.0,R,0.0,D,B,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_TRUE(float_equals(att.prc_lever_demand, 0.0f));
}

TEST_F(NmeaAttitudeTest, PRC_RPMControl) {
  char sentence[128];
  make_sentence("$HEPRC,50.0,A,1800.0,R,80.0,P,B,02", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check RPM demand
  EXPECT_TRUE(float_equals(att.prc_rpm_demand, 1800.0f));

  // Check RPM mode (R = RPM)
  EXPECT_EQ(att.prc_rpm_mode, 'R');
}

TEST_F(NmeaAttitudeTest, PRC_PitchControl) {
  char sentence[128];
  make_sentence("$HEPRC,60.0,A,1500.0,P,70.5,D,B,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check pitch demand
  EXPECT_TRUE(float_equals(att.prc_pitch_demand, 70.5f));

  // Check pitch mode (D = Degrees)
  EXPECT_EQ(att.prc_pitch_mode, 'D');
}

TEST_F(NmeaAttitudeTest, PRC_OperatingLocation) {
  // Test different operating locations
  const char* locations[] = {"B", "P", "S", "C", "E", "W"};
  const char* location_names[] = {"Bridge",  "Port",        "Starboard",
                                  "Central", "Engine room", "Wing"};

  for (int i = 0; i < 6; i++) {
    char sentence_template[100];
    snprintf(sentence_template, sizeof(sentence_template),
             "$HEPRC,50.0,A,1200.0,R,80.0,P,%s,01", locations[i]);

    char sentence[128];
    make_sentence(sentence_template, sentence);
    EXPECT_EQ(parse(sentence), NMEA_OK)
        << "Failed for location: " << location_names[i];

    nmea_attitude_state_t att;
    EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
    EXPECT_EQ(att.prc_location, locations[i][0])
        << "Location mismatch for: " << location_names[i];
  }
}

TEST_F(NmeaAttitudeTest, PRC_EngineNumber) {
  nmea_attitude_state_t att;
  char sentence[128];

  // Test engine number 1
  make_sentence("$HEPRC,50.0,A,1200.0,R,80.0,P,B,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.prc_engine_number, 1);

  // Test engine number 2
  make_sentence("$HEPRC,50.0,A,1200.0,R,80.0,P,B,02", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.prc_engine_number, 2);
}

TEST_F(NmeaAttitudeTest, PRC_StatusCodes) {
  nmea_attitude_state_t att;
  char sentence[128];

  // Test RPM mode: P (Propeller pitch), R (RPM), V (Void/not available)
  make_sentence("$HEPRC,50.0,A,1200.0,P,80.0,P,B,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.prc_rpm_mode, 'P');

  make_sentence("$HEPRC,50.0,A,1200.0,R,80.0,P,B,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.prc_rpm_mode, 'R');

  // Test pitch mode: P (Propeller pitch), D (Degrees), V (Void)
  make_sentence("$HEPRC,50.0,A,1200.0,R,80.0,D,B,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.prc_pitch_mode, 'D');
}

TEST_F(NmeaAttitudeTest, PRC_EmptyFields) {
  char sentence[128];
  make_sentence("$HEPRC,,,,,,,,,", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Should still be marked valid
  EXPECT_TRUE(att.prc_valid);
}

/* ========================================================================== */
/*                              TRC TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaAttitudeTest, TRC_ValidSentence) {
  char sentence[128];
  make_sentence("$HETRC,01,800.0,P,45.0,D,180.0,B,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check thruster number
  EXPECT_EQ(att.trc_thruster_number, 1);

  // Check RPM demand and mode
  EXPECT_TRUE(float_equals(att.trc_rpm_demand, 800.0f));
  EXPECT_EQ(att.trc_rpm_mode, 'P');

  // Check pitch demand and mode
  EXPECT_TRUE(float_equals(att.trc_pitch_demand, 45.0f));
  EXPECT_EQ(att.trc_pitch_mode, 'D');

  // Check azimuth demand
  EXPECT_TRUE(float_equals(att.trc_azimuth_demand, 180.0f));

  // Check operating location
  EXPECT_EQ(att.trc_location, 'B');

  // Check status flag
  EXPECT_EQ(att.trc_status_flag, 'R');  // Remote

  // Check valid flag
  EXPECT_TRUE(att.trc_valid);
}

TEST_F(NmeaAttitudeTest, TRC_BowThruster) {
  // Bow thrusters are odd numbered (1, 3, 5, etc.)
  char sentence[128];
  make_sentence("$HETRC,01,750.0,R,50.0,D,90.0,B,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  EXPECT_EQ(att.trc_thruster_number, 1);
  EXPECT_TRUE(att.trc_thruster_number % 2 == 1);  // Odd = bow
}

TEST_F(NmeaAttitudeTest, TRC_SternThruster) {
  // Stern thrusters are even numbered (2, 4, 6, etc.)
  char sentence[128];
  make_sentence("$HETRC,02,750.0,R,50.0,D,270.0,B,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  EXPECT_EQ(att.trc_thruster_number, 2);
  EXPECT_TRUE(att.trc_thruster_number % 2 == 0);  // Even = stern
}

TEST_F(NmeaAttitudeTest, TRC_RPMControl) {
  char sentence[128];
  make_sentence("$HETRC,01,1200.0,R,60.0,D,0.0,B,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check RPM demand
  EXPECT_TRUE(float_equals(att.trc_rpm_demand, 1200.0f));

  // Check RPM mode (R = RPM)
  EXPECT_EQ(att.trc_rpm_mode, 'R');
}

TEST_F(NmeaAttitudeTest, TRC_PitchControl) {
  char sentence[128];
  make_sentence("$HETRC,01,800.0,P,35.5,D,45.0,B,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check pitch demand
  EXPECT_TRUE(float_equals(att.trc_pitch_demand, 35.5f));

  // Check pitch mode (D = Degrees)
  EXPECT_EQ(att.trc_pitch_mode, 'D');
}

TEST_F(NmeaAttitudeTest, TRC_AzimuthControl) {
  // Test various azimuth angles (0-360 degrees)
  const float azimuth_angles[] = {0.0f, 45.0f, 90.0f, 180.0f, 270.0f, 359.9f};

  for (float angle : azimuth_angles) {
    char sentence_template[128];
    snprintf(sentence_template, sizeof(sentence_template),
             "$HETRC,01,800.0,R,45.0,D,%.1f,B,R", angle);

    char sentence[128];
    make_sentence(sentence_template, sentence);
    EXPECT_EQ(parse(sentence), NMEA_OK) << "Failed for azimuth: " << angle;

    nmea_attitude_state_t att;
    EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
    EXPECT_TRUE(float_equals(att.trc_azimuth_demand, angle))
        << "Azimuth mismatch for: " << angle;
  }
}

TEST_F(NmeaAttitudeTest, TRC_OperatingLocation) {
  nmea_attitude_state_t att;
  char sentence[128];

  make_sentence("$HETRC,01,800.0,R,45.0,D,180.0,B,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.trc_location, 'B');  // Bridge

  make_sentence("$HETRC,01,800.0,R,45.0,D,180.0,C,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.trc_location, 'C');  // Central
}

TEST_F(NmeaAttitudeTest, TRC_StatusFlag) {
  nmea_attitude_state_t att;
  char sentence[128];

  // Test Remote status
  make_sentence("$HETRC,01,800.0,R,45.0,D,180.0,B,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.trc_status_flag, 'R');  // Remote

  // Test Central/Local status
  make_sentence("$HETRC,01,800.0,R,45.0,D,180.0,B,C", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.trc_status_flag, 'C');  // Central
}

TEST_F(NmeaAttitudeTest, TRC_EmptyFields) {
  char sentence[128];
  make_sentence("$HETRC,,,,,,,,,", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Should still be marked valid
  EXPECT_TRUE(att.trc_valid);
}

/* ========================================================================== */
/*                              TRD TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaAttitudeTest, TRD_ValidSentence) {
  char sentence[128];
  make_sentence("$HETRD,01,785.0,P,44.2,D,179.5", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check thruster number
  EXPECT_EQ(att.trd_thruster_number, 1);

  // Check RPM response and mode
  EXPECT_TRUE(float_equals(att.trd_rpm_response, 785.0f));
  EXPECT_EQ(att.trd_rpm_mode, 'P');

  // Check pitch response and mode
  EXPECT_TRUE(float_equals(att.trd_pitch_response, 44.2f));
  EXPECT_EQ(att.trd_pitch_mode, 'D');

  // Check azimuth response
  EXPECT_TRUE(float_equals(att.trd_azimuth_response, 179.5f));

  // Check valid flag
  EXPECT_TRUE(att.trd_valid);
}

TEST_F(NmeaAttitudeTest, TRD_ResponseValues) {
  // TRD provides response (actual) values which may differ from TRC demand
  // values
  char sentence[128];
  make_sentence("$HETRD,01,795.0,R,43.8,D,178.2", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Response values should reflect actual thruster state
  EXPECT_TRUE(float_equals(att.trd_rpm_response, 795.0f));
  EXPECT_TRUE(float_equals(att.trd_pitch_response, 43.8f));
  EXPECT_TRUE(float_equals(att.trd_azimuth_response, 178.2f));
}

TEST_F(NmeaAttitudeTest, TRD_AllModes) {
  nmea_attitude_state_t att;
  char sentence[128];

  // Test RPM mode P (Propeller pitch)
  make_sentence("$HETRD,01,800.0,P,45.0,D,180.0", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.trd_rpm_mode, 'P');

  // Test RPM mode R (RPM)
  make_sentence("$HETRD,01,800.0,R,45.0,D,180.0", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.trd_rpm_mode, 'R');

  // Test pitch mode D (Degrees)
  EXPECT_EQ(att.trd_pitch_mode, 'D');

  // Test pitch mode P (Propeller pitch)
  make_sentence("$HETRD,01,800.0,R,45.0,P,180.0", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_EQ(att.trd_pitch_mode, 'P');
}

TEST_F(NmeaAttitudeTest, TRD_AzimuthResponse) {
  nmea_attitude_state_t att;
  char sentence[128];

  // Test azimuth response at various angles
  make_sentence("$HETRD,01,800.0,R,45.0,D,0.0", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_TRUE(float_equals(att.trd_azimuth_response, 0.0f));

  make_sentence("$HETRD,01,800.0,R,45.0,D,90.0", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_TRUE(float_equals(att.trd_azimuth_response, 90.0f));

  make_sentence("$HETRD,01,800.0,R,45.0,D,270.0", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
  EXPECT_TRUE(float_equals(att.trd_azimuth_response, 270.0f));
}

TEST_F(NmeaAttitudeTest, TRD_EmptyFields) {
  char sentence[128];
  make_sentence("$HETRD,,,,,,,", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Should still be marked valid
  EXPECT_TRUE(att.trd_valid);
}

/* ========================================================================== */
/*                         STATE MANAGEMENT TESTS                             */
/* ========================================================================== */

TEST_F(NmeaAttitudeTest, GetAttitudeData) {
  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);
}

TEST_F(NmeaAttitudeTest, MultiSentenceUpdate) {
  char sentence[128];

  // Parse multiple different sentence types
  make_sentence("$HEHRM,10.0,5.0,12.0,11.0,A,15.0,14.0,120000,15,06", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  make_sentence("$HEPRC,50.0,A,1200.0,R,80.0,P,B,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  make_sentence("$HETRC,01,800.0,R,45.0,D,180.0,B,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  make_sentence("$HETRD,01,785.0,R,44.2,D,179.5", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  // Get state and verify all data is present
  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&ctx, &att), NMEA_OK);

  // Check HRM data
  EXPECT_TRUE(att.hrm_valid);
  EXPECT_TRUE(float_equals(att.hrm_heel_angle, 10.0f));

  // Check PRC data
  EXPECT_TRUE(att.prc_valid);
  EXPECT_TRUE(float_equals(att.prc_lever_demand, 50.0f));

  // Check TRC data
  EXPECT_TRUE(att.trc_valid);
  EXPECT_EQ(att.trc_thruster_number, 1);

  // Check TRD data
  EXPECT_TRUE(att.trd_valid);
  EXPECT_TRUE(float_equals(att.trd_rpm_response, 785.0f));
}

TEST_F(NmeaAttitudeTest, StateInitialization) {
  // Create new context
  nmea_context_t test_ctx;
  nmea_config_t test_config = {};
  test_config.enabled_modules = 0xFFFFFFFF;
  test_config.validate_checksums = true;

  EXPECT_EQ(nmea_init(&test_ctx, &test_config), NMEA_OK);

  // Get attitude data from fresh context
  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(&test_ctx, &att), NMEA_OK);

  // All valid flags should be false initially
  EXPECT_FALSE(att.hrm_valid);
  EXPECT_FALSE(att.prc_valid);
  EXPECT_FALSE(att.trc_valid);
  EXPECT_FALSE(att.trd_valid);

  nmea_cleanup(&test_ctx);
}

TEST_F(NmeaAttitudeTest, NullParameterHandling) {
  // Null state pointer
  EXPECT_EQ(nmea_get_attitude_data(&ctx, nullptr), NMEA_ERROR_NULL_PARAM);

  // Null context pointer
  nmea_attitude_state_t att;
  EXPECT_EQ(nmea_get_attitude_data(nullptr, &att), NMEA_ERROR_NULL_PARAM);
}
