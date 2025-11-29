/**
 * @file nmea_attitude.c
 * @brief Attitude and motion module implementation
 *
 * Implements parsers for attitude and motion sentences including
 * heel angle, roll measurement, set and drift, propulsion control,
 * and thruster control and response.
 */

#include "modules/nmea_attitude.h"
#include "nmea_internal.h"
#include <string.h>

#if NMEA_MODULE_ATTITUDE_ENABLED

/* ========================================================================== */
/*         HRM - HEEL ANGLE, ROLL PERIOD AND ROLL AMPLITUDE MEASUREMENT      */
/* ========================================================================== */

#if NMEA_SENTENCE_HRM_ENABLED
/**
 * @brief Parse HRM - Heel Angle, Roll Period and Roll Amplitude Measurement
 *
 * Format: $--HRM,x.x,x.x,x.x,x.x,A,x.x,x.x,hhmmss.ss,xx,xx*hh
 * Example: $--HRM,5.2,8.5,3.1,4.7,A,10.5,12.3,120530,15,11*hh
 *
 * Fields:
 * 0: Sentence ID (HRM)
 * 1: Actual heel angle (degrees, +starboard/-port)
 * 2: Roll period (seconds)
 * 3: Roll amplitude port side (degrees)
 * 4: Roll amplitude starboard side (degrees)
 * 5: Status (A=valid, V=invalid)
 * 6: Roll peak hold value port side (degrees)
 * 7: Roll peak hold value starboard side (degrees)
 * 8: Peak hold reset time (UTC)
 * 9: Peak hold reset day (01-31)
 * 10: Peak hold reset month (01-12)
 */
nmea_result_t nmea_parse_hrm(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_attitude_state_t *att = &ctx->attitude;

    /* Parse heel angle (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &att->hrm_heel_angle);
    }

    /* Parse roll period (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_float(tokens->tokens[2], &att->hrm_roll_period);
    }

    /* Parse roll amplitude port (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &att->hrm_roll_amplitude_port);
    }

    /* Parse roll amplitude starboard (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &att->hrm_roll_amplitude_starboard);
    }

    /* Parse status (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        att->hrm_status = tokens->tokens[5][0];
    }

    /* Parse peak hold port (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &att->hrm_peak_hold_port);
    }

    /* Parse peak hold starboard (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nmea_parse_float(tokens->tokens[7], &att->hrm_peak_hold_starboard);
    }

    /* Parse peak reset time (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_time(tokens->tokens[8], &att->hrm_peak_reset_time);
    }

    /* Parse peak reset day (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        int32_t day = 0;
        if (nmea_parse_int(tokens->tokens[9], &day) == NMEA_OK) {
            att->hrm_peak_reset_day = (uint8_t)day;
        }
    }

    /* Parse peak reset month (field 10) */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        int32_t month = 0;
        if (nmea_parse_int(tokens->tokens[10], &month) == NMEA_OK) {
            att->hrm_peak_reset_month = (uint8_t)month;
        }
    }

    att->hrm_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_HRM_ENABLED */

/* ========================================================================== */
/*                    PRC - PROPULSION REMOTE CONTROL STATUS                  */
/* ========================================================================== */

#if NMEA_SENTENCE_PRC_ENABLED
/**
 * @brief Parse PRC - Propulsion Remote Control Status
 *
 * Format: $--PRC,x.x,A,x.x,a,x.x,a,a,x*hh
 * Example: $--PRC,45.5,A,1500,R,12.3,D,B,0*hh
 *
 * Fields:
 * 0: Sentence ID (PRC)
 * 1: Lever demand position (-100 to 100%)
 * 2: Lever demand status (A/V)
 * 3: RPM demand value
 * 4: RPM mode indicator (P/R/V)
 * 5: Pitch demand value
 * 6: Pitch mode indicator (P/D/V)
 * 7: Operating location (B/P/S/C/E/W)
 * 8: Engine/propeller shaft number
 */
