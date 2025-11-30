/**
 * @file nmea_comm.c
 * @brief Communications module implementation
 *
 * Implements parsers for communication protocol sentences including
 * digital selective calling, text messages, version information,
 * user identification, security, and acknowledgements.
 */

#include "modules/nmea_comm.h"
#include "nmea_internal.h"
#include <string.h>

#if NMEA_MODULE_COMM_ENABLED

/* ========================================================================== */
/*                          DSC - DIGITAL SELECTIVE CALLING                   */
/* ========================================================================== */

#if NMEA_SENTENCE_DSC_ENABLED
/**
 * @brief Parse DSC - Digital Selective Calling Information
 *
 * Format: $--DSC,xx,xxxxxxxxxx,xx,xx,xx,x.x,x.x,xxxxxxxxxx,xx,a,a*hh
 * Example: $--DSC,12,003669987,12,06,00,2182.0,,,,E,E*hh
 *
 * Fields:
 * 0: Sentence ID (DSC)
 * 1: Format specifier
 * 2: Address (MMSI)
 * 3: Category
 * 4: Nature of distress or first telecommand
 * 5: Type of communication or second telecommand
 * 6: Position or channel/frequency (time field)
 * 7: Position or channel/frequency (position/channel field)
 * 8: MMSI of ship in distress
 * 9: Nature of distress
 * 10: Acknowledgement (R/B/S)
 * 11: Expansion indicator (E or null)
 */
nmea_result_t nmea_parse_dsc(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_comm_state_t *comm = &ctx->comm;

    /* Parse format specifier (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        strncpy(comm->dsc_category, tokens->tokens[1], sizeof(comm->dsc_category) - 1);
        comm->dsc_category[sizeof(comm->dsc_category) - 1] = '\0';
    }

    /* Parse address/MMSI (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        strncpy(comm->dsc_address, tokens->tokens[2], sizeof(comm->dsc_address) - 1);
        comm->dsc_address[sizeof(comm->dsc_address) - 1] = '\0';
    }

    /* Parse nature of distress or telecommand (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        strncpy(comm->dsc_nature, tokens->tokens[4], sizeof(comm->dsc_nature) - 1);
        comm->dsc_nature[sizeof(comm->dsc_nature) - 1] = '\0';
    }

    /* Parse type of communication (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        strncpy(comm->dsc_type, tokens->tokens[5], sizeof(comm->dsc_type) - 1);
        comm->dsc_type[sizeof(comm->dsc_type) - 1] = '\0';
    }

    /* Parse time or telephone number (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        strncpy(comm->dsc_time, tokens->tokens[6], sizeof(comm->dsc_time) - 1);
        comm->dsc_time[sizeof(comm->dsc_time) - 1] = '\0';
    }

    /* Parse position or channel/frequency (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        strncpy(comm->dsc_position, tokens->tokens[7], sizeof(comm->dsc_position) - 1);
        comm->dsc_position[sizeof(comm->dsc_position) - 1] = '\0';
    }

    /* Parse MMSI of ship in distress (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        strncpy(comm->dsc_mmsi_distress, tokens->tokens[8], sizeof(comm->dsc_mmsi_distress) - 1);
        comm->dsc_mmsi_distress[sizeof(comm->dsc_mmsi_distress) - 1] = '\0';
    }

    /* Parse acknowledgement type (field 10) */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        comm->dsc_ack = tokens->tokens[10][0];
    }

    /* Parse expansion indicator (field 11) */
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
        comm->dsc_expansion = (tokens->tokens[11][0] == 'E' || tokens->tokens[11][0] == 'e');
    }

    comm->dsc_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_DSC_ENABLED */

/* ========================================================================== */
/*                          DSE - EXPANDED DIGITAL SELECTIVE CALLING          */
/* ========================================================================== */

#if NMEA_SENTENCE_DSE_ENABLED
/**
 * @brief Parse DSE - Expanded Digital Selective Calling
 *
 * Format: $--DSE,x,x,a,xxxxxxxxxx,xx,c--c,..........,xx,c--c*hh
 * Example: $--DSE,1,1,Q,003669987,12,SHIPNAME*hh
 *
 * Fields:
 * 0: Sentence ID (DSE)
 * 1: Total number of sentences
 * 2: Sentence number
 * 3: Query/reply flag (Q/R/A)
 * 4: Vessel MMSI
 * 5+: Data sets (code field, data field pairs)
 */
