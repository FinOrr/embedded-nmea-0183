#include "test_helpers.h"

// Waypoint module tests: WPL, WNC, WCV

class WaypointTests : public NmeaTestBase {};

// WPL: Waypoint location
TEST_F(WaypointTests, WPL_ParsesLatitudeLongitudeAndId) {
  // $--WPL,llll.ll,a,yyyyy.yy,a,c--c*hh
  const char* base =
      "$GPWPL,4807.038,N,01131.000,E,HOME";  // 48°07.038' N, 11°31.000' E
  const char* sentence = make_sentence(base, buffer);

  ASSERT_EQ(parse(sentence), NMEA_OK);
  const auto& wp = ctx.waypoint;
  EXPECT_TRUE(wp.wpl_latitude.valid);
  EXPECT_TRUE(wp.wpl_longitude.valid);
  EXPECT_TRUE(wp.wpl_valid);

  // Latitude 48 + 7.038/60 = 48.1173
  EXPECT_TRUE(float_equals(wp.wpl_latitude.degrees, 48.1173f, 0.0005f));
  // Longitude 11 + 31.000/60 = 11.5166667
  EXPECT_TRUE(float_equals(wp.wpl_longitude.degrees, 11.5166667f, 0.0005f));
  EXPECT_STREQ(wp.wpl_id, "HOME");
}

TEST_F(WaypointTests, WPL_AllowsMissingFieldsWithoutCrash) {
  // Missing ID (field 5) -> should not set valid
  const char* base = "$GPWPL,4807.038,N,01131.000,E";
  const char* sentence = make_sentence(base, buffer);
  ASSERT_EQ(parse(sentence), NMEA_OK);
  const auto& wp = ctx.waypoint;
  EXPECT_TRUE(wp.wpl_latitude.valid);
  EXPECT_TRUE(wp.wpl_longitude.valid);
  EXPECT_FALSE(wp.wpl_valid);  // id missing -> not marked valid
}

// WNC: Distance waypoint to waypoint
TEST_F(WaypointTests, WNC_ParsesDistancesAndIds) {
  // $--WNC,x.x,N,x.x,K,c--c,c--c*hh
  // Include checksum on last FROM field to exercise stripping
  const char* base_no_cs = "$GPWNC,12.3,N,22.8,K,TO123,FROM456";
  const char* sentence = make_sentence(base_no_cs, buffer);

  ASSERT_EQ(parse(sentence), NMEA_OK);
  const auto& wp = ctx.waypoint;
  EXPECT_TRUE(wp.wnc_valid);
  EXPECT_TRUE(float_equals(wp.wnc_distance_nm, 12.3f));
  EXPECT_TRUE(float_equals(wp.wnc_distance_km, 22.8f));
  EXPECT_STREQ(wp.wnc_to_id, "TO123");
  EXPECT_STREQ(wp.wnc_from_id, "FROM456");
}

TEST_F(WaypointTests, WNC_AllowsMissingOptionalFields) {
  // Only NM distance provided; KM empty; IDs present
  const char* base = "$GPWNC,5.0,N,,K,TOX,FRY";
  const char* sentence = make_sentence(base, buffer);

  ASSERT_EQ(parse(sentence), NMEA_OK);
  const auto& wp = ctx.waypoint;
  EXPECT_TRUE(wp.wnc_valid);
  EXPECT_TRUE(float_equals(wp.wnc_distance_nm, 5.0f));
  // KM may remain default (0) if empty
  EXPECT_TRUE(float_equals(wp.wnc_distance_km, 0.0f));
  EXPECT_STREQ(wp.wnc_to_id, "TOX");
  EXPECT_STREQ(wp.wnc_from_id, "FRY");
}

// WCV: Waypoint closure velocity
TEST_F(WaypointTests, WCV_ParsesVelocityWaypointIdMode) {
  // $--WCV,x.x,N,c--c,a*hh
  const char* base = "$GPWCV,8.5,N,WP17,A";
  const char* sentence = make_sentence(base, buffer);

  ASSERT_EQ(parse(sentence), NMEA_OK);
  const auto& wp = ctx.waypoint;
  EXPECT_TRUE(wp.wcv_valid);
  EXPECT_TRUE(float_equals(wp.wcv_velocity, 8.5f));
  EXPECT_STREQ(wp.wcv_waypoint_id, "WP17");
  EXPECT_EQ(wp.wcv_mode, 'A');
}

TEST_F(WaypointTests, WCV_MissingModeNotValid) {
  const char* base = "$GPWCV,2.0,N,XYZ";  // missing mode field
  const char* sentence = make_sentence(base, buffer);

  ASSERT_EQ(parse(sentence), NMEA_OK);
  const auto& wp = ctx.waypoint;
  EXPECT_FALSE(wp.wcv_valid);
  EXPECT_TRUE(float_equals(wp.wcv_velocity, 2.0f));
  EXPECT_STREQ(wp.wcv_waypoint_id, "XYZ");
}
