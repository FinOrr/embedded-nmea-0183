/**
 * @file nmea_waypoint.h
 * @brief Waypoint management module
 */

#ifndef NMEA_WAYPOINT_H
#define NMEA_WAYPOINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_config.h"
#include "nmea_types.h"

#if NMEA_MODULE_WAYPOINT_ENABLED

/**
 * @brief Waypoint management state data
 */
typedef struct {
  /* WPL - Waypoint location */
  nmea_coordinate_t wpl_latitude;  /**< Waypoint latitude */
  nmea_coordinate_t wpl_longitude; /**< Waypoint longitude */
  char wpl_id[16];                 /**< Waypoint identifier */

  /* WNC - Distance waypoint to waypoint */
  float wnc_distance_nm; /**< Distance, nautical miles */
  float wnc_distance_km; /**< Distance, kilometers */
  char wnc_to_id[16];    /**< TO waypoint identifier */
  char wnc_from_id[16];  /**< FROM waypoint identifier */

  /* WCV - Waypoint closure velocity */
  float wcv_velocity;       /**< Velocity component, knots */
  char wcv_waypoint_id[16]; /**< Waypoint identifier */
  char wcv_mode;            /**< Mode: A/D/E/M/S/N */

  /* Validity flags */
  bool wpl_valid; /**< WPL data valid */
  bool wnc_valid; /**< WNC data valid */
  bool wcv_valid; /**< WCV data valid */
} nmea_waypoint_state_t;

/* Parser function declarations */
struct nmea_context;
struct nmea_tokens;

#if NMEA_SENTENCE_WPL_ENABLED
nmea_result_t nmea_parse_wpl(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_WNC_ENABLED
nmea_result_t nmea_parse_wnc(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_WCV_ENABLED
nmea_result_t nmea_parse_wcv(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#endif /* NMEA_MODULE_WAYPOINT_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_WAYPOINT_H */
