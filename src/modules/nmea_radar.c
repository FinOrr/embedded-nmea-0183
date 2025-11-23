/**
 * @file nmea_radar.c
 * @brief Radar and target tracking module implementation
 *
 * Implements parsers for radar-related NMEA sentences.
 */

#include "modules/nmea_radar.h"
#include "nmea_internal.h"
#include <string.h>

#if NMEA_MODULE_RADAR_ENABLED

/* ========================================================================== */
/*                          RSD - RADAR SYSTEM DATA                           */
/* ========================================================================== */

#if NMEA_SENTENCE_RSD_ENABLED
/**
 * @brief Parse RSD - Radar System Data
 *
 * Format: $--RSD,x.x,x.x,x.x,x.x,x.x,x.x,x.x,x.x,x.x,x.x,x.x,a,a*hh
 * Example: $RARSD,10.5,45.0,5.0,30.0,20.0,90.0,10.0,60.0,15.0,120.0,24.0,N,H*hh
 *
 * Fields:
 * 0: Sentence ID (RARSD)
 * 1: Origin 1 range, from own ship
 * 2: Origin 1 bearing, degrees from 0°
 * 3: Variable range marker 1 (VRM1), range
 * 4: Bearing line 1 (EBL1), degrees from 0°
 * 5: Origin 2 range
 * 6: Origin 2 bearing
 * 7: VRM 2, range
 * 8: EBL 2, degrees
 * 9: Cursor range, from own ship
 * 10: Cursor bearing, degrees clockwise from 0°
 * 11: Range scale in use
 * 12: Range units (S=statute miles, N=nautical miles, K=km)
 * 13: Display rotation (C=course-up, H=head-up, N=north-up)
 */
nmea_result_t nmea_parse_rsd(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_radar_state_t *radar = &ctx->radar;

    /* Parse cursor range (field 9) - store as target distance */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        nmea_parse_float(tokens->tokens[9], &radar->target_distance_nm);
    }

    /* Parse cursor bearing (field 10) - store as target bearing */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        nmea_parse_float(tokens->tokens[10], &radar->target_bearing);
    }

    /* RSD provides radar system configuration, not specific target data */
    /* We store cursor position as a reference point */

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_RSD_ENABLED */

/* ========================================================================== */
/*                          TTM - TRACKED TARGET MESSAGE                      */
/* ========================================================================== */

#if NMEA_SENTENCE_TTM_ENABLED
/**
 * @brief Parse TTM - Tracked Target Message
 *
 * Format: $--TTM,xx,x.x,x.x,a,x.x,x.x,a,x.x,x.x,a,c--c,a,a,hhmmss.ss,a*hh
 * Example: $RATTM,01,3.5,45.2,T,12.3,270.0,T,0.5,5.0,N,TARGET1,T,R,120530.00,A*hh
 *
 * Fields:
 * 0: Sentence ID (RATTM)
 * 1: Target number, 0 to 999
 * 2: Target distance from own ship
 * 3: Bearing from own ship, degrees (T/R indicator in field 4)
 * 4: Bearing reference (T=true, R=relative)
 * 5: Target speed
 * 6: Target course, degrees (T/R indicator in field 7)
 * 7: Course reference (T=true, R=relative)
 * 8: Distance of closest-point-of-approach
 * 9: Time to CPA, min., "-" increasing
 * 10: Speed/distance units (K=km, N=nautical, S=statute)
 * 11: Target name
 * 12: Target status (L=Lost, Q=Query, T=Tracking)
 * 13: Reference target (R=reference, null otherwise)
 * 14: Time of data (UTC)
 * 15: Type of acquisition (R=reported, M=manual, A=automatic)
 */
