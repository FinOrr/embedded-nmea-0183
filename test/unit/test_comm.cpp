/**
 * @file test_comm.cpp
 * @brief Unit tests for Communications module parsers (DSC, DSE, VER, UID, SPW,
 * TUT, TXT, NAK, RLM, STN)
 */

#include <modules/nmea_comm.h>

#include "test_helpers.h"

class NmeaCommTest : public NmeaTestBase {};

/* ========================================================================== */
/*                               DSC TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCommTest, DSC_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "GPDSC"));
  char sentence[256];
  make_sentence("$GPDSC,12,003669987,12,06,00,2182.0,CH16,003669987,12,E,E",
                sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_comm_state_t comm;
  EXPECT_EQ(nmea_get_comm_data(&ctx, &comm), NMEA_OK);
  EXPECT_STREQ(comm.dsc_category, "12");
  EXPECT_STREQ(comm.dsc_address, "003669987");
  EXPECT_STREQ(comm.dsc_nature, "06");
  EXPECT_STREQ(comm.dsc_type, "00");
  EXPECT_STREQ(comm.dsc_time, "2182.0");
  EXPECT_STREQ(comm.dsc_position, "CH16");
  EXPECT_STREQ(comm.dsc_mmsi_distress, "003669987");
  EXPECT_EQ(comm.dsc_ack, 'E');
  EXPECT_TRUE(comm.dsc_expansion);
  EXPECT_TRUE(comm.dsc_valid);
}

/* ========================================================================== */
/*                               DSE TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCommTest, DSE_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "GPDSE"));
  char sentence[256];
  make_sentence("$GPDSE,1,1,Q,003669987,12,SHIPNAME", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_comm_state_t comm;
  EXPECT_EQ(nmea_get_comm_data(&ctx, &comm), NMEA_OK);
  EXPECT_EQ(comm.dse_total_sentences, 1u);
  EXPECT_EQ(comm.dse_sentence_number, 1u);
  EXPECT_EQ(comm.dse_query_reply, 'Q');
  EXPECT_STREQ(comm.dse_mmsi, "003669987");
  EXPECT_TRUE(comm.dse_valid);
}

/* ========================================================================== */
/*                               VER TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCommTest, VER_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "GPVER"));
  char sentence[256];
  make_sentence("$GPVER,1,1,GP,ACME,12345,SN001,MODEL1,v1.0,h1.0,0", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_comm_state_t comm;
  EXPECT_EQ(nmea_get_comm_data(&ctx, &comm), NMEA_OK);
  EXPECT_EQ(comm.ver_total_sentences, 1u);
  EXPECT_EQ(comm.ver_sentence_number, 1u);
  EXPECT_STREQ(comm.ver_device_type, "GP");
  EXPECT_STREQ(comm.ver_vendor_id, "ACME");
  EXPECT_STREQ(comm.ver_unique_id, "12345");
  EXPECT_STREQ(comm.ver_serial_number, "SN001");
  EXPECT_STREQ(comm.ver_model_code, "MODEL1");
  EXPECT_STREQ(comm.ver_sw_revision, "v1.0");
  EXPECT_STREQ(comm.ver_hw_revision, "h1.0");
  EXPECT_EQ(comm.ver_message_id, 0u);
  EXPECT_TRUE(comm.ver_valid);
}

/* ========================================================================== */
/*                               UID TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCommTest, UID_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "GPUID"));
  char sentence[256];
  make_sentence("$GPUID,HEPSLGN02376,DB Los 23", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_comm_state_t comm;
  EXPECT_EQ(nmea_get_comm_data(&ctx, &comm), NMEA_OK);
  EXPECT_STREQ(comm.uid_code1, "HEPSLGN02376");
  EXPECT_STREQ(comm.uid_code2, "DB Los 23");
  EXPECT_TRUE(comm.uid_valid);
}

/* ========================================================================== */
/*                               SPW TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCommTest, SPW_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IISPW"));
  char sentence[256];
  make_sentence("$IISPW,EPV,211000001,2,SESAME", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_comm_state_t comm;
  EXPECT_EQ(nmea_get_comm_data(&ctx, &comm), NMEA_OK);
  EXPECT_STREQ(comm.spw_sentence, "EPV");
  EXPECT_STREQ(comm.spw_unique_id, "211000001");
  EXPECT_EQ(comm.spw_level, 2u);
  EXPECT_STREQ(comm.spw_password, "SESAME");
  EXPECT_TRUE(comm.spw_valid);
}

/* ========================================================================== */
/*                               TUT TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCommTest, TUT_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "SDTUT"));
  char sentence[256];
  make_sentence("$SDTUT,SD,01,01,1,U,6D45702C5371967A", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_comm_state_t comm;
  EXPECT_EQ(nmea_get_comm_data(&ctx, &comm), NMEA_OK);
  EXPECT_STREQ(comm.tut_source_id, "SD");
  EXPECT_EQ(comm.tut_total_sentences, 1u);
  EXPECT_EQ(comm.tut_sentence_number, 1u);
  EXPECT_EQ(comm.tut_message_id, 1u);
  EXPECT_STREQ(comm.tut_translation_code, "U");
  EXPECT_STREQ(comm.tut_text_body, "6D45702C5371967A");
  EXPECT_TRUE(comm.tut_valid);
}

/* ========================================================================== */
/*                               TXT TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCommTest, TXT_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "GPTXT"));
  char sentence[256];
  make_sentence("$GPTXT,01,01,25,DR MODE-ANTENNA FAULT^21", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_comm_state_t comm;
  EXPECT_EQ(nmea_get_comm_data(&ctx, &comm), NMEA_OK);
  EXPECT_EQ(comm.txt_total_sentences, 1u);
  EXPECT_EQ(comm.txt_sentence_number, 1u);
  EXPECT_EQ(comm.txt_identifier, 25u);
  EXPECT_STREQ(comm.txt_message, "DR MODE-ANTENNA FAULT^21");
  EXPECT_TRUE(comm.txt_valid);
}

/* ========================================================================== */
/*                               NAK TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCommTest, NAK_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "GPNAK"));
  char sentence[256];
  make_sentence("$GPNAK,GP,GGA,12345,1,Query not supported", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_comm_state_t comm;
  EXPECT_EQ(nmea_get_comm_data(&ctx, &comm), NMEA_OK);
  EXPECT_STREQ(comm.nak_talker_id, "GP");
  EXPECT_STREQ(comm.nak_sentence, "GGA");
  EXPECT_STREQ(comm.nak_unique_id, "12345");
  EXPECT_EQ(comm.nak_reason_code, 1u);
  EXPECT_STREQ(comm.nak_description, "Query not supported");
  EXPECT_TRUE(comm.nak_valid);
}

/* ========================================================================== */
/*                               RLM TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCommTest, RLM_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "GPRLM"));
  char sentence[256];
  make_sentence("$GPRLM,123456789ABCDEF,120530.00,1,A1B2", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_comm_state_t comm;
  EXPECT_EQ(nmea_get_comm_data(&ctx, &comm), NMEA_OK);
  EXPECT_STREQ(comm.rlm_beacon_id, "123456789ABCDEF");
  EXPECT_EQ(comm.rlm_time.hour, 12u);
  EXPECT_EQ(comm.rlm_time.minute, 5u);
  EXPECT_EQ(comm.rlm_time.second, 30u);
  EXPECT_EQ(comm.rlm_time.millisecond, 0u);
  EXPECT_EQ(comm.rlm_message_code, '1');
  EXPECT_STREQ(comm.rlm_message_body, "A1B2");
  EXPECT_TRUE(comm.rlm_valid);
}

/* ========================================================================== */
/*                               STN TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaCommTest, STN_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "SDSTN"));
  char sentence[256];
  make_sentence("$SDSTN,01", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_comm_state_t comm;
  EXPECT_EQ(nmea_get_comm_data(&ctx, &comm), NMEA_OK);
  EXPECT_EQ(comm.stn_talker_number, 1u);
  EXPECT_TRUE(comm.stn_valid);
}

/* ========================================================================== */
/*                      Negative Path Sanity (Proof)                          */
/* ========================================================================== */

TEST_F(NmeaCommTest, Proof_NoDispatchOrHeaderIssues) {
  // Sanity: All tested sentences should be enabled and parse OK using
  // make_sentence()
  const char* sentences[] = {"GPDSC", "GPDSE", "GPVER", "GPUID", "IISPW",
                             "SDTUT", "GPTXT", "GPNAK", "GPRLM", "SDSTN"};
  for (size_t i = 0; i < sizeof(sentences) / sizeof(sentences[0]); ++i) {
    EXPECT_TRUE(nmea_is_sentence_enabled(&ctx, sentences[i])) << sentences[i];
  }
}