nmea_result_t nmea_parse_dse(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_comm_state_t *comm = &ctx->comm;

    /* Parse total sentences (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t total = 0;
        if (nmea_parse_int(tokens->tokens[1], &total) == NMEA_OK) {
            comm->dse_total_sentences = (uint8_t)total;
        }
    }

    /* Parse sentence number (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        int32_t num = 0;
        if (nmea_parse_int(tokens->tokens[2], &num) == NMEA_OK) {
            comm->dse_sentence_number = (uint8_t)num;
        }
    }

    /* Parse query/reply flag (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        comm->dse_query_reply = tokens->tokens[3][0];
    }

    /* Parse vessel MMSI (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        strncpy(comm->dse_mmsi, tokens->tokens[4], sizeof(comm->dse_mmsi) - 1);
        comm->dse_mmsi[sizeof(comm->dse_mmsi) - 1] = '\0';
    }

    /* Note: Additional data sets in fields 5+ are application-specific */
    /* and would require custom parsing based on ITU-R M.821-1 codes */

    comm->dse_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_DSE_ENABLED */

/* ========================================================================== */
/*                          VER - VERSION                                     */
/* ========================================================================== */

#if NMEA_SENTENCE_VER_ENABLED
/**
 * @brief Parse VER - Version
 *
 * Format: $--VER,x,x,aa,c--c,c--c,c--c,c--c,c--c,c--c,x*hh
 * Example: $GPVER,1,1,GP,ACME,12345,SN001,MODEL1,v1.0,h1.0,0*hh
 *
 * Fields:
 * 0: Sentence ID (VER)
 * 1: Total number of sentences (1-9)
 * 2: Sentence number (1-9)
 * 3: Device type
 * 4: Vendor ID
 * 5: Unique identifier
 * 6: Manufacturer serial number
 * 7: Model code
 * 8: Software revision
 * 9: Hardware revision
 * 10: Sequential message identifier
 */
nmea_result_t nmea_parse_ver(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_comm_state_t *comm = &ctx->comm;

    /* Parse total sentences (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t total = 0;
        if (nmea_parse_int(tokens->tokens[1], &total) == NMEA_OK) {
            comm->ver_total_sentences = (uint8_t)total;
        }
    }

    /* Parse sentence number (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        int32_t num = 0;
        if (nmea_parse_int(tokens->tokens[2], &num) == NMEA_OK) {
            comm->ver_sentence_number = (uint8_t)num;
        }
    }

    /* Parse device type (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        strncpy(comm->ver_device_type, tokens->tokens[3], sizeof(comm->ver_device_type) - 1);
        comm->ver_device_type[sizeof(comm->ver_device_type) - 1] = '\0';
    }

    /* Parse vendor ID (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        strncpy(comm->ver_vendor_id, tokens->tokens[4], sizeof(comm->ver_vendor_id) - 1);
        comm->ver_vendor_id[sizeof(comm->ver_vendor_id) - 1] = '\0';
    }

    /* Parse unique identifier (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        strncpy(comm->ver_unique_id, tokens->tokens[5], sizeof(comm->ver_unique_id) - 1);
        comm->ver_unique_id[sizeof(comm->ver_unique_id) - 1] = '\0';
    }

    /* Parse serial number (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        strncpy(comm->ver_serial_number, tokens->tokens[6], sizeof(comm->ver_serial_number) - 1);
        comm->ver_serial_number[sizeof(comm->ver_serial_number) - 1] = '\0';
    }

    /* Parse model code (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        strncpy(comm->ver_model_code, tokens->tokens[7], sizeof(comm->ver_model_code) - 1);
        comm->ver_model_code[sizeof(comm->ver_model_code) - 1] = '\0';
    }

    /* Parse software revision (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        strncpy(comm->ver_sw_revision, tokens->tokens[8], sizeof(comm->ver_sw_revision) - 1);
        comm->ver_sw_revision[sizeof(comm->ver_sw_revision) - 1] = '\0';
    }

    /* Parse hardware revision (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        strncpy(comm->ver_hw_revision, tokens->tokens[9], sizeof(comm->ver_hw_revision) - 1);
        comm->ver_hw_revision[sizeof(comm->ver_hw_revision) - 1] = '\0';
    }

    /* Parse message identifier (field 10) */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        int32_t msg_id = 0;
        if (nmea_parse_int(tokens->tokens[10], &msg_id) == NMEA_OK) {
            comm->ver_message_id = (uint8_t)msg_id;
        }
    }

    comm->ver_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_VER_ENABLED */

/* ========================================================================== */
/*                          UID - USER IDENTIFICATION                         */
/* ========================================================================== */

#if NMEA_SENTENCE_UID_ENABLED
/**
 * @brief Parse UID - User Identification Code Transmission
 *
 * Format: $--UID,c--c,c--c*hh
 * Example: $GPUID,HEPSLGN02376,DB Los 23*hh
 *
 * Fields:
 * 0: Sentence ID (UID)
 * 1: User identification code 1 (up to 20 characters)
 * 2: User identification code 2 (optional, up to 20 characters)
 */
nmea_result_t nmea_parse_uid(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_comm_state_t *comm = &ctx->comm;

    /* Parse user ID code 1 (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        strncpy(comm->uid_code1, tokens->tokens[1], sizeof(comm->uid_code1) - 1);
        comm->uid_code1[sizeof(comm->uid_code1) - 1] = '\0';
    }

    /* Parse user ID code 2 (field 2, optional) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        strncpy(comm->uid_code2, tokens->tokens[2], sizeof(comm->uid_code2) - 1);
        comm->uid_code2[sizeof(comm->uid_code2) - 1] = '\0';
    }

    comm->uid_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_UID_ENABLED */

/* ========================================================================== */
/*                          SPW - SECURITY PASSWORD                           */
/* ========================================================================== */

#if NMEA_SENTENCE_SPW_ENABLED
/**
 * @brief Parse SPW - Security Password Sentence
 *
 * Format: $--SPW,ccc,c--c,x,c--c*hh
 * Example: $IISPW,EPV,211000001,2,SESAME*hh
 *
 * Fields:
 * 0: Sentence ID (SPW)
 * 1: Password protected sentence formatter
 * 2: Unique identifier (e.g., MMSI)
 * 3: Password level (1=User, 2=Administrator, 3-9=Reserved)
 * 4: Password (up to 32 characters)
 */
nmea_result_t nmea_parse_spw(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_comm_state_t *comm = &ctx->comm;

    /* Parse protected sentence (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        strncpy(comm->spw_sentence, tokens->tokens[1], sizeof(comm->spw_sentence) - 1);
        comm->spw_sentence[sizeof(comm->spw_sentence) - 1] = '\0';
    }

    /* Parse unique identifier (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        strncpy(comm->spw_unique_id, tokens->tokens[2], sizeof(comm->spw_unique_id) - 1);
        comm->spw_unique_id[sizeof(comm->spw_unique_id) - 1] = '\0';
    }

    /* Parse password level (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        int32_t level = 0;
        if (nmea_parse_int(tokens->tokens[3], &level) == NMEA_OK) {
            comm->spw_level = (uint8_t)level;
        }
    }

    /* Parse password (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        strncpy(comm->spw_password, tokens->tokens[4], sizeof(comm->spw_password) - 1);
        comm->spw_password[sizeof(comm->spw_password) - 1] = '\0';
    }

    comm->spw_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_SPW_ENABLED */

/* ========================================================================== */
/*                          TUT - MULTI-LANGUAGE TEXT                         */
/* ========================================================================== */

#if NMEA_SENTENCE_TUT_ENABLED
/**
 * @brief Parse TUT - Transmission of Multi-language Text
 *
 * Format: $--TUT,aa,hh,hh,x,c--c,h--h*hh
 * Example: $SDTUT,SD,01,01,1,U,6D45702C5371967A*hh
 *
 * Fields:
 * 0: Sentence ID (TUT)
 * 1: Source identifier
 * 2: Total number of sentences (hex, 01-FF)
 * 3: Sentence number (hex, 01-FF)
 * 4: Sequential message identifier (0-9)
 * 5: Translation code (U=Unicode, A=ASCII, 1-16=ISO/IEC 8859, P<aaa>=Proprietary)
 * 6: Text body (hex-encoded)
 */
nmea_result_t nmea_parse_tut(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_comm_state_t *comm = &ctx->comm;

    /* Parse source identifier (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        strncpy(comm->tut_source_id, tokens->tokens[1], sizeof(comm->tut_source_id) - 1);
        comm->tut_source_id[sizeof(comm->tut_source_id) - 1] = '\0';
    }

    /* Parse total sentences (field 2, hex) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        int32_t total = 0;
        if (nmea_parse_int(tokens->tokens[2], &total) == NMEA_OK) {
            comm->tut_total_sentences = (uint8_t)total;
        }
    }

    /* Parse sentence number (field 3, hex) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        int32_t num = 0;
        if (nmea_parse_int(tokens->tokens[3], &num) == NMEA_OK) {
            comm->tut_sentence_number = (uint8_t)num;
        }
    }

    /* Parse message identifier (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        int32_t msg_id = 0;
        if (nmea_parse_int(tokens->tokens[4], &msg_id) == NMEA_OK) {
            comm->tut_message_id = (uint8_t)msg_id;
        }
    }

    /* Parse translation code (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        strncpy(comm->tut_translation_code, tokens->tokens[5], sizeof(comm->tut_translation_code) - 1);
        comm->tut_translation_code[sizeof(comm->tut_translation_code) - 1] = '\0';
    }

    /* Parse text body (field 6, hex-encoded) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        strncpy(comm->tut_text_body, tokens->tokens[6], sizeof(comm->tut_text_body) - 1);
        comm->tut_text_body[sizeof(comm->tut_text_body) - 1] = '\0';
    }

    comm->tut_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_TUT_ENABLED */

/* ========================================================================== */
/*                          TXT - TEXT TRANSMISSION                           */
/* ========================================================================== */

#if NMEA_SENTENCE_TXT_ENABLED
/**
 * @brief Parse TXT - Text Transmission
 *
 * Format: $--TXT,xx,xx,xx,c--c*hh
 * Example: $GPTXT,01,01,25,DR MODE-ANTENNA FAULT^21*hh
 *
 * Fields:
 * 0: Sentence ID (TXT)
 * 1: Total number of sentences (01-99)
 * 2: Sentence number (01-99)
 * 3: Text identifier (01-99)
 * 4: Text message (up to 61 characters)
 */
nmea_result_t nmea_parse_txt(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_comm_state_t *comm = &ctx->comm;

    /* Parse total sentences (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t total = 0;
        if (nmea_parse_int(tokens->tokens[1], &total) == NMEA_OK) {
            comm->txt_total_sentences = (uint8_t)total;
        }
    }

    /* Parse sentence number (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        int32_t num = 0;
        if (nmea_parse_int(tokens->tokens[2], &num) == NMEA_OK) {
            comm->txt_sentence_number = (uint8_t)num;
        }
    }

    /* Parse text identifier (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        int32_t id = 0;
        if (nmea_parse_int(tokens->tokens[3], &id) == NMEA_OK) {
            comm->txt_identifier = (uint8_t)id;
        }
    }

    /* Parse text message (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        strncpy(comm->txt_message, tokens->tokens[4], sizeof(comm->txt_message) - 1);
        comm->txt_message[sizeof(comm->txt_message) - 1] = '\0';
    }

    comm->txt_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_TXT_ENABLED */

/* ========================================================================== */
/*                          NAK - NEGATIVE ACKNOWLEDGEMENT                    */
/* ========================================================================== */

#if NMEA_SENTENCE_NAK_ENABLED
/**
 * @brief Parse NAK - Negative Acknowledgement
 *
 * Format: $--NAK,cc,ccc,c--c,x.x,c--c*hh
 * Example: $GPNAK,GP,GGA,12345,1,Query not supported*hh
 *
 * Fields:
 * 0: Sentence ID (NAK)
 * 1: Talker identifier
 * 2: Affected sentence formatter
 * 3: Unique identifier
 * 4: Reason code (0-49, or >50 for equipment-specific)
 * 5: Descriptive text
 */
nmea_result_t nmea_parse_nak(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_comm_state_t *comm = &ctx->comm;

    /* Parse talker identifier (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        strncpy(comm->nak_talker_id, tokens->tokens[1], sizeof(comm->nak_talker_id) - 1);
        comm->nak_talker_id[sizeof(comm->nak_talker_id) - 1] = '\0';
    }

    /* Parse affected sentence (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        strncpy(comm->nak_sentence, tokens->tokens[2], sizeof(comm->nak_sentence) - 1);
        comm->nak_sentence[sizeof(comm->nak_sentence) - 1] = '\0';
    }

    /* Parse unique identifier (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        strncpy(comm->nak_unique_id, tokens->tokens[3], sizeof(comm->nak_unique_id) - 1);
        comm->nak_unique_id[sizeof(comm->nak_unique_id) - 1] = '\0';
    }

    /* Parse reason code (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        int32_t code = 0;
        if (nmea_parse_int(tokens->tokens[4], &code) == NMEA_OK) {
            comm->nak_reason_code = (uint8_t)code;
        }
    }

    /* Parse description (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        strncpy(comm->nak_description, tokens->tokens[5], sizeof(comm->nak_description) - 1);
        comm->nak_description[sizeof(comm->nak_description) - 1] = '\0';
    }

    comm->nak_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_NAK_ENABLED */

/* ========================================================================== */
/*                          RLM - RETURN LINK MESSAGE                         */
/* ========================================================================== */

#if NMEA_SENTENCE_RLM_ENABLED
/**
 * @brief Parse RLM - Return Link Message
 *
 * Format: $--RLM,hhhhhhhhhhhhhhh,hhmmss.ss,h,h--h*hh
 * Example: $GPRLM,123456789ABCDEF,120530.00,1,A1B2*hh
 *
 * Fields:
 * 0: Sentence ID (RLM)
 * 1: Beacon ID (15 hex characters)
 * 2: Time of reception (UTC)
 * 3: Message code (0=Reserved, 1=Ack, 2=Command, 3=Message, F=Test)
 * 4: Message body (variable length hex)
 */
nmea_result_t nmea_parse_rlm(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_comm_state_t *comm = &ctx->comm;

    /* Parse beacon ID (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        strncpy(comm->rlm_beacon_id, tokens->tokens[1], sizeof(comm->rlm_beacon_id) - 1);
        comm->rlm_beacon_id[sizeof(comm->rlm_beacon_id) - 1] = '\0';
    }

    /* Parse time of reception (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_time(tokens->tokens[2], &comm->rlm_time);
    }

    /* Parse message code (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        comm->rlm_message_code = tokens->tokens[3][0];
    }

    /* Parse message body (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        strncpy(comm->rlm_message_body, tokens->tokens[4], sizeof(comm->rlm_message_body) - 1);
        comm->rlm_message_body[sizeof(comm->rlm_message_body) - 1] = '\0';
    }

    comm->rlm_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_RLM_ENABLED */

/* ========================================================================== */
/*                          STN - MULTIPLE DATA ID                            */
/* ========================================================================== */

#if NMEA_SENTENCE_STN_ENABLED
/**
 * @brief Parse STN - Multiple Data ID
 *
 * Format: $--STN,xx*hh
 * Example: $SDSTN,01*hh
 *
 * Fields:
 * 0: Sentence ID (STN)
 * 1: Talker ID number (00-99)
 */
nmea_result_t nmea_parse_stn(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_comm_state_t *comm = &ctx->comm;

    /* Parse talker ID number (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t talker_num = 0;
        if (nmea_parse_int(tokens->tokens[1], &talker_num) == NMEA_OK) {
            comm->stn_talker_number = (uint8_t)talker_num;
        }
    }

    comm->stn_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_STN_ENABLED */

#endif /* NMEA_MODULE_COMM_ENABLED */
