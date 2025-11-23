/**
 * @file nmea_sensor.c
 * @brief Environmental and motion sensor module implementation
 *
 * Implements parsers for sensor-related NMEA sentences.
 */

#include "../../inc/modules/nmea_sensor.h"
#include "../nmea_internal.h"
#include <string.h>

#if NMEA_MODULE_SENSOR_ENABLED

/* ========================================================================== */
/*                          DBT - DEPTH BELOW TRANSDUCER                      */
/* ========================================================================== */

#if NMEA_SENTENCE_DBT_ENABLED
/**
 * @brief Parse DBT - Depth Below Transducer
 *
 * Format: $--DBT,x.x,f,x.x,M,x.x,F*hh
 * Example: $SDDBT,12.3,f,3.7,M,2.0,F*3A
 *
 * Fields:
 * 0: Sentence ID (SDDBT)
 * 1: Water depth, feet
 * 2: f (feet)
 * 3: Water depth, meters
 * 4: M (meters)
 * 5: Water depth, fathoms
 * 6: F (fathoms)
 */
nmea_result_t nmea_parse_dbt(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_sensor_state_t *sensor = &ctx->sensor;

    /* Parse depth in feet (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &sensor->depth_below_transducer_feet);
    }

    /* Parse depth in meters (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        if (nmea_parse_float(tokens->tokens[3], &sensor->depth_below_transducer_meters) == NMEA_OK) {
            sensor->depth_valid = true;
        }
    }

    /* Parse depth in fathoms (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_float(tokens->tokens[5], &sensor->depth_below_transducer_fathoms);
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_DBT_ENABLED */

/* ========================================================================== */
/*                          DPT - DEPTH                                       */
/* ========================================================================== */

#if NMEA_SENTENCE_DPT_ENABLED
/**
 * @brief Parse DPT - Depth
 *
 * Format: $--DPT,x.x,x.x,x.x*hh
 * Example: $SDDPT,3.7,0.5,100.0*47
 *
 * Fields:
 * 0: Sentence ID (SDDPT)
 * 1: Water depth relative to transducer, meters
 * 2: Offset from transducer, meters (+ = to waterline, - = to keel)
 * 3: Maximum range scale in use
 */
nmea_result_t nmea_parse_dpt(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_sensor_state_t *sensor = &ctx->sensor;

    /* Parse depth (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        if (nmea_parse_float(tokens->tokens[1], &sensor->depth_meters) == NMEA_OK) {
            sensor->depth_valid = true;
        }
    }

    /* Parse offset (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_float(tokens->tokens[2], &sensor->depth_offset_meters);
    }

    /* Parse maximum range (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &sensor->depth_range_meters);
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_DPT_ENABLED */

/* ========================================================================== */
/*                          MTW - WATER TEMPERATURE                           */
/* ========================================================================== */

#if NMEA_SENTENCE_MTW_ENABLED
/**
 * @brief Parse MTW - Water Temperature
 *
 * Format: $--MTW,x.x,C*hh
 * Example: $IIMTW,15.5,C*21
 *
 * Fields:
 * 0: Sentence ID (IIMTW)
 * 1: Temperature, degrees C
 * 2: C (Celsius)
 */
nmea_result_t nmea_parse_mtw(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_sensor_state_t *sensor = &ctx->sensor;

    /* Parse temperature (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        if (nmea_parse_float(tokens->tokens[1], &sensor->water_temperature_celsius) == NMEA_OK) {
            sensor->water_temperature_valid = true;
        }
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_MTW_ENABLED */

/* ========================================================================== */
/*                          MWD - WIND DIRECTION AND SPEED                    */
/* ========================================================================== */

#if NMEA_SENTENCE_MWD_ENABLED
/**
 * @brief Parse MWD - Wind Direction and Speed
 *
 * Format: $--MWD,x.x,T,x.x,M,x.x,N,x.x,M*hh
 * Example: $IIMWD,123.4,T,127.8,M,12.5,N,6.4,M*5E
 *
 * Fields:
 * 0: Sentence ID (IIMWD)
 * 1: Wind direction, 0-359 degrees true
 * 2: T (true)
 * 3: Wind direction, 0-359 degrees magnetic
 * 4: M (magnetic)
 * 5: Wind speed, knots
 * 6: N (knots)
 * 7: Wind speed, m/s
 * 8: M (m/s)
 */
nmea_result_t nmea_parse_mwd(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_sensor_state_t *sensor = &ctx->sensor;

    /* Parse wind direction true (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &sensor->wind_direction_true);
    }

    /* Parse wind direction magnetic (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &sensor->wind_direction_magnetic);
    }

    /* Parse wind speed knots (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        if (nmea_parse_float(tokens->tokens[5], &sensor->wind_speed_knots) == NMEA_OK) {
            sensor->wind_valid = true;
        }
    }

    /* Parse wind speed m/s (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nmea_parse_float(tokens->tokens[7], &sensor->wind_speed_mps);
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_MWD_ENABLED */

/* ========================================================================== */
/*                          MWV - WIND SPEED AND ANGLE                        */
/* ========================================================================== */

#if NMEA_SENTENCE_MWV_ENABLED
/**
 * @brief Parse MWV - Wind Speed and Angle
 *
 * Format: $--MWV,x.x,a,x.x,a,A*hh
 * Example: $IIMWV,45.0,R,12.5,N,A*2F
 *
 * Fields:
 * 0: Sentence ID (IIMWV)
 * 1: Wind angle, 0-359 degrees
 * 2: Reference (R=Relative, T=True/Theoretical)
 * 3: Wind speed
 * 4: Wind speed units (N=knots, M=m/s, K=km/h)
 * 5: Status (A=valid, V=invalid)
 */
nmea_result_t nmea_parse_mwv(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_sensor_state_t *sensor = &ctx->sensor;

    /* Parse wind angle (field 1) - stored based on reference type */
    float wind_angle = 0.0f;
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &wind_angle);
    }

    /* Parse reference (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        sensor->wind_reference = tokens->tokens[2][0];
    }

    /* Parse wind speed (field 3) */
    float wind_speed = 0.0f;
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &wind_speed);
    }

    /* Parse wind speed units and convert (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        char unit = tokens->tokens[4][0];

        if (unit == 'N' || unit == 'n') {
            /* Knots */
            sensor->wind_speed_knots = wind_speed;
            sensor->wind_speed_mps = wind_speed * 0.514444f;
            sensor->wind_speed_kmh = wind_speed * 1.852f;
        } else if (unit == 'M' || unit == 'm') {
            /* m/s */
            sensor->wind_speed_mps = wind_speed;
            sensor->wind_speed_knots = wind_speed / 0.514444f;
            sensor->wind_speed_kmh = wind_speed * 3.6f;
        } else if (unit == 'K' || unit == 'k') {
            /* km/h */
            sensor->wind_speed_kmh = wind_speed;
            sensor->wind_speed_knots = wind_speed / 1.852f;
            sensor->wind_speed_mps = wind_speed / 3.6f;
        }
    }

    /* Parse status (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        char status = tokens->tokens[5][0];
        if (status == 'A' || status == 'a') {
            sensor->wind_valid = true;
        }
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_MWV_ENABLED */

