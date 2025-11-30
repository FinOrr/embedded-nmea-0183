/**
 * @file nmea_gnss.h
 * @brief GNSS/GPS module for NMEA parser
 *
 * This module handles all GNSS and GPS-related NMEA sentences including:
 * - Position fixing (GGA, GLL, GNS, RMC, RMB, RMA)
 * - Satellite information (GSA, GSV)
 * - Quality metrics (GBS, GST, GRS, GFA)
 *
 * @note This header is automatically included by nmea.h when
 * NMEA_MODULE_GNSS_ENABLED is set.
 */

#ifndef NMEA_GNSS_H
#define NMEA_GNSS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_config.h"
#include "nmea_types.h"

#if NMEA_MODULE_GNSS_ENABLED

/* ========================================================================== */
/*                          GNSS-SPECIFIC TYPES                               */
/* ========================================================================== */

/**
 * @brief GNSS fix mode (from GSA sentence)
 */
typedef enum {
  NMEA_FIX_MODE_NOT_AVAILABLE = 1, /**< Fix not available */
  NMEA_FIX_MODE_2D = 2,            /**< 2D fix */
  NMEA_FIX_MODE_3D = 3,            /**< 3D fix */
} nmea_fix_mode_t;

/**
 * @brief GNSS fix selection mode (from GSA sentence)
 */
typedef enum {
  NMEA_FIX_SELECTION_MANUAL = 'M', /**< Manual fix selection */
  NMEA_FIX_SELECTION_AUTO = 'A',   /**< Automatic fix selection */
} nmea_fix_selection_t;

/**
 * @brief Satellite information (from GSV sentence)
 */
typedef struct {
  uint8_t prn;       /**< Satellite PRN number */
  uint8_t elevation; /**< Elevation in degrees (0-90) */
  uint16_t azimuth;  /**< Azimuth in degrees (0-359) */
  uint8_t snr;       /**< Signal-to-noise ratio (dB, 0-99) */
  bool valid;        /**< True if data is valid */
} nmea_satellite_t;

/**
 * @brief Maximum number of satellites tracked
 */
#define NMEA_MAX_SATELLITES 32

/**
 * @brief Maximum number of satellites used in fix
 */
#define NMEA_MAX_SATELLITES_USED 12

/**
 * @brief Dilution of Precision values (from GSA sentence)
 */
typedef struct {
  float pdop; /**< Position DOP */
  float hdop; /**< Horizontal DOP */
  float vdop; /**< Vertical DOP */
  bool valid; /**< True if data is valid */
} nmea_dop_t;

/**
 * @brief Pseudorange error statistics (from GST sentence)
 */
typedef struct {
  float rms_std_dev;   /**< RMS standard deviation of pseudoranges (meters) */
  float std_dev_major; /**< Standard deviation of semi-major axis (meters) */
  float std_dev_minor; /**< Standard deviation of semi-minor axis (meters) */
  float orientation;   /**< Orientation of semi-major axis (degrees) */
  float std_dev_lat;   /**< Standard deviation of latitude (meters) */
  float std_dev_lon;   /**< Standard deviation of longitude (meters) */
  float std_dev_alt;   /**< Standard deviation of altitude (meters) */
  bool valid;          /**< True if data is valid */
} nmea_error_statistics_t;

/* ========================================================================== */
/*                          GNSS STATE STRUCTURE                              */
/* ========================================================================== */

/**
 * @brief GNSS module state
 *
 * This structure holds all parsed GNSS/GPS data from various sentence types.
 * Fields are updated as relevant sentences are parsed.
 */
