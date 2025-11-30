/**
 * @file test_navigation.cpp
 * @brief Unit tests for Navigation module parsers
 */

#include <modules/nmea_navigation.h>

#include "test_helpers.h"

class NmeaNavigationTest : public NmeaTestBase {};

/* ========================================================================== */
/*                              APB TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaNavigationTest, APB_ValidSentence) {
  const char* sentence =
      "$GPAPB,A,A,0.10,R,N,V,V,011.0,M,DEST,011.3,M,011.3,M,A";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_navigation_state_t nav;
  EXPECT_EQ(nmea_get_navigation_data(&ctx, &nav), NMEA_OK);

  // Check status flags
  EXPECT_EQ(nav.apb_status1, 'A');
  EXPECT_EQ(nav.apb_status2, 'A');

  // Check cross-track error
  EXPECT_TRUE(float_equals(nav.xte_magnitude, 0.10f));
  EXPECT_EQ(nav.xte_steer_direction, 'R');

  // Check arrival indicators
  EXPECT_EQ(nav.arrival_circle_entered, 'V');
  EXPECT_EQ(nav.perpendicular_passed, 'V');

  // Check bearings
  EXPECT_TRUE(float_equals(nav.bearing_origin_to_dest, 11.0f));
  EXPECT_EQ(nav.bearing_origin_type, 'M');
  EXPECT_STREQ(nav.destination_waypoint_id, "DEST");
  EXPECT_TRUE(float_equals(nav.bearing_position_to_dest, 11.3f));
  EXPECT_EQ(nav.bearing_position_type, 'M');

  // Check heading
  EXPECT_TRUE(float_equals(nav.heading_to_steer, 11.3f));
  EXPECT_EQ(nav.heading_to_steer_type, 'M');
  EXPECT_EQ(nav.apb_mode, 'A');

  EXPECT_TRUE(nav.apb_valid);
}

/* ========================================================================== */
/*                              BWC TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaNavigationTest, BWC_ValidSentence) {
  const char* sentence =
      "$GPBWC,220516,5130.02,N,00046.34,W,213.8,T,218.0,M,0004.6,N,EGLM,A";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_navigation_state_t nav;
  EXPECT_EQ(nmea_get_navigation_data(&ctx, &nav), NMEA_OK);

  // Check UTC time
  EXPECT_EQ(nav.waypoint_utc.hour, 22);
  EXPECT_EQ(nav.waypoint_utc.minute, 5);
  EXPECT_EQ(nav.waypoint_utc.second, 16);

  // Check waypoint position
  EXPECT_TRUE(float_equals(nav.waypoint_latitude.degrees, 51.5003, 0.001));
  EXPECT_TRUE(float_equals(nav.waypoint_longitude.degrees, -0.7723, 0.001));

  // Check bearings
  EXPECT_TRUE(float_equals(nav.bearing_true, 213.8f));
  EXPECT_TRUE(float_equals(nav.bearing_magnetic, 218.0f));

  // Check distance and waypoint ID
  EXPECT_TRUE(float_equals(nav.distance_nm, 4.6f));
  EXPECT_STREQ(nav.waypoint_id, "EGLM");
  EXPECT_EQ(nav.bw_mode, 'A');

  EXPECT_TRUE(nav.bwc_valid);
}

/* ========================================================================== */
/*                              BWR TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaNavigationTest, BWR_ValidSentence) {
  const char* sentence =
      "$GPBWR,161229,3723.02,N,12102.45,W,234.9,T,240.3,M,001.4,N,004,A";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_navigation_state_t nav;
  EXPECT_EQ(nmea_get_navigation_data(&ctx, &nav), NMEA_OK);

  // Check UTC time
  EXPECT_EQ(nav.waypoint_utc.hour, 16);
  EXPECT_EQ(nav.waypoint_utc.minute, 12);
  EXPECT_EQ(nav.waypoint_utc.second, 29);

  // Check position
  EXPECT_TRUE(float_equals(nav.waypoint_latitude.degrees, 37.3837, 0.001));
  EXPECT_TRUE(float_equals(nav.waypoint_longitude.degrees, -121.0408, 0.001));

  // Check bearings and distance
  EXPECT_TRUE(float_equals(nav.bearing_true, 234.9f));
  EXPECT_TRUE(float_equals(nav.bearing_magnetic, 240.3f));
  EXPECT_TRUE(float_equals(nav.distance_nm, 1.4f));
  EXPECT_STREQ(nav.waypoint_id, "004");
  EXPECT_EQ(nav.bw_mode, 'A');

  EXPECT_TRUE(nav.bwr_valid);
}

/* ========================================================================== */
/*                              BEC TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaNavigationTest, BEC_ValidSentence) {
  const char* sentence =
      "$GPBEC,081837,3751.65,S,14507.36,E,097.0,T,103.2,M,0.4,N,WAYPT";
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_navigation_state_t nav;
  EXPECT_EQ(nmea_get_navigation_data(&ctx, &nav), NMEA_OK);

  // Check UTC time
  EXPECT_EQ(nav.bec_utc.hour, 8);
  EXPECT_EQ(nav.bec_utc.minute, 18);
  EXPECT_EQ(nav.bec_utc.second, 37);

  // Check DR position
  EXPECT_TRUE(float_equals(nav.bec_waypoint_lat.degrees, -37.8608, 0.001));
  EXPECT_TRUE(float_equals(nav.bec_waypoint_lon.degrees, 145.1227, 0.001));

  // Check bearings
  EXPECT_TRUE(float_equals(nav.bec_bearing_true, 97.0f));
  EXPECT_TRUE(float_equals(nav.bec_bearing_magnetic, 103.2f));

  // Check distance and ID
  EXPECT_TRUE(float_equals(nav.bec_distance_nm, 0.4f));
  EXPECT_STREQ(nav.bec_waypoint_id, "WAYPT");

  EXPECT_TRUE(nav.bec_valid);
}

/* ========================================================================== */
/*                              BOD TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaNavigationTest, BOD_ValidSentence) {
  char sentence[256];
  make_sentence("$GPBOD,099.3,T,105.6,M,POINTB,POINTA", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_navigation_state_t nav;
  EXPECT_EQ(nmea_get_navigation_data(&ctx, &nav), NMEA_OK);

  // Check bearings
  EXPECT_TRUE(float_equals(nav.bod_bearing_true, 99.3f));
  EXPECT_TRUE(float_equals(nav.bod_bearing_magnetic, 105.6f));

  // Check waypoint IDs
  EXPECT_STREQ(nav.bod_dest_id, "POINTB");
  EXPECT_STREQ(nav.bod_origin_id, "POINTA");

  EXPECT_TRUE(nav.bod_valid);
}

/* ========================================================================== */
/*                              BWW TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaNavigationTest, BWW_ValidSentence) {
  char sentence[256];
  make_sentence("$GPBWW,097.0,T,103.2,M,WPT2,WPT1", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_navigation_state_t nav;
  EXPECT_EQ(nmea_get_navigation_data(&ctx, &nav), NMEA_OK);

  // Check bearings
  EXPECT_TRUE(float_equals(nav.bww_bearing_true, 97.0f));
  EXPECT_TRUE(float_equals(nav.bww_bearing_magnetic, 103.2f));

  // Check waypoint IDs
  EXPECT_STREQ(nav.bww_to_id, "WPT2");
  EXPECT_STREQ(nav.bww_from_id, "WPT1");

  EXPECT_TRUE(nav.bww_valid);
}

/* ========================================================================== */
/*                              RTE TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaNavigationTest, RTE_ValidSentence) {
  char sentence[256];
  make_sentence("$GPRTE,2,1,c,0,W3IWI,DRIVWY,32CEDR,32-29,32BKLD,32-I95",
                sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_navigation_state_t nav;
  EXPECT_EQ(nmea_get_navigation_data(&ctx, &nav), NMEA_OK);

  // Check sentence info
  EXPECT_EQ(nav.rte_total_sentences, 2);
  EXPECT_EQ(nav.rte_sentence_number, 1);
  EXPECT_EQ(nav.rte_mode, 'c');
  EXPECT_STREQ(nav.route_id, "0");

  // Check waypoints (starting from field 5)
  EXPECT_EQ(nav.waypoint_count, 6);
  EXPECT_STREQ(nav.waypoint_list[0], "W3IWI");
  EXPECT_STREQ(nav.waypoint_list[1], "DRIVWY");
  EXPECT_STREQ(nav.waypoint_list[2], "32CEDR");
  EXPECT_STREQ(nav.waypoint_list[3], "32-29");
  EXPECT_STREQ(nav.waypoint_list[4], "32BKLD");
  EXPECT_STREQ(nav.waypoint_list[5], "32-I95");

  EXPECT_TRUE(nav.rte_valid);
}

TEST_F(NmeaNavigationTest, RTE_SecondSentence) {
  // Second sentence of a multi-sentence route
  char sentence[256];
  make_sentence("$GPRTE,2,2,c,0,YARD,GRAVEL,LTLKG", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_navigation_state_t nav;
  EXPECT_EQ(nmea_get_navigation_data(&ctx, &nav), NMEA_OK);

  EXPECT_EQ(nav.rte_total_sentences, 2);
  EXPECT_EQ(nav.rte_sentence_number, 2);
  EXPECT_EQ(nav.rte_mode, 'c');
  EXPECT_TRUE(nav.rte_valid);
}

/* ========================================================================== */
/*                              AAM TESTS                                     */
/* ========================================================================== */

