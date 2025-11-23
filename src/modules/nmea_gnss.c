/**
 * @file nmea_gnss.c
 * @brief GNSS/GPS module implementation
 *
 * Implements parsers for GNSS-related NMEA sentences.
 */

#include "../../inc/modules/nmea_gnss.h"
#include "../nmea_internal.h"
#include <string.h>

#if NMEA_MODULE_GNSS_ENABLED

/* ========================================================================== */
/*                          GGA - FIX DATA                                    */
/* ========================================================================== */

#if NMEA_SENTENCE_GGA_ENABLED
/**
 * @brief Parse GGA - Global Positioning System Fix Data
 *
 * Format: $GPGGA,time,lat,NS,lon,EW,quality,numSV,HDOP,alt,M,sep,M,diffAge,diffStation*cs
 * Example: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
 *
 * Fields:
 * 0: Sentence ID (GPGGA)
 * 1: UTC Time (hhmmss.sss)
 * 2: Latitude (ddmm.mmmm)
 * 3: N/S indicator
 * 4: Longitude (dddmm.mmmm)
 * 5: E/W indicator
 * 6: Fix quality (0-8)
 * 7: Number of satellites
 * 8: HDOP
 * 9: Altitude (meters)
 * 10: Altitude units (M)
 * 11: Geoid separation (meters)
 * 12: Geoid separation units (M)
 * 13: Age of differential corrections
 * 14: Differential station ID
 */
nmea_result_t nmea_parse_gga(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse UTC time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &gnss->utc_time);
    }

    /* Parse position (fields 2-5) */
    if (tokens->token_count > 5) {
        nmea_parse_coordinate(tokens->tokens[2], tokens->tokens[3], &gnss->latitude);
        nmea_parse_coordinate(tokens->tokens[4], tokens->tokens[5], &gnss->longitude);
    }

    /* Parse fix quality (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        int quality;
        if (nmea_parse_int(tokens->tokens[6], &quality) == NMEA_OK) {
            if (quality >= 0 && quality <= 8) {
                gnss->fix_quality = (nmea_quality_t)quality;
            }
        }
    }

    /* Parse number of satellites (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        int sats;
        if (nmea_parse_int(tokens->tokens[7], &sats) == NMEA_OK) {
            gnss->satellites_used = (uint8_t)sats;
        }
    }

    /* Parse HDOP (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &gnss->dop.hdop);
        gnss->dop.valid = true;
    }

    /* Parse altitude (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        if (nmea_parse_float(tokens->tokens[9], &gnss->altitude_meters) == NMEA_OK) {
            gnss->altitude_valid = true;
        }
    }

    /* Parse geoid separation (field 11) */
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
        if (nmea_parse_float(tokens->tokens[11], &gnss->geoid_separation_meters) == NMEA_OK) {
            gnss->geoid_separation_valid = true;
        }
    }

    /* Parse age of differential corrections (field 13) */
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        if (nmea_parse_float(tokens->tokens[13], &gnss->dgps_age_seconds) == NMEA_OK) {
            gnss->dgps_age_valid = true;
        }
    }

    /* Parse differential station ID (field 14) */
    if (tokens->token_count > 14 && !nmea_is_empty_token(tokens->tokens[14])) {
        int station;
        if (nmea_parse_int(tokens->tokens[14], &station) == NMEA_OK) {
            gnss->dgps_station_id = (uint16_t)station;
            gnss->dgps_station_id_valid = true;
        }
    }

    /* Mark data as valid */
    gnss->data_valid = true;

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_GGA_ENABLED */

/* ========================================================================== */
/*                          RMC - RECOMMENDED MINIMUM                         */
/* ========================================================================== */

