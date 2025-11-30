#include <gtest/gtest.h>
#include <modules/nmea_system.h>
#include <nmea.h>

#include "test_helpers.h"

class NmeaSystemTest : public NmeaTestBase {};

static void assert_enabled(nmea_context_t* ctx, const char* sid) {
  ASSERT_TRUE(nmea_is_sentence_enabled(ctx, sid))
      << "Sentence not enabled or missing dispatch: " << sid;
}

TEST_F(NmeaSystemTest, HSS_ParseBasic) {
  char out[256];
  const char* s = make_sentence("$INHSS,POINT01,123.45,A", out);
  ASSERT_EQ(parse(s), NMEA_OK);
  nmea_system_state_t sys{};
  ASSERT_EQ(nmea_get_system_data(&ctx, &sys), NMEA_OK);
  EXPECT_STREQ(sys.hss_measurement_point, "POINT01");
  EXPECT_TRUE(float_equals(sys.hss_measurement_value, 123.45f));
  EXPECT_EQ(sys.hss_status, 'A');
  EXPECT_TRUE(sys.hss_valid);
}

TEST_F(NmeaSystemTest, EVE_ParseBasic) {
  char out[256];
  const char* s = make_sentence("$INEVE,120530.00,TAG01,Anchor deployed", out);
  ASSERT_EQ(parse(s), NMEA_OK);
  nmea_system_state_t sys{};
  ASSERT_EQ(nmea_get_system_data(&ctx, &sys), NMEA_OK);
  EXPECT_EQ(sys.eve_time.hour, 12);
  EXPECT_EQ(sys.eve_time.minute, 5);
  EXPECT_TRUE(sys.eve_valid);
  EXPECT_STREQ(sys.eve_tag_code, "TAG01");
  EXPECT_STREQ(sys.eve_description, "Anchor deployed");
}

TEST_F(NmeaSystemTest, ETL_ParseOrder) {
  char out[256];
  const char* s = make_sentence("$INETL,120530.00,O,03,20,B,0", out);
  ASSERT_EQ(parse(s), NMEA_OK);
  nmea_system_state_t sys{};
  ASSERT_EQ(nmea_get_system_data(&ctx, &sys), NMEA_OK);
  EXPECT_EQ(sys.etl_message_type, 'O');
  EXPECT_EQ(sys.etl_position, 3);
  EXPECT_EQ(sys.etl_sub_position, 20);
  EXPECT_EQ(sys.etl_location, 'B');
  EXPECT_EQ(sys.etl_engine_number, 0);
  EXPECT_TRUE(sys.etl_valid);
}

TEST_F(NmeaSystemTest, NRM_ParseMasks) {
  char out[256];
  const char* s = make_sentence("$INNRM,1,2,00001E1F,0FFFFFFF,R", out);
  ASSERT_EQ(parse(s), NMEA_OK);
  nmea_system_state_t sys{};
  ASSERT_EQ(nmea_get_system_data(&ctx, &sys), NMEA_OK);
  EXPECT_EQ(sys.nrm_function_code, 1);
  EXPECT_EQ(sys.nrm_frequency_index, 2);
  EXPECT_EQ(sys.nrm_transmitter_mask, 0x00001E1F);
  EXPECT_EQ(sys.nrm_message_type_mask, 0x0FFFFFFF);
  EXPECT_EQ(sys.nrm_status_flag, 'R');
  EXPECT_TRUE(sys.nrm_valid);
}

TEST_F(NmeaSystemTest, NRX_ParseMessage) {
  char out[256];
  const char* s = make_sentence(
      "$CRNRX,007,001,00,IE69,1,135600,27,06,2001,241,3,A,Message text", out);
  ASSERT_EQ(parse(s), NMEA_OK);
  nmea_system_state_t sys{};
  ASSERT_EQ(nmea_get_system_data(&ctx, &sys), NMEA_OK);
  EXPECT_EQ(sys.nrx_total_sentences, 7);
  EXPECT_EQ(sys.nrx_sentence_number, 1);
  EXPECT_EQ(sys.nrx_message_id, 0);
  EXPECT_STREQ(sys.nrx_message_code, "IE69");
  EXPECT_EQ(sys.nrx_frequency_index, 1);
  EXPECT_EQ(sys.nrx_date.day, 27);
  EXPECT_EQ(sys.nrx_date.month, 6);
  EXPECT_EQ(sys.nrx_date.year, 2001);
  EXPECT_EQ(sys.nrx_total_chars, 241);
  EXPECT_EQ(sys.nrx_bad_chars, 3);
  EXPECT_EQ(sys.nrx_status, 'A');
  EXPECT_STREQ(sys.nrx_message_body, "Message text");
  EXPECT_TRUE(sys.nrx_valid);
}