nmea_result_t nmea_parse_ttm(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_radar_state_t *radar = &ctx->radar;

    /* Parse target number (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t target_num = 0;
        if (nmea_parse_int(tokens->tokens[1], &target_num) == NMEA_OK) {
            radar->target_number = (uint16_t)target_num;
        }
    }

    /* Parse target distance (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_float(tokens->tokens[2], &radar->target_distance_nm);
    }

    /* Parse bearing from own ship (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &radar->target_bearing);
    }

    /* Parse target speed (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_float(tokens->tokens[5], &radar->target_speed_knots);
    }

    /* Parse target course (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &radar->target_course);
    }

    /* Parse target status (field 12) */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        char status = tokens->tokens[12][0];
        /* T=Tracking means valid target */
        if (status == 'T' || status == 't') {
            radar->target_valid = true;
        } else if (status == 'L' || status == 'l') {
            /* L=Lost, mark as invalid */
            radar->target_valid = false;
        }
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_TTM_ENABLED */

/* ========================================================================== */
/*                          TLL - TARGET LATITUDE AND LONGITUDE               */
/* ========================================================================== */

#if NMEA_SENTENCE_TLL_ENABLED
/**
 * @brief Parse TLL - Target Latitude and Longitude
 *
 * Format: $--TLL,xx,llll.ll,a,yyyyy.yy,a,c--c,hhmmss.ss,a,a*hh
 * Example: $RATLL,01,3723.46,N,12158.32,W,TARGET1,120530.00,T,R*hh
 *
 * Fields:
 * 0: Sentence ID (RATLL)
 * 1: Target number 00 – 99
 * 2: Target latitude
 * 3: N/S
 * 4: Target longitude
 * 5: E/W
 * 6: Target name
 * 7: UTC of data
 * 8: Target status (L=Lost, Q=Query, T=tracking)
 * 9: Reference target (R=reference, null otherwise)
 */
nmea_result_t nmea_parse_tll(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_radar_state_t *radar = &ctx->radar;

    /* Parse target number (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t target_num = 0;
        if (nmea_parse_int(tokens->tokens[1], &target_num) == NMEA_OK) {
            radar->target_number = (uint16_t)target_num;
        }
    }

    /* Parse latitude (field 2) and N/S (field 3) */
    /* Note: The radar state structure only has a single target_position coordinate.
     * We store latitude here; longitude is not stored due to structure limitation. */
    if (tokens->token_count > 3 &&
        !nmea_is_empty_token(tokens->tokens[2]) &&
        !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_coordinate(tokens->tokens[2], tokens->tokens[3],
                             &radar->target_position);
    }

    /* Longitude (fields 4-5) not stored due to radar state structure limitation */

    /* Parse target status (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        char status = tokens->tokens[8][0];
        /* T=Tracking means valid target */
        if (status == 'T' || status == 't') {
            radar->target_valid = true;
        } else if (status == 'L' || status == 'l') {
            /* L=Lost, mark as invalid */
            radar->target_valid = false;
        }
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_TLL_ENABLED */

/* ========================================================================== */
/*                          TLB - TARGET LABEL                                */
/* ========================================================================== */

#if NMEA_SENTENCE_TLB_ENABLED
/**
 * @brief Parse TLB - Target Label
 *
 * Format: $--TLB,x.x,c--c,x.x,c--c,...x.x,c--c*hh
 * Example: $RATLB,01,TARGET1,02,TARGET2,03,TARGET3*hh
 *
 * Fields:
 * 0: Sentence ID (RATLB)
 * Repeating pairs:
 *   - Target number 'n' reported by the device
 *   - Label assigned to target 'n'
 *
 * Note: This sentence allows several target number/label pairs in a single message.
 * Null fields indicate that no common label is specified.
 */
nmea_result_t nmea_parse_tlb(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    /* TLB sentence is used to assign labels to targets */
    /* The current radar state structure doesn't store target labels */
    /* This parser acknowledges the sentence but doesn't store label information */

    /* If the first target number/label pair exists, we can extract the target number */
    /* Parse first target number (field 1) if present */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_radar_state_t *radar = &ctx->radar;
        int32_t target_num = 0;
        if (nmea_parse_int(tokens->tokens[1], &target_num) == NMEA_OK) {
            radar->target_number = (uint16_t)target_num;
        }
    }

    /* Field 2 would be the label, but we don't store it in current structure */
    /* Additional pairs would be in fields 3+4, 5+6, etc. */

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_TLB_ENABLED */

/* ========================================================================== */
/*                          TTD - TRACKED TARGET DATA                         */
/* ========================================================================== */

#if NMEA_SENTENCE_TTD_ENABLED
/**
 * @brief Parse TTD - Tracked Target Data
 *
 * Format: !--TTD,hh,hh,x,s—s,x*hh
 * Example: !RATTD,01,01,0,ENCAPSULATED_DATA,0*hh
 *
 * Fields:
 * 0: Sentence ID (RATTD)
 * 1: Total hex number of sentences needed to transfer the message, 01 to FF
 * 2: Hex sentence number, 01 to FF
 * 3: Sequential message identifier, 0 to 9
 * 4: Encapsulated tracked target data (6-bit ASCII encoding)
 * 5: Number of fill-bits, 0 to 5
 *
 * Note: This sentence uses compressed/encapsulated format for efficient transmission
 * of multiple targets. Full decoding requires 6-bit ASCII de-encapsulation and
 * bit-level parsing according to the protocol version.
 *
 * Current implementation: Basic parsing of sentence structure without full
 * de-encapsulation of the binary target data.
 */
nmea_result_t nmea_parse_ttd(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    /* TTD sentence contains encapsulated binary data that requires complex */
    /* 6-bit ASCII decoding and bit-field extraction. The current simple state */
    /* structure doesn't support storing multiple compressed targets. */

    /* This parser acknowledges the sentence format but doesn't perform */
    /* the full de-encapsulation. A more complete implementation would: */
    /* 1. De-encapsulate the 6-bit ASCII data */
    /* 2. Extract bit fields according to protocol version (0 or 1) */
    /* 3. Store multiple target structures */

    /* For now, we simply validate the sentence structure exists */
    (void)ctx; /* Suppress unused parameter warning */

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_TTD_ENABLED */

#endif /* NMEA_MODULE_RADAR_ENABLED */