typedef struct {
  /* Position data (GGA, GLL, GNS, RMC, RMA, RMB) */
  nmea_coordinate_t latitude;    /**< Current latitude */
  nmea_coordinate_t longitude;   /**< Current longitude */
  float altitude_meters;         /**< Altitude above mean sea level (meters) */
  bool altitude_valid;           /**< True if altitude is valid */
  float geoid_separation_meters; /**< Geoid separation (meters) */
  bool geoid_separation_valid;   /**< True if geoid separation is valid */

  /* Time data (GGA, RMC, GNS) */
  nmea_time_t utc_time; /**< UTC time */
  nmea_date_t utc_date; /**< UTC date (from RMC) */

  /* Quality indicators (GGA, GNS, RMC) */
  nmea_quality_t fix_quality;         /**< Fix quality indicator */
  nmea_fix_mode_t fix_mode;           /**< 2D/3D fix mode (from GSA) */
  nmea_fix_selection_t fix_selection; /**< Manual/Auto selection (from GSA) */
  char mode_indicator; /**< Mode indicator (A=Autonomous, D=Differential, etc.)
                        */
  char mode_indicators[8]; /**< Multi-system mode indicators (GNS:
                              GPS/GLONASS/Galileo) */
  char nav_status; /**< Navigational status (S=Safe, C=Caution, U=Unsafe, V=Not
                      valid) */

  /* Satellite information (GGA, GSA, GSV) */
  uint8_t satellites_used;    /**< Number of satellites used in fix */
  uint8_t satellites_in_view; /**< Number of satellites in view */
  uint8_t satellite_prns[NMEA_MAX_SATELLITES_USED]; /**< PRNs of satellites used
                                                       in fix */
  nmea_satellite_t
      satellites[NMEA_MAX_SATELLITES]; /**< Detailed satellite info */

  /* Precision metrics (GSA, GST) */
  nmea_dop_t dop;                      /**< Dilution of precision */
  nmea_error_statistics_t error_stats; /**< Error statistics */

  /* Speed and course (RMC, VTG from navigation but also in GNSS context) */
  nmea_speed_t speed;                /**< Ground speed */
  nmea_direction_t course;           /**< Course over ground */
  float magnetic_variation;          /**< Magnetic variation (degrees) */
  bool magnetic_variation_valid;     /**< True if magnetic variation is valid */
  char magnetic_variation_direction; /**< E or W */

  /* Age of differential corrections (GGA, GBS) */
  float dgps_age_seconds;     /**< Age of DGPS data (seconds) */
  bool dgps_age_valid;        /**< True if DGPS age is valid */
  uint16_t dgps_station_id;   /**< DGPS reference station ID */
  bool dgps_station_id_valid; /**< True if station ID is valid */

  /* Navigation status (RMC) */
  char status;     /**< Status: A=Active, V=Void */
  bool data_valid; /**< True if data is active/valid */

  /* Residuals (GRS) */
  float range_residuals[NMEA_MAX_SATELLITES_USED]; /**< Range residuals (meters)
                                                    */
  bool range_residuals_valid; /**< True if residuals are valid */

  /* Fix accuracy (GFA) */
  float horizontal_accuracy; /**< Horizontal accuracy (meters) */
  float vertical_accuracy;   /**< Vertical accuracy (meters) */
  bool accuracy_valid;       /**< True if accuracy data is valid */
  float
      horizontal_protection_level; /**< Horizontal protection level (meters) */
  float vertical_protection_level; /**< Vertical protection level (meters) */
  char integrity_status[4];        /**< Integrity status (GFA: RAIM/SBAS/GIC) */

  /* Satellite fault detection (GBS) */
  float expected_error_lat;    /**< Expected error in latitude (meters) */
  float expected_error_lon;    /**< Expected error in longitude (meters) */
  float expected_error_alt;    /**< Expected error in altitude (meters) */
  uint8_t failed_satellite_id; /**< ID of most likely failed satellite */
  float failed_satellite_probability;  /**< Probability of missed detection */
  float failed_satellite_bias;         /**< Estimate of bias (meters) */
  float failed_satellite_bias_std_dev; /**< Std dev of bias estimate (meters) */
  bool gbs_valid;                      /**< True if GBS data is valid */

  /* Waypoint navigation (RMB, RMA) */
  float cross_track_error_nm;       /**< Cross track error (nautical miles) */
  char steer_direction;             /**< Direction to steer (L/R) */
  char origin_waypoint_id[16];      /**< Origin waypoint ID */
  char dest_waypoint_id[16];        /**< Destination waypoint ID */
  nmea_coordinate_t dest_latitude;  /**< Destination latitude */
  nmea_coordinate_t dest_longitude; /**< Destination longitude */
  float range_to_dest_nm;    /**< Range to destination (nautical miles) */
  float bearing_to_dest_deg; /**< Bearing to destination (degrees true) */
  float dest_closing_velocity_knots; /**< Closing velocity (knots) */
  char arrival_status;               /**< Arrival status (A/V) */
  float time_diff_a_us;     /**< LORAN-C time difference A (microseconds) */
  float time_diff_b_us;     /**< LORAN-C time difference B (microseconds) */
  bool waypoint_data_valid; /**< True if waypoint data is valid */

} nmea_gnss_state_t;

/* ========================================================================== */
/*                     SENTENCE PARSER DECLARATIONS                           */
/* ========================================================================== */

/**
 * @brief Parser function declarations
 *
 * These are internal parser functions. Users should not call these directly.
 * They are invoked automatically by nmea_parse().
 *
 * @internal
 */

#if NMEA_SENTENCE_GGA_ENABLED
/** Parse GGA - Global Positioning System Fix Data */
nmea_result_t nmea_parse_gga(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_GLL_ENABLED
/** Parse GLL - Geographic Position - Latitude/Longitude */
nmea_result_t nmea_parse_gll(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_GNS_ENABLED
/** Parse GNS - GNSS Fix Data */
nmea_result_t nmea_parse_gns(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_RMC_ENABLED
/** Parse RMC - Recommended Minimum Specific GNSS Data */
nmea_result_t nmea_parse_rmc(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_RMB_ENABLED
/** Parse RMB - Recommended Minimum Navigation Information */
nmea_result_t nmea_parse_rmb(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_RMA_ENABLED
/** Parse RMA - Recommended Minimum LORAN-C Data */
nmea_result_t nmea_parse_rma(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_GSA_ENABLED
/** Parse GSA - GNSS DOP and Active Satellites */
nmea_result_t nmea_parse_gsa(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_GSV_ENABLED
/** Parse GSV - GNSS Satellites in View */
nmea_result_t nmea_parse_gsv(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_GBS_ENABLED
/** Parse GBS - GNSS Satellite Fault Detection */
nmea_result_t nmea_parse_gbs(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_GST_ENABLED
/** Parse GST - GNSS Pseudorange Noise Statistics */
nmea_result_t nmea_parse_gst(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_GRS_ENABLED
/** Parse GRS - GNSS Range Residuals */
nmea_result_t nmea_parse_grs(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#if NMEA_SENTENCE_GFA_ENABLED
/** Parse GFA - GNSS Fix Accuracy and Integrity */
nmea_result_t nmea_parse_gfa(nmea_context_t* ctx, const nmea_tokens_t* tokens);
#endif

#endif /* NMEA_MODULE_GNSS_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_GNSS_H */