nmea_result_t nmea_parse_prc(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_attitude_state_t *att = &ctx->attitude;

    /* Parse lever demand (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &att->prc_lever_demand);
    }

    /* Parse lever status (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        att->prc_lever_status = tokens->tokens[2][0];
    }

    /* Parse RPM demand (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &att->prc_rpm_demand);
    }

    /* Parse RPM mode (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        att->prc_rpm_mode = tokens->tokens[4][0];
    }

    /* Parse pitch demand (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_float(tokens->tokens[5], &att->prc_pitch_demand);
    }

    /* Parse pitch mode (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        att->prc_pitch_mode = tokens->tokens[6][0];
    }

    /* Parse location (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        att->prc_location = tokens->tokens[7][0];
    }

    /* Parse engine number (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        int32_t engine = 0;
        if (nmea_parse_int(tokens->tokens[8], &engine) == NMEA_OK) {
            att->prc_engine_number = (uint8_t)engine;
        }
    }

    att->prc_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_PRC_ENABLED */

/* ========================================================================== */
/*                       TRC - THRUSTER CONTROL DATA                          */
/* ========================================================================== */

#if NMEA_SENTENCE_TRC_ENABLED
/**
 * @brief Parse TRC - Thruster Control Data
 *
 * Format: $--TRC,x,x.x,a,x.x,a,x.x,a,a*hh
 * Example: $--TRC,1,1200,R,45.5,D,180.0,B,R*hh
 *
 * Fields:
 * 0: Sentence ID (TRC)
 * 1: Thruster number (odd=bow, even=stern)
 * 2: RPM demand value
 * 3: RPM mode indicator (P/R/V)
 * 4: Pitch demand value
 * 5: Pitch mode indicator (P/D/V)
 * 6: Azimuth demand (0-360 degrees)
 * 7: Operating location (B/P/S/C/E/W)
 * 8: Status flag (R/C)
 */
nmea_result_t nmea_parse_trc(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_attitude_state_t *att = &ctx->attitude;

    /* Parse thruster number (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t thruster = 0;
        if (nmea_parse_int(tokens->tokens[1], &thruster) == NMEA_OK) {
            att->trc_thruster_number = (uint8_t)thruster;
        }
    }

    /* Parse RPM demand (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_float(tokens->tokens[2], &att->trc_rpm_demand);
    }

    /* Parse RPM mode (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        att->trc_rpm_mode = tokens->tokens[3][0];
    }

    /* Parse pitch demand (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &att->trc_pitch_demand);
    }

    /* Parse pitch mode (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        att->trc_pitch_mode = tokens->tokens[5][0];
    }

    /* Parse azimuth demand (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &att->trc_azimuth_demand);
    }

    /* Parse location (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        att->trc_location = tokens->tokens[7][0];
    }

    /* Parse status flag (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        att->trc_status_flag = tokens->tokens[8][0];
    }

    att->trc_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_TRC_ENABLED */

/* ========================================================================== */
/*                      TRD - THRUSTER RESPONSE DATA                          */
/* ========================================================================== */

#if NMEA_SENTENCE_TRD_ENABLED
/**
 * @brief Parse TRD - Thruster Response Data
 *
 * Format: $--TRD,x,x.x,a,x.x,a,x.x*hh
 * Example: $--TRD,1,1180,R,43.2,D,178.5*hh
 *
 * Fields:
 * 0: Sentence ID (TRD)
 * 1: Thruster number (odd=bow, even=stern)
 * 2: RPM response value
 * 3: RPM mode indicator (P/R/V)
 * 4: Pitch response value
 * 5: Pitch mode indicator (P/D/V)
 * 6: Azimuth response (0-360 degrees)
 */
nmea_result_t nmea_parse_trd(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_attitude_state_t *att = &ctx->attitude;

    /* Parse thruster number (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t thruster = 0;
        if (nmea_parse_int(tokens->tokens[1], &thruster) == NMEA_OK) {
            att->trd_thruster_number = (uint8_t)thruster;
        }
    }

    /* Parse RPM response (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_float(tokens->tokens[2], &att->trd_rpm_response);
    }

    /* Parse RPM mode (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        att->trd_rpm_mode = tokens->tokens[3][0];
    }

    /* Parse pitch response (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &att->trd_pitch_response);
    }

    /* Parse pitch mode (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        att->trd_pitch_mode = tokens->tokens[5][0];
    }

    /* Parse azimuth response (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &att->trd_azimuth_response);
    }

    att->trd_valid = true;
    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_TRD_ENABLED */

#endif /* NMEA_MODULE_ATTITUDE_ENABLED */
