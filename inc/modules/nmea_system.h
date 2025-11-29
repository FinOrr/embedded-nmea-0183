/**
 * @file nmea_system.h
 * @brief System status and monitoring module
 *
 * Provides support for system status and monitoring sentences including:
 * - Heartbeat supervision (HBT)
 * - Hull stress surveillance (HSS)
 * - Event messages (EVE)
 * - Engine telegraph (ETL)
 * - NAVTEX receiver (NRM/NRX)
 * - Navigation status (NSR)
 * - Device position (POS)
 * - MSK receiver (MSK/MSS)
 * - Rudder order (ROR)
 */

#ifndef NMEA_SYSTEM_H
#define NMEA_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_types.h"
#include "nmea_config.h"

#if NMEA_MODULE_SYSTEM_ENABLED

/**
 * @brief System module state structure
 */
typedef struct {
    /* HBT - Heartbeat Supervision (not in spec files, but mentioned in status) */
    nmea_time_t heartbeat_time;         /**< Heartbeat time */
    bool heartbeat_valid;                /**< Heartbeat data valid */

    /* HSS - Hull Stress Surveillance */
    char hss_measurement_point[16];      /**< Measurement point ID */
    float hss_measurement_value;         /**< Measurement value */
    char hss_status;                     /**< Data status (A/V) */
    bool hss_valid;                      /**< HSS data valid */

    /* EVE - General Event Message */
    nmea_time_t eve_time;                /**< Event time */
    char eve_tag_code[16];               /**< Tag code for event source */
    char eve_description[64];            /**< Event description */
    bool eve_valid;                      /**< EVE data valid */

    /* ETL - Engine Telegraph Operation Status */
    nmea_time_t etl_time;                /**< Event time */
    char etl_message_type;               /**< Message type (O/A) */
    uint8_t etl_position;                /**< Telegraph position (00-15) */
    uint8_t etl_sub_position;            /**< Sub-telegraph position (20/30/40) */
    char etl_location;                   /**< Operating location (B/P/S/C/E/W) */
    uint8_t etl_engine_number;           /**< Engine/propeller shaft number */
    bool etl_valid;                      /**< ETL data valid */

    /* NRM - NAVTEX Receiver Mask */
    uint8_t nrm_function_code;           /**< Function code (0-9) */
    uint8_t nrm_frequency_index;         /**< Frequency table index (1-9) */
    uint32_t nrm_transmitter_mask;       /**< Transmitter coverage area mask */
    uint32_t nrm_message_type_mask;      /**< Message type mask */
    char nrm_status_flag;                /**< Sentence status (R/C) */
    bool nrm_valid;                      /**< NRM data valid */

    /* NRX - NAVTEX Received Message */
    uint16_t nrx_total_sentences;        /**< Number of sentences (001-999) */
    uint16_t nrx_sentence_number;        /**< Sentence number (001-999) */
    uint8_t nrx_message_id;              /**< Sequential message ID (00-99) */
    char nrx_message_code[8];            /**< NAVTEX message code */
    uint8_t nrx_frequency_index;         /**< Frequency table index (0-9) */
    nmea_time_t nrx_time;                /**< UTC of receipt */
    nmea_date_t nrx_date;                /**< Date of receipt */
    uint16_t nrx_total_chars;            /**< Total characters in message */
    uint16_t nrx_bad_chars;              /**< Total bad characters */
    char nrx_status;                     /**< Status (A/V) */
    char nrx_message_body[256];          /**< Message body text */
    bool nrx_valid;                      /**< NRX data valid */

    /* NSR - Navigation Status Report */
    char nsr_heading_integrity;          /**< Heading integrity (P/F/D/N) */
    char nsr_heading_plausibility;       /**< Heading plausibility (A/V/N) */
    char nsr_position_integrity;         /**< Position integrity (P/F/D/N) */
    char nsr_position_plausibility;      /**< Position plausibility (A/V/N) */
    char nsr_stw_integrity;              /**< STW integrity (P/F/D/N) */
    char nsr_stw_plausibility;           /**< STW plausibility (A/V/N) */
    char nsr_sog_cog_integrity;          /**< SOG/COG integrity (P/F/D/N) */
    char nsr_sog_cog_plausibility;       /**< SOG/COG plausibility (A/V/N) */
    char nsr_depth_integrity;            /**< Depth integrity (P/F/D/N) */
    char nsr_depth_plausibility;         /**< Depth plausibility (A/V/N) */
    char nsr_stw_mode;                   /**< STW mode (W/E/M/N) */
    char nsr_time_integrity;             /**< Time integrity (P/F/D/N) */
    char nsr_time_plausibility;          /**< Time plausibility (A/V/N) */
    bool nsr_valid;                      /**< NSR data valid */

    /* POS - Device Position and Ship Dimensions */
    char pos_equipment_id[4];            /**< Equipment identification */
    uint8_t pos_equipment_number;        /**< Equipment number (00-99) */
    char pos_validity;                   /**< Position validity (A/V) */
    float pos_x_coordinate;              /**< X-coordinate in meters */
    float pos_y_coordinate;              /**< Y-coordinate in meters */
    float pos_z_coordinate;              /**< Z-coordinate in meters */
    char pos_ship_validity;              /**< Ship dimensions validity (A/V) */
    float pos_ship_length;               /**< Ship length in meters */
    float pos_ship_width;                /**< Ship width in meters */
    char pos_status_flag;                /**< Status flag (R/C) */
    bool pos_valid;                      /**< POS data valid */

    /* MSK - MSK Receiver Interface */
    float msk_frequency;                 /**< Beacon frequency (kHz) */
    char msk_frequency_mode;             /**< Auto/manual (A/M) */
    float msk_bit_rate;                  /**< Bit rate (25/50/100/200) */
    char msk_bit_rate_mode;              /**< Auto/manual (A/M) */
    float msk_status_interval;           /**< MSS status interval (seconds) */
    uint8_t msk_channel;                 /**< Channel number */
    char msk_status_flag;                /**< Status flag (R/C) */
    bool msk_valid;                      /**< MSK data valid */

    /* MSS - MSK Receiver Signal Status */
    float mss_signal_strength;           /**< Signal strength (dB/1uV/m) */
    float mss_snr;                       /**< Signal-to-noise ratio (dB) */
    float mss_frequency;                 /**< Beacon frequency (kHz) */
    float mss_bit_rate;                  /**< Bit rate (bit/s) */
    uint8_t mss_channel;                 /**< Channel number */
    bool mss_valid;                      /**< MSS data valid */

    /* ROR - Rudder Order Status (not in spec files, assumed structure) */
    float ror_starboard_order;           /**< Starboard rudder order */
    float ror_port_order;                /**< Port rudder order */
    bool ror_valid;                      /**< ROR data valid */
} nmea_system_state_t;

/* Forward declarations */
struct nmea_context;
struct nmea_tokens;

/* Parser declarations */
#if NMEA_SENTENCE_HBT_ENABLED
nmea_result_t nmea_parse_hbt(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_HSS_ENABLED
nmea_result_t nmea_parse_hss(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_EVE_ENABLED
nmea_result_t nmea_parse_eve(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_ETL_ENABLED
nmea_result_t nmea_parse_etl(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_NRM_ENABLED
nmea_result_t nmea_parse_nrm(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_NRX_ENABLED
nmea_result_t nmea_parse_nrx(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_NSR_ENABLED
nmea_result_t nmea_parse_nsr(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_POS_ENABLED
nmea_result_t nmea_parse_pos(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_MSK_ENABLED
nmea_result_t nmea_parse_msk(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_MSS_ENABLED
nmea_result_t nmea_parse_mss(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_ROR_ENABLED
nmea_result_t nmea_parse_ror(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#endif /* NMEA_MODULE_SYSTEM_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_SYSTEM_H */
