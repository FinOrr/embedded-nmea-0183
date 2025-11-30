/**
 * @file test_safety.cpp
 * @brief Unit tests for Safety module parsers (ALR, ALF, ALC, ALA, AKD, ACN,
 * ACK, ARC, FIR, MOB, DOR)
 */

#include <modules/nmea_safety.h>

#include "test_helpers.h"

class NmeaSafetyTest : public NmeaTestBase {};

/* ========================================================================== */
/*                               ALR TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, ALR_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIALR"));
  char sentence[256];
  make_sentence("$IIALR,120530.00,001,A,A,HEADING ALARM", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_EQ(safety.alarm_time.hour, 12u);
  EXPECT_EQ(safety.alarm_id, 1u);
  EXPECT_TRUE(safety.alarm_active);
  EXPECT_EQ(safety.alarm_state, 'A');
  EXPECT_STREQ(safety.alarm_condition, "HEADING ALARM");
}

/* ========================================================================== */
/*                               ALF TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, ALF_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIALF"));
  char sentence[256];
  make_sentence("$IIALF,1,1,0,124304.50,A,W,A,,192,1,1,0,LOST TARGET",
                sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_EQ(safety.alarm_time.hour, 12u);
  EXPECT_EQ(safety.alarm_id, 192u);
  EXPECT_EQ(safety.alarm_state, 'A');
  EXPECT_TRUE(safety.alarm_active);
  EXPECT_STREQ(safety.alarm_condition, "LOST TARGET");
}

/* ========================================================================== */
/*                               ALC TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, ALC_FirstAlertCaptured) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIALC"));
  char sentence[256];
  make_sentence("$IIALC,01,01,00,2,ABC,123,1,5,XYZ,456,2,3", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_EQ(safety.alarm_id, 123u);
}

/* ========================================================================== */
/*                               ALA TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, ALA_ValidSentence) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIALA"));
  char sentence[256];
  make_sentence("$IIALA,120530.00,AB,CD,01,123,H,A,HIGH TEMPERATURE", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_EQ(safety.alarm_time.hour, 12u);
  EXPECT_EQ(safety.alarm_id, 123u);
  EXPECT_TRUE(safety.alarm_active);
  EXPECT_EQ(safety.alarm_state, 'A');
  EXPECT_STREQ(safety.alarm_condition, "HIGH TEMPERATURE");
}

/* ========================================================================== */
/*                               AKD TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, AKD_Acknowledgement) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIAKD"));
  char sentence[256];
  make_sentence("$IIAKD,120530.00,AB,CD,01,123,EF,GH,02", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_EQ(safety.alarm_time.hour, 12u);
  EXPECT_EQ(safety.alarm_id, 123u);
  EXPECT_EQ(safety.alarm_state, 'A');
}

/* ========================================================================== */
/*                               ACN TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, ACN_Command) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIACN"));
  char sentence[256];
  make_sentence("$IIACN,120530.00,ABC,123,1,A,C", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_EQ(safety.alarm_time.hour, 12u);
  EXPECT_EQ(safety.alarm_id, 123u);
  EXPECT_EQ(safety.alarm_state, 'A');
}

/* ========================================================================== */
/*                               ACK TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, ACK_AcknowledgeAlarm) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIACK"));
  char sentence[256];
  make_sentence("$IIACK,001", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_EQ(safety.alarm_id, 1u);
  EXPECT_EQ(safety.alarm_state, 'A');
}

/* ========================================================================== */
/*                               ARC TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, ARC_RefusedCommand) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIARC"));
  char sentence[256];
  make_sentence("$IIARC,120530.00,ABC,123,1,A", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_EQ(safety.alarm_time.hour, 12u);
  EXPECT_EQ(safety.alarm_id, 123u);
  EXPECT_EQ(safety.alarm_state, 'A');
}

/* ========================================================================== */
/*                               FIR TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, FIR_FireDetected) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIFIR"));
  char sentence[256];
  make_sentence("$IIFIR,E,120530.00,FD,AB,123,001,A,A,FIRE DETECTED ZONE 1",
                sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_EQ(safety.alarm_time.hour, 12u);
  EXPECT_EQ(safety.alarm_id, 1u);
  EXPECT_TRUE(safety.alarm_active);
  EXPECT_EQ(safety.alarm_state, 'A');
  EXPECT_STREQ(safety.alarm_condition, "FIRE DETECTED ZONE 1");
}

/* ========================================================================== */
/*                               MOB TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, MOB_Activated) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIMOB"));
  char sentence[256];
  make_sentence(
      "$IIMOB,000FF,A,120530,1,0,120545,3723.46,N,12158.32,W,10,5,123456789,0",
      sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_TRUE(safety.alarm_active);
  EXPECT_EQ(safety.alarm_state, 'A');
  EXPECT_EQ(safety.alarm_id, 999u);
  EXPECT_STREQ(safety.alarm_condition, "MAN OVERBOARD");
}

/* ========================================================================== */
/*                               DOR TESTS                                    */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, DOR_DoorOpen) {
  ASSERT_TRUE(nmea_is_sentence_enabled(&ctx, "IIDOR"));
  char sentence[256];
  make_sentence("$IIDOR,E,120530.00,WT,AB,123,001,O,C,DOOR 1 OPEN", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);
  nmea_safety_state_t safety;
  EXPECT_EQ(nmea_get_safety_data(&ctx, &safety), NMEA_OK);
  EXPECT_EQ(safety.alarm_time.hour, 12u);
  EXPECT_EQ(safety.alarm_id, 1u);
  EXPECT_EQ(safety.alarm_state, 'O');
  EXPECT_TRUE(safety.alarm_active);
  EXPECT_STREQ(safety.alarm_condition, "DOOR 1 OPEN");
}

/* ========================================================================== */
/*                      Negative Path Sanity (Proof)                          */
/* ========================================================================== */

TEST_F(NmeaSafetyTest, Proof_NoDispatchOrHeaderIssues) {
  const char* sentences[] = {"IIALR", "IIALF", "IIALC", "IIALA",
                             "IIAKD", "IIACN", "IIACK", "IIARC",
                             "IIFIR", "IIMOB", "IIDOR"};
  for (size_t i = 0; i < sizeof(sentences) / sizeof(sentences[0]); ++i) {
    EXPECT_TRUE(nmea_is_sentence_enabled(&ctx, sentences[i])) << sentences[i];
  }
}