#if NMEA_SENTENCE_RMC_ENABLED
/**
 * @brief Parse RMC - Recommended Minimum Specific GNSS Data
 *
 * Format: $GPRMC,time,status,lat,NS,lon,EW,speed,course,date,magvar,varDir,mode*cs
 * Example: $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W,A*6A
 *
 * Fields:
 * 0: Sentence ID (GPRMC)
 * 1: UTC Time (hhmmss.sss)
 * 2: Status (A=Active, V=Void)
 * 3: Latitude (ddmm.mmmm)
 * 4: N/S indicator
 * 5: Longitude (dddmm.mmmm)
 * 6: E/W indicator
 * 7: Speed over ground (knots)
 * 8: Course over ground (degrees true)
 * 9: Date (ddmmyy)
 * 10: Magnetic variation (degrees)
 * 11: Variation direction (E/W)
 * 12: Mode indicator (A/D/E/N)
 */
nmea_result_t nmea_parse_rmc(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse UTC time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &gnss->utc_time);
    }

    /* Parse status (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        gnss->status = tokens->tokens[2][0];
        gnss->data_valid = (gnss->status == 'A' || gnss->status == 'a');
    }

    /* Parse position (fields 3-6) */
    if (tokens->token_count > 6 && gnss->data_valid) {
        nmea_parse_coordinate(tokens->tokens[3], tokens->tokens[4], &gnss->latitude);
        nmea_parse_coordinate(tokens->tokens[5], tokens->tokens[6], &gnss->longitude);
    }

    /* Parse speed over ground (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        if (nmea_parse_float(tokens->tokens[7], &gnss->speed.knots) == NMEA_OK) {
            gnss->speed.kmh = gnss->speed.knots * 1.852;
            gnss->speed.mps = gnss->speed.knots * 0.514444;
            gnss->speed.valid = true;
        }
    }

    /* Parse course over ground (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        if (nmea_parse_float(tokens->tokens[8], &gnss->course.degrees) == NMEA_OK) {
            gnss->course.is_true = true;  /* COG is true north */
            gnss->course.valid = true;
        }
    }

    /* Parse date (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        nmea_parse_date(tokens->tokens[9], &gnss->utc_date);
    }

    /* Parse magnetic variation (fields 10-11) */
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[10])) {
        if (nmea_parse_float(tokens->tokens[10], &gnss->magnetic_variation) == NMEA_OK) {
            /* Apply direction (W is negative) */
            if (tokens->tokens[11][0] == 'W' || tokens->tokens[11][0] == 'w') {
                gnss->magnetic_variation = -gnss->magnetic_variation;
            }
            gnss->magnetic_variation_direction = tokens->tokens[11][0];
            gnss->magnetic_variation_valid = true;
        }
    }

    /* Parse mode indicator (field 12) */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        gnss->mode_indicator = tokens->tokens[12][0];
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_RMC_ENABLED */

/* ========================================================================== */
/*                          GLL - GEOGRAPHIC POSITION                         */
/* ========================================================================== */

#if NMEA_SENTENCE_GLL_ENABLED
/**
 * @brief Parse GLL - Geographic Position - Latitude/Longitude
 *
 * Format: $GPGLL,lat,NS,lon,EW,time,status,mode*cs
 * Example: $GPGLL,4807.038,N,01131.000,E,123519,A,A*5C
 */
