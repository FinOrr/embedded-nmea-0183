/**
 * @file nmea_misc.h
 * @brief Miscellaneous sensors and data module
 *
 * Provides support for miscellaneous sentences including:
 * - Transducer measurements (XDR)
 * - Cross-track error (XTE/XTR)
 * - Time and date (ZDA, ZDL, ZFO, ZTG)
 * - Datum reference (DTM)
 * - Water current (CUR)
 * - Frequency information (FSI/SFI)
 * - Revolutions (RPM)
 * - Display control (DDC)
 * - Equipment property (EPV)
 * - Transmitter log (TRL)
 * - Water level detection (WAT)
 */

#ifndef NMEA_MISC_H
#define NMEA_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_types.h"
#include "nmea_config.h"

#if NMEA_MODULE_MISC_ENABLED

/**
 * @brief Miscellaneous module state structure
 */
typedef struct {
    /* XDR - Transducer Measurement */
    char xdr_type[4];                    /**< Transducer type */
    float xdr_measurement;               /**< Measurement value */
    char xdr_units[4];                   /**< Units of measure */
    char xdr_id[16];                     /**< Transducer ID */
    bool xdr_valid;                      /**< XDR data valid */

    /* XTE - Cross-Track Error, Measured */
    char xte_status1;                    /**< Status (A/V) */
    char xte_status2;                    /**< Status (A/V) */
    float xte_magnitude;                 /**< Cross-track error (nm) */
    char xte_direction;                  /**< Direction to steer (L/R) */
    char xte_mode;                       /**< Mode indicator (A/D/E/M/S/N) */
    bool xte_valid;                      /**< XTE data valid */

    /* XTR - Cross-Track Error, Dead Reckoning */
    float xtr_magnitude;                 /**< Cross-track error (nm) */
    char xtr_direction;                  /**< Direction to steer (L/R) */
    bool xtr_valid;                      /**< XTR data valid */

    /* ZDA - Time and Date */
    nmea_time_t zda_time;                /**< UTC time */
    nmea_date_t zda_date;                /**< UTC date */
    int8_t zda_local_zone_hours;         /**< Local zone hours offset */
    int8_t zda_local_zone_minutes;       /**< Local zone minutes offset */
    bool zda_valid;                      /**< ZDA data valid */

    /* ZDL - Time and Distance to Variable Point */
    nmea_time_t zdl_time_to_point;       /**< Time to point */
    float zdl_distance;                  /**< Distance to point (nm) */
    char zdl_point_type;                 /**< Point type (W/R/T/C) */
    bool zdl_valid;                      /**< ZDL data valid */

    /* ZFO - UTC and Time from Origin Waypoint */
    nmea_time_t zfo_utc;                 /**< UTC of observation */
    nmea_time_t zfo_elapsed_time;        /**< Elapsed time from origin */
    char zfo_origin_id[16];              /**< Origin waypoint ID */
    bool zfo_valid;                      /**< ZFO data valid */

    /* ZTG - UTC and Time to Destination Waypoint */
    nmea_time_t ztg_utc;                 /**< UTC of observation */
    nmea_time_t ztg_time_to_go;          /**< Time to destination */
    char ztg_dest_id[16];                /**< Destination waypoint ID */
    bool ztg_valid;                      /**< ZTG data valid */

    /* DTM - Datum Reference */
    char dtm_local_datum[4];             /**< Local datum code */
    char dtm_subdivision;                /**< Subdivision code */
    float dtm_lat_offset;                /**< Latitude offset (minutes) */
    char dtm_lat_offset_dir;             /**< Latitude offset direction (N/S) */
    float dtm_lon_offset;                /**< Longitude offset (minutes) */
    char dtm_lon_offset_dir;             /**< Longitude offset direction (E/W) */
    float dtm_altitude_offset;           /**< Altitude offset (meters) */
    char dtm_ref_datum[4];               /**< Reference datum */
    bool dtm_valid;                      /**< DTM data valid */

    /* CUR - Water Current Layer */
    char cur_validity;                   /**< Data validity (A/V) */
    uint8_t cur_data_set_number;        /**< Data set number (0-9) */
    uint8_t cur_layer_number;            /**< Layer number */
    float cur_depth;                     /**< Current depth (meters) */
    float cur_direction;                 /**< Current direction (degrees) */
    char cur_direction_ref;              /**< Direction reference (T/R) */
    float cur_speed;                     /**< Current speed (knots) */
    float cur_ref_layer_depth;           /**< Reference layer depth (meters) */
    float cur_heading;                   /**< Heading */
    char cur_heading_ref;                /**< Heading reference (T/M) */
    char cur_speed_ref;                  /**< Speed reference (P/W/B) */
    bool cur_valid;                      /**< CUR data valid */

    /* FSI - Frequency Set Information */
    char fsi_tx_freq[8];                 /**< Transmit frequency */
    char fsi_rx_freq[8];                 /**< Receive frequency */
    char fsi_mode;                       /**< Mode of operation */
    uint8_t fsi_power_level;             /**< Power level (0-9) */
    char fsi_status_flag;                /**< Status flag (R/C) */
    bool fsi_valid;                      /**< FSI data valid */

    /* SFI - Scanning Frequency Information */
    uint8_t sfi_total_sentences;         /**< Total sentences */
    uint8_t sfi_sentence_number;         /**< Sentence number */
    char sfi_frequencies[6][8];          /**< Scanning frequencies */
    char sfi_modes[6];                   /**< Operating modes */
    bool sfi_valid;                      /**< SFI data valid */

    /* RPM - Revolutions */
    char rpm_source;                     /**< Source (S/E) */
    uint8_t rpm_number;                  /**< Engine/shaft number */
    float rpm_speed;                     /**< Speed (RPM) */
    float rpm_pitch;                     /**< Propeller pitch (%) */
    char rpm_status;                     /**< Status (A/V) */
    bool rpm_valid;                      /**< RPM data valid */

    /* DDC - Display Dimming Control */
    char ddc_dimming_preset;             /**< Dimming preset (D/K/N/O) */
    uint8_t ddc_brightness;              /**< Brightness (0-99) */
    char ddc_color_palette;              /**< Color palette (D/K/N/O) */
    char ddc_status_flag;                /**< Status flag (R/C) */
    bool ddc_valid;                      /**< DDC data valid */

    /* EPV - Equipment Property Value */
    char epv_status_flag;                /**< Status flag (R/C) */
    char epv_equipment_type[4];          /**< Equipment type */
    char epv_unique_id[20];              /**< Unique identifier */
    uint16_t epv_property_id;            /**< Property identifier */
    char epv_property_value[32];         /**< Property value */
    bool epv_valid;                      /**< EPV data valid */

    /* TRL - Transmitter Non-functioning Log */
    uint8_t trl_total_entries;           /**< Total log entries */
    uint8_t trl_entry_number;            /**< Log entry number */
    uint8_t trl_message_id;              /**< Sequential message ID */
    char trl_switch_off_date[10];        /**< Switch off date */
    nmea_time_t trl_switch_off_time;     /**< Switch off time */
    char trl_switch_on_date[10];         /**< Switch on date */
    nmea_time_t trl_switch_on_time;      /**< Switch on time */
    uint8_t trl_reason_code;             /**< Reason code */
    bool trl_valid;                      /**< TRL data valid */

    /* WAT - Water Level Detection */
    char wat_message_type;               /**< Message type (S/E/F) */
    nmea_time_t wat_timestamp;           /**< Timestamp */
    char wat_alarm_type[4];              /**< Alarm system type */
    char wat_location1[4];               /**< First location indicator */
    char wat_location2[4];               /**< Second location indicator */
    uint16_t wat_detection_point;        /**< Detection point number */
    char wat_alarm_condition;            /**< Alarm condition (N/H/J/L/K/X) */
    char wat_override;                   /**< Override setting (O/N) */
    char wat_description[32];            /**< Message description */
    bool wat_valid;                      /**< WAT data valid */
} nmea_misc_state_t;

