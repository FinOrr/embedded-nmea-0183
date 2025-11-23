/**
 * @file nmea_navigation.h
 * @brief Navigation and routing module
 */

#ifndef NMEA_NAVIGATION_H
#define NMEA_NAVIGATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../nmea_types.h"
#include "../nmea_config.h"

#if NMEA_MODULE_NAVIGATION_ENABLED

/**
 * @brief Navigation and routing state data
 */
typedef struct {
    /* APB - Autopilot sentence B */
    char apb_status1;                  /**< General warning flag: A = OK, V = warning */
    char apb_status2;                  /**< Cycle lock warning: A = OK, V = warning */
    float xte_magnitude;               /**< Cross-track error magnitude, nm */
    char xte_steer_direction;          /**< Direction to steer: L/R */
    char arrival_circle_entered;       /**< A = entered, V = not entered */
    char perpendicular_passed;         /**< A = passed, V = not passed */
    float bearing_origin_to_dest;      /**< Bearing from origin to dest, degrees */
    char bearing_origin_type;          /**< M = magnetic, T = true */
    char destination_waypoint_id[16];  /**< Destination waypoint ID */
    float bearing_position_to_dest;    /**< Bearing from position to dest, degrees */
    char bearing_position_type;        /**< M = magnetic, T = true */
    float heading_to_steer;            /**< Heading to steer to dest, degrees */
    char heading_to_steer_type;        /**< M = magnetic, T = true */
    char apb_mode;                     /**< Mode: A/D/E/M/S/N */

    /* BWC/BWR - Bearing and distance to waypoint */
    nmea_time_t waypoint_utc;          /**< UTC time of observation */
    nmea_coordinate_t waypoint_latitude;  /**< Waypoint latitude */
    nmea_coordinate_t waypoint_longitude; /**< Waypoint longitude */
    float bearing_true;                /**< Bearing to waypoint, degrees true */
    float bearing_magnetic;            /**< Bearing to waypoint, degrees magnetic */
    float distance_nm;                 /**< Distance to waypoint, nautical miles */
    char waypoint_id[16];              /**< Waypoint ID */
    char bw_mode;                      /**< Mode: A/D/E/M/S/N */

    /* BEC - Bearing and distance to waypoint (dead reckoning) */
    nmea_time_t bec_utc;               /**< UTC time of DR observation */
    nmea_coordinate_t bec_waypoint_lat;   /**< DR waypoint latitude */
    nmea_coordinate_t bec_waypoint_lon;   /**< DR waypoint longitude */
    float bec_bearing_true;            /**< DR bearing, degrees true */
    float bec_bearing_magnetic;        /**< DR bearing, degrees magnetic */
    float bec_distance_nm;             /**< DR distance, nm */
    char bec_waypoint_id[16];          /**< DR waypoint ID */

    /* BOD - Bearing origin to destination */
    float bod_bearing_true;            /**< Bearing origin to dest, degrees true */
    float bod_bearing_magnetic;        /**< Bearing origin to dest, degrees magnetic */
    char bod_dest_id[16];              /**< Destination waypoint ID */
    char bod_origin_id[16];            /**< Origin waypoint ID */

    /* BWW - Bearing waypoint to waypoint */
    float bww_bearing_true;            /**< Bearing WP to WP, degrees true */
    float bww_bearing_magnetic;        /**< Bearing WP to WP, degrees magnetic */
    char bww_to_id[16];                /**< TO waypoint ID */
    char bww_from_id[16];              /**< FROM waypoint ID */

    /* RTE - Routes */
    uint8_t rte_total_sentences;       /**< Total number of RTE sentences */
    uint8_t rte_sentence_number;       /**< Current sentence number */
    char rte_mode;                     /**< c = complete, w = working */
    char route_id[16];                 /**< Route identifier */
    char waypoint_list[12][16];        /**< Waypoint IDs (up to 12) */
    uint8_t waypoint_count;            /**< Number of waypoints in list */

    /* AAM - Waypoint arrival alarm */
    char aam_arrival_circle;           /**< A = entered, V = not entered */
    char aam_perpendicular;            /**< A = passed, V = not passed */
    float aam_radius;                  /**< Arrival circle radius, nm */
    char aam_waypoint_id[16];          /**< Waypoint ID */

    /* Validity flags */
    bool apb_valid;                    /**< APB data valid */
    bool bwc_valid;                    /**< BWC data valid */
    bool bwr_valid;                    /**< BWR data valid */
    bool bec_valid;                    /**< BEC data valid */
    bool bod_valid;                    /**< BOD data valid */
    bool bww_valid;                    /**< BWW data valid */
    bool rte_valid;                    /**< RTE data valid */
    bool aam_valid;                    /**< AAM data valid */
} nmea_navigation_state_t;

/* Parser function declarations */
struct nmea_context;
struct nmea_tokens;

#if NMEA_SENTENCE_APB_ENABLED
nmea_result_t nmea_parse_apb(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_BWC_ENABLED
nmea_result_t nmea_parse_bwc(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_BWR_ENABLED
nmea_result_t nmea_parse_bwr(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_BEC_ENABLED
nmea_result_t nmea_parse_bec(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_BOD_ENABLED
nmea_result_t nmea_parse_bod(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_BWW_ENABLED
nmea_result_t nmea_parse_bww(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_RTE_ENABLED
nmea_result_t nmea_parse_rte(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_AAM_ENABLED
nmea_result_t nmea_parse_aam(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#endif /* NMEA_MODULE_NAVIGATION_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_NAVIGATION_H */