nmea_result_t nmea_parse_gll(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse position (fields 1-4) */
    if (tokens->token_count > 4) {
        nmea_parse_coordinate(tokens->tokens[1], tokens->tokens[2], &gnss->latitude);
        nmea_parse_coordinate(tokens->tokens[3], tokens->tokens[4], &gnss->longitude);
    }

    /* Parse UTC time (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_time(tokens->tokens[5], &gnss->utc_time);
    }

    /* Parse status (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        gnss->status = tokens->tokens[6][0];
        gnss->data_valid = (gnss->status == 'A' || gnss->status == 'a');
    }

    /* Parse mode (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        gnss->mode_indicator = tokens->tokens[7][0];
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_GLL_ENABLED */

/* ========================================================================== */
/*                 PLACEHOLDER PARSERS (TO BE IMPLEMENTED)                    */
/* ========================================================================== */

#if NMEA_SENTENCE_GNS_ENABLED
/**
 * @brief Parse GNS - GNSS Fix Data
 *
 * Format: $--GNS,hhmmss.ss,llll.ll,a,yyyyy.yy,a,c--c,xx,x.x,x.x,x.x,x.x,x.x,a*hh
 *
 * Fields:
 * 0: Sentence ID (GPGNS, GLGNS, GAGNS, GNGNS)
 * 1: UTC Time (hhmmss.ss)
 * 2: Latitude (llll.ll)
 * 3: N/S indicator
 * 4: Longitude (yyyyy.yy)
 * 5: E/W indicator
 * 6: Mode indicators (A/D/E/F/M/N/P/R/S for GPS/GLONASS/Galileo)
 * 7: Number of satellites
 * 8: HDOP
 * 9: Antenna altitude (meters)
 * 10: Geoidal separation (meters)
 * 11: Age of differential data
 * 12: Differential station ID
 * 13: Navigational status (S/C/U/V)
 */
nmea_result_t nmea_parse_gns(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse UTC time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &gnss->utc_time);
    }

    /* Parse position (fields 2-5) */
    if (tokens->token_count > 5) {
        nmea_parse_coordinate(tokens->tokens[2], tokens->tokens[3], &gnss->latitude);
        nmea_parse_coordinate(tokens->tokens[4], tokens->tokens[5], &gnss->longitude);
    }

    /* Parse mode indicators (field 6) - variable length string */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        const char *mode = tokens->tokens[6];
        size_t len = strlen(mode);
        if (len > 0 && len < sizeof(gnss->mode_indicators)) {
            strncpy(gnss->mode_indicators, mode, sizeof(gnss->mode_indicators) - 1);
            gnss->mode_indicators[sizeof(gnss->mode_indicators) - 1] = '\0';
            /* Set data valid if not all 'N' (no fix) */
            gnss->data_valid = (mode[0] != 'N' && mode[0] != 'n');
        }
    }

    /* Parse number of satellites (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        int sats;
        if (nmea_parse_int(tokens->tokens[7], &sats) == NMEA_OK) {
            gnss->satellites_used = (uint8_t)sats;
        }
    }

    /* Parse HDOP (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &gnss->dop.hdop);
        gnss->dop.valid = true;
    }

    /* Parse antenna altitude (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        if (nmea_parse_float(tokens->tokens[9], &gnss->altitude_meters) == NMEA_OK) {
            gnss->altitude_valid = true;
        }
    }

    /* Parse geoid separation (field 10) */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        if (nmea_parse_float(tokens->tokens[10], &gnss->geoid_separation_meters) == NMEA_OK) {
            gnss->geoid_separation_valid = true;
        }
    }

    /* Parse age of differential data (field 11) */
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
        if (nmea_parse_float(tokens->tokens[11], &gnss->dgps_age_seconds) == NMEA_OK) {
            gnss->dgps_age_valid = true;
        }
    }

    /* Parse differential station ID (field 12) */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        int station;
        if (nmea_parse_int(tokens->tokens[12], &station) == NMEA_OK) {
            gnss->dgps_station_id = (uint16_t)station;
            gnss->dgps_station_id_valid = true;
        }
    }

    /* Parse navigational status (field 13) */
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        gnss->nav_status = tokens->tokens[13][0];
    }

    return NMEA_OK;
}
#endif

#if NMEA_SENTENCE_GSA_ENABLED
/**
 * @brief Parse GSA - GNSS DOP and Active Satellites
 *
 * Format: $--GSA,a,x,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,x.x,x.x,x.x*hh
 *
 * Fields:
 * 0: Sentence ID (GPGSA, GLGSA, GAGSA, etc.)
 * 1: Mode (M=Manual, A=Automatic)
 * 2: Fix type (1=No fix, 2=2D, 3=3D)
 * 3-14: PRN numbers of satellites used (12 fields)
 * 15: PDOP
 * 16: HDOP
 * 17: VDOP
 */
