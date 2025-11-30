/**
 * @file nmea_heading.c
 * @brief Heading and compass module implementation
 *
 * Implements parsers for heading, compass, and vessel control sentences:
 * - HDG: Heading, deviation and variation
 * - HDT: Heading true
 * - ROT: Rate of turn
 * - VTG: Course over ground and ground speed
 * - VDR: Set and drift
 * - OSD: Own ship data
 * - HMR: Heading monitor receive
 * - HTC: Heading/track control command/data
 */

#include "modules/nmea_heading.h"
#include "nmea_internal.h"
#include <string.h>
#include <stdlib.h>

#if NMEA_MODULE_HEADING_ENABLED

/* ========================================================================== */
/*                           HDG - Heading                                    */
/* ========================================================================== */

#if NMEA_SENTENCE_HDG_ENABLED
/**
 * @brief Parse HDG - Heading, deviation and variation
 *
 * Format: $--HDG,x.x,x.x,a,x.x,a*hh
 * Fields:
 *  1: Magnetic sensor heading, degrees
 *  2: Magnetic deviation, degrees
 *  3: Deviation direction (E/W)
 *  4: Magnetic variation, degrees
 *  5: Variation direction (E/W)
 */
nmea_result_t nmea_parse_hdg(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_heading_state_t *heading = &ctx->heading;

    /* Parse magnetic sensor heading (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        if (nmea_parse_float(tokens->tokens[1], &heading->heading_magnetic_sensor) == NMEA_OK) {
            heading->hdg_valid = true;
        }
    }

    /* Parse magnetic deviation (field 2 and 3) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_float(tokens->tokens[2], &heading->magnetic_deviation);
        if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
            heading->deviation_direction = tokens->tokens[3][0];
            /* Apply sign: E is positive, W is negative */
            if (heading->deviation_direction == 'W') {
                heading->magnetic_deviation = -heading->magnetic_deviation;
            }
        }
    }

    /* Parse magnetic variation (field 4 and 5) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &heading->magnetic_variation);
        if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
            heading->variation_direction = tokens->tokens[5][0];
            /* Apply sign: E is positive, W is negative */
            if (heading->variation_direction == 'W') {
                heading->magnetic_variation = -heading->magnetic_variation;
            }
        }
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                        HDT - Heading True                                  */
/* ========================================================================== */

#if NMEA_SENTENCE_HDT_ENABLED
/**
 * @brief Parse HDT - Heading true
 *
 * Format: $--HDT,x.x,T*hh
 * Fields:
 *  1: Heading, degrees true
 *  2: 'T' (True indicator)
 */
