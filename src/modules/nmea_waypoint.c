/**
 * @file nmea_waypoint.c
 * @brief Waypoint management module implementation
 */

#include "../../inc/modules/nmea_waypoint.h"
#include "../nmea_internal.h"
#include <string.h>

#if NMEA_MODULE_WAYPOINT_ENABLED

/**
 * @brief Parse WPL sentence - Waypoint location
 *
 * Format: $--WPL,llll.ll,a,yyyyy.yy,a,c--c*hh<CR><LF>
 * Fields:
 *   1: Waypoint latitude
 *   2: N/S
 *   3: Waypoint longitude
 *   4: E/W
 *   5: Waypoint identifier
 */
#if NMEA_SENTENCE_WPL_ENABLED
nmea_result_t nmea_parse_wpl(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_waypoint_state_t *waypoint = &ctx->waypoint;

    /* Parse latitude (fields 1-2) */
    if (tokens->token_count > 2) {
        nmea_parse_coordinate(tokens->tokens[1], tokens->tokens[2], &waypoint->wpl_latitude);
    }

    /* Parse longitude (fields 3-4) */
    if (tokens->token_count > 4) {
        nmea_parse_coordinate(tokens->tokens[3], tokens->tokens[4], &waypoint->wpl_longitude);
    }

    /* Parse waypoint identifier (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        strncpy(waypoint->wpl_id, tokens->tokens[5], sizeof(waypoint->wpl_id) - 1);
        waypoint->wpl_id[sizeof(waypoint->wpl_id) - 1] = '\0';
        waypoint->wpl_valid = true;
    }

    return NMEA_OK;
}
#endif

/**
 * @brief Parse WNC sentence - Distance waypoint to waypoint
 *
 * Format: $--WNC,x.x,N,x.x,K,c--c,c--c*hh<CR><LF>
 * Fields:
 *   1: Distance, nautical miles
 *   2: N (nautical miles)
 *   3: Distance, km
 *   4: K (kilometers)
 *   5: TO waypoint identifier
 *   6: FROM waypoint identifier
 */
#if NMEA_SENTENCE_WNC_ENABLED
nmea_result_t nmea_parse_wnc(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_waypoint_state_t *waypoint = &ctx->waypoint;

    /* Parse distance in nautical miles (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &waypoint->wnc_distance_nm);
    }

    /* Parse distance in kilometers (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &waypoint->wnc_distance_km);
    }

    /* Parse TO waypoint identifier (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        strncpy(waypoint->wnc_to_id, tokens->tokens[5], sizeof(waypoint->wnc_to_id) - 1);
        waypoint->wnc_to_id[sizeof(waypoint->wnc_to_id) - 1] = '\0';
    }

    /* Parse FROM waypoint identifier (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        strncpy(waypoint->wnc_from_id, tokens->tokens[6], sizeof(waypoint->wnc_from_id) - 1);
        waypoint->wnc_from_id[sizeof(waypoint->wnc_from_id) - 1] = '\0';
        waypoint->wnc_valid = true;
    }

    return NMEA_OK;
}
#endif

/**
 * @brief Parse WCV sentence - Waypoint closure velocity
 *
 * Format: $--WCV,x.x,N,c--c,a*hh<CR><LF>
 * Fields:
 *   1: Velocity component, knots
 *   2: N (knots)
 *   3: Waypoint identifier
 *   4: Mode indicator (A/D/E/M/S/N)
 */
#if NMEA_SENTENCE_WCV_ENABLED
nmea_result_t nmea_parse_wcv(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_waypoint_state_t *waypoint = &ctx->waypoint;

    /* Parse velocity (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &waypoint->wcv_velocity);
    }

    /* Parse waypoint identifier (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        strncpy(waypoint->wcv_waypoint_id, tokens->tokens[3], sizeof(waypoint->wcv_waypoint_id) - 1);
        waypoint->wcv_waypoint_id[sizeof(waypoint->wcv_waypoint_id) - 1] = '\0';
    }

    /* Parse mode indicator (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        waypoint->wcv_mode = tokens->tokens[4][0];
        waypoint->wcv_valid = true;
    }

    return NMEA_OK;
}
#endif

#endif /* NMEA_MODULE_WAYPOINT_ENABLED */