nmea_result_t nmea_parse_gsa(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse mode (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        char mode = tokens->tokens[1][0];
        if (mode == 'M' || mode == 'm') {
            gnss->fix_selection = NMEA_FIX_SELECTION_MANUAL;
        } else if (mode == 'A' || mode == 'a') {
            gnss->fix_selection = NMEA_FIX_SELECTION_AUTO;
        }
    }

    /* Parse fix type (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        int fix_type;
        if (nmea_parse_int(tokens->tokens[2], &fix_type) == NMEA_OK) {
            if (fix_type >= 1 && fix_type <= 3) {
                gnss->fix_mode = (nmea_fix_mode_t)fix_type;
            }
        }
    }

    /* Parse satellite PRNs (fields 3-14) */
    uint8_t sat_count = 0;
    for (size_t i = 3; i < 15 && i < tokens->token_count; i++) {
        if (!nmea_is_empty_token(tokens->tokens[i])) {
            int prn;
            if (nmea_parse_int(tokens->tokens[i], &prn) == NMEA_OK) {
                if (sat_count < NMEA_MAX_SATELLITES_USED) {
                    gnss->satellite_prns[sat_count++] = (uint8_t)prn;
                }
            }
        }
    }
    gnss->satellites_used = sat_count;

    /* Parse PDOP (field 15) */
    if (tokens->token_count > 15 && !nmea_is_empty_token(tokens->tokens[15])) {
        if (nmea_parse_float(tokens->tokens[15], &gnss->dop.pdop) == NMEA_OK) {
            gnss->dop.valid = true;
        }
    }

    /* Parse HDOP (field 16) */
    if (tokens->token_count > 16 && !nmea_is_empty_token(tokens->tokens[16])) {
        if (nmea_parse_float(tokens->tokens[16], &gnss->dop.hdop) == NMEA_OK) {
            gnss->dop.valid = true;
        }
    }

    /* Parse VDOP (field 17) */
    if (tokens->token_count > 17 && !nmea_is_empty_token(tokens->tokens[17])) {
        if (nmea_parse_float(tokens->tokens[17], &gnss->dop.vdop) == NMEA_OK) {
            gnss->dop.valid = true;
        }
    }

    return NMEA_OK;
}
#endif

#if NMEA_SENTENCE_GSV_ENABLED
/**
 * @brief Parse GSV - GNSS Satellites in View
 *
 * Format: $--GSV,x,x,xx,xx,xx,xxx,xx..............,xx,xx,xxx,xx,h*hh
 *
 * Fields:
 * 0: Sentence ID
 * 1: Total number of messages (1-9)
 * 2: Message number (1-9)
 * 3: Total number of satellites in view
 * 4-7: Satellite 1 (PRN, elevation, azimuth, SNR)
 * 8-11: Satellite 2
 * 12-15: Satellite 3
 * 16-19: Satellite 4
 * 20: Signal ID (optional)
 */
