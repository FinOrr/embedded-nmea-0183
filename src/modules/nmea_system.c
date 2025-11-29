/**
 * @file nmea_system.c
 * @brief System status and monitoring module implementation
 *
 * Implements parsers for system status and monitoring sentences including
 * heartbeat, hull stress, events, engine telegraph, NAVTEX, navigation
 * status, device position, MSK receiver, and rudder order.
 */

#include "modules/nmea_system.h"
#include "nmea_internal.h"
#include <string.h>
#include <stdlib.h>

#if NMEA_MODULE_SYSTEM_ENABLED

/* ========================================================================== */
/*                          HBT - HEARTBEAT SUPERVISION                       */
/* ========================================================================== */

#if NMEA_SENTENCE_HBT_ENABLED
/**
 * @brief Parse HBT - Heartbeat Supervision Sentence
 *
 * Format: $--HBT,x.x,A,x*hh
 * Example: $--HBT,10.0,A,123*hh
 * (Assumed format based on typical heartbeat patterns)
 *
 * Fields:
 * 0: Sentence ID (HBT)
 * 1: Interval (seconds)
 * 2: Status
 * 3: Sequential ID
 */
nmea_result_t nmea_parse_hbt(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Minimal parsing - structure assumed based on typical heartbeat sentences */
    sys->heartbeat_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_HBT_ENABLED */

/* ========================================================================== */
/*                          HSS - HULL STRESS SURVEILLANCE                    */
/* ========================================================================== */

#if NMEA_SENTENCE_HSS_ENABLED
/**
 * @brief Parse HSS - Hull Stress Surveillance Systems
 *
 * Format: $--HSS,c--c,x.x,A*hh
 * Example: $--HSS,POINT01,123.45,A*hh
 *
 * Fields:
 * 0: Sentence ID (HSS)
 * 1: Measurement point ID
 * 2: Measurement value
 * 3: Data status (A=valid, V=invalid)
 */
nmea_result_t nmea_parse_hss(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Parse measurement point ID (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        strncpy(sys->hss_measurement_point, tokens->tokens[1], sizeof(sys->hss_measurement_point) - 1);
        sys->hss_measurement_point[sizeof(sys->hss_measurement_point) - 1] = '\0';
    }

    /* Parse measurement value (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_float(tokens->tokens[2], &sys->hss_measurement_value);
    }

    /* Parse data status (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        sys->hss_status = tokens->tokens[3][0];
    }

    sys->hss_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_HSS_ENABLED */

/* ========================================================================== */
/*                          EVE - GENERAL EVENT MESSAGE                       */
/* ========================================================================== */

#if NMEA_SENTENCE_EVE_ENABLED
/**
 * @brief Parse EVE - General Event Message
 *
 * Format: $--EVE,hhmmss.ss,c--c,c--c*hh
 * Example: $--EVE,120530.00,TAG01,Anchor deployed*hh
 *
 * Fields:
 * 0: Sentence ID (EVE)
 * 1: Event time
 * 2: Tag code for identification of source
 * 3: Event description
 */
nmea_result_t nmea_parse_eve(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Parse event time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &sys->eve_time);
    }

    /* Parse tag code (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        strncpy(sys->eve_tag_code, tokens->tokens[2], sizeof(sys->eve_tag_code) - 1);
        sys->eve_tag_code[sizeof(sys->eve_tag_code) - 1] = '\0';
    }

    /* Parse event description (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        strncpy(sys->eve_description, tokens->tokens[3], sizeof(sys->eve_description) - 1);
        sys->eve_description[sizeof(sys->eve_description) - 1] = '\0';
    }

    sys->eve_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_EVE_ENABLED */

/* ========================================================================== */
/*                          ETL - ENGINE TELEGRAPH OPERATION STATUS           */
/* ========================================================================== */

#if NMEA_SENTENCE_ETL_ENABLED
/**
 * @brief Parse ETL - Engine Telegraph Operation Status
 *
 * Format: $--ETL,hhmmss.ss,a,xx,xx,a,x*hh
 * Example: $--ETL,120530.00,O,03,20,B,0*hh
 *
 * Fields:
 * 0: Sentence ID (ETL)
 * 1: Event time
 * 2: Message type (O=Order, A=Answer-back)
 * 3: Position indicator (00-15)
 * 4: Sub-telegraph position (20/30/40)
 * 5: Operating location (B/P/S/C/E/W)
 * 6: Engine/propeller shaft number (0-9)
 */
nmea_result_t nmea_parse_etl(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Parse event time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &sys->etl_time);
    }

    /* Parse message type (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        sys->etl_message_type = tokens->tokens[2][0];
    }

    /* Parse telegraph position (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        int32_t pos = 0;
        if (nmea_parse_int(tokens->tokens[3], &pos) == NMEA_OK) {
            sys->etl_position = (uint8_t)pos;
        }
    }

    /* Parse sub-telegraph position (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        int32_t sub_pos = 0;
        if (nmea_parse_int(tokens->tokens[4], &sub_pos) == NMEA_OK) {
            sys->etl_sub_position = (uint8_t)sub_pos;
        }
    }

    /* Parse operating location (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        sys->etl_location = tokens->tokens[5][0];
    }

    /* Parse engine number (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        int32_t engine = 0;
        if (nmea_parse_int(tokens->tokens[6], &engine) == NMEA_OK) {
            sys->etl_engine_number = (uint8_t)engine;
        }
    }

    sys->etl_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ETL_ENABLED */

