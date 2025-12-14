/**
 * @file nmea_comm.h
 * @brief Communications module
 *
 * Provides support for communication protocol sentences including:
 * - Digital Selective Calling (DSC/DSE)
 * - Text messages (TXT/TUT)
 * - Version information (VER)
 * - User identification (UID)
 * - Security (SPW)
 * - Acknowledgements (NAK)
 * - Return link messages (RLM)
 * - Data source identification (STN)
 */

#ifndef NMEA_COMM_H
#define NMEA_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_config.h"
#include "nmea_types.h"

#if NMEA_MODULE_COMM_ENABLED

/**
 * @brief Communication module state structure
 */
typedef struct {
  /* DSC - Digital Selective Calling */
  char dsc_address[16];       /**< MMSI or geographic area */
  char dsc_category[4];       /**< Format specifier */
  char dsc_nature[4];         /**< Nature of distress or telecommand */
  char dsc_type[4];           /**< Type of communication */
  char dsc_mmsi_distress[16]; /**< MMSI of ship in distress */
  char dsc_position[16];      /**< Position or channel/frequency */
  char dsc_time[8];           /**< Time or telephone number */
  char dsc_ack;               /**< Acknowledgement type (R/B/S) */
  bool dsc_expansion;         /**< Expansion indicator */
  bool dsc_valid;             /**< DSC data valid */

  /* DSE - Expanded Digital Selective Calling */
  char dse_mmsi[16];           /**< Vessel MMSI */
  char dse_query_reply;        /**< Query/reply flag (Q/R/A) */
  uint8_t dse_total_sentences; /**< Total number of sentences */
  uint8_t dse_sentence_number; /**< Current sentence number */
  bool dse_valid;              /**< DSE data valid */

  /* TXT - Text Transmission */
  char txt_message[256];       /**< Text message content */
  uint8_t txt_identifier;      /**< Text identifier (01-99) */
  uint8_t txt_total_sentences; /**< Total sentences in message */
  uint8_t txt_sentence_number; /**< Current sentence number */
  bool txt_valid;              /**< Text message valid */

  /* TUT - Multi-language Text */
  char tut_source_id[4];        /**< Source identifier */
  char tut_translation_code[8]; /**< Translation code */
  char tut_text_body[128];      /**< Hex-encoded text body */
  uint8_t tut_total_sentences;  /**< Total sentences in message */
  uint8_t tut_sentence_number;  /**< Current sentence number */
  uint8_t tut_message_id;       /**< Sequential message identifier */
  bool tut_valid;               /**< TUT data valid */

  /* VER - Version */
  char ver_device_type[4];     /**< Device type */
  char ver_vendor_id[8];       /**< Vendor ID */
  char ver_unique_id[20];      /**< Unique identifier */
  char ver_serial_number[36];  /**< Manufacturer serial number */
  char ver_model_code[36];     /**< Model code/product code */
  char ver_sw_revision[36];    /**< Software revision */
  char ver_hw_revision[36];    /**< Hardware revision */
  uint8_t ver_total_sentences; /**< Total sentences needed */
  uint8_t ver_sentence_number; /**< Current sentence number */
  uint8_t ver_message_id;      /**< Sequential message identifier */
  bool ver_valid;              /**< Version data valid */

  /* UID - User Identification */
  char uid_code1[24]; /**< User identification code 1 */
  char uid_code2[24]; /**< User identification code 2 (optional) */
  bool uid_valid;     /**< UID data valid */

  /* SPW - Security Password */
  char spw_sentence[8];   /**< Password protected sentence */
  char spw_unique_id[20]; /**< Unique identifier */
  char spw_password[36];  /**< Password text */
  uint8_t spw_level;      /**< Password level (1-9) */
  bool spw_valid;         /**< SPW data valid */

  /* NAK - Negative Acknowledgement */
  char nak_talker_id[4];    /**< Talker identifier */
  char nak_sentence[8];     /**< Affected sentence formatter */
  char nak_unique_id[20];   /**< Unique identifier */
  char nak_description[64]; /**< Descriptive text */
  uint8_t nak_reason_code;  /**< Reason code */
  bool nak_valid;           /**< NAK data valid */

  /* RLM - Return Link Message */
  char rlm_beacon_id[20];    /**< Beacon ID (15 hex chars) */
  nmea_time_t rlm_time;      /**< Time of reception */
  char rlm_message_code;     /**< Message code (0-F) */
  char rlm_message_body[32]; /**< Message body (hex) */
  bool rlm_valid;            /**< RLM data valid */

  /* STN - Multiple Data ID */
  uint8_t stn_talker_number; /**< Talker ID number (00-99) */
  bool stn_valid;            /**< STN data valid */
} nmea_comm_state_t;

/* Forward declarations */
struct nmea_context;
struct nmea_tokens;

/* Parser declarations */
#if NMEA_SENTENCE_DSC_ENABLED
nmea_result_t nmea_parse_dsc(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_DSE_ENABLED
nmea_result_t nmea_parse_dse(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_VER_ENABLED
nmea_result_t nmea_parse_ver(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_UID_ENABLED
nmea_result_t nmea_parse_uid(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_SPW_ENABLED
nmea_result_t nmea_parse_spw(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_TUT_ENABLED
nmea_result_t nmea_parse_tut(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_TXT_ENABLED
nmea_result_t nmea_parse_txt(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_NAK_ENABLED
nmea_result_t nmea_parse_nak(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_RLM_ENABLED
nmea_result_t nmea_parse_rlm(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#if NMEA_SENTENCE_STN_ENABLED
nmea_result_t nmea_parse_stn(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif

#endif /* NMEA_MODULE_COMM_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_COMM_H */