nmea_result_t nmea_parse_gsv(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse message number (field 2) */
    int msg_num = 1;
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_int(tokens->tokens[2], &msg_num);
    }

    /* Parse total satellites in view (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        int sats_in_view;
        if (nmea_parse_int(tokens->tokens[3], &sats_in_view) == NMEA_OK) {
            gnss->satellites_in_view = (uint8_t)sats_in_view;
        }
    }

    /* Parse up to 4 satellites per message */
    size_t base_idx = (msg_num - 1) * 4;  /* Starting index for this message */

    for (int sat = 0; sat < 4; sat++) {
        size_t field_base = 4 + (sat * 4);
        size_t sat_idx = base_idx + sat;

        if (sat_idx >= NMEA_MAX_SATELLITES) {
            break;  /* Exceeded maximum satellites */
        }

        /* Check if we have enough tokens for this satellite */
        if (field_base >= tokens->token_count) {
            break;
        }

        nmea_satellite_t *satellite = &gnss->satellites[sat_idx];
        satellite->valid = false;

        /* Parse PRN (field base + 0) */
        if (!nmea_is_empty_token(tokens->tokens[field_base])) {
            int prn;
            if (nmea_parse_int(tokens->tokens[field_base], &prn) == NMEA_OK) {
                satellite->prn = (uint8_t)prn;
                satellite->valid = true;
            }
        }

        /* Parse elevation (field base + 1) */
        if (field_base + 1 < tokens->token_count &&
            !nmea_is_empty_token(tokens->tokens[field_base + 1])) {
            int elev;
            if (nmea_parse_int(tokens->tokens[field_base + 1], &elev) == NMEA_OK) {
                satellite->elevation = (uint8_t)elev;
            }
        }

        /* Parse azimuth (field base + 2) */
        if (field_base + 2 < tokens->token_count &&
            !nmea_is_empty_token(tokens->tokens[field_base + 2])) {
            int azim;
            if (nmea_parse_int(tokens->tokens[field_base + 2], &azim) == NMEA_OK) {
                satellite->azimuth = (uint16_t)azim;
            }
        }

        /* Parse SNR (field base + 3) */
        if (field_base + 3 < tokens->token_count &&
            !nmea_is_empty_token(tokens->tokens[field_base + 3])) {
            int snr;
            if (nmea_parse_int(tokens->tokens[field_base + 3], &snr) == NMEA_OK) {
                satellite->snr = (uint8_t)snr;
            }
        }
    }

    return NMEA_OK;
}
#endif

#if NMEA_SENTENCE_GBS_ENABLED
/**
 * @brief Parse GBS - GNSS Satellite Fault Detection (RAIM)
 *
 * Format: $--GBS,hhmmss.ss,x.x,x.x,x.x,xx,x.x,x.x,x.x,h,h*hh
 *
 * Fields:
 * 0: Sentence ID
 * 1: UTC time of GGA/GNS fix
 * 2: Expected error in latitude (meters)
 * 3: Expected error in longitude (meters)
 * 4: Expected error in altitude (meters)
 * 5: ID of most likely failed satellite
 * 6: Probability of missed detection
 * 7: Estimate of bias on failed satellite (meters)
 * 8: Standard deviation of bias estimate (meters)
 * 9: GNSS System ID (optional)
 * 10: GNSS Signal ID (optional)
 */
nmea_result_t nmea_parse_gbs(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse UTC time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &gnss->utc_time);
    }

    /* Parse expected error in latitude (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        if (nmea_parse_float(tokens->tokens[2], &gnss->expected_error_lat) == NMEA_OK) {
            gnss->gbs_valid = true;
        }
    }

    /* Parse expected error in longitude (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &gnss->expected_error_lon);
    }

    /* Parse expected error in altitude (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &gnss->expected_error_alt);
    }

    /* Parse ID of most likely failed satellite (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        int sat_id;
        if (nmea_parse_int(tokens->tokens[5], &sat_id) == NMEA_OK) {
            gnss->failed_satellite_id = (uint8_t)sat_id;
        }
    }

    /* Parse probability of missed detection (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &gnss->failed_satellite_probability);
    }

    /* Parse estimate of bias (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nmea_parse_float(tokens->tokens[7], &gnss->failed_satellite_bias);
    }

    /* Parse standard deviation of bias estimate (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &gnss->failed_satellite_bias_std_dev);
    }

    /* Note: GNSS System ID (field 9) and Signal ID (field 10) are optional
     * and not currently stored in the state structure */

    return NMEA_OK;
}
#endif