/* ========================================================================== */
/*                          NRM - NAVTEX RECEIVER MASK                        */
/* ========================================================================== */

#if NMEA_SENTENCE_NRM_ENABLED
/**
 * @brief Parse NRM - NAVTEX Receiver Mask
 *
 * Format: $--NRM,x,x,hhhhhhhh,hhhhhhhh,a*hh
 * Example: $INNRM,1,2,00001E1F,0FFFFFFF,R*hh
 *
 * Fields:
 * 0: Sentence ID (NRM)
 * 1: Function code (0-9)
 * 2: Frequency table index (1-9)
 * 3: Transmitter coverage area mask (hex)
 * 4: Message type mask (hex)
 * 5: Sentence status flag (R/C)
 */
nmea_result_t nmea_parse_nrm(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Parse function code (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t code = 0;
        if (nmea_parse_int(tokens->tokens[1], &code) == NMEA_OK) {
            sys->nrm_function_code = (uint8_t)code;
        }
    }

    /* Parse frequency index (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        int32_t freq = 0;
        if (nmea_parse_int(tokens->tokens[2], &freq) == NMEA_OK) {
            sys->nrm_frequency_index = (uint8_t)freq;
        }
    }

    /* Parse transmitter mask (field 3, hex) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        sys->nrm_transmitter_mask = (uint32_t)strtoul(tokens->tokens[3], NULL, 16);
    }

    /* Parse message type mask (field 4, hex) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        sys->nrm_message_type_mask = (uint32_t)strtoul(tokens->tokens[4], NULL, 16);
    }

    /* Parse status flag (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        sys->nrm_status_flag = tokens->tokens[5][0];
    }

    sys->nrm_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_NRM_ENABLED */

/* ========================================================================== */
/*                          NRX - NAVTEX RECEIVED MESSAGE                     */
/* ========================================================================== */

#if NMEA_SENTENCE_NRX_ENABLED
/**
 * @brief Parse NRX - NAVTEX Received Message
 *
 * Format: $--NRX,xxx,xxx,xx,aaxx,x,hhmmss.ss,xx,xx,xxxx,x.x,x.x,A,c--c*hh
 * Example: $CRNRX,007,001,00,IE69,1,135600,27,06,2001,241,3,A,Message text*hh
 *
 * Fields:
 * 0: Sentence ID (NRX)
 * 1: Number of sentences (001-999)
 * 2: Sentence number (001-999)
 * 3: Sequential message ID (00-99)
 * 4: NAVTEX message code
 * 5: Frequency table index (0-9)
 * 6: UTC of receipt
 * 7: Day (1-31)
 * 8: Month (1-12)
 * 9: Year
 * 10: Total characters
 * 11: Bad characters
 * 12: Status (A/V)
 * 13: Message body
 */
nmea_result_t nmea_parse_nrx(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Parse total sentences (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t total = 0;
        if (nmea_parse_int(tokens->tokens[1], &total) == NMEA_OK) {
            sys->nrx_total_sentences = (uint16_t)total;
        }
    }

    /* Parse sentence number (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        int32_t num = 0;
        if (nmea_parse_int(tokens->tokens[2], &num) == NMEA_OK) {
            sys->nrx_sentence_number = (uint16_t)num;
        }
    }

    /* Parse message ID (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        int32_t id = 0;
        if (nmea_parse_int(tokens->tokens[3], &id) == NMEA_OK) {
            sys->nrx_message_id = (uint8_t)id;
        }
    }

    /* Parse message code (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        strncpy(sys->nrx_message_code, tokens->tokens[4], sizeof(sys->nrx_message_code) - 1);
        sys->nrx_message_code[sizeof(sys->nrx_message_code) - 1] = '\0';
    }

    /* Parse frequency index (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        int32_t freq = 0;
        if (nmea_parse_int(tokens->tokens[5], &freq) == NMEA_OK) {
            sys->nrx_frequency_index = (uint8_t)freq;
        }
    }

    /* Parse time (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_time(tokens->tokens[6], &sys->nrx_time);
    }

    /* Parse day (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        int32_t day = 0;
        if (nmea_parse_int(tokens->tokens[7], &day) == NMEA_OK) {
            sys->nrx_date.day = (uint8_t)day;
        }
    }

    /* Parse month (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        int32_t month = 0;
        if (nmea_parse_int(tokens->tokens[8], &month) == NMEA_OK) {
            sys->nrx_date.month = (uint8_t)month;
        }
    }

    /* Parse year (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        int32_t year = 0;
        if (nmea_parse_int(tokens->tokens[9], &year) == NMEA_OK) {
            sys->nrx_date.year = (uint16_t)year;
        }
    }

    /* Parse total characters (field 10) */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        int32_t chars = 0;
        if (nmea_parse_int(tokens->tokens[10], &chars) == NMEA_OK) {
            sys->nrx_total_chars = (uint16_t)chars;
        }
    }

    /* Parse bad characters (field 11) */
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
        int32_t bad = 0;
        if (nmea_parse_int(tokens->tokens[11], &bad) == NMEA_OK) {
            sys->nrx_bad_chars = (uint16_t)bad;
        }
    }

    /* Parse status (field 12) */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        sys->nrx_status = tokens->tokens[12][0];
    }

    /* Parse message body (field 13) */
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        strncpy(sys->nrx_message_body, tokens->tokens[13], sizeof(sys->nrx_message_body) - 1);
        sys->nrx_message_body[sizeof(sys->nrx_message_body) - 1] = '\0';
    }

    sys->nrx_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_NRX_ENABLED */

