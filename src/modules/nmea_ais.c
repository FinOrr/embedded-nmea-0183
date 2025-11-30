/**
 * @file nmea_ais.c
 * @brief Automatic Identification System (AIS) module implementation
 */

#include "modules/nmea_ais.h"

#include <stdlib.h>
#include <string.h>

#include "nmea_internal.h"

#if NMEA_MODULE_AIS_ENABLED

/**
 * @brief Parse uint32_t value from token
 */
static nmea_result_t nmea_parse_uint32(const char* token, uint32_t* value) {
  if (value == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  if (nmea_is_empty_token(token)) {
    return NMEA_ERROR_NO_DATA;
  }

  char* endptr;
  unsigned long val = strtoul(token, &endptr, 10);

  if (endptr == token || *endptr != '\0') {
    return NMEA_ERROR_PARSE_FAILED;
  }

  *value = (uint32_t)val;
  return NMEA_OK;
}

/**
 * @brief Parse VDM sentence - AIS VHF Data-Link Message
 *
 * Format: !--VDM,x,x,x,a,s--s,x*hh<CR><LF>
 * Fields:
 *   1: Total number of sentences, 1-9
 *   2: Sentence number, 1-9
 *   3: Sequential message identifier, 0-9
 *   4: AIS channel (A/B/C/D)
 *   5: Encapsulated ITU-R M.1371 message
 *   6: Number of fill-bits, 0-5
 */
#if NMEA_SENTENCE_VDM_ENABLED
nmea_result_t nmea_parse_vdm(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse total sentences (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_int(tokens->tokens[1], (int*)&ais->vdm_total_sentences);
  }

  /* Parse sentence number (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_int(tokens->tokens[2], (int*)&ais->vdm_sentence_number);
  }

  /* Parse sequential ID (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_int(tokens->tokens[3], (int*)&ais->vdm_sequential_id);
  }

  /* Parse AIS channel (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    ais->vdm_channel = tokens->tokens[4][0];
  }

  /* Parse encapsulated payload (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    strncpy(ais->vdm_payload, tokens->tokens[5], sizeof(ais->vdm_payload) - 1);
    ais->vdm_payload[sizeof(ais->vdm_payload) - 1] = '\0';
  }

  /* Parse fill-bits (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    nmea_parse_int(tokens->tokens[6], (int*)&ais->vdm_fill_bits);
    ais->vdm_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse VDO sentence - AIS VHF Data-Link Own-Vessel Report
 *
 * Format: !--VDO,x,x,x,a,s--s,x*hh<CR><LF>
 * Fields: Same as VDM
 */
#if NMEA_SENTENCE_VDO_ENABLED
nmea_result_t nmea_parse_vdo(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse total sentences (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_int(tokens->tokens[1], (int*)&ais->vdm_total_sentences);
  }

  /* Parse sentence number (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_int(tokens->tokens[2], (int*)&ais->vdm_sentence_number);
  }

  /* Parse sequential ID (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_int(tokens->tokens[3], (int*)&ais->vdm_sequential_id);
  }

  /* Parse AIS channel (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    ais->vdm_channel = tokens->tokens[4][0];
  }

  /* Parse encapsulated payload (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    strncpy(ais->vdm_payload, tokens->tokens[5], sizeof(ais->vdm_payload) - 1);
    ais->vdm_payload[sizeof(ais->vdm_payload) - 1] = '\0';
  }

  /* Parse fill-bits (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    nmea_parse_int(tokens->tokens[6], (int*)&ais->vdm_fill_bits);
    ais->vdo_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse ABK sentence - AIS Addressed and Binary Broadcast
 * Acknowledgement
 *
 * Format: $--ABK,xxxxxxxxx,x,x.x,x,x*hh<CR><LF>
 * Fields:
 *   1: MMSI of addressed AIS unit
 *   2: AIS channel of reception (A/B)
 *   3: ITU-R M.1371 Message ID
 *   4: Message sequence number
 *   5: Type of acknowledgement
 */
#if NMEA_SENTENCE_ABK_ENABLED
nmea_result_t nmea_parse_abk(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse MMSI (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_uint32(tokens->tokens[1], &ais->abk_mmsi);
  }

  /* Parse AIS channel (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    ais->abk_channel = tokens->tokens[2][0];
  }

  /* Parse message ID (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_int(tokens->tokens[3], (int*)&ais->abk_message_id);
  }

  /* Parse sequence number (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    nmea_parse_int(tokens->tokens[4], (int*)&ais->abk_sequence_number);
  }

  /* Parse acknowledgement type (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    nmea_parse_int(tokens->tokens[5], (int*)&ais->abk_ack_type);
    ais->abk_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse ABM sentence - AIS Addressed Binary Message
 *
 * Format: !--ABM,x,x,x,xxxxxxxxx,x,xx,s--s,x*hh<CR><LF>
 * Fields:
 *   1: Total number of sentences, 1-9
 *   2: Sentence number, 1-9
 *   3: Sequential message ID, 0-3
 *   4: MMSI of destination
 *   5: AIS channel for broadcast
 *   6: ITU-R M.1371 Message ID
 *   7: Encapsulated data
 *   8: Number of fill-bits, 0-5
 */
#if NMEA_SENTENCE_ABM_ENABLED
nmea_result_t nmea_parse_abm(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse total sentences (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_int(tokens->tokens[1], (int*)&ais->abm_total_sentences);
  }

  /* Parse sentence number (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_int(tokens->tokens[2], (int*)&ais->abm_sentence_number);
  }

  /* Parse sequential ID (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_int(tokens->tokens[3], (int*)&ais->abm_sequential_id);
  }

  /* Parse destination MMSI (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    nmea_parse_uint32(tokens->tokens[4], &ais->abm_destination_mmsi);
  }

  /* Parse channel (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    nmea_parse_int(tokens->tokens[5], (int*)&ais->abm_channel);
  }

  /* Parse message ID (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    nmea_parse_int(tokens->tokens[6], (int*)&ais->abm_message_id);
  }

  /* Parse encapsulated data (field 7) */
  if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
    strncpy(ais->abm_data, tokens->tokens[7], sizeof(ais->abm_data) - 1);
    ais->abm_data[sizeof(ais->abm_data) - 1] = '\0';
  }

  /* Parse fill-bits (field 8) */
  if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
    nmea_parse_int(tokens->tokens[8], (int*)&ais->abm_fill_bits);
    ais->abm_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse BBM sentence - AIS Broadcast Binary Message
 *
 * Format: !--BBM,x,x,x,x,xx,s--s,x*hh<CR><LF>
 * Fields:
 *   1: Total number of sentences, 1-9
 *   2: Sentence number, 1-9
 *   3: Sequential message ID, 0-9
 *   4: AIS channel for broadcast
 *   5: ITU-R M.1371 Message ID
 *   6: Encapsulated data
 *   7: Number of fill-bits, 0-5
 */
#if NMEA_SENTENCE_BBM_ENABLED
nmea_result_t nmea_parse_bbm(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse total sentences (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_int(tokens->tokens[1], (int*)&ais->bbm_total_sentences);
  }

  /* Parse sentence number (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_int(tokens->tokens[2], (int*)&ais->bbm_sentence_number);
  }

  /* Parse sequential ID (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_int(tokens->tokens[3], (int*)&ais->bbm_sequential_id);
  }

  /* Parse channel (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    nmea_parse_int(tokens->tokens[4], (int*)&ais->bbm_channel);
  }

  /* Parse message ID (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    nmea_parse_int(tokens->tokens[5], (int*)&ais->bbm_message_id);
  }

  /* Parse encapsulated data (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    strncpy(ais->bbm_data, tokens->tokens[6], sizeof(ais->bbm_data) - 1);
    ais->bbm_data[sizeof(ais->bbm_data) - 1] = '\0';
  }

  /* Parse fill-bits (field 7) */
  if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
    nmea_parse_int(tokens->tokens[7], (int*)&ais->bbm_fill_bits);
    ais->bbm_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse AIR sentence - AIS Interrogation Request
 *
 * Format: $--AIR,xxxxxxxxx,x.x,x,x.x,x,xxxxxxxxx,x.x,x,a,x.x,x.x,x.x*hh<CR><LF>
 * Fields:
 *   1: MMSI of interrogated station-1
 *   2: First message number requested from station-1
 *   3: Message sub-section
 *   4: Second message number requested from station-1
 *   5: Message sub-section
 *   6: MMSI of interrogated station-2
 *   7: Message number requested from station-2
 *   8: Message sub-section
 *   9: Channel of interrogation (A/B)
 *   10: Station-1 reply slot
 *   11: Station-1 reply slot
 *   12: Station-2 reply slot
 */
#if NMEA_SENTENCE_AIR_ENABLED
nmea_result_t nmea_parse_air(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse station 1 MMSI (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_uint32(tokens->tokens[1], &ais->air_station1_mmsi);
  }

  /* Parse station 1 first message (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_int(tokens->tokens[2], (int*)&ais->air_station1_msg1);
  }

  /* Parse station 1 first message subsection (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_int(tokens->tokens[3], (int*)&ais->air_station1_msg1_subsection);
  }

  /* Parse station 1 second message (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    nmea_parse_int(tokens->tokens[4], (int*)&ais->air_station1_msg2);
  }

  /* Parse station 1 second message subsection (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    nmea_parse_int(tokens->tokens[5], (int*)&ais->air_station1_msg2_subsection);
  }

  /* Parse station 2 MMSI (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    nmea_parse_uint32(tokens->tokens[6], &ais->air_station2_mmsi);
  }

  /* Parse station 2 message (field 7) */
  if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
    nmea_parse_int(tokens->tokens[7], (int*)&ais->air_station2_msg);
  }

  /* Parse station 2 message subsection (field 8) */
  if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
    nmea_parse_int(tokens->tokens[8], (int*)&ais->air_station2_msg_subsection);
  }

  /* Parse channel (field 9) */
  if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
    ais->air_channel = tokens->tokens[9][0];
  }

  /* Parse reply slots (fields 10-12) */
  if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
    nmea_parse_int(tokens->tokens[10], (int*)&ais->air_msg_id1_1_slot);
  }
  if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
    nmea_parse_int(tokens->tokens[11], (int*)&ais->air_msg_id1_2_slot);
  }
  if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
    nmea_parse_int(tokens->tokens[12], (int*)&ais->air_msg_id2_1_slot);
    ais->air_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse ACA sentence - AIS Channel Assignment
 *
 * Format:
 * $--ACA,x,llll.ll,a,yyyyy.yy,a,llll.ll,a,yyyyy.yy,a,x,xxxx,x,xxxx,x,x,x,a,x,hhmmss.ss*hh<CR><LF>
 * Fields:
 *   1: Sequence number, 0-9
 *   2-3: Region NE corner latitude (N/S)
 *   4-5: Region NE corner longitude (E/W)
 *   6-7: Region SW corner latitude (N/S)
 *   8-9: Region SW corner longitude (E/W)
 *   10: Transition zone size
 *   11: Channel A
 *   12: Channel A bandwidth
 *   13: Channel B
 *   14: Channel B bandwidth
 *   15: Tx/Rx mode control
 *   16: Power level control
 *   17: Information source
 *   18: In-use flag
 *   19: Time of in-use change
 */
#if NMEA_SENTENCE_ACA_ENABLED
nmea_result_t nmea_parse_aca(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse sequence number (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_int(tokens->tokens[1], (int*)&ais->aca_sequence);
  }

  /* Parse NE corner coordinates (fields 2-5) */
  if (tokens->token_count > 3) {
    nmea_parse_coordinate(tokens->tokens[2], tokens->tokens[3],
                          &ais->aca_ne_latitude);
  }
  if (tokens->token_count > 5) {
    nmea_parse_coordinate(tokens->tokens[4], tokens->tokens[5],
                          &ais->aca_ne_longitude);
  }

  /* Parse SW corner coordinates (fields 6-9) */
  if (tokens->token_count > 7) {
    nmea_parse_coordinate(tokens->tokens[6], tokens->tokens[7],
                          &ais->aca_sw_latitude);
  }
  if (tokens->token_count > 9) {
    nmea_parse_coordinate(tokens->tokens[8], tokens->tokens[9],
                          &ais->aca_sw_longitude);
  }

  /* Parse transition zone (field 10) */
  if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
    nmea_parse_int(tokens->tokens[10], (int*)&ais->aca_transition_zone);
  }

  /* Parse channel A (field 11) */
  if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
    nmea_parse_int(tokens->tokens[11], (int*)&ais->aca_channel_a);
  }

  /* Parse channel A bandwidth (field 12) */
  if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
    nmea_parse_int(tokens->tokens[12], (int*)&ais->aca_channel_a_bandwidth);
  }

  /* Parse channel B (field 13) */
  if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
    nmea_parse_int(tokens->tokens[13], (int*)&ais->aca_channel_b);
  }

  /* Parse channel B bandwidth (field 14) */
  if (tokens->token_count > 14 && !nmea_is_empty_token(tokens->tokens[14])) {
    nmea_parse_int(tokens->tokens[14], (int*)&ais->aca_channel_b_bandwidth);
  }

  /* Parse Tx/Rx mode (field 15) */
  if (tokens->token_count > 15 && !nmea_is_empty_token(tokens->tokens[15])) {
    nmea_parse_int(tokens->tokens[15], (int*)&ais->aca_txrx_mode);
  }

  /* Parse power level (field 16) */
  if (tokens->token_count > 16 && !nmea_is_empty_token(tokens->tokens[16])) {
    nmea_parse_int(tokens->tokens[16], (int*)&ais->aca_power);
  }

  /* Parse information source (field 17) */
  if (tokens->token_count > 17 && !nmea_is_empty_token(tokens->tokens[17])) {
    ais->aca_info_source = tokens->tokens[17][0];
  }

  /* Parse in-use flag (field 18) */
  if (tokens->token_count > 18 && !nmea_is_empty_token(tokens->tokens[18])) {
    nmea_parse_int(tokens->tokens[18], (int*)&ais->aca_in_use);
  }

  /* Parse in-use time (field 19) */
  if (tokens->token_count > 19 && !nmea_is_empty_token(tokens->tokens[19])) {
    nmea_parse_time(tokens->tokens[19], &ais->aca_in_use_time);
    ais->aca_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse ACS sentence - AIS Channel Management Information Source
 *
 * Format: $--ACS,x,xxxxxxxxx,hhmmss.ss,xx,xx,xxxx*hh<CR><LF>
 * Fields:
 *   1: Sequence number, 0-9
 *   2: MMSI of originator
 *   3: UTC of receipt
 *   4: Day, 01-31
 *   5: Month, 01-12
 *   6: Year
 */
#if NMEA_SENTENCE_ACS_ENABLED
nmea_result_t nmea_parse_acs(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse sequence number (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_int(tokens->tokens[1], (int*)&ais->acs_sequence);
  }

  /* Parse MMSI (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_uint32(tokens->tokens[2], &ais->acs_mmsi);
  }

  /* Parse receipt time (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_time(tokens->tokens[3], &ais->acs_receipt_time);
  }

  /* Parse day (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    nmea_parse_int(tokens->tokens[4], (int*)&ais->acs_day);
  }

  /* Parse month (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    nmea_parse_int(tokens->tokens[5], (int*)&ais->acs_month);
  }

  /* Parse year (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    nmea_parse_int(tokens->tokens[6], (int*)&ais->acs_year);
    ais->acs_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse LRF sentence - AIS Long Range Function
 *
 * Format: $--LRF,x,xxxxxxxxx,c--c,c--c,c--c*hh<CR><LF>
 * Fields:
 *   1: Sequence number, 0-9
 *   2: MMSI of requestor
 *   3: Name of requestor, 1-20 chars
 *   4: Function request, 1-26 chars
 *   5: Function reply status
 */
#if NMEA_SENTENCE_LRF_ENABLED
nmea_result_t nmea_parse_lrf(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse sequence number (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_int(tokens->tokens[1], (int*)&ais->lrf_sequence);
  }

  /* Parse requestor MMSI (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_uint32(tokens->tokens[2], &ais->lrf_requestor_mmsi);
  }

  /* Parse requestor name (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    strncpy(ais->lrf_requestor_name, tokens->tokens[3],
            sizeof(ais->lrf_requestor_name) - 1);
    ais->lrf_requestor_name[sizeof(ais->lrf_requestor_name) - 1] = '\0';
  }

  /* Parse function request (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    strncpy(ais->lrf_function_request, tokens->tokens[4],
            sizeof(ais->lrf_function_request) - 1);
    ais->lrf_function_request[sizeof(ais->lrf_function_request) - 1] = '\0';
  }

  /* Parse function reply (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    /* Strip any trailing checksum if present */
    char reply_buf[sizeof(ais->lrf_function_reply)];
    strncpy(reply_buf, tokens->tokens[5], sizeof(reply_buf) - 1);
    reply_buf[sizeof(reply_buf) - 1] = '\0';
    nmea_strip_checksum(reply_buf);
    strncpy(ais->lrf_function_reply, reply_buf,
            sizeof(ais->lrf_function_reply) - 1);
    ais->lrf_function_reply[sizeof(ais->lrf_function_reply) - 1] = '\0';
    ais->lrf_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse LRI sentence - AIS Long Range Interrogation
 *
 * Format:
 * $--LRI,x,a,xxxxxxxxx,xxxxxxxxx,llll.ll,a,yyyyy.yy,a,llll.ll,a,yyyyy.yy,a*hh<CR><LF>
 * Fields:
 *   1: Sequence number, 0-9
 *   2: Control flag
 *   3: MMSI of requestor
 *   4: MMSI of destination
 *   5-6: Latitude NE corner (N/S)
 *   7-8: Longitude NE corner (E/W)
 *   9-10: Latitude SW corner (N/S)
 *   11-12: Longitude SW corner (E/W)
 */
#if NMEA_SENTENCE_LRI_ENABLED
nmea_result_t nmea_parse_lri(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse sequence number (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_int(tokens->tokens[1], (int*)&ais->lri_sequence);
  }

  /* Parse control flag (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    ais->lri_control_flag = tokens->tokens[2][0];
  }

  /* Parse requestor MMSI (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_uint32(tokens->tokens[3], &ais->lri_requestor_mmsi);
  }

  /* Parse destination MMSI (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    nmea_parse_uint32(tokens->tokens[4], &ais->lri_destination_mmsi);
  }

  /* Parse NE corner coordinates (fields 5-8) */
  if (tokens->token_count > 6) {
    nmea_parse_coordinate(tokens->tokens[5], tokens->tokens[6],
                          &ais->lri_ne_latitude);
  }
  if (tokens->token_count > 8) {
    nmea_parse_coordinate(tokens->tokens[7], tokens->tokens[8],
                          &ais->lri_ne_longitude);
  }

  /* Parse SW corner coordinates (fields 9-12) */
  if (tokens->token_count > 10) {
    nmea_parse_coordinate(tokens->tokens[9], tokens->tokens[10],
                          &ais->lri_sw_latitude);
  }
  if (tokens->token_count > 12) {
    nmea_parse_coordinate(tokens->tokens[11], tokens->tokens[12],
                          &ais->lri_sw_longitude);
    ais->lri_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse SSD sentence - AIS Ship Static Data
 *
 * Format: $--SSD,c--c,c--c,xxx,xxx,xx,xx,c,ac*hh<CR><LF>
 * Fields:
 *   1: Ship's call sign, 1-7 chars
 *   2: Ship's name, 1-20 chars
 *   3: Position reference point A (bow), 0-511m
 *   4: Position reference point B (stern), 0-511m
 *   5: Position reference point C (port), 0-63m
 *   6: Position reference point D (starboard), 0-63m
 *   7: DTE indicator flag
 *   8: Source identifier
 */
#if NMEA_SENTENCE_SSD_ENABLED
nmea_result_t nmea_parse_ssd(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse call sign (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    strncpy(ais->ssd_callsign, tokens->tokens[1],
            sizeof(ais->ssd_callsign) - 1);
    ais->ssd_callsign[sizeof(ais->ssd_callsign) - 1] = '\0';
  }

  /* Parse ship name (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    strncpy(ais->ssd_name, tokens->tokens[2], sizeof(ais->ssd_name) - 1);
    ais->ssd_name[sizeof(ais->ssd_name) - 1] = '\0';
  }

  /* Parse position reference A (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_int(tokens->tokens[3], (int*)&ais->ssd_pos_ref_a);
  }

  /* Parse position reference B (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    nmea_parse_int(tokens->tokens[4], (int*)&ais->ssd_pos_ref_b);
  }

  /* Parse position reference C (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    nmea_parse_int(tokens->tokens[5], (int*)&ais->ssd_pos_ref_c);
  }

  /* Parse position reference D (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    nmea_parse_int(tokens->tokens[6], (int*)&ais->ssd_pos_ref_d);
  }

  /* Parse DTE indicator (field 7) */
  if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
    nmea_parse_int(tokens->tokens[7], (int*)&ais->ssd_dte);
  }

  /* Parse source identifier (field 8) */
  if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
    strncpy(ais->ssd_source, tokens->tokens[8], sizeof(ais->ssd_source) - 1);
    ais->ssd_source[sizeof(ais->ssd_source) - 1] = '\0';
    ais->ssd_valid = true;
  }

  return NMEA_OK;
}
#endif

/**
 * @brief Parse VSD sentence - AIS Voyage Static Data
 *
 * Format: $--VSD,x.x,x.x,x.x,c--c,hhmmss.ss,xx,xx,x.x,x.x*hh<CR><LF>
 * Fields:
 *   1: Type of ship and cargo, 0-255
 *   2: Maximum draught, 0-25.5m
 *   3: Persons on-board, 0-8191
 *   4: Destination, 1-20 chars
 *   5: ETA time (UTC)
 *   6: ETA day, 00-31
 *   7: ETA month, 00-12
 *   8: Regional application flag
 *   9: Regional application flag
 */
#if NMEA_SENTENCE_VSD_ENABLED
nmea_result_t nmea_parse_vsd(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_ais_state_t* ais = &ctx->ais;

  /* Parse ship type (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_int(tokens->tokens[1], (int*)&ais->vsd_ship_type);
  }

  /* Parse draught (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_float(tokens->tokens[2], &ais->vsd_draught);
  }

  /* Parse persons on-board (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_int(tokens->tokens[3], (int*)&ais->vsd_persons);
  }

  /* Parse destination (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    strncpy(ais->vsd_destination, tokens->tokens[4],
            sizeof(ais->vsd_destination) - 1);
    ais->vsd_destination[sizeof(ais->vsd_destination) - 1] = '\0';
  }

  /* Parse ETA time (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    nmea_parse_time(tokens->tokens[5], &ais->vsd_eta_time);
  }

  /* Parse ETA day (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    nmea_parse_int(tokens->tokens[6], (int*)&ais->vsd_eta_day);
  }

  /* Parse ETA month (field 7) */
  if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
    nmea_parse_int(tokens->tokens[7], (int*)&ais->vsd_eta_month);
  }

  /* Parse regional flags (fields 8-9) */
  if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
    nmea_parse_int(tokens->tokens[8], (int*)&ais->vsd_regional_flag1);
  }
  if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
    nmea_parse_int(tokens->tokens[9], (int*)&ais->vsd_regional_flag2);
    ais->vsd_valid = true;
  }

  return NMEA_OK;
}
#endif

#endif /* NMEA_MODULE_AIS_ENABLED */