#if NMEA_SENTENCE_GST_ENABLED
/**
 * @brief Parse GST - GNSS Pseudorange Error Statistics
 *
 * Format: $--GST,hhmmss.ss,x.x,x.x,x.x,x.x,x.x,x.x,x.x*hh
 *
 * Fields:
 * 0: Sentence ID
 * 1: UTC time
 * 2: RMS value of standard deviation of range inputs
 * 3: Standard deviation of semi-major axis of error ellipse (meters)
 * 4: Standard deviation of semi-minor axis of error ellipse (meters)
 * 5: Orientation of semi-major axis (degrees from true north)
 * 6: Standard deviation of latitude error (meters)
 * 7: Standard deviation of longitude error (meters)
 * 8: Standard deviation of altitude error (meters)
 */
nmea_result_t nmea_parse_gst(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse UTC time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &gnss->utc_time);
    }

    /* Parse RMS std dev (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        if (nmea_parse_float(tokens->tokens[2], &gnss->error_stats.rms_std_dev) == NMEA_OK) {
            gnss->error_stats.valid = true;
        }
    }

    /* Parse std dev of semi-major axis (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &gnss->error_stats.std_dev_major);
    }

    /* Parse std dev of semi-minor axis (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &gnss->error_stats.std_dev_minor);
    }

    /* Parse orientation of semi-major axis (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_float(tokens->tokens[5], &gnss->error_stats.orientation);
    }

    /* Parse std dev of latitude (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &gnss->error_stats.std_dev_lat);
    }

    /* Parse std dev of longitude (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nmea_parse_float(tokens->tokens[7], &gnss->error_stats.std_dev_lon);
    }

    /* Parse std dev of altitude (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        nmea_parse_float(tokens->tokens[8], &gnss->error_stats.std_dev_alt);
    }

    return NMEA_OK;
}
#endif

#if NMEA_SENTENCE_GRS_ENABLED
/**
 * @brief Parse GRS - GNSS Range Residuals
 *
 * Format: $--GRS,hhmmss.ss,x,x.x,x.x,x.x,x.x,x.x,x.x,x.x,x.x,x.x,x.x,x.x,x.x,h,h*hh
 *
 * Fields:
 * 0: Sentence ID
 * 1: UTC time
 * 2: Mode (0=residuals used in position, 1=residuals recomputed after)
 * 3-14: Range residuals for satellites (meters, up to 12)
 * 15: Signal ID (optional)
 * 16: System ID (optional)
 */
nmea_result_t nmea_parse_grs(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse UTC time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &gnss->utc_time);
    }

    /* Parse mode (field 2) - not currently stored in state */
    /* Mode: 0 = residuals used for position, 1 = recomputed */

    /* Parse range residuals (fields 3-14) */
    size_t residual_count = 0;
    for (size_t i = 3; i < 15 && i < tokens->token_count; i++) {
        if (!nmea_is_empty_token(tokens->tokens[i])) {
            float residual;
            if (nmea_parse_float(tokens->tokens[i], &residual) == NMEA_OK) {
                if (residual_count < NMEA_MAX_SATELLITES_USED) {
                    gnss->range_residuals[residual_count++] = residual;
                    gnss->range_residuals_valid = true;
                }
            }
        }
    }

    /* Note: Signal ID (field 15) and System ID (field 16) are optional
     * and not currently stored in the state structure */

    return NMEA_OK;
}
#endif

#if NMEA_SENTENCE_GFA_ENABLED
/**
 * @brief Parse GFA - GNSS Fix Accuracy and Integrity
 *
 * Format: $--GFA,hhmmss.ss,x.x,x.x,x.x,x.x,x.x,x.x,x.x,c--c*hh
 *
 * Fields:
 * 0: Sentence ID
 * 1: UTC time
 * 2: Horizontal protection level (meters)
 * 3: Vertical protection level (meters)
 * 4: Standard deviation of semi-major axis of error ellipse (meters)
 * 5: Standard deviation of semi-minor axis of error ellipse (meters)
 * 6: Orientation of semi-major axis (degrees from true north)
 * 7: Standard deviation of altitude (meters)
 * 8: Selected accuracy level (meters)
 * 9: Integrity status (V=Not in use, S=Safe, C=Caution, U=Unsafe)
 */