/* ========================================================================== */
/*                          NSR - NAVIGATION STATUS REPORT                    */
/* ========================================================================== */

#if NMEA_SENTENCE_NSR_ENABLED
/**
 * @brief Parse NSR - Navigation Status Report
 *
 * Format: $--NSR,a,A,a,A,a,A,a,A,a,A,a,a,A*hh
 * Example: $INNSR,P,A,P,A,P,A,P,A,P,A,W,P,A*hh
 *
 * Fields:
 * 0: Sentence ID (NSR)
 * 1: Integrity of heading (P/F/D/N)
 * 2: Plausibility of heading (A/V/N)
 * 3: Integrity of position (P/F/D/N)
 * 4: Plausibility of position (A/V/N)
 * 5: Integrity of STW (P/F/D/N)
 * 6: Plausibility of STW (A/V/N)
 * 7: Integrity of SOG/COG (P/F/D/N)
 * 8: Plausibility of SOG/COG (A/V/N)
 * 9: Integrity of depth (P/F/D/N)
 * 10: Plausibility of depth (A/V/N)
 * 11: Mode of STW (W/E/M/N)
 * 12: Integrity of time (P/F/D/N)
 * 13: Plausibility of time (A/V/N)
 */
nmea_result_t nmea_parse_nsr(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Parse all status fields */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        sys->nsr_heading_integrity = tokens->tokens[1][0];
    }
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        sys->nsr_heading_plausibility = tokens->tokens[2][0];
    }
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        sys->nsr_position_integrity = tokens->tokens[3][0];
    }
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        sys->nsr_position_plausibility = tokens->tokens[4][0];
    }
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        sys->nsr_stw_integrity = tokens->tokens[5][0];
    }
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        sys->nsr_stw_plausibility = tokens->tokens[6][0];
    }
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        sys->nsr_sog_cog_integrity = tokens->tokens[7][0];
    }
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        sys->nsr_sog_cog_plausibility = tokens->tokens[8][0];
    }
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        sys->nsr_depth_integrity = tokens->tokens[9][0];
    }
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        sys->nsr_depth_plausibility = tokens->tokens[10][0];
    }
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
        sys->nsr_stw_mode = tokens->tokens[11][0];
    }
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        sys->nsr_time_integrity = tokens->tokens[12][0];
    }
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        sys->nsr_time_plausibility = tokens->tokens[13][0];
    }

    sys->nsr_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_NSR_ENABLED */

