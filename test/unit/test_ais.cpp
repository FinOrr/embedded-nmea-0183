/**
 * @file test_ais.cpp
 * @brief Unit tests for AIS module parsers
 */

#include <modules/nmea_ais.h>

#include "test_helpers.h"

class NmeaAisTest : public NmeaTestBase {};

/* ========================================================================== */
/*                              VDM / VDO TESTS                               */
/* ========================================================================== */

TEST_F(NmeaAisTest, VDM_ValidSentence) {
  // !--VDM,total,sentno,seq,channel,payload,fillbits*hh
  char sentence_buf[256];
  const char* sentence = make_sentence("!AIVDM,2,1,3,A,ABCDE,2", sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.vdm_total_sentences, 2);
  EXPECT_EQ(ais.vdm_sentence_number, 1);
  EXPECT_EQ(ais.vdm_sequential_id, 3);
  EXPECT_EQ(ais.vdm_channel, 'A');
  EXPECT_STREQ(ais.vdm_payload, "ABCDE");
  EXPECT_EQ(ais.vdm_fill_bits, 2);
  EXPECT_TRUE(ais.vdm_valid);
}

TEST_F(NmeaAisTest, VDO_ValidSentence) {
  char sentence_buf[256];
  const char* sentence = make_sentence("!AIVDO,1,1,0,B,HELLO,5", sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.vdm_total_sentences, 1);
  EXPECT_EQ(ais.vdm_sentence_number, 1);
  EXPECT_EQ(ais.vdm_sequential_id, 0);
  EXPECT_EQ(ais.vdm_channel, 'B');
  EXPECT_STREQ(ais.vdm_payload, "HELLO");
  EXPECT_EQ(ais.vdm_fill_bits, 5);
  EXPECT_TRUE(ais.vdo_valid);
}

/* ========================================================================== */
/*                              ABK / ABM / BBM                               */
/* ========================================================================== */

TEST_F(NmeaAisTest, ABK_ValidSentence) {
  char sentence_buf[256];
  const char* sentence =
      make_sentence("$SDABK,123456789,A,7,2,1", sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.abk_mmsi, 123456789u);
  EXPECT_EQ(ais.abk_channel, 'A');
  EXPECT_EQ(ais.abk_message_id, 7);
  EXPECT_EQ(ais.abk_sequence_number, 2);
  EXPECT_EQ(ais.abk_ack_type, 1);
  EXPECT_TRUE(ais.abk_valid);
}

TEST_F(NmeaAisTest, ABM_ValidSentence) {
  char sentence_buf[256];
  const char* sentence =
      make_sentence("!AIABM,1,1,0,123456789,1,25,HELLO-WORLD,2", sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.abm_total_sentences, 1);
  EXPECT_EQ(ais.abm_sentence_number, 1);
  EXPECT_EQ(ais.abm_sequential_id, 0);
  EXPECT_EQ(ais.abm_destination_mmsi, 123456789u);
  EXPECT_EQ(ais.abm_channel, 1);
  EXPECT_EQ(ais.abm_message_id, 25);
  EXPECT_STREQ(ais.abm_data, "HELLO-WORLD");
  EXPECT_EQ(ais.abm_fill_bits, 2);
  EXPECT_TRUE(ais.abm_valid);
}

TEST_F(NmeaAisTest, BBM_ValidSentence) {
  char sentence_buf[256];
  const char* sentence =
      make_sentence("!AIBBM,2,2,9,1,25,DATA-STR,0", sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.bbm_total_sentences, 2);
  EXPECT_EQ(ais.bbm_sentence_number, 2);
  EXPECT_EQ(ais.bbm_sequential_id, 9);
  EXPECT_EQ(ais.bbm_channel, 1);
  EXPECT_EQ(ais.bbm_message_id, 25);
  EXPECT_STREQ(ais.bbm_data, "DATA-STR");
  EXPECT_EQ(ais.bbm_fill_bits, 0);
  EXPECT_TRUE(ais.bbm_valid);
}

