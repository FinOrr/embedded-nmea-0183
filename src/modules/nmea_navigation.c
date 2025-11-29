/**
 * @file nmea_navigation.c
 * @brief Navigation and routing module implementation
 */

#include "modules/nmea_navigation.h"
#include "nmea_internal.h"
#include <string.h>

#if NMEA_MODULE_NAVIGATION_ENABLED

/* ========================================================================== */
/*                      APB - Autopilot Sentence B                            */
/* ========================================================================== */

#if NMEA_SENTENCE_APB_ENABLED
nmea_result_t nmea_parse_apb(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_navigation_state_t *nav = &ctx->navigation;

    /* Field 1: Status 1 */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nav->apb_status1 = tokens->tokens[1][0];
    }

    /* Field 2: Status 2 */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nav->apb_status2 = tokens->tokens[2][0];
    }

    /* Field 3: XTE magnitude */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &nav->xte_magnitude);
    }

    /* Field 4: Direction to steer */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nav->xte_steer_direction = tokens->tokens[4][0];
    }

    /* Field 6: Arrival circle entered */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nav->arrival_circle_entered = tokens->tokens[6][0];
    }

    /* Field 7: Perpendicular passed */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nav->perpendicular_passed = tokens->tokens[7][0];
    }

    /* Field 8: Bearing origin to destination */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &nav->bearing_origin_to_dest);
    }

    /* Field 9: Bearing type (M/T) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        nav->bearing_origin_type = tokens->tokens[9][0];
    }

    /* Field 10: Destination waypoint ID */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        strncpy(nav->destination_waypoint_id, tokens->tokens[10], sizeof(nav->destination_waypoint_id) - 1);
        nav->destination_waypoint_id[sizeof(nav->destination_waypoint_id) - 1] = '\0';
    }

    /* Field 11: Bearing position to destination */
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
        nmea_parse_float(tokens->tokens[11], &nav->bearing_position_to_dest);
    }

    /* Field 12: Bearing type (M/T) */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        nav->bearing_position_type = tokens->tokens[12][0];
    }

    /* Field 13: Heading to steer */
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        nmea_parse_float(tokens->tokens[13], &nav->heading_to_steer);
    }

    /* Field 14: Heading type (M/T) */
    if (tokens->token_count > 14 && !nmea_is_empty_token(tokens->tokens[14])) {
        nav->heading_to_steer_type = tokens->tokens[14][0];
    }

    /* Field 15: Mode indicator */
    if (tokens->token_count > 15 && !nmea_is_empty_token(tokens->tokens[15])) {
        nav->apb_mode = tokens->tokens[15][0];
        if (nav->apb_mode != 'N') {
            nav->apb_valid = true;
        }
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                  BWC - Bearing and Distance to Waypoint                    */
/* ========================================================================== */

#if NMEA_SENTENCE_BWC_ENABLED
nmea_result_t nmea_parse_bwc(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_navigation_state_t *nav = &ctx->navigation;

    /* Field 1: UTC time */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &nav->waypoint_utc);
    }

    /* Field 2, 3: Waypoint latitude */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_coordinate(tokens->tokens[2], tokens->tokens[3], &nav->waypoint_latitude);
    }

    /* Field 4, 5: Waypoint longitude */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_coordinate(tokens->tokens[4], tokens->tokens[5], &nav->waypoint_longitude);
    }

    /* Field 6: Bearing true */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &nav->bearing_true);
    }

    /* Field 8: Bearing magnetic */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &nav->bearing_magnetic);
    }

    /* Field 10: Distance */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        nmea_parse_float(tokens->tokens[10], &nav->distance_nm);
    }

    /* Field 12: Waypoint ID */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        strncpy(nav->waypoint_id, tokens->tokens[12], sizeof(nav->waypoint_id) - 1);
        nav->waypoint_id[sizeof(nav->waypoint_id) - 1] = '\0';
        nav->bwc_valid = true;
    }

    /* Field 13: Mode (if present) */
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        nav->bw_mode = tokens->tokens[13][0];
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                  BWR - Bearing and Distance (Rhumb Line)                   */
/* ========================================================================== */