nmea_result_t nmea_parse_gfa(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse UTC time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &gnss->utc_time);
    }

    /* Parse horizontal protection level (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        if (nmea_parse_float(tokens->tokens[2], &gnss->horizontal_protection_level) == NMEA_OK) {
            gnss->accuracy_valid = true;
        }
    }

    /* Parse vertical protection level (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_float(tokens->tokens[3], &gnss->vertical_protection_level);
    }

    /* Parse std dev of semi-major axis (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_float(tokens->tokens[4], &gnss->error_stats.std_dev_major);
    }

    /* Parse std dev of semi-minor axis (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        nmea_parse_float(tokens->tokens[5], &gnss->error_stats.std_dev_minor);
    }

    /* Parse orientation of semi-major axis (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &gnss->error_stats.orientation);
    }

    /* Parse std dev of altitude (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nmea_parse_float(tokens->tokens[7], &gnss->error_stats.std_dev_alt);
    }

    /* Parse selected accuracy level (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        /* Store in horizontal_accuracy for now */
        nmea_parse_float(tokens->tokens[8], &gnss->horizontal_accuracy);
    }

    /* Parse integrity status (field 9) - variable length string */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        const char *status = tokens->tokens[9];
        size_t len = strlen(status);
        if (len > 0 && len < sizeof(gnss->integrity_status)) {
            strncpy(gnss->integrity_status, status, sizeof(gnss->integrity_status) - 1);
            gnss->integrity_status[sizeof(gnss->integrity_status) - 1] = '\0';
        }
    }

    return NMEA_OK;
}
#endif

#if NMEA_SENTENCE_RMB_ENABLED
/**
 * @brief Parse RMB - Recommended Minimum Navigation Information
 *
 * Format: $--RMB,A,x.x,a,c--c,c--c,llll.ll,a,yyyyy.yy,a,x.x,x.x,x.x,A,a*hh
 *
 * Fields:
 * 0: Sentence ID
 * 1: Status (A=data valid, V=warning)
 * 2: Cross track error (nautical miles)
 * 3: Direction to steer (L/R)
 * 4: Origin waypoint ID
 * 5: Destination waypoint ID
 * 6: Destination latitude
 * 7: N/S
 * 8: Destination longitude
 * 9: E/W
 * 10: Range to destination (nautical miles)
 * 11: Bearing to destination (degrees true)
 * 12: Destination closing velocity (knots)
 * 13: Arrival status (A=arrived, V=not arrived)
 * 14: Mode indicator (A/D/E/M/S/N)
 */
nmea_result_t nmea_parse_rmb(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse status (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        gnss->status = tokens->tokens[1][0];
        gnss->data_valid = (gnss->status == 'A' || gnss->status == 'a');
    }

    /* Parse cross track error (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        if (nmea_parse_float(tokens->tokens[2], &gnss->cross_track_error_nm) == NMEA_OK) {
            gnss->waypoint_data_valid = true;
        }
    }

    /* Parse direction to steer (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        gnss->steer_direction = tokens->tokens[3][0];
    }

    /* Parse origin waypoint ID (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        strncpy(gnss->origin_waypoint_id, tokens->tokens[4],
                sizeof(gnss->origin_waypoint_id) - 1);
        gnss->origin_waypoint_id[sizeof(gnss->origin_waypoint_id) - 1] = '\0';
    }

    /* Parse destination waypoint ID (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        strncpy(gnss->dest_waypoint_id, tokens->tokens[5],
                sizeof(gnss->dest_waypoint_id) - 1);
        gnss->dest_waypoint_id[sizeof(gnss->dest_waypoint_id) - 1] = '\0';
    }

    /* Parse destination latitude/longitude (fields 6-9) */
    if (tokens->token_count > 9) {
        nmea_parse_coordinate(tokens->tokens[6], tokens->tokens[7], &gnss->dest_latitude);
        nmea_parse_coordinate(tokens->tokens[8], tokens->tokens[9], &gnss->dest_longitude);
    }

    /* Parse range to destination (field 10) */
    if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
        nmea_parse_float(tokens->tokens[10], &gnss->range_to_dest_nm);
    }

    /* Parse bearing to destination (field 11) */
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
        nmea_parse_float(tokens->tokens[11], &gnss->bearing_to_dest_deg);
    }

    /* Parse destination closing velocity (field 12) */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        nmea_parse_float(tokens->tokens[12], &gnss->dest_closing_velocity_knots);
    }

    /* Parse arrival status (field 13) */
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        gnss->arrival_status = tokens->tokens[13][0];
    }

    /* Parse mode indicator (field 14) */
    if (tokens->token_count > 14 && !nmea_is_empty_token(tokens->tokens[14])) {
        gnss->mode_indicator = tokens->tokens[14][0];
    }

    return NMEA_OK;
}
#endif