/* Forward declarations */
struct nmea_context;
struct nmea_tokens;

/* Parser declarations */
#if NMEA_SENTENCE_XDR_ENABLED
nmea_result_t nmea_parse_xdr(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_XTE_ENABLED
nmea_result_t nmea_parse_xte(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_XTR_ENABLED
nmea_result_t nmea_parse_xtr(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_ZDA_ENABLED
nmea_result_t nmea_parse_zda(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_ZDL_ENABLED
nmea_result_t nmea_parse_zdl(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_ZFO_ENABLED
nmea_result_t nmea_parse_zfo(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_ZTG_ENABLED
nmea_result_t nmea_parse_ztg(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_DTM_ENABLED
nmea_result_t nmea_parse_dtm(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_CUR_ENABLED
nmea_result_t nmea_parse_cur(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_FSI_ENABLED
nmea_result_t nmea_parse_fsi(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_GEN_ENABLED
nmea_result_t nmea_parse_gen(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_RPM_ENABLED
nmea_result_t nmea_parse_rpm(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_SFI_ENABLED
nmea_result_t nmea_parse_sfi(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_DDC_ENABLED
nmea_result_t nmea_parse_ddc(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_EPV_ENABLED
nmea_result_t nmea_parse_epv(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_TRL_ENABLED
nmea_result_t nmea_parse_trl(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_WAT_ENABLED
nmea_result_t nmea_parse_wat(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif

#endif /* NMEA_MODULE_MISC_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_MISC_H */