#if NMEA_SENTENCE_BWR_ENABLED
nmea_result_t nmea_parse_bwr(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_navigation_state_t *nav = &ctx->navigation;

    /* Field 1: UTC time */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &nav->waypoint_utc);
    }

    /* Field 2, 3: Waypoint latitude */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_coordinate(tokens->tokens[2], tokens->tokens[3], &nav->waypoint_latitude);
    }

    /* Field 4, 5: Waypoint longitude */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_coordinate(tokens->tokens[4], tokens->tokens[5], &nav->waypoint_longitude);
    }

    /* Field 6: Bearing true */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &nav->bearing_true);
    }

    /* Field 8: Bearing magnetic */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &nav->bearing_magnetic);
    }

    /* Field 10: Distance */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        nmea_parse_float(tokens->tokens[10], &nav->distance_nm);
    }

    /* Field 12: Waypoint ID */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        strncpy(nav->waypoint_id, tokens->tokens[12], sizeof(nav->waypoint_id) - 1);
        nav->waypoint_id[sizeof(nav->waypoint_id) - 1] = '\0';
        nav->bwr_valid = true;
    }

    /* Field 13: Mode */
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        nav->bw_mode = tokens->tokens[13][0];
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*              BEC - Bearing and Distance (Dead Reckoning)                   */
/* ========================================================================== */

#if NMEA_SENTENCE_BEC_ENABLED
nmea_result_t nmea_parse_bec(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_navigation_state_t *nav = &ctx->navigation;

    /* Field 1: UTC time */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &nav->bec_utc);
    }

    /* Field 2, 3: Waypoint latitude */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_coordinate(tokens->tokens[2], tokens->tokens[3], &nav->bec_waypoint_lat);
    }

    /* Field 4, 5: Waypoint longitude */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_coordinate(tokens->tokens[4], tokens->tokens[5], &nav->bec_waypoint_lon);
    }

    /* Field 6: Bearing true */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &nav->bec_bearing_true);
    }

    /* Field 8: Bearing magnetic */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &nav->bec_bearing_magnetic);
    }

    /* Field 10: Distance */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        nmea_parse_float(tokens->tokens[10], &nav->bec_distance_nm);
    }

    /* Field 12: Waypoint ID */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        strncpy(nav->bec_waypoint_id, tokens->tokens[12], sizeof(nav->bec_waypoint_id) - 1);
        nav->bec_waypoint_id[sizeof(nav->bec_waypoint_id) - 1] = '\0';
        nav->bec_valid = true;
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                    BOD - Bearing Origin to Destination                     */
/* ========================================================================== */