/* ========================================================================== */
/*                          VBW - DUAL GROUND/WATER SPEED                     */
/* ========================================================================== */

#if NMEA_SENTENCE_VBW_ENABLED
/**
 * @brief Parse VBW - Dual Ground/Water Speed
 *
 * Format: $--VBW,x.x,x.x,A,x.x,x.x,A,x.x,A,x.x,A*hh
 * Example: $IIVBW,12.3,0.5,A,10.2,0.3,A,1.2,A,0.8,A*52
 *
 * Fields:
 * 0: Sentence ID (IIVBW)
 * 1: Longitudinal water speed, knots
 * 2: Transverse water speed, knots
 * 3: Status: water speed (A=valid, V=invalid)
 * 4: Longitudinal ground speed, knots
 * 5: Transverse ground speed, knots
 * 6: Status: ground speed (A=valid, V=invalid)
 * 7: Stern transverse water speed, knots
 * 8: Status: stern water speed (A=valid, V=invalid)
 * 9: Stern transverse ground speed, knots
 * 10: Status: stern ground speed (A=valid, V=invalid)
 */
nmea_result_t nmea_parse_vbw(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_sensor_state_t *sensor = &ctx->sensor;

    /* Parse longitudinal water speed (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &sensor->speed_water_knots);
    }

    /* Parse water speed status (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        char status = tokens->tokens[3][0];
        if (status == 'A' || status == 'a') {
            sensor->speed_valid = true;
        }
    }

    /* Parse longitudinal ground speed (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &sensor->speed_ground_knots);
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_VBW_ENABLED */

/* ========================================================================== */
/*                          VHW - WATER SPEED AND HEADING                     */
/* ========================================================================== */

#if NMEA_SENTENCE_VHW_ENABLED
/**
 * @brief Parse VHW - Water Speed and Heading
 *
 * Format: $--VHW,x.x,T,x.x,M,x.x,N,x.x,K*hh
 * Example: $IIVHW,123.4,T,127.8,M,12.5,N,23.2,K*5A
 *
 * Fields:
 * 0: Sentence ID (IIVHW)
 * 1: Heading, degrees true
 * 2: T (true)
 * 3: Heading, degrees magnetic
 * 4: M (magnetic)
 * 5: Speed, knots
 * 6: N (knots)
 * 7: Speed, km/h
 * 8: K (km/h)
 */
nmea_result_t nmea_parse_vhw(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_sensor_state_t *sensor = &ctx->sensor;

    /* Parse heading true (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &sensor->heading_true);
    }

    /* Parse heading magnetic (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &sensor->heading_magnetic);
    }

    /* Parse speed knots (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        if (nmea_parse_float(tokens->tokens[5], &sensor->water_speed_knots) == NMEA_OK) {
            sensor->water_speed_heading_valid = true;
        }
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_VHW_ENABLED */

/* ========================================================================== */
/*                          VLW - DUAL GROUND/WATER DISTANCE                  */
/* ========================================================================== */

#if NMEA_SENTENCE_VLW_ENABLED
/**
 * @brief Parse VLW - Dual Ground/Water Distance
 *
 * Format: $--VLW,x.x,N,x.x,N,x.x,N,x.x,N*hh
 * Example: $IIVLW,1234.5,N,123.4,N,987.6,N,98.7,N*43
 *
 * Fields:
 * 0: Sentence ID (IIVLW)
 * 1: Total cumulative water distance, nautical miles
 * 2: N (nautical miles)
 * 3: Water distance since reset, nautical miles
 * 4: N (nautical miles)
 * 5: Total cumulative ground distance, nautical miles
 * 6: N (nautical miles)
 * 7: Ground distance since reset, nautical miles
 * 8: N (nautical miles)
 */
nmea_result_t nmea_parse_vlw(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_sensor_state_t *sensor = &ctx->sensor;

    /* Parse total cumulative water distance (field 1) - not stored in current state */
    /* Parse water distance since reset (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &sensor->distance_water_nm);
    }

    /* Parse total cumulative ground distance (field 5) - not stored in current state */
    /* Parse ground distance since reset (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nmea_parse_float(tokens->tokens[7], &sensor->distance_ground_nm);
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_VLW_ENABLED */

/* ========================================================================== */
/*                          VPW - SPEED PARALLEL TO WIND                      */
/* ========================================================================== */

#if NMEA_SENTENCE_VPW_ENABLED
/**
 * @brief Parse VPW - Speed Measured Parallel to Wind
 *
 * Format: $--VPW,x.x,N,x.x,M*hh
 * Example: $IIVPW,5.2,N,2.7,M*38
 *
 * Fields:
 * 0: Sentence ID (IIVPW)
 * 1: Speed, knots (negative = downwind)
 * 2: N (knots)
 * 3: Speed, m/s (negative = downwind)
 * 4: M (m/s)
 */
nmea_result_t nmea_parse_vpw(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_sensor_state_t *sensor = &ctx->sensor;

    /* Parse speed in knots (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        /* VPW speed is stored in the water speed field as it's a type of water speed */
        nmea_parse_float(tokens->tokens[1], &sensor->speed_water_knots);
    }

    /* Parse speed in m/s (field 3) - could be used for validation */
    /* Currently not stored separately in the state structure */

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_VPW_ENABLED */

#endif /* NMEA_MODULE_SENSOR_ENABLED */