/* ========================================================================== */
/*                              AIR / ACA / ACS                               */
/* ========================================================================== */

TEST_F(NmeaAisTest, AIR_ValidSentence) {
  char sentence_buf[256];
  const char* sentence = make_sentence(
      "$SDAIR,123456789,1,0,2,1,987654321,3,0,B,12,34,56", sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.air_station1_mmsi, 123456789u);
  EXPECT_EQ(ais.air_station1_msg1, 1);
  EXPECT_EQ(ais.air_station1_msg1_subsection, 0);
  EXPECT_EQ(ais.air_station1_msg2, 2);
  EXPECT_EQ(ais.air_station1_msg2_subsection, 1);
  EXPECT_EQ(ais.air_station2_mmsi, 987654321u);
  EXPECT_EQ(ais.air_station2_msg, 3);
  EXPECT_EQ(ais.air_station2_msg_subsection, 0);
  EXPECT_EQ(ais.air_channel, 'B');
  EXPECT_EQ(ais.air_msg_id1_1_slot, 12);
  EXPECT_EQ(ais.air_msg_id1_2_slot, 34);
  EXPECT_EQ(ais.air_msg_id2_1_slot, 56);
  EXPECT_TRUE(ais.air_valid);
}

TEST_F(NmeaAisTest, ACA_ValidSentence) {
  char sentence_buf[256];
  const char* sentence = make_sentence(
      "$SDACA,5,4807.038,N,01131.000,E,4707.038,S,01031.000,W,2,2087,25,2091,"
      "25,3,1,M,1,123519",
      sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.aca_sequence, 5);
  EXPECT_TRUE(float_equals(ais.aca_ne_latitude.degrees, 48.1173));
  EXPECT_TRUE(float_equals(ais.aca_ne_longitude.degrees, 11.5166667));
  EXPECT_TRUE(float_equals(ais.aca_sw_latitude.degrees, -47.1173));
  EXPECT_TRUE(float_equals(ais.aca_sw_longitude.degrees, -10.5166667));
  EXPECT_EQ(ais.aca_transition_zone, 2);
  EXPECT_EQ(ais.aca_channel_a, 2087);
  EXPECT_EQ(ais.aca_channel_a_bandwidth, 25);
  EXPECT_EQ(ais.aca_channel_b, 2091);
  EXPECT_EQ(ais.aca_channel_b_bandwidth, 25);
  EXPECT_EQ(ais.aca_txrx_mode, 3);
  EXPECT_EQ(ais.aca_power, 1);
  EXPECT_EQ(ais.aca_info_source, 'M');
  EXPECT_EQ(ais.aca_in_use, 1);
  EXPECT_EQ(ais.aca_in_use_time.hour, 12);
  EXPECT_EQ(ais.aca_in_use_time.minute, 35);
  EXPECT_EQ(ais.aca_in_use_time.second, 19);
  EXPECT_TRUE(ais.aca_valid);
}

TEST_F(NmeaAisTest, ACS_ValidSentence) {
  char sentence_buf[256];
  const char* sentence =
      make_sentence("$SDACS,7,123456789,123519,30,11,2025", sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.acs_sequence, 7);
  EXPECT_EQ(ais.acs_mmsi, 123456789u);
  EXPECT_EQ(ais.acs_receipt_time.hour, 12);
  EXPECT_EQ(ais.acs_receipt_time.minute, 35);
  EXPECT_EQ(ais.acs_receipt_time.second, 19);
  EXPECT_EQ(ais.acs_day, 30);
  EXPECT_EQ(ais.acs_month, 11);
  EXPECT_EQ(ais.acs_year, 2025);
  EXPECT_TRUE(ais.acs_valid);
}

/* ========================================================================== */
/*                              LRF / LRI                                     */
/* ========================================================================== */