nmea_result_t nmea_parse_hdt(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_heading_state_t *heading = &ctx->heading;

    /* Parse true heading (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        if (nmea_parse_float(tokens->tokens[1], &heading->heading_true) == NMEA_OK) {
            heading->hdt_valid = true;
        }
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                        ROT - Rate of Turn                                  */
/* ========================================================================== */

#if NMEA_SENTENCE_ROT_ENABLED
/**
 * @brief Parse ROT - Rate of turn
 *
 * Format: $--ROT,x.x,A*hh
 * Fields:
 *  1: Rate of turn, degrees/min (negative = bow turns to port)
 *  2: Status: A = valid, V = invalid
 */
nmea_result_t nmea_parse_rot(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_heading_state_t *heading = &ctx->heading;

    /* Parse rate of turn (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &heading->rate_of_turn);
    }

    /* Parse status (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        heading->rot_status = tokens->tokens[2][0];
        if (heading->rot_status == 'A') {
            heading->rot_valid = true;
        }
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                   VTG - Course Over Ground and Speed                       */
/* ========================================================================== */

#if NMEA_SENTENCE_VTG_ENABLED
/**
 * @brief Parse VTG - Course over ground and ground speed
 *
 * Format: $--VTG,x.x,T,x.x,M,x.x,N,x.x,K,a*hh
 * Fields:
 *  1: Course over ground, degrees true
 *  2: 'T' (True indicator)
 *  3: Course over ground, degrees magnetic
 *  4: 'M' (Magnetic indicator)
 *  5: Speed over ground, knots
 *  6: 'N' (Knots indicator)
 *  7: Speed over ground, km/h
 *  8: 'K' (km/h indicator)
 *  9: Mode indicator (A/D/E/M/P/S/N)
 */
nmea_result_t nmea_parse_vtg(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_heading_state_t *heading = &ctx->heading;

    /* Parse course over ground true (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &heading->course_true);
    }

    /* Parse course over ground magnetic (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &heading->course_magnetic);
    }

    /* Parse speed over ground in knots (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_float(tokens->tokens[5], &heading->speed_knots);
    }

    /* Parse speed over ground in km/h (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nmea_parse_float(tokens->tokens[7], &heading->speed_kmh);
    }

    /* Parse mode indicator (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        heading->vtg_mode = tokens->tokens[9][0];
        if (heading->vtg_mode != 'N') {
            heading->vtg_valid = true;
        }
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                        VDR - Set and Drift                                 */
/* ========================================================================== */

#if NMEA_SENTENCE_VDR_ENABLED
/**
 * @brief Parse VDR - Set and drift
 *
 * Format: $--VDR,x.x,T,x.x,M,x.x,N*hh
 * Fields:
 *  1: Current direction, degrees true
 *  2: 'T' (True indicator)
 *  3: Current direction, degrees magnetic
 *  4: 'M' (Magnetic indicator)
 *  5: Current speed, knots
 *  6: 'N' (Knots indicator)
 */
nmea_result_t nmea_parse_vdr(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_heading_state_t *heading = &ctx->heading;

    /* Parse current direction true (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        if (nmea_parse_float(tokens->tokens[1], &heading->current_direction_true) == NMEA_OK) {
            heading->vdr_valid = true;
        }
    }

    /* Parse current direction magnetic (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &heading->current_direction_magnetic);
    }

    /* Parse current speed in knots (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_float(tokens->tokens[5], &heading->current_speed_knots);
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                         OSD - Own Ship Data                                */
/* ========================================================================== */

#if NMEA_SENTENCE_OSD_ENABLED
/**
 * @brief Parse OSD - Own ship data
 *
 * Format: $--OSD,x.x,A,x.x,a,x.x,a,x.x,x.x,a*hh
 * Fields:
 *  1: Heading, degrees true
 *  2: Heading status (A = valid, V = invalid)
 *  3: Vessel course, degrees true
 *  4: Course reference (B/M/W/R/P)
 *  5: Vessel speed
 *  6: Speed reference (B/M/W/R/P)
 *  7: Vessel set, degrees true
 *  8: Vessel drift (speed)
 *  9: Speed units (K = km/h, N = knots, S = statute mph)
 */
nmea_result_t nmea_parse_osd(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_heading_state_t *heading = &ctx->heading;

    /* Parse heading true (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_float(tokens->tokens[1], &heading->osd_heading_true);
    }

    /* Parse heading status (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        heading->osd_heading_status = tokens->tokens[2][0];
        if (heading->osd_heading_status == 'A') {
            heading->osd_valid = true;
        }
    }

    /* Parse vessel course (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &heading->vessel_course_true);
    }

    /* Parse course reference (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        heading->course_reference = tokens->tokens[4][0];
    }

    /* Parse vessel speed (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_float(tokens->tokens[5], &heading->vessel_speed);
    }

    /* Parse speed reference (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        heading->speed_reference = tokens->tokens[6][0];
    }

    /* Parse vessel set (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nmea_parse_float(tokens->tokens[7], &heading->vessel_set_true);
    }

    /* Parse vessel drift (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &heading->vessel_drift);
    }

    /* Parse speed units (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        heading->speed_units = tokens->tokens[9][0];
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                      HMR - Heading Monitor Receive                         */
/* ========================================================================== */

#if NMEA_SENTENCE_HMR_ENABLED
/**
 * @brief Parse HMR - Heading monitor receive
 *
 * Format: $--HMR,c--c,c--c,x.x,x.x,A,x.x,A,a,x.x,a,x.x,A,a,x.x,a,x.x,a*hh
 * Fields:
 *  1: Heading sensor 1 ID
 *  2: Heading sensor 2 ID
 *  3: Set difference by HMS, degrees
 *  4: Actual heading sensor difference, degrees
 *  5: Warning flag (A = within limit, V = exceeds limit)
 *  6: Actual heading reading sensor 1, degrees
 *  7: Status heading sensor 1 (A = valid, V = invalid)
 *  8: Sensor 1 type (M = magnetic, T = true)
 *  9: Deviation sensor 1, degrees
 * 10: Deviation sensor 1 direction (E/W)
 * 11: Actual heading reading sensor 2, degrees
 * 12: Status heading sensor 2 (A = valid, V = invalid)
 * 13: Sensor 2 type (M = magnetic, T = true)
 * 14: Deviation sensor 2, degrees
 * 15: Deviation sensor 2 direction (E/W)
 * 16: Variation, degrees
 * 17: Variation direction (E/W)
 */
nmea_result_t nmea_parse_hmr(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_heading_state_t *heading = &ctx->heading;

    /* Parse sensor 1 ID (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        strncpy(heading->sensor1_id, tokens->tokens[1], sizeof(heading->sensor1_id) - 1);
        heading->sensor1_id[sizeof(heading->sensor1_id) - 1] = '\0';
    }

    /* Parse sensor 2 ID (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        strncpy(heading->sensor2_id, tokens->tokens[2], sizeof(heading->sensor2_id) - 1);
        heading->sensor2_id[sizeof(heading->sensor2_id) - 1] = '\0';
    }

    /* Parse set difference (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &heading->set_difference);
    }

    /* Parse actual difference (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &heading->actual_difference);
    }

    /* Parse warning flag (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        heading->difference_warning = tokens->tokens[5][0];
    }

    /* Parse sensor 1 heading (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &heading->sensor1_heading);
    }

    /* Parse sensor 1 status (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        heading->sensor1_status = tokens->tokens[7][0];
        if (heading->sensor1_status == 'A') {
            heading->hmr_valid = true;
        }
    }

    /* Parse sensor 1 type (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        heading->sensor1_type = tokens->tokens[8][0];
    }

    /* Parse sensor 1 deviation (field 9 and 10) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        nmea_parse_float(tokens->tokens[9], &heading->sensor1_deviation);
        if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
            heading->sensor1_deviation_dir = tokens->tokens[10][0];
            if (heading->sensor1_deviation_dir == 'W') {
                heading->sensor1_deviation = -heading->sensor1_deviation;
            }
        }
    }

    /* Parse sensor 2 heading (field 11) */
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
        nmea_parse_float(tokens->tokens[11], &heading->sensor2_heading);
    }

    /* Parse sensor 2 status (field 12) */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        heading->sensor2_status = tokens->tokens[12][0];
    }

    /* Parse sensor 2 type (field 13) */
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        heading->sensor2_type = tokens->tokens[13][0];
    }

    /* Parse sensor 2 deviation (field 14 and 15) */
    if (tokens->token_count > 14 && !nmea_is_empty_token(tokens->tokens[14])) {
        nmea_parse_float(tokens->tokens[14], &heading->sensor2_deviation);
        if (tokens->token_count > 15 && !nmea_is_empty_token(tokens->tokens[15])) {
            heading->sensor2_deviation_dir = tokens->tokens[15][0];
            if (heading->sensor2_deviation_dir == 'W') {
                heading->sensor2_deviation = -heading->sensor2_deviation;
            }
        }
    }

    /* Parse variation (field 16 and 17) */
    if (tokens->token_count > 16 && !nmea_is_empty_token(tokens->tokens[16])) {
        nmea_parse_float(tokens->tokens[16], &heading->hmr_variation);
        if (tokens->token_count > 17 && !nmea_is_empty_token(tokens->tokens[17])) {
            heading->hmr_variation_dir = tokens->tokens[17][0];
            if (heading->hmr_variation_dir == 'W') {
                heading->hmr_variation = -heading->hmr_variation;
            }
        }
    }

    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                    HTC - Heading/Track Control                             */
/* ========================================================================== */

#if NMEA_SENTENCE_HTC_ENABLED
/**
 * @brief Parse HTC - Heading/track control command/data
 *
 * Format: $--HTC,A,x.x,a,a,a,x.x,x.x,x.x,x.x,x.x,x.x,x.x,a,a*hh (command)
 *         $--HTD,A,x.x,a,a,a,x.x,x.x,x.x,x.x,x.x,x.x,x.x,a,A,A,A,x.x*hh (data)
 *
 * Fields (common to both):
 *  1: Override (A = in use, V = not in use)
 *  2: Commanded rudder angle, degrees
 *  3: Commanded rudder direction (L = port, R = starboard)
 *  4: Selected steering mode (M/S/H/T/R)
 *  5: Turn mode (N/T/R)
 *  6: Commanded rudder limit, degrees
 *  7: Commanded off-heading limit, degrees
 *  8: Commanded radius of turn, n.miles
 *  9: Commanded rate of turn, degrees/min
 * 10: Commanded heading-to-steer, degrees
 * 11: Commanded off-track limit, n.miles
 * 12: Commanded track, degrees
 * 13: Heading reference in use (T/M)
 * 14: Rudder status (A = within limits, V = limit reached)
 *
 * HTD has additional fields:
 * 15: Off-heading status
 * 16: Off-track status
 * 17: Vessel heading, degrees
 */
nmea_result_t nmea_parse_htc(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_heading_state_t *heading = &ctx->heading;

    /* Parse override (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        heading->htc_override = tokens->tokens[1][0];
    }

    /* Parse commanded rudder angle (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_float(tokens->tokens[2], &heading->commanded_rudder_angle);
    }

    /* Parse rudder direction (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        heading->rudder_direction = tokens->tokens[3][0];
    }

    /* Parse steering mode (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        heading->steering_mode = tokens->tokens[4][0];
    }

    /* Parse turn mode (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        heading->turn_mode = tokens->tokens[5][0];
    }

    /* Parse rudder limit (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &heading->rudder_limit);
    }

    /* Parse off-heading limit (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nmea_parse_float(tokens->tokens[7], &heading->off_heading_limit);
    }

    /* Parse commanded radius (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &heading->commanded_radius);
    }

    /* Parse commanded rate of turn (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        nmea_parse_float(tokens->tokens[9], &heading->commanded_rate_of_turn);
    }

    /* Parse commanded heading (field 10) */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        nmea_parse_float(tokens->tokens[10], &heading->commanded_heading);
    }

    /* Parse off-track limit (field 11) */
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
        nmea_parse_float(tokens->tokens[11], &heading->off_track_limit);
    }

    /* Parse commanded track (field 12) */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        nmea_parse_float(tokens->tokens[12], &heading->commanded_track);
    }

    /* Parse heading reference (field 13) */
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        heading->heading_reference = tokens->tokens[13][0];
        heading->htc_valid = true;
    }

    /* Parse rudder status (field 14) */
    if (tokens->token_count > 14 && !nmea_is_empty_token(tokens->tokens[14])) {
        heading->rudder_status = tokens->tokens[14][0];
    }

    /* For HTD sentences - additional fields */
    /* Parse off-heading status (field 15) */
    if (tokens->token_count > 15 && !nmea_is_empty_token(tokens->tokens[15])) {
        heading->off_heading_status = tokens->tokens[15][0];
    }

    /* Parse off-track status (field 16) */
    if (tokens->token_count > 16 && !nmea_is_empty_token(tokens->tokens[16])) {
        heading->off_track_status = tokens->tokens[16][0];
    }

    /* Parse vessel heading (field 17) */
    if (tokens->token_count > 17 && !nmea_is_empty_token(tokens->tokens[17])) {
        nmea_parse_float(tokens->tokens[17], &heading->vessel_heading);
    }

    return NMEA_OK;
}
#endif

#endif /* NMEA_MODULE_HEADING_ENABLED */