TEST_F(NmeaNavigationTest, AAM_ValidSentence) {
  char sentence[256];
  make_sentence("$GPAAM,A,A,0.10,N,WPTNME", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_navigation_state_t nav;
  EXPECT_EQ(nmea_get_navigation_data(&ctx, &nav), NMEA_OK);

  // Check arrival status
  EXPECT_EQ(nav.aam_arrival_circle, 'A');
  EXPECT_EQ(nav.aam_perpendicular, 'A');

  // Check radius
  EXPECT_TRUE(float_equals(nav.aam_radius, 0.10f));

  // Check waypoint ID
  EXPECT_STREQ(nav.aam_waypoint_id, "WPTNME");

  EXPECT_TRUE(nav.aam_valid);
}

TEST_F(NmeaNavigationTest, AAM_NotEntered) {
  char sentence[256];
  make_sentence("$GPAAM,V,V,0.50,N,WPT02", sentence);
  EXPECT_EQ(parse(sentence), NMEA_OK);

  nmea_navigation_state_t nav;
  EXPECT_EQ(nmea_get_navigation_data(&ctx, &nav), NMEA_OK);

  // Check arrival status - not entered
  EXPECT_EQ(nav.aam_arrival_circle, 'V');
  EXPECT_EQ(nav.aam_perpendicular, 'V');

  EXPECT_TRUE(float_equals(nav.aam_radius, 0.50f));
  EXPECT_TRUE(nav.aam_valid);
}