#if NMEA_SENTENCE_RMA_ENABLED
/**
 * @brief Parse RMA - Recommended Minimum Specific LORAN-C Data
 *
 * Format: $--RMA,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,x.x,x.x,x.x,a,a*hh
 *
 * Fields:
 * 0: Sentence ID
 * 1: Status (A=data valid, V=blink/cycle/SNR warning)
 * 2: Latitude
 * 3: N/S
 * 4: Longitude
 * 5: E/W
 * 6: Time difference A (microseconds)
 * 7: Time difference B (microseconds)
 * 8: Speed over ground (knots)
 * 9: Course over ground (degrees true)
 * 10: Magnetic variation (degrees)
 * 11: Magnetic variation direction (E/W)
 * 12: Mode indicator (A/D/E/M/S/N)
 */
nmea_result_t nmea_parse_rma(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_gnss_state_t *gnss = &ctx->gnss;

    /* Parse status (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        gnss->status = tokens->tokens[1][0];
        gnss->data_valid = (gnss->status == 'A' || gnss->status == 'a');
    }

    /* Parse position (fields 2-5) */
    if (tokens->token_count > 5) {
        nmea_parse_coordinate(tokens->tokens[2], tokens->tokens[3], &gnss->latitude);
        nmea_parse_coordinate(tokens->tokens[4], tokens->tokens[5], &gnss->longitude);
    }

    /* Parse time difference A (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        nmea_parse_float(tokens->tokens[6], &gnss->time_diff_a_us);
    }

    /* Parse time difference B (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        nmea_parse_float(tokens->tokens[7], &gnss->time_diff_b_us);
    }

    /* Parse speed over ground (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        if (nmea_parse_float(tokens->tokens[8], &gnss->speed.knots) == NMEA_OK) {
            gnss->speed.kmh = gnss->speed.knots * 1.852f;
            gnss->speed.mps = gnss->speed.knots * 0.514444f;
            gnss->speed.valid = true;
        }
    }

    /* Parse course over ground (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        if (nmea_parse_float(tokens->tokens[9], &gnss->course.degrees) == NMEA_OK) {
            gnss->course.is_true = true;  /* COG is true north */
            gnss->course.valid = true;
        }
    }

    /* Parse magnetic variation (fields 10-11) */
    if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[10])) {
        if (nmea_parse_float(tokens->tokens[10], &gnss->magnetic_variation) == NMEA_OK) {
            /* Apply direction (W is negative) */
            if (tokens->tokens[11][0] == 'W' || tokens->tokens[11][0] == 'w') {
                gnss->magnetic_variation = -gnss->magnetic_variation;
            }
            gnss->magnetic_variation_direction = tokens->tokens[11][0];
            gnss->magnetic_variation_valid = true;
        }
    }

    /* Parse mode indicator (field 12) */
    if (tokens->token_count > 12 && !nmea_is_empty_token(tokens->tokens[12])) {
        gnss->mode_indicator = tokens->tokens[12][0];
    }

    return NMEA_OK;
}
#endif

#endif /* NMEA_MODULE_GNSS_ENABLED */
