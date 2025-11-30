/**
 * @file nmea_ais.h
 * @brief Automatic Identification System (AIS) module
 */

#ifndef NMEA_AIS_H
#define NMEA_AIS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_config.h"
#include "nmea_types.h"

#if NMEA_MODULE_AIS_ENABLED

/**
 * @brief AIS state data
 */
typedef struct {
  /* VDM/VDO - AIS VHF Data-Link Message */
  uint8_t vdm_total_sentences; /**< Total sentences for message */
  uint8_t vdm_sentence_number; /**< Current sentence number */
  uint8_t vdm_sequential_id;   /**< Sequential message ID, 0-9 */
  char vdm_channel;            /**< AIS channel: A/B/C/D */
  char vdm_payload[256];       /**< Encapsulated ITU-R M.1371 message */
  uint8_t vdm_fill_bits;       /**< Number of fill-bits, 0-5 */

  /* ABK - AIS Addressed and Binary Broadcast Acknowledgement */
  uint32_t abk_mmsi;           /**< MMSI of addressed AIS unit */
  char abk_channel;            /**< AIS channel of reception: A/B */
  uint8_t abk_message_id;      /**< ITU-R M.1371 Message ID */
  uint8_t abk_sequence_number; /**< Message sequence number */
  uint8_t abk_ack_type;        /**< Type of acknowledgement, 0-4 */

  /* ABM - AIS Addressed Binary Message */
  uint8_t abm_total_sentences;   /**< Total sentences, 1-9 */
  uint8_t abm_sentence_number;   /**< Sentence number, 1-9 */
  uint8_t abm_sequential_id;     /**< Sequential ID, 0-3 */
  uint32_t abm_destination_mmsi; /**< Destination MMSI */
  uint8_t abm_channel;           /**< Broadcast channel, 0-3 */
  uint8_t abm_message_id;        /**< ITU-R M.1371 Message ID */
  char abm_data[256];            /**< Encapsulated data */
  uint8_t abm_fill_bits;         /**< Fill-bits, 0-5 */

  /* BBM - AIS Broadcast Binary Message */
  uint8_t bbm_total_sentences; /**< Total sentences, 1-9 */
  uint8_t bbm_sentence_number; /**< Sentence number, 1-9 */
  uint8_t bbm_sequential_id;   /**< Sequential ID, 0-9 */
  uint8_t bbm_channel;         /**< Broadcast channel, 0-3 */
  uint8_t bbm_message_id;      /**< ITU-R M.1371 Message ID */
  char bbm_data[256];          /**< Encapsulated data */
  uint8_t bbm_fill_bits;       /**< Fill-bits, 0-5 */

  /* AIR - AIS Interrogation Request */
  uint32_t air_station1_mmsi; /**< Station 1 MMSI */
  uint8_t air_station1_msg1;  /**< Station 1 first message requested */
  uint8_t air_station1_msg1_subsection; /**< Message sub-section */
  uint8_t air_station1_msg2; /**< Station 1 second message requested */
  uint8_t air_station1_msg2_subsection; /**< Message sub-section */
  uint32_t air_station2_mmsi;           /**< Station 2 MMSI */
  uint8_t air_station2_msg;             /**< Station 2 message requested */
  uint8_t air_station2_msg_subsection;  /**< Message sub-section */
  char air_channel;                     /**< Channel: A/B */
  uint16_t air_msg_id1_1_slot;          /**< Station 1 reply slot */
  uint16_t air_msg_id1_2_slot;          /**< Station 1 reply slot */
  uint16_t air_msg_id2_1_slot;          /**< Station 2 reply slot */

  /* ACA - AIS Channel Assignment */
  uint8_t aca_sequence;               /**< Sequence number, 0-9 */
  nmea_coordinate_t aca_ne_latitude;  /**< NE corner latitude */
  nmea_coordinate_t aca_ne_longitude; /**< NE corner longitude */
  nmea_coordinate_t aca_sw_latitude;  /**< SW corner latitude */
  nmea_coordinate_t aca_sw_longitude; /**< SW corner longitude */
  uint8_t aca_transition_zone;        /**< Transition zone size */
  uint16_t aca_channel_a;             /**< Channel A */
  uint8_t aca_channel_a_bandwidth;    /**< Channel A bandwidth */
  uint16_t aca_channel_b;             /**< Channel B */
  uint8_t aca_channel_b_bandwidth;    /**< Channel B bandwidth */
  uint8_t aca_txrx_mode;              /**< Tx/Rx mode control, 0-5 */
  uint8_t aca_power;                  /**< Power level: 0=high, 1=low */
  char aca_info_source;               /**< Info source: A/B/C/D/M */
  uint8_t aca_in_use;                 /**< In-use flag: 0/1 */
  nmea_time_t aca_in_use_time;        /**< Time of in-use change */

  /* ACS - AIS Channel Management Information Source */
  uint8_t acs_sequence;         /**< Sequence number, 0-9 */
  uint32_t acs_mmsi;            /**< MMSI of originator */
  nmea_time_t acs_receipt_time; /**< UTC of receipt */
  uint8_t acs_day;              /**< Day, 01-31 */
  uint8_t acs_month;            /**< Month, 01-12 */
  uint16_t acs_year;            /**< Year */

  /* LRF - AIS Long Range Function */
  uint8_t lrf_sequence;          /**< Sequence number, 0-9 */
  uint32_t lrf_requestor_mmsi;   /**< MMSI of requestor */
  char lrf_requestor_name[21];   /**< Name of requestor */
  char lrf_function_request[27]; /**< Function request chars */
  char lrf_function_reply[27];   /**< Function reply status */

  /* LRI - AIS Long Range Interrogation */
  uint8_t lri_sequence;               /**< Sequence number, 0-9 */
  char lri_control_flag;              /**< Control flag: 0/1 */
  uint32_t lri_requestor_mmsi;        /**< MMSI of requestor */
  uint32_t lri_destination_mmsi;      /**< MMSI of destination */
  nmea_coordinate_t lri_ne_latitude;  /**< NE corner latitude */
  nmea_coordinate_t lri_ne_longitude; /**< NE corner longitude */
  nmea_coordinate_t lri_sw_latitude;  /**< SW corner latitude */
  nmea_coordinate_t lri_sw_longitude; /**< SW corner longitude */

  /* SSD - AIS Ship Static Data */
  char ssd_callsign[8];   /**< Ship's call sign */
  char ssd_name[21];      /**< Ship's name */
  uint16_t ssd_pos_ref_a; /**< Dist from bow, 0-511m */
  uint16_t ssd_pos_ref_b; /**< Dist from stern, 0-511m */
  uint8_t ssd_pos_ref_c;  /**< Dist from port, 0-63m */
  uint8_t ssd_pos_ref_d;  /**< Dist from starboard, 0-63m */
  uint8_t ssd_dte;        /**< DTE indicator: 0/1 */
  char ssd_source[3];     /**< Source identifier */

  /* VSD - AIS Voyage Static Data */
  uint8_t vsd_ship_type;      /**< Type of ship/cargo, 0-255 */
  float vsd_draught;          /**< Maximum draught, 0-25.5m */
  uint16_t vsd_persons;       /**< Persons on-board, 0-8191 */
  char vsd_destination[21];   /**< Destination */
  nmea_time_t vsd_eta_time;   /**< ETA time */
  uint8_t vsd_eta_day;        /**< ETA day, 00-31 */
  uint8_t vsd_eta_month;      /**< ETA month, 00-12 */
  uint8_t vsd_regional_flag1; /**< Regional app flag, 0-15 */
  uint8_t vsd_regional_flag2; /**< Regional app flag, 0-15 */

  /* Validity flags */
  bool vdm_valid; /**< VDM data valid */
  bool vdo_valid; /**< VDO data valid */
  bool abk_valid; /**< ABK data valid */
  bool abm_valid; /**< ABM data valid */
  bool bbm_valid; /**< BBM data valid */
  bool air_valid; /**< AIR data valid */
  bool aca_valid; /**< ACA data valid */
  bool acs_valid; /**< ACS data valid */
  bool lrf_valid; /**< LRF data valid */
  bool lri_valid; /**< LRI data valid */
  bool ssd_valid; /**< SSD data valid */
  bool vsd_valid; /**< VSD data valid */
} nmea_ais_state_t;

/* Parser function declarations */
struct nmea_context;
struct nmea_tokens;

#if NMEA_SENTENCE_VDM_ENABLED
nmea_result_t nmea_parse_vdm(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_VDO_ENABLED
nmea_result_t nmea_parse_vdo(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_ABK_ENABLED
nmea_result_t nmea_parse_abk(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_ABM_ENABLED
nmea_result_t nmea_parse_abm(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_BBM_ENABLED
nmea_result_t nmea_parse_bbm(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_AIR_ENABLED
nmea_result_t nmea_parse_air(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_ACA_ENABLED
nmea_result_t nmea_parse_aca(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_ACS_ENABLED
nmea_result_t nmea_parse_acs(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_LRF_ENABLED
nmea_result_t nmea_parse_lrf(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_LRI_ENABLED
nmea_result_t nmea_parse_lri(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_SSD_ENABLED
nmea_result_t nmea_parse_ssd(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#if NMEA_SENTENCE_VSD_ENABLED
nmea_result_t nmea_parse_vsd(struct nmea_context* ctx,
                             const struct nmea_tokens* tokens);
#endif

#endif /* NMEA_MODULE_AIS_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_AIS_H */