/* ========================================================================== */
/*                          POS - DEVICE POSITION AND SHIP DIMENSIONS         */
/* ========================================================================== */

#if NMEA_SENTENCE_POS_ENABLED
/**
 * @brief Parse POS - Device Position and Ship Dimensions
 *
 * Format: $--POS,cc,xx,a,x.x,x.x,x.x,a,x.x,x.x,a*hh
 * Example: $GPPOS,GP,01,A,2.5,15.0,10.0,A,50.0,12.0,R*hh
 *
 * Fields:
 * 0: Sentence ID (POS)
 * 1: Equipment identification
 * 2: Equipment number (00-99)
 * 3: Position validity (A/V)
 * 4: X-coordinate (meters)
 * 5: Y-coordinate (meters)
 * 6: Z-coordinate (meters)
 * 7: Ship dimensions validity (A/V)
 * 8: Ship length (meters)
 * 9: Ship width (meters)
 * 10: Status flag (R/C)
 */
nmea_result_t nmea_parse_pos(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Parse equipment ID (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        strncpy(sys->pos_equipment_id, tokens->tokens[1], sizeof(sys->pos_equipment_id) - 1);
        sys->pos_equipment_id[sizeof(sys->pos_equipment_id) - 1] = '\0';
    }

    /* Parse equipment number (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        int32_t num = 0;
        if (nmea_parse_int(tokens->tokens[2], &num) == NMEA_OK) {
            sys->pos_equipment_number = (uint8_t)num;
        }
    }

    /* Parse position validity (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        sys->pos_validity = tokens->tokens[3][0];
    }

    /* Parse X-coordinate (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &sys->pos_x_coordinate);
    }

    /* Parse Y-coordinate (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_float(tokens->tokens[5], &sys->pos_y_coordinate);
    }

    /* Parse Z-coordinate (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &sys->pos_z_coordinate);
    }

    /* Parse ship dimensions validity (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        sys->pos_ship_validity = tokens->tokens[7][0];
    }

    /* Parse ship length (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &sys->pos_ship_length);
    }

    /* Parse ship width (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        nmea_parse_float(tokens->tokens[9], &sys->pos_ship_width);
    }

    /* Parse status flag (field 10) */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        sys->pos_status_flag = tokens->tokens[10][0];
    }

    sys->pos_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_POS_ENABLED */