TEST_F(NmeaSystemTest, NSR_ParseStatus) {
  char out[256];
  const char* s = make_sentence("$INNSR,P,A,P,A,P,A,P,A,P,A,W,P,A", out);
  ASSERT_EQ(parse(s), NMEA_OK);
  nmea_system_state_t sys{};
  ASSERT_EQ(nmea_get_system_data(&ctx, &sys), NMEA_OK);
  EXPECT_EQ(sys.nsr_heading_integrity, 'P');
  EXPECT_EQ(sys.nsr_heading_plausibility, 'A');
  EXPECT_EQ(sys.nsr_position_integrity, 'P');
  EXPECT_EQ(sys.nsr_position_plausibility, 'A');
  EXPECT_EQ(sys.nsr_stw_integrity, 'P');
  EXPECT_EQ(sys.nsr_stw_plausibility, 'A');
  EXPECT_EQ(sys.nsr_sog_cog_integrity, 'P');
  EXPECT_EQ(sys.nsr_sog_cog_plausibility, 'A');
  EXPECT_EQ(sys.nsr_depth_integrity, 'P');
  EXPECT_EQ(sys.nsr_depth_plausibility, 'A');
  EXPECT_EQ(sys.nsr_stw_mode, 'W');
  EXPECT_EQ(sys.nsr_time_integrity, 'P');
  EXPECT_EQ(sys.nsr_time_plausibility, 'A');
  EXPECT_TRUE(sys.nsr_valid);
}

TEST_F(NmeaSystemTest, POS_ParseDevicePosition) {
  char out[256];
  const char* s =
      make_sentence("$GPPOS,GP,01,A,2.5,15.0,10.0,A,50.0,12.0,R", out);
  ASSERT_EQ(parse(s), NMEA_OK);
  nmea_system_state_t sys{};
  ASSERT_EQ(nmea_get_system_data(&ctx, &sys), NMEA_OK);
  EXPECT_STREQ(sys.pos_equipment_id, "GP");
  EXPECT_EQ(sys.pos_equipment_number, 1);
  EXPECT_EQ(sys.pos_validity, 'A');
  EXPECT_TRUE(float_equals(sys.pos_x_coordinate, 2.5f));
  EXPECT_TRUE(float_equals(sys.pos_y_coordinate, 15.0f));
  EXPECT_TRUE(float_equals(sys.pos_z_coordinate, 10.0f));
  EXPECT_EQ(sys.pos_ship_validity, 'A');
  EXPECT_TRUE(float_equals(sys.pos_ship_length, 50.0f));
  EXPECT_TRUE(float_equals(sys.pos_ship_width, 12.0f));
  EXPECT_EQ(sys.pos_status_flag, 'R');
  EXPECT_TRUE(sys.pos_valid);
}

TEST_F(NmeaSystemTest, MSK_ParseInterface) {
  char out[256];
  const char* s = make_sentence("$INMSK,310.5,M,100,M,60.0,1,R", out);
  ASSERT_EQ(parse(s), NMEA_OK);
  nmea_system_state_t sys{};
  ASSERT_EQ(nmea_get_system_data(&ctx, &sys), NMEA_OK);
  EXPECT_TRUE(float_equals(sys.msk_frequency, 310.5f));
  EXPECT_EQ(sys.msk_frequency_mode, 'M');
  EXPECT_TRUE(float_equals(sys.msk_bit_rate, 100.0f));
  EXPECT_EQ(sys.msk_bit_rate_mode, 'M');
  EXPECT_TRUE(float_equals(sys.msk_status_interval, 60.0f));
  EXPECT_EQ(sys.msk_channel, 1);
  EXPECT_EQ(sys.msk_status_flag, 'R');
  EXPECT_TRUE(sys.msk_valid);
}

TEST_F(NmeaSystemTest, MSS_ParseSignalStatus) {
  char out[256];
  const char* s = make_sentence("$INMSS,55.2,12.5,310.5,100,1", out);
  ASSERT_EQ(parse(s), NMEA_OK);
  nmea_system_state_t sys{};
  ASSERT_EQ(nmea_get_system_data(&ctx, &sys), NMEA_OK);
  EXPECT_TRUE(float_equals(sys.mss_signal_strength, 55.2f));
  EXPECT_TRUE(float_equals(sys.mss_snr, 12.5f));
  EXPECT_TRUE(float_equals(sys.mss_frequency, 310.5f));
  EXPECT_TRUE(float_equals(sys.mss_bit_rate, 100.0f));
  EXPECT_EQ(sys.mss_channel, 1);
  EXPECT_TRUE(sys.mss_valid);
}