TEST_F(NmeaAisTest, LRF_ValidSentence) {
  char sentence_buf[256];
  const char* sentence =
      make_sentence("$SDLRF,3,123456789,REQNAME,FUNCTION,OK", sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.lrf_sequence, 3);
  EXPECT_EQ(ais.lrf_requestor_mmsi, 123456789u);
  EXPECT_STREQ(ais.lrf_requestor_name, "REQNAME");
  EXPECT_STREQ(ais.lrf_function_request, "FUNCTION");
  EXPECT_STREQ(ais.lrf_function_reply, "OK");
  EXPECT_TRUE(ais.lrf_valid);
}

TEST_F(NmeaAisTest, LRI_ValidSentence) {
  char sentence_buf[256];
  const char* sentence = make_sentence(
      "$SDLRI,1,A,123456789,987654321,4807.038,N,01131.000,E,4707.038,S,01031."
      "000,W",
      sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.lri_sequence, 1);
  EXPECT_EQ(ais.lri_control_flag, 'A');
  EXPECT_EQ(ais.lri_requestor_mmsi, 123456789u);
  EXPECT_EQ(ais.lri_destination_mmsi, 987654321u);
  EXPECT_TRUE(float_equals(ais.lri_ne_latitude.degrees, 48.1173));
  EXPECT_TRUE(float_equals(ais.lri_ne_longitude.degrees, 11.5166667));
  EXPECT_TRUE(float_equals(ais.lri_sw_latitude.degrees, -47.1173));
  EXPECT_TRUE(float_equals(ais.lri_sw_longitude.degrees, -10.5166667));
  EXPECT_TRUE(ais.lri_valid);
}

/* ========================================================================== */
/*                              SSD / VSD                                     */
/* ========================================================================== */

TEST_F(NmeaAisTest, SSD_ValidSentence) {
  char sentence_buf[256];
  const char* sentence =
      make_sentence("$SDSSD,CALLSGN,SHIPNAME,100,200,10,12,1,SD", sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_STREQ(ais.ssd_callsign, "CALLSGN");
  EXPECT_STREQ(ais.ssd_name, "SHIPNAME");
  EXPECT_EQ(ais.ssd_pos_ref_a, 100);
  EXPECT_EQ(ais.ssd_pos_ref_b, 200);
  EXPECT_EQ(ais.ssd_pos_ref_c, 10);
  EXPECT_EQ(ais.ssd_pos_ref_d, 12);
  EXPECT_EQ(ais.ssd_dte, 1);
  EXPECT_STREQ(ais.ssd_source, "SD");
  EXPECT_TRUE(ais.ssd_valid);
}

TEST_F(NmeaAisTest, VSD_ValidSentence) {
  char sentence_buf[256];
  const char* sentence =
      make_sentence("$SDVSD,45,3.4,12,DESTPORT,123519,30,11,2,3", sentence_buf);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_ais_state_t ais;
  EXPECT_EQ(nmea_get_ais_data(&ctx, &ais), NMEA_OK);

  EXPECT_EQ(ais.vsd_ship_type, 45);
  EXPECT_TRUE(float_equals(ais.vsd_draught, 3.4f));
  EXPECT_EQ(ais.vsd_persons, 12);
  EXPECT_STREQ(ais.vsd_destination, "DESTPORT");
  EXPECT_EQ(ais.vsd_eta_time.hour, 12);
  EXPECT_EQ(ais.vsd_eta_time.minute, 35);
  EXPECT_EQ(ais.vsd_eta_time.second, 19);
  EXPECT_EQ(ais.vsd_eta_day, 30);
  EXPECT_EQ(ais.vsd_eta_month, 11);
  EXPECT_EQ(ais.vsd_regional_flag1, 2);
  EXPECT_EQ(ais.vsd_regional_flag2, 3);
  EXPECT_TRUE(ais.vsd_valid);
}