/* ========================================================================== */
/*                          MSK - MSK RECEIVER INTERFACE                      */
/* ========================================================================== */

#if NMEA_SENTENCE_MSK_ENABLED
/**
 * @brief Parse MSK - MSK Receiver Interface
 *
 * Format: $--MSK,x.x,a,x.x,a,x.x,x,a*hh
 * Example: $--MSK,310.5,M,100,M,60.0,1,R*hh
 *
 * Fields:
 * 0: Sentence ID (MSK)
 * 1: Beacon frequency (283.5-325.0 kHz)
 * 2: Auto/manual frequency (A/M)
 * 3: Beacon bit rate (25/50/100/200)
 * 4: Auto/manual bit rate (A/M)
 * 5: Interval for MSS status (seconds)
 * 6: Channel number
 * 7: Sentence status flag (R/C)
 */
nmea_result_t nmea_parse_msk(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Parse frequency (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &sys->msk_frequency);
    }

    /* Parse frequency mode (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        sys->msk_frequency_mode = tokens->tokens[2][0];
    }

    /* Parse bit rate (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &sys->msk_bit_rate);
    }

    /* Parse bit rate mode (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        sys->msk_bit_rate_mode = tokens->tokens[4][0];
    }

    /* Parse status interval (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_float(tokens->tokens[5], &sys->msk_status_interval);
    }

    /* Parse channel (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        int32_t ch = 0;
        if (nmea_parse_int(tokens->tokens[6], &ch) == NMEA_OK) {
            sys->msk_channel = (uint8_t)ch;
        }
    }

    /* Parse status flag (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        sys->msk_status_flag = tokens->tokens[7][0];
    }

    sys->msk_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_MSK_ENABLED */

/* ========================================================================== */
/*                          MSS - MSK RECEIVER SIGNAL STATUS                  */
/* ========================================================================== */

#if NMEA_SENTENCE_MSS_ENABLED
/**
 * @brief Parse MSS - MSK Receiver Signal Status
 *
 * Format: $--MSS,x.x,x.x,x.x,x.x,x*hh
 * Example: $--MSS,55.2,12.5,310.5,100,1*hh
 *
 * Fields:
 * 0: Sentence ID (MSS)
 * 1: Signal strength (dB/1uV/m)
 * 2: Signal-to-noise ratio (dB)
 * 3: Beacon frequency (283.5-325.0 kHz)
 * 4: Beacon bit rate (25/50/100/200 bit/s)
 * 5: Channel number
 */
nmea_result_t nmea_parse_mss(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Parse signal strength (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &sys->mss_signal_strength);
    }

    /* Parse SNR (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_float(tokens->tokens[2], &sys->mss_snr);
    }

    /* Parse frequency (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &sys->mss_frequency);
    }

    /* Parse bit rate (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &sys->mss_bit_rate);
    }

    /* Parse channel (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        int32_t ch = 0;
        if (nmea_parse_int(tokens->tokens[5], &ch) == NMEA_OK) {
            sys->mss_channel = (uint8_t)ch;
        }
    }

    sys->mss_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_MSS_ENABLED */

/* ========================================================================== */
/*                          ROR - RUDDER ORDER STATUS                         */
/* ========================================================================== */

#if NMEA_SENTENCE_ROR_ENABLED
/**
 * @brief Parse ROR - Rudder Order Status
 *
 * Format: $--ROR,x.x,A,x.x,A*hh (assumed format)
 * Example: $--ROR,15.0,A,0.0,V*hh
 *
 * Fields:
 * 0: Sentence ID (ROR)
 * 1: Starboard rudder order
 * 2: Starboard status
 * 3: Port rudder order
 * 4: Port status
 */
nmea_result_t nmea_parse_ror(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_system_state_t *sys = &ctx->system;

    /* Parse starboard order (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &sys->ror_starboard_order);
    }

    /* Parse port order (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &sys->ror_port_order);
    }

    sys->ror_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ROR_ENABLED */

#endif /* NMEA_MODULE_SYSTEM_ENABLED */
