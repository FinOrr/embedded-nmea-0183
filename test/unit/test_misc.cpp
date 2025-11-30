/**
 * @file test_misc.cpp
 * @brief Unit tests for Miscellaneous module parsers
 */

#include <modules/nmea_misc.h>

#include "test_helpers.h"

class NmeaMiscTest : public NmeaTestBase {};

/* ========================================================================== */
/*                              XDR TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, XDR_Temperature) {
  char sentence[256];
  make_sentence("$IIXDR,C,19.52,C,TempAir", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_STREQ(misc.xdr_type, "C");
  EXPECT_TRUE(float_equals(misc.xdr_measurement, 19.52f));
  EXPECT_STREQ(misc.xdr_units, "C");
  EXPECT_STREQ(misc.xdr_id, "TempAir");
  EXPECT_TRUE(misc.xdr_valid);
}

TEST_F(NmeaMiscTest, XDR_Pressure) {
  char sentence[256];
  make_sentence("$IIXDR,P,1.02481,B,Barometer", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_STREQ(misc.xdr_type, "P");
  EXPECT_TRUE(float_equals(misc.xdr_measurement, 1.02481f));
  EXPECT_STREQ(misc.xdr_units, "B");
  EXPECT_STREQ(misc.xdr_id, "Barometer");
  EXPECT_TRUE(misc.xdr_valid);
}

/* ========================================================================== */
/*                              XTE TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, XTE_ValidSentence) {
  char sentence[256];
  make_sentence("$GPXTE,A,A,0.67,L,N,A", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_EQ(misc.xte_status1, 'A');
  EXPECT_EQ(misc.xte_status2, 'A');
  EXPECT_TRUE(float_equals(misc.xte_magnitude, 0.67f));
  EXPECT_EQ(misc.xte_direction, 'L');
  EXPECT_EQ(misc.xte_mode, 'A');
  EXPECT_TRUE(misc.xte_valid);
}

TEST_F(NmeaMiscTest, XTE_RightSteer) {
  char sentence[256];
  make_sentence("$GPXTE,A,A,0.25,R,N,D", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_TRUE(float_equals(misc.xte_magnitude, 0.25f));
  EXPECT_EQ(misc.xte_direction, 'R');
  EXPECT_EQ(misc.xte_mode, 'D');
  EXPECT_TRUE(misc.xte_valid);
}

/* ========================================================================== */
/*                              XTR TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, XTR_ValidSentence) {
  char sentence[256];
  make_sentence("$GPXTR,0.52,L", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_TRUE(float_equals(misc.xtr_magnitude, 0.52f));
  EXPECT_EQ(misc.xtr_direction, 'L');
  EXPECT_TRUE(misc.xtr_valid);
}

/* ========================================================================== */
/*                              ZDA TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, ZDA_ValidSentence) {
  // Custom scenario: UTC time 05:12:30 on 15-Mar-2024 with local zone offset
  // +05:30
  char sentence[256];
  make_sentence("$GPZDA,051230,15,03,2024,+05,30", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  // Check UTC time
  EXPECT_EQ(misc.zda_time.hour, 5);
  EXPECT_EQ(misc.zda_time.minute, 12);
  EXPECT_EQ(misc.zda_time.second, 30);

  // Check date
  EXPECT_EQ(misc.zda_date.day, 15);
  EXPECT_EQ(misc.zda_date.month, 3);
  EXPECT_EQ(misc.zda_date.year, 2024);

  // Check local zone offset
  EXPECT_EQ(misc.zda_local_zone_hours, 5);
  EXPECT_EQ(misc.zda_local_zone_minutes, 30);

  EXPECT_TRUE(misc.zda_valid);
}

TEST_F(NmeaMiscTest, ZDA_CustomTimezone) {
  // Custom scenario: UTC time 07:15:30 on 21-Dec-2023 with local zone offset
  // -03:30
  char sentence[256];
  make_sentence("$GPZDA,071530,21,12,2023,-03,30", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  // Check UTC time
  EXPECT_EQ(misc.zda_time.hour, 7);
  EXPECT_EQ(misc.zda_time.minute, 15);
  EXPECT_EQ(misc.zda_time.second, 30);

  // Check date
  EXPECT_EQ(misc.zda_date.day, 21);
  EXPECT_EQ(misc.zda_date.month, 12);
  EXPECT_EQ(misc.zda_date.year, 2023);

  // Check local zone offset
  EXPECT_EQ(misc.zda_local_zone_hours, -3);
  EXPECT_EQ(misc.zda_local_zone_minutes, 30);

  EXPECT_TRUE(misc.zda_valid);
}

/* ========================================================================== */
/*                              ZDL TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, ZDL_ValidSentence) {
  char sentence[256];
  make_sentence("$GPZDL,123456,5.2,W", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  // Check time to point
  EXPECT_EQ(misc.zdl_time_to_point.hour, 12);
  EXPECT_EQ(misc.zdl_time_to_point.minute, 34);
  EXPECT_EQ(misc.zdl_time_to_point.second, 56);

  // Check distance and type
  EXPECT_TRUE(float_equals(misc.zdl_distance, 5.2f));
  EXPECT_EQ(misc.zdl_point_type, 'W');

  EXPECT_TRUE(misc.zdl_valid);
}

/* ========================================================================== */
/*                              ZFO TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, ZFO_ValidSentence) {
  char sentence[256];
  make_sentence("$GPZFO,091234,012345,ORIGIN", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  // Check UTC
  EXPECT_EQ(misc.zfo_utc.hour, 9);
  EXPECT_EQ(misc.zfo_utc.minute, 12);
  EXPECT_EQ(misc.zfo_utc.second, 34);

  // Check elapsed time
  EXPECT_EQ(misc.zfo_elapsed_time.hour, 1);
  EXPECT_EQ(misc.zfo_elapsed_time.minute, 23);
  EXPECT_EQ(misc.zfo_elapsed_time.second, 45);

  // Check origin ID
  EXPECT_STREQ(misc.zfo_origin_id, "ORIGIN");

  EXPECT_TRUE(misc.zfo_valid);
}

/* ========================================================================== */
/*                              ZTG TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, ZTG_ValidSentence) {
  char sentence[256];
  make_sentence("$GPZTG,141530,023015,DEST01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  // Check UTC
  EXPECT_EQ(misc.ztg_utc.hour, 14);
  EXPECT_EQ(misc.ztg_utc.minute, 15);
  EXPECT_EQ(misc.ztg_utc.second, 30);

  // Check time to go
  EXPECT_EQ(misc.ztg_time_to_go.hour, 2);
  EXPECT_EQ(misc.ztg_time_to_go.minute, 30);
  EXPECT_EQ(misc.ztg_time_to_go.second, 15);

  // Check destination ID
  EXPECT_STREQ(misc.ztg_dest_id, "DEST01");

  EXPECT_TRUE(misc.ztg_valid);
}

/* ========================================================================== */
/*                              DTM TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, DTM_ValidSentence) {
  char sentence[256];
  make_sentence("$GPDTM,W84,,0.0,N,0.0,E,0.0,W84", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_STREQ(misc.dtm_local_datum, "W84");
  EXPECT_TRUE(float_equals(misc.dtm_lat_offset, 0.0f));
  EXPECT_EQ(misc.dtm_lat_offset_dir, 'N');
  EXPECT_TRUE(float_equals(misc.dtm_lon_offset, 0.0f));
  EXPECT_EQ(misc.dtm_lon_offset_dir, 'E');
  EXPECT_TRUE(float_equals(misc.dtm_altitude_offset, 0.0f));
  EXPECT_STREQ(misc.dtm_ref_datum, "W84");

  EXPECT_TRUE(misc.dtm_valid);
}

TEST_F(NmeaMiscTest, DTM_WithOffsets) {
  char sentence[256];
  make_sentence("$GPDTM,999,,0.08,N,0.07,E,-47.7,W84", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_STREQ(misc.dtm_local_datum, "999");
  EXPECT_TRUE(float_equals(misc.dtm_lat_offset, 0.08f));
  EXPECT_EQ(misc.dtm_lat_offset_dir, 'N');
  EXPECT_TRUE(float_equals(misc.dtm_lon_offset, 0.07f));
  EXPECT_EQ(misc.dtm_lon_offset_dir, 'E');
  EXPECT_TRUE(float_equals(misc.dtm_altitude_offset, -47.7f));

  EXPECT_TRUE(misc.dtm_valid);
}

/* ========================================================================== */
/*                              CUR TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, CUR_ValidSentence) {
  char sentence[256];
  make_sentence("$IICUR,A,1,2,45.3,234.5,T,1.2,120.0,180.5,T,P", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_EQ(misc.cur_validity, 'A');
  EXPECT_EQ(misc.cur_data_set_number, 1);
  EXPECT_EQ(misc.cur_layer_number, 2);
  EXPECT_TRUE(float_equals(misc.cur_depth, 45.3f));
  EXPECT_TRUE(float_equals(misc.cur_direction, 234.5f));
  EXPECT_EQ(misc.cur_direction_ref, 'T');
  EXPECT_TRUE(float_equals(misc.cur_speed, 1.2f));
  EXPECT_TRUE(float_equals(misc.cur_ref_layer_depth, 120.0f));
  EXPECT_TRUE(float_equals(misc.cur_heading, 180.5f));
  EXPECT_EQ(misc.cur_heading_ref, 'T');
  EXPECT_EQ(misc.cur_speed_ref, 'P');

  EXPECT_TRUE(misc.cur_valid);
}

/* ========================================================================== */
/*                              FSI TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, FSI_ValidSentence) {
  char sentence[256];
  make_sentence("$IIFSI,156800,156800,c,5,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_STREQ(misc.fsi_tx_freq, "156800");
  EXPECT_STREQ(misc.fsi_rx_freq, "156800");
  EXPECT_EQ(misc.fsi_mode, 'c');
  EXPECT_EQ(misc.fsi_power_level, 5);
  EXPECT_EQ(misc.fsi_status_flag, 'R');

  EXPECT_TRUE(misc.fsi_valid);
}

/* ========================================================================== */
/*                              SFI TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, SFI_ValidSentence) {
  char sentence[256];
  make_sentence("$IISFI,2,1,156025,c,156050,c,156075,c", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_EQ(misc.sfi_total_sentences, 2);
  EXPECT_EQ(misc.sfi_sentence_number, 1);

  // Check first three frequencies
  EXPECT_STREQ(misc.sfi_frequencies[0], "156025");
  EXPECT_EQ(misc.sfi_modes[0], 'c');
  EXPECT_STREQ(misc.sfi_frequencies[1], "156050");
  EXPECT_EQ(misc.sfi_modes[1], 'c');
  EXPECT_STREQ(misc.sfi_frequencies[2], "156075");
  EXPECT_EQ(misc.sfi_modes[2], 'c');

  EXPECT_TRUE(misc.sfi_valid);
}

/* ========================================================================== */
/*                              RPM TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, RPM_ValidSentence) {
  char sentence[256];
  make_sentence("$IIRPM,E,1,1500.0,25.5,A", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_EQ(misc.rpm_source, 'E');
  EXPECT_EQ(misc.rpm_number, 1);
  EXPECT_TRUE(float_equals(misc.rpm_speed, 1500.0f));
  EXPECT_TRUE(float_equals(misc.rpm_pitch, 25.5f));
  EXPECT_EQ(misc.rpm_status, 'A');

  EXPECT_TRUE(misc.rpm_valid);
}

TEST_F(NmeaMiscTest, RPM_ShaftSource) {
  char sentence[256];
  make_sentence("$IIRPM,S,2,850.5,0.0,A", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_EQ(misc.rpm_source, 'S');
  EXPECT_EQ(misc.rpm_number, 2);
  EXPECT_TRUE(float_equals(misc.rpm_speed, 850.5f));

  EXPECT_TRUE(misc.rpm_valid);
}

/* ========================================================================== */
/*                              DDC TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, DDC_ValidSentence) {
  char sentence[256];
  make_sentence("$IIDDC,N,75,D,R", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_EQ(misc.ddc_dimming_preset, 'N');
  EXPECT_EQ(misc.ddc_brightness, 75);
  EXPECT_EQ(misc.ddc_color_palette, 'D');
  EXPECT_EQ(misc.ddc_status_flag, 'R');

  EXPECT_TRUE(misc.ddc_valid);
}

/* ========================================================================== */
/*                              EPV TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, EPV_ValidSentence) {
  char sentence[256];
  make_sentence("$IIEPV,R,GYRO,SN12345,100,45.5", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_EQ(misc.epv_status_flag, 'R');
  EXPECT_STREQ(misc.epv_equipment_type, "GYRO");
  EXPECT_STREQ(misc.epv_unique_id, "SN12345");
  EXPECT_EQ(misc.epv_property_id, 100);
  EXPECT_STREQ(misc.epv_property_value, "45.5");

  EXPECT_TRUE(misc.epv_valid);
}

/* ========================================================================== */
/*                              TRL TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, TRL_ValidSentence) {
  char sentence[256];
  make_sentence("$IITRL,5,1,0,311200,143022,311200,150130,3", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_EQ(misc.trl_total_entries, 5);
  EXPECT_EQ(misc.trl_entry_number, 1);
  EXPECT_EQ(misc.trl_message_id, 0);

  EXPECT_STREQ(misc.trl_switch_off_date, "311200");
  EXPECT_EQ(misc.trl_switch_off_time.hour, 14);
  EXPECT_EQ(misc.trl_switch_off_time.minute, 30);
  EXPECT_EQ(misc.trl_switch_off_time.second, 22);

  EXPECT_STREQ(misc.trl_switch_on_date, "311200");
  EXPECT_EQ(misc.trl_switch_on_time.hour, 15);
  EXPECT_EQ(misc.trl_switch_on_time.minute, 1);
  EXPECT_EQ(misc.trl_switch_on_time.second, 30);

  EXPECT_EQ(misc.trl_reason_code, 3);

  EXPECT_TRUE(misc.trl_valid);
}

/* ========================================================================== */
/*                              WAT TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, WAT_ValidSentence) {
  char sentence[256];
  make_sentence("$IIWAT,S,123456,ALM1,LOC1,LOC2,5,H,N,Water detected",
                sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_misc_state_t misc;
  EXPECT_EQ(nmea_get_misc_data(&ctx, &misc), NMEA_OK);

  EXPECT_EQ(misc.wat_message_type, 'S');

  EXPECT_EQ(misc.wat_timestamp.hour, 12);
  EXPECT_EQ(misc.wat_timestamp.minute, 34);
  EXPECT_EQ(misc.wat_timestamp.second, 56);

  EXPECT_STREQ(misc.wat_alarm_type, "ALM1");
  EXPECT_STREQ(misc.wat_location1, "LOC1");
  EXPECT_STREQ(misc.wat_location2, "LOC2");
  EXPECT_EQ(misc.wat_detection_point, 5);
  EXPECT_EQ(misc.wat_alarm_condition, 'H');
  EXPECT_EQ(misc.wat_override, 'N');
  EXPECT_STREQ(misc.wat_description, "Water detected");

  EXPECT_TRUE(misc.wat_valid);
}

/* ========================================================================== */
/*                              GEN TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaMiscTest, GEN_ValidSentence) {
  // GEN is a generic binary sentence - just test basic parsing
  char sentence[256];
  make_sentence("$IIGEN,1,1,0,ABCD1234", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  // GEN doesn't populate specific state fields, just verify it parses OK
}
