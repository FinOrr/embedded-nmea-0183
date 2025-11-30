/**
 * @file nmea_heading.h
 * @brief Heading and compass module for NMEA parser
 */

#ifndef NMEA_HEADING_H
#define NMEA_HEADING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_config.h"
#include "nmea_types.h"

#if NMEA_MODULE_HEADING_ENABLED

/**
 * @brief Heading and compass state data
 */
typedef struct {
  /* HDG - Heading, deviation and variation */
  float heading_magnetic_sensor; /**< Magnetic sensor heading, degrees */
  float magnetic_deviation;      /**< Magnetic deviation, degrees (E positive, W
                                    negative) */
  float magnetic_variation;      /**< Magnetic variation, degrees (E positive, W
                                    negative) */
  char deviation_direction;      /**< Deviation direction: 'E' or 'W' */
  char variation_direction;      /**< Variation direction: 'E' or 'W' */

  /* HDT - Heading true */
  float heading_true; /**< True heading, degrees */

  /* ROT - Rate of turn */
  float rate_of_turn; /**< Rate of turn, degrees/min (negative = port) */
  char rot_status;    /**< 'A' = valid, 'V' = invalid */

  /* VTG - Course over ground and ground speed */
  float course_true;     /**< Course over ground, degrees true */
  float course_magnetic; /**< Course over ground, degrees magnetic */
  float speed_knots;     /**< Speed over ground, knots */
  float speed_kmh;       /**< Speed over ground, km/h */
  char vtg_mode;         /**< Mode indicator: A/D/E/M/P/S/N */

  /* VDR - Set and drift */
  float current_direction_true;     /**< Current direction, degrees true */
  float current_direction_magnetic; /**< Current direction, degrees magnetic */
  float current_speed_knots;        /**< Current speed, knots */

  /* OSD - Own ship data */
  float osd_heading_true;   /**< Heading, degrees true */
  char osd_heading_status;  /**< 'A' = valid, 'V' = invalid */
  float vessel_course_true; /**< Vessel course, degrees true */
  char course_reference;    /**< B/M/W/R/P */
  float vessel_speed;       /**< Vessel speed */
  char speed_reference;     /**< B/M/W/R/P */
  char speed_units;         /**< K/N/S (km/h, knots, statute mph) */
  float vessel_set_true;    /**< Vessel set, degrees true */
  float vessel_drift;       /**< Vessel drift (speed) */

  /* HMR - Heading monitor receive */
  char sensor1_id[8];         /**< Heading sensor 1 ID */
  char sensor2_id[8];         /**< Heading sensor 2 ID */
  float set_difference;       /**< Set difference by HMS, degrees */
  float actual_difference;    /**< Actual heading sensor difference, degrees */
  char difference_warning;    /**< 'A' = within limit, 'V' = exceeds limit */
  float sensor1_heading;      /**< Heading reading sensor 1, degrees */
  char sensor1_status;        /**< 'A' = valid, 'V' = invalid */
  char sensor1_type;          /**< 'M' = magnetic, 'T' = true */
  float sensor1_deviation;    /**< Deviation sensor 1, degrees */
  char sensor1_deviation_dir; /**< 'E' or 'W' */
  float sensor2_heading;      /**< Heading reading sensor 2, degrees */
  char sensor2_status;        /**< 'A' = valid, 'V' = invalid */
  char sensor2_type;          /**< 'M' = magnetic, 'T' = true */
  float sensor2_deviation;    /**< Deviation sensor 2, degrees */
  char sensor2_deviation_dir; /**< 'E' or 'W' */
  float hmr_variation;        /**< Variation, degrees */
  char hmr_variation_dir;     /**< 'E' or 'W' */

  /* HTC - Heading/track control */
  char htc_override;            /**< 'A' = in use, 'V' = not in use */
  float commanded_rudder_angle; /**< Commanded rudder angle, degrees */
  char rudder_direction;        /**< 'L' = port, 'R' = starboard */
  char steering_mode;           /**< M/S/H/T/R */
  char turn_mode;               /**< N/T/R */
  float rudder_limit;           /**< Commanded rudder limit, degrees */
  float off_heading_limit;      /**< Commanded off-heading limit, degrees */
  float commanded_radius;       /**< Commanded radius of turn, n.miles */
  float commanded_rate_of_turn; /**< Commanded rate of turn, degrees/min */
  float commanded_heading;      /**< Commanded heading-to-steer, degrees */
  float off_track_limit;        /**< Commanded off-track limit, n.miles */
  float commanded_track;        /**< Commanded track, degrees */
  char heading_reference;       /**< 'T' = true, 'M' = magnetic */
  char rudder_status;           /**< 'A' = within limits, 'V' = limit reached */
  char off_heading_status;      /**< 'A' = within limits, 'V' = limit reached */
  char off_track_status;        /**< 'A' = within limits, 'V' = limit reached */
  float vessel_heading;         /**< Vessel heading, degrees */

  /* Validity flags */
  bool hdg_valid; /**< HDG data valid */
  bool hdt_valid; /**< HDT data valid */
  bool rot_valid; /**< ROT data valid */
  bool vtg_valid; /**< VTG data valid */
  bool vdr_valid; /**< VDR data valid */
  bool osd_valid; /**< OSD data valid */
  bool hmr_valid; /**< HMR data valid */
  bool htc_valid; /**< HTC data valid */
} nmea_heading_state_t;

/* Parser function declarations */
struct nmea_context;
struct nmea_tokens;

#if NMEA_SENTENCE_HDG_ENABLED
nmea_result_t nmea_parse_hdg(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_HDT_ENABLED
nmea_result_t nmea_parse_hdt(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_ROT_ENABLED
nmea_result_t nmea_parse_rot(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_VTG_ENABLED
nmea_result_t nmea_parse_vtg(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_VDR_ENABLED
nmea_result_t nmea_parse_vdr(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_OSD_ENABLED
nmea_result_t nmea_parse_osd(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_HMR_ENABLED
nmea_result_t nmea_parse_hmr(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_HTC_ENABLED
nmea_result_t nmea_parse_htc(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#endif /* NMEA_MODULE_HEADING_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_HEADING_H */