#if NMEA_SENTENCE_BOD_ENABLED
nmea_result_t nmea_parse_bod(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_navigation_state_t *nav = &ctx->navigation;

    /* Field 1: Bearing true */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &nav->bod_bearing_true);
    }

    /* Field 3: Bearing magnetic */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &nav->bod_bearing_magnetic);
    }

    /* Field 5: Destination waypoint ID */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        strncpy(nav->bod_dest_id, tokens->tokens[5], sizeof(nav->bod_dest_id) - 1);
        nav->bod_dest_id[sizeof(nav->bod_dest_id) - 1] = '\0';
    }

    /* Field 6: Origin waypoint ID */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        /* Strip any trailing checksum from last field */
        char temp[32];
        strncpy(temp, tokens->tokens[6], sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = '\0';
        nmea_strip_checksum(temp);
        
        strncpy(nav->bod_origin_id, temp, sizeof(nav->bod_origin_id) - 1);
        nav->bod_origin_id[sizeof(nav->bod_origin_id) - 1] = '\0';
        nav->bod_valid = true;
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                   BWW - Bearing Waypoint to Waypoint                       */
/* ========================================================================== */

#if NMEA_SENTENCE_BWW_ENABLED
nmea_result_t nmea_parse_bww(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_navigation_state_t *nav = &ctx->navigation;

    /* Field 1: Bearing true */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &nav->bww_bearing_true);
    }

    /* Field 3: Bearing magnetic */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &nav->bww_bearing_magnetic);
    }

    /* Field 5: TO waypoint ID */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        strncpy(nav->bww_to_id, tokens->tokens[5], sizeof(nav->bww_to_id) - 1);
        nav->bww_to_id[sizeof(nav->bww_to_id) - 1] = '\0';
    }

    /* Field 6: FROM waypoint ID */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        /* Strip any trailing checksum from last field */
        char temp[32];
        strncpy(temp, tokens->tokens[6], sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = '\0';
        nmea_strip_checksum(temp);
        
        strncpy(nav->bww_from_id, temp, sizeof(nav->bww_from_id) - 1);
        nav->bww_from_id[sizeof(nav->bww_from_id) - 1] = '\0';
        nav->bww_valid = true;
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                           RTE - Routes                                     */
/* ========================================================================== */

#if NMEA_SENTENCE_RTE_ENABLED
nmea_result_t nmea_parse_rte(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_navigation_state_t *nav = &ctx->navigation;

    /* Field 1: Total sentences */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int temp;
        if (nmea_parse_int(tokens->tokens[1], &temp) == NMEA_OK) {
            nav->rte_total_sentences = (uint8_t)temp;
        }
    }

    /* Field 2: Sentence number */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        int temp;
        if (nmea_parse_int(tokens->tokens[2], &temp) == NMEA_OK) {
            nav->rte_sentence_number = (uint8_t)temp;
        }
    }

    /* Field 3: Mode (c = complete, w = working) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nav->rte_mode = tokens->tokens[3][0];
    }

    /* Field 4: Route ID */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        strncpy(nav->route_id, tokens->tokens[4], sizeof(nav->route_id) - 1);
        nav->route_id[sizeof(nav->route_id) - 1] = '\0';
    }

    /* Fields 5+: Waypoint IDs */
    nav->waypoint_count = 0;
    for (size_t i = 5; i < tokens->token_count && nav->waypoint_count < 12; i++) {
        if (!nmea_is_empty_token(tokens->tokens[i])) {
            char temp[32];
            strncpy(temp, tokens->tokens[i], sizeof(temp) - 1);
            temp[sizeof(temp) - 1] = '\0';
            
            /* Strip checksum from last waypoint if it's the last token */
            if (i == tokens->token_count - 1) {
                nmea_strip_checksum(temp);
            }
            
            strncpy(nav->waypoint_list[nav->waypoint_count], temp,
                    sizeof(nav->waypoint_list[0]) - 1);
            nav->waypoint_list[nav->waypoint_count][sizeof(nav->waypoint_list[0]) - 1] = '\0';
            nav->waypoint_count++;
        }
    }

    if (nav->waypoint_count > 0) {
        nav->rte_valid = true;
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                      AAM - Waypoint Arrival Alarm                          */
/* ========================================================================== */

#if NMEA_SENTENCE_AAM_ENABLED
nmea_result_t nmea_parse_aam(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_navigation_state_t *nav = &ctx->navigation;

    /* Field 1: Arrival circle entered */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nav->aam_arrival_circle = tokens->tokens[1][0];
    }

    /* Field 2: Perpendicular passed */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nav->aam_perpendicular = tokens->tokens[2][0];
    }

    /* Field 3: Arrival circle radius */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &nav->aam_radius);
    }

    /* Field 5: Waypoint ID */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        /* Strip any trailing checksum from last field */
        char temp[32];
        strncpy(temp, tokens->tokens[5], sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = '\0';
        nmea_strip_checksum(temp);
        
        strncpy(nav->aam_waypoint_id, temp, sizeof(nav->aam_waypoint_id) - 1);
        nav->aam_waypoint_id[sizeof(nav->aam_waypoint_id) - 1] = '\0';
        nav->aam_valid = true;
    }

    return NMEA_OK;
}
#endif

#endif /* NMEA_MODULE_NAVIGATION_ENABLED */
