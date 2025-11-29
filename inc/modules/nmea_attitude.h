/**
 * @file nmea_attitude.h
 * @brief Attitude and motion module
 *
 * Provides support for attitude and motion sentences including:
 * - Heel angle and roll measurement (HRM)
 * - Set and drift (VDR)
 * - Propulsion remote control (PRC)
 * - Thruster control and response (TRC/TRD)
 */

#ifndef NMEA_ATTITUDE_H
#define NMEA_ATTITUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_types.h"
#include "nmea_config.h"

#if NMEA_MODULE_ATTITUDE_ENABLED

/**
 * @brief Attitude module state structure
 */
typedef struct {
    /* HRM - Heel Angle, Roll Period, Roll Amplitude Measurement */
    float hrm_heel_angle;                /**< Actual heel angle (degrees, +starboard/-port) */
    float hrm_roll_period;               /**< Roll period (seconds) */
    float hrm_roll_amplitude_port;       /**< Roll amplitude port side (degrees) */
    float hrm_roll_amplitude_starboard;  /**< Roll amplitude starboard side (degrees) */
    char hrm_status;                     /**< Status (A/V) */
    float hrm_peak_hold_port;            /**< Roll peak hold value port (degrees) */
    float hrm_peak_hold_starboard;       /**< Roll peak hold value starboard (degrees) */
    nmea_time_t hrm_peak_reset_time;    /**< Peak hold reset time */
    uint8_t hrm_peak_reset_day;         /**< Peak hold reset day (1-31) */
    uint8_t hrm_peak_reset_month;       /**< Peak hold reset month (1-12) */
    bool hrm_valid;                      /**< HRM data valid */

    /* VDR - Set and Drift */
    float vdr_direction_true;            /**< Current direction true (degrees) */
    float vdr_direction_magnetic;        /**< Current direction magnetic (degrees) */
    float vdr_speed_knots;               /**< Current speed (knots) */
    bool vdr_valid;                      /**< VDR data valid */

    /* PRC - Propulsion Remote Control Status */
    float prc_lever_demand;              /**< Lever demand position (-100 to 100%) */
    char prc_lever_status;               /**< Lever demand status (A/V) */
    float prc_rpm_demand;                /**< RPM demand value */
    char prc_rpm_mode;                   /**< RPM mode (P/R/V) */
    float prc_pitch_demand;              /**< Pitch demand value */
    char prc_pitch_mode;                 /**< Pitch mode (P/D/V) */
    char prc_location;                   /**< Operating location (B/P/S/C/E/W) */
    uint8_t prc_engine_number;           /**< Engine/propeller shaft number */
    bool prc_valid;                      /**< PRC data valid */

    /* TRC - Thruster Control Data */
    uint8_t trc_thruster_number;        /**< Thruster number (odd=bow, even=stern) */
    float trc_rpm_demand;                /**< RPM demand value */
    char trc_rpm_mode;                   /**< RPM mode (P/R/V) */
    float trc_pitch_demand;              /**< Pitch demand value */
    char trc_pitch_mode;                 /**< Pitch mode (P/D/V) */
    float trc_azimuth_demand;            /**< Azimuth demand (0-360 degrees) */
    char trc_location;                   /**< Operating location (B/P/S/C/E/W) */
    char trc_status_flag;                /**< Status flag (R/C) */
    bool trc_valid;                      /**< TRC data valid */

    /* TRD - Thruster Response Data */
    uint8_t trd_thruster_number;        /**< Thruster number (odd=bow, even=stern) */
    float trd_rpm_response;              /**< RPM response value */
    char trd_rpm_mode;                   /**< RPM mode (P/R/V) */
    float trd_pitch_response;            /**< Pitch response value */
    char trd_pitch_mode;                 /**< Pitch mode (P/D/V) */
    float trd_azimuth_response;          /**< Azimuth response (0-360 degrees) */
    bool trd_valid;                      /**< TRD data valid */
} nmea_attitude_state_t;

/* Forward declarations */
struct nmea_context;
struct nmea_tokens;

/* Parser declarations */
#if NMEA_SENTENCE_HRM_ENABLED
nmea_result_t nmea_parse_hrm(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_VDR_ENABLED
nmea_result_t nmea_parse_vdr(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_PRC_ENABLED
nmea_result_t nmea_parse_prc(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_TRC_ENABLED
nmea_result_t nmea_parse_trc(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_TRD_ENABLED
nmea_result_t nmea_parse_trd(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#endif /* NMEA_MODULE_ATTITUDE_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_ATTITUDE_H */
