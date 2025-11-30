/**
 * @file nmea_sensor.h
 * @brief Environmental and motion sensor module for NMEA parser
 *
 * This module handles sensor-related NMEA sentences including:
 * - Depth sensors (DBT, DPT)
 * - Temperature sensors (MTW)
 * - Wind sensors (MWD, MWV)
 * - Speed/distance sensors (VBW, VHW, VLW, VPW)
 */

#ifndef NMEA_SENSOR_H
#define NMEA_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_config.h"
#include "nmea_types.h"

#if NMEA_MODULE_SENSOR_ENABLED

/* ========================================================================== */
/*                          SENSOR STATE STRUCTURE                            */
/* ========================================================================== */

typedef struct {
  /* Depth sensors */
  float depth_below_transducer_meters;
  float depth_below_transducer_feet;
  float depth_below_transducer_fathoms;
  bool depth_valid;

  float depth_meters;        /**< DPT - Depth */
  float depth_offset_meters; /**< Offset from transducer */
  float depth_range_meters;  /**< Maximum range scale */

  /* Temperature */
  float water_temperature_celsius;
  bool water_temperature_valid;

  /* Wind */
  float wind_direction_true;
  float wind_direction_magnetic;
  float wind_speed_knots;
  float wind_speed_mps;
  float wind_speed_kmh;
  char wind_reference; /**< R=Relative, T=True */
  bool wind_valid;

  /* Speed/Distance */
  float speed_water_knots;  /**< Speed through water */
  float speed_ground_knots; /**< Speed over ground */
  float distance_water_nm;  /**< Distance through water */
  float distance_ground_nm; /**< Distance over ground */
  bool speed_valid;

  /* Water speed and heading */
  float water_speed_knots;
  float heading_true;
  float heading_magnetic;
  bool water_speed_heading_valid;

} nmea_sensor_state_t;

/* Parser declarations */
#if NMEA_SENTENCE_DBT_ENABLED
nmea_result_t nmea_parse_dbt(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_DPT_ENABLED
nmea_result_t nmea_parse_dpt(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_MTW_ENABLED
nmea_result_t nmea_parse_mtw(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_MWD_ENABLED
nmea_result_t nmea_parse_mwd(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_MWV_ENABLED
nmea_result_t nmea_parse_mwv(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_VBW_ENABLED
nmea_result_t nmea_parse_vbw(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_VHW_ENABLED
nmea_result_t nmea_parse_vhw(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_VLW_ENABLED
nmea_result_t nmea_parse_vlw(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_VPW_ENABLED
nmea_result_t nmea_parse_vpw(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#endif /* NMEA_MODULE_SENSOR_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_SENSOR_H */
