/**
 * @file nmea_misc.c
 * @brief Miscellaneous sensors and data module implementation
 *
 * Implements parsers for miscellaneous sentences including transducer
 * measurements, cross-track error, time and date, datum reference,
 * water current, frequency information, revolutions, display control,
 * equipment properties, transmitter log, and water level detection.
 */

#include "modules/nmea_misc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nmea_internal.h"

#if NMEA_MODULE_MISC_ENABLED

/* ========================================================================== */
/*                      XDR - TRANSDUCER MEASUREMENT                          */
/* ========================================================================== */

#if NMEA_SENTENCE_XDR_ENABLED
/**
 * @brief Parse XDR - Transducer Measurement
 *
 * Format: $--XDR,a,x.x,a,c--c,...*hh
 * Example: $--XDR,C,19.5,C,TempAir*hh
 *
 * Fields (repeating sets of 4):
 * 0: Sentence ID (XDR)
 * 1: Transducer type (C/A/D/F/N/P/R/T/H/U/I/S/G)
 * 2: Measurement data
 * 3: Units of measure (C/D/M/H/etc)
 * 4: Transducer ID
 * ... (additional transducer sets)
 *
 * Note: This implementation parses only the first transducer set
 */
nmea_result_t nmea_parse_xdr(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse first transducer type (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    strncpy(misc->xdr_type, tokens->tokens[1], sizeof(misc->xdr_type) - 1);
  }

  /* Parse measurement data (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_float(tokens->tokens[2], &misc->xdr_measurement);
  }

  /* Parse units (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    strncpy(misc->xdr_units, tokens->tokens[3], sizeof(misc->xdr_units) - 1);
  }

  /* Parse transducer ID (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    char temp[sizeof(misc->xdr_id)];
    strncpy(temp, tokens->tokens[4], sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    nmea_strip_checksum(temp);
    strncpy(misc->xdr_id, temp, sizeof(misc->xdr_id) - 1);
    misc->xdr_id[sizeof(misc->xdr_id) - 1] = '\0';
  }

  misc->xdr_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_XDR_ENABLED */

/* ========================================================================== */
/*                    XTE - CROSS-TRACK ERROR, MEASURED                       */
/* ========================================================================== */

#if NMEA_SENTENCE_XTE_ENABLED
/**
 * @brief Parse XTE - Cross-Track Error, Measured
 *
 * Format: $--XTE,A,A,x.x,a,N,a*hh
 * Example: $--XTE,A,A,0.67,L,N,D*hh
 *
 * Fields:
 * 0: Sentence ID (XTE)
 * 1: Status 1 (A=valid, V=invalid)
 * 2: Status 2 (A=valid, V=invalid)
 * 3: Cross-track error magnitude (nm)
 * 4: Direction to steer (L/R)
 * 5: Units (N=nautical miles)
 * 6: Mode indicator (A/D/E/M/S/N)
 */
nmea_result_t nmea_parse_xte(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse status 1 (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    misc->xte_status1 = tokens->tokens[1][0];
  }

  /* Parse status 2 (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    misc->xte_status2 = tokens->tokens[2][0];
  }

  /* Parse magnitude (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_float(tokens->tokens[3], &misc->xte_magnitude);
  }

  /* Parse direction (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    misc->xte_direction = tokens->tokens[4][0];
  }

  /* Parse mode indicator (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    misc->xte_mode = tokens->tokens[6][0];
  }

  misc->xte_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_XTE_ENABLED */

/* ========================================================================== */
/*                 XTR - CROSS-TRACK ERROR, DEAD RECKONING                    */
/* ========================================================================== */

#if NMEA_SENTENCE_XTR_ENABLED
/**
 * @brief Parse XTR - Cross-Track Error, Dead Reckoning
 *
 * Format: $--XTR,x.x,a,N*hh
 * Example: $--XTR,0.45,L,N*hh
 *
 * Fields:
 * 0: Sentence ID (XTR)
 * 1: Magnitude of cross-track error (nm)
 * 2: Direction to steer (L/R)
 * 3: Units (N=nautical miles)
 */
nmea_result_t nmea_parse_xtr(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse magnitude (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_float(tokens->tokens[1], &misc->xtr_magnitude);
  }

  /* Parse direction (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    misc->xtr_direction = tokens->tokens[2][0];
  }

  misc->xtr_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_XTR_ENABLED */

/* ========================================================================== */
/*                            ZDA - TIME AND DATE                             */
/* ========================================================================== */

#if NMEA_SENTENCE_ZDA_ENABLED
/**
 * @brief Parse ZDA - Time and Date
 *
 * Format: $--ZDA,hhmmss.ss,xx,xx,xxxx,xx,xx*hh
 * Example: $--ZDA,234500,09,06,2025,-12,45*hh
 *
 * Fields:
 * 0: Sentence ID (ZDA)
 * 1: UTC time (hhmmss.ss)
 * 2: Day (01-31 UTC)
 * 3: Month (01-12 UTC)
 * 4: Year (UTC)
 * 5: Local zone hours (00 to ±13)
 * 6: Local zone minutes (00 to +59)
 */
nmea_result_t nmea_parse_zda(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse UTC time (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_time(tokens->tokens[1], &misc->zda_time);
  }

  /* Parse day (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    int32_t day = 0;
    if (nmea_parse_int(tokens->tokens[2], &day) == NMEA_OK) {
      misc->zda_date.day = (uint8_t)day;
    }
  }

  /* Parse month (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    int32_t month = 0;
    if (nmea_parse_int(tokens->tokens[3], &month) == NMEA_OK) {
      misc->zda_date.month = (uint8_t)month;
    }
  }

  /* Parse year (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    int32_t year = 0;
    if (nmea_parse_int(tokens->tokens[4], &year) == NMEA_OK) {
      misc->zda_date.year = (uint16_t)year;
    }
  }

  /* Parse local zone hours (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    int32_t hours = 0;
    if (nmea_parse_int(tokens->tokens[5], &hours) == NMEA_OK) {
      misc->zda_local_zone_hours = (int8_t)hours;
    }
  }

  /* Parse local zone minutes (field 6) */
  if (tokens->token_count >= 7 && !nmea_is_empty_token(tokens->tokens[6])) {
    /* Strip checksum if present on last field before numeric parse */
    char temp[16];
    strncpy(temp, tokens->tokens[6], sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    nmea_strip_checksum(temp);
    int32_t minutes = 0;
    if (nmea_parse_int(temp, &minutes) == NMEA_OK) {
      misc->zda_local_zone_minutes = (int8_t)minutes;
    }
  }

  misc->zda_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ZDA_ENABLED */

/* ========================================================================== */
/*               ZDL - TIME AND DISTANCE TO VARIABLE POINT                    */
/* ========================================================================== */

#if NMEA_SENTENCE_ZDL_ENABLED
/**
 * @brief Parse ZDL - Time and Distance to Variable Point
 *
 * Format: $--ZDL,hhmmss.ss,x.x,a*hh
 * Example: $--ZDL,123045,5.2,W*hh
 *
 * Fields:
 * 0: Sentence ID (ZDL)
 * 1: Time to point (hhmmss.ss)
 * 2: Distance to point (nm)
 * 3: Point type (W/R/T/C)
 */
nmea_result_t nmea_parse_zdl(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse time to point (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_time(tokens->tokens[1], &misc->zdl_time_to_point);
  }

  /* Parse distance (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_float(tokens->tokens[2], &misc->zdl_distance);
  }

  /* Parse point type (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    misc->zdl_point_type = tokens->tokens[3][0];
  }

  misc->zdl_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ZDL_ENABLED */

/* ========================================================================== */
/*              ZFO - UTC AND TIME FROM ORIGIN WAYPOINT                       */
/* ========================================================================== */

#if NMEA_SENTENCE_ZFO_ENABLED
/**
 * @brief Parse ZFO - UTC and Time from Origin Waypoint
 *
 * Format: $--ZFO,hhmmss.ss,hhmmss.ss,c--c*hh
 * Example: $--ZFO,120530,003015,ORIGIN*hh
 *
 * Fields:
 * 0: Sentence ID (ZFO)
 * 1: UTC of observation
 * 2: Elapsed time from origin waypoint
 * 3: Origin waypoint ID
 */
nmea_result_t nmea_parse_zfo(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse UTC (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_time(tokens->tokens[1], &misc->zfo_utc);
  }

  /* Parse elapsed time (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_time(tokens->tokens[2], &misc->zfo_elapsed_time);
  }

  /* Parse origin waypoint ID (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    char temp[sizeof(misc->zfo_origin_id)];
    strncpy(temp, tokens->tokens[3], sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    nmea_strip_checksum(temp);
    strncpy(misc->zfo_origin_id, temp, sizeof(misc->zfo_origin_id) - 1);
    misc->zfo_origin_id[sizeof(misc->zfo_origin_id) - 1] = '\0';
  }

  misc->zfo_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ZFO_ENABLED */

/* ========================================================================== */
/*           ZTG - UTC AND TIME TO DESTINATION WAYPOINT                       */
/* ========================================================================== */

#if NMEA_SENTENCE_ZTG_ENABLED
/**
 * @brief Parse ZTG - UTC and Time to Destination Waypoint
 *
 * Format: $--ZTG,hhmmss.ss,hhmmss.ss,c--c*hh
 * Example: $--ZTG,120530,015230,DEST*hh
 *
 * Fields:
 * 0: Sentence ID (ZTG)
 * 1: UTC of observation
 * 2: Time remaining to destination
 * 3: Destination waypoint ID
 */
nmea_result_t nmea_parse_ztg(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse UTC (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    nmea_parse_time(tokens->tokens[1], &misc->ztg_utc);
  }

  /* Parse time to go (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_time(tokens->tokens[2], &misc->ztg_time_to_go);
  }

  /* Parse destination waypoint ID (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    char temp[sizeof(misc->ztg_dest_id)];
    strncpy(temp, tokens->tokens[3], sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    nmea_strip_checksum(temp);
    strncpy(misc->ztg_dest_id, temp, sizeof(misc->ztg_dest_id) - 1);
    misc->ztg_dest_id[sizeof(misc->ztg_dest_id) - 1] = '\0';
  }

  misc->ztg_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ZTG_ENABLED */

/* ========================================================================== */
/*                         DTM - DATUM REFERENCE                              */
/* ========================================================================== */

#if NMEA_SENTENCE_DTM_ENABLED
/**
 * @brief Parse DTM - Datum Reference
 *
 * Format: $--DTM,ccc,a,x.x,a,x.x,a,x.x,ccc*hh
 * Example: $--DTM,W84,,0.0,N,0.0,E,0.0,W84*hh
 *
 * Fields:
 * 0: Sentence ID (DTM)
 * 1: Local datum code
 * 2: Local datum subdivision code
 * 3: Latitude offset (minutes)
 * 4: N/S
 * 5: Longitude offset (minutes)
 * 6: E/W
 * 7: Altitude offset (meters)
 * 8: Reference datum
 */
nmea_result_t nmea_parse_dtm(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse local datum (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    strncpy(misc->dtm_local_datum, tokens->tokens[1],
            sizeof(misc->dtm_local_datum) - 1);
  }

  /* Parse subdivision (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    misc->dtm_subdivision = tokens->tokens[2][0];
  }

  /* Parse latitude offset (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_float(tokens->tokens[3], &misc->dtm_lat_offset);
  }

  /* Parse latitude direction (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    misc->dtm_lat_offset_dir = tokens->tokens[4][0];
  }

  /* Parse longitude offset (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    nmea_parse_float(tokens->tokens[5], &misc->dtm_lon_offset);
  }

  /* Parse longitude direction (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    misc->dtm_lon_offset_dir = tokens->tokens[6][0];
  }

  /* Parse altitude offset (field 7) */
  if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
    nmea_parse_float(tokens->tokens[7], &misc->dtm_altitude_offset);
  }

  /* Parse reference datum (field 8) */
  if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
    strncpy(misc->dtm_ref_datum, tokens->tokens[8],
            sizeof(misc->dtm_ref_datum) - 1);
  }

  misc->dtm_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_DTM_ENABLED */

/* ========================================================================== */
/*                        CUR - WATER CURRENT LAYER                           */
/* ========================================================================== */

#if NMEA_SENTENCE_CUR_ENABLED
/**
 * @brief Parse CUR - Water Current Layer
 *
 * Format: $--CUR,A,x,x,x.x,x.x,a,x.x,x.x,x.x,a,a*hh
 * Example: $--CUR,A,1,2,15.5,215.3,T,1.5,10.0,180.5,T,P*hh
 *
 * Fields:
 * 0: Sentence ID (CUR)
 * 1: Data validity (A/V)
 * 2: Data set number (0-9)
 * 3: Layer number
 * 4: Current depth (meters)
 * 5: Current direction (degrees)
 * 6: Direction reference (T/R)
 * 7: Current speed (knots)
 * 8: Reference layer depth (meters)
 * 9: Heading
 * 10: Heading reference (T/M)
 * 11: Speed reference (P/W/B)
 */
nmea_result_t nmea_parse_cur(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse validity (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    misc->cur_validity = tokens->tokens[1][0];
  }

  /* Parse data set number (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    int32_t set_num = 0;
    if (nmea_parse_int(tokens->tokens[2], &set_num) == NMEA_OK) {
      misc->cur_data_set_number = (uint8_t)set_num;
    }
  }

  /* Parse layer number (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    int32_t layer = 0;
    if (nmea_parse_int(tokens->tokens[3], &layer) == NMEA_OK) {
      misc->cur_layer_number = (uint8_t)layer;
    }
  }

  /* Parse depth (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    nmea_parse_float(tokens->tokens[4], &misc->cur_depth);
  }

  /* Parse direction (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    nmea_parse_float(tokens->tokens[5], &misc->cur_direction);
  }

  /* Parse direction reference (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    misc->cur_direction_ref = tokens->tokens[6][0];
  }

  /* Parse speed (field 7) */
  if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
    nmea_parse_float(tokens->tokens[7], &misc->cur_speed);
  }

  /* Parse reference layer depth (field 8) */
  if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
    nmea_parse_float(tokens->tokens[8], &misc->cur_ref_layer_depth);
  }

  /* Parse heading (field 9) */
  if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
    nmea_parse_float(tokens->tokens[9], &misc->cur_heading);
  }

  /* Parse heading reference (field 10) */
  if (tokens->token_count > 10 && !nmea_is_empty_token(tokens->tokens[10])) {
    misc->cur_heading_ref = tokens->tokens[10][0];
  }

  /* Parse speed reference (field 11) */
  if (tokens->token_count > 11 && !nmea_is_empty_token(tokens->tokens[11])) {
    misc->cur_speed_ref = tokens->tokens[11][0];
  }

  misc->cur_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_CUR_ENABLED */

/* ========================================================================== */
/*                   FSI - FREQUENCY SET INFORMATION                          */
/* ========================================================================== */

#if NMEA_SENTENCE_FSI_ENABLED
/**
 * @brief Parse FSI - Frequency Set Information
 *
 * Format: $--FSI,xxxxxx,xxxxxx,c,x,a*hh
 * Example: $--FSI,156800,156800,D,5,R*hh
 *
 * Fields:
 * 0: Sentence ID (FSI)
 * 1: Transmitting frequency (kHz)
 * 2: Receiving frequency (kHz)
 * 3: Mode of operation
 * 4: Power level (0-9)
 * 5: Status flag (R/C)
 */
nmea_result_t nmea_parse_fsi(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse transmit frequency (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    strncpy(misc->fsi_tx_freq, tokens->tokens[1],
            sizeof(misc->fsi_tx_freq) - 1);
  }

  /* Parse receive frequency (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    strncpy(misc->fsi_rx_freq, tokens->tokens[2],
            sizeof(misc->fsi_rx_freq) - 1);
  }

  /* Parse mode (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    misc->fsi_mode = tokens->tokens[3][0];
  }

  /* Parse power level (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    int32_t power = 0;
    if (nmea_parse_int(tokens->tokens[4], &power) == NMEA_OK) {
      misc->fsi_power_level = (uint8_t)power;
    }
  }

  /* Parse status flag (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    misc->fsi_status_flag = tokens->tokens[5][0];
  }

  misc->fsi_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_FSI_ENABLED */

/* ========================================================================== */
/*                       GEN - GENERIC BINARY MESSAGE                         */
/* ========================================================================== */

#if NMEA_SENTENCE_GEN_ENABLED
/**
 * @brief Parse GEN - Generic Binary Message
 *
 * Format: $--GEN,...*hh
 * Example: $--GEN,data*hh
 *
 * Note: GEN is a generic sentence with variable format
 * This is a minimal placeholder implementation
 */
nmea_result_t nmea_parse_gen(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  /* GEN is a generic sentence - minimal parsing */
  (void)tokens;

  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_GEN_ENABLED */

/* ========================================================================== */
/*                            RPM - REVOLUTIONS                               */
/* ========================================================================== */

#if NMEA_SENTENCE_RPM_ENABLED
/**
 * @brief Parse RPM - Revolutions
 *
 * Format: $--RPM,a,x,x.x,x.x,A*hh
 * Example: $--RPM,S,1,1500.5,45.2,A*hh
 *
 * Fields:
 * 0: Sentence ID (RPM)
 * 1: Source (S=shaft, E=engine)
 * 2: Engine/shaft number
 * 3: Speed (RPM)
 * 4: Propeller pitch (% of maximum)
 * 5: Status (A=valid, V=invalid)
 */
nmea_result_t nmea_parse_rpm(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse source (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    misc->rpm_source = tokens->tokens[1][0];
  }

  /* Parse engine/shaft number (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    int32_t num = 0;
    if (nmea_parse_int(tokens->tokens[2], &num) == NMEA_OK) {
      misc->rpm_number = (uint8_t)num;
    }
  }

  /* Parse speed (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    nmea_parse_float(tokens->tokens[3], &misc->rpm_speed);
  }

  /* Parse pitch (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    nmea_parse_float(tokens->tokens[4], &misc->rpm_pitch);
  }

  /* Parse status (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    misc->rpm_status = tokens->tokens[5][0];
  }

  misc->rpm_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_RPM_ENABLED */

/* ========================================================================== */
/*                  SFI - SCANNING FREQUENCY INFORMATION                      */
/* ========================================================================== */

#if NMEA_SENTENCE_SFI_ENABLED
/**
 * @brief Parse SFI - Scanning Frequency Information
 *
 * Format: $--SFI,x,x,xxxxxx,c,...*hh
 * Example: $--SFI,2,1,156800,D,157000,D*hh
 *
 * Fields:
 * 0: Sentence ID (SFI)
 * 1: Total number of sentences
 * 2: Sentence number
 * 3-n: Frequency/mode pairs (up to 6)
 */
nmea_result_t nmea_parse_sfi(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse total sentences (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    int32_t total = 0;
    if (nmea_parse_int(tokens->tokens[1], &total) == NMEA_OK) {
      misc->sfi_total_sentences = (uint8_t)total;
    }
  }

  /* Parse sentence number (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    int32_t num = 0;
    if (nmea_parse_int(tokens->tokens[2], &num) == NMEA_OK) {
      misc->sfi_sentence_number = (uint8_t)num;
    }
  }

  /* Parse frequency/mode pairs (fields 3+) */
  int freq_idx = 0;
  for (int i = 3; i < (int)tokens->token_count && freq_idx < 6;
       i += 2, freq_idx++) {
    /* Parse frequency */
    if (i < (int)tokens->token_count &&
        !nmea_is_empty_token(tokens->tokens[i])) {
      strncpy(misc->sfi_frequencies[freq_idx], tokens->tokens[i],
              sizeof(misc->sfi_frequencies[freq_idx]) - 1);
    }

    /* Parse mode */
    if ((i + 1) < (int)tokens->token_count &&
        !nmea_is_empty_token(tokens->tokens[i + 1])) {
      misc->sfi_modes[freq_idx] = tokens->tokens[i + 1][0];
    }
  }

  misc->sfi_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_SFI_ENABLED */

/* ========================================================================== */
/*                    DDC - DISPLAY DIMMING CONTROL                           */
/* ========================================================================== */

#if NMEA_SENTENCE_DDC_ENABLED
/**
 * @brief Parse DDC - Display Dimming Control
 *
 * Format: $--DDC,a,xx,a,a*hh
 * Example: $--DDC,D,75,N,R*hh
 *
 * Fields:
 * 0: Sentence ID (DDC)
 * 1: Dimming preset (D/K/N/O)
 * 2: Brightness (0-99)
 * 3: Color palette (D/K/N/O)
 * 4: Status flag (R/C)
 */
nmea_result_t nmea_parse_ddc(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse dimming preset (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    misc->ddc_dimming_preset = tokens->tokens[1][0];
  }

  /* Parse brightness (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    int32_t brightness = 0;
    if (nmea_parse_int(tokens->tokens[2], &brightness) == NMEA_OK) {
      misc->ddc_brightness = (uint8_t)brightness;
    }
  }

  /* Parse color palette (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    misc->ddc_color_palette = tokens->tokens[3][0];
  }

  /* Parse status flag (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    misc->ddc_status_flag = tokens->tokens[4][0];
  }

  misc->ddc_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_DDC_ENABLED */

/* ========================================================================== */
/*                   EPV - EQUIPMENT PROPERTY VALUE                           */
/* ========================================================================== */

#if NMEA_SENTENCE_EPV_ENABLED
/**
 * @brief Parse EPV - Equipment Property Value
 *
 * Format: $--EPV,a,ccc,c--c,xxxx,c--c*hh
 * Example: $--EPV,R,GPS,ID12345,100,FIRMWARE*hh
 *
 * Fields:
 * 0: Sentence ID (EPV)
 * 1: Status flag (R/C)
 * 2: Equipment type
 * 3: Unique identifier
 * 4: Property identifier
 * 5: Property value
 */
nmea_result_t nmea_parse_epv(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse status flag (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    misc->epv_status_flag = tokens->tokens[1][0];
  }

  /* Parse equipment type (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    strncpy(misc->epv_equipment_type, tokens->tokens[2],
            sizeof(misc->epv_equipment_type) - 1);
  }

  /* Parse unique ID (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    strncpy(misc->epv_unique_id, tokens->tokens[3],
            sizeof(misc->epv_unique_id) - 1);
  }

  /* Parse property ID (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    int32_t prop_id = 0;
    if (nmea_parse_int(tokens->tokens[4], &prop_id) == NMEA_OK) {
      misc->epv_property_id = (uint16_t)prop_id;
    }
  }

  /* Parse property value (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    char temp[sizeof(misc->epv_property_value)];
    strncpy(temp, tokens->tokens[5], sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    nmea_strip_checksum(temp);
    strncpy(misc->epv_property_value, temp, sizeof(misc->epv_property_value) - 1);
    misc->epv_property_value[sizeof(misc->epv_property_value) - 1] = '\0';
  }

  misc->epv_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_EPV_ENABLED */

/* ========================================================================== */
/*                TRL - TRANSMITTER NON-FUNCTIONING LOG                       */
/* ========================================================================== */

#if NMEA_SENTENCE_TRL_ENABLED
/**
 * @brief Parse TRL - Transmitter Non-functioning Log
 *
 * Format: $--TRL,xx,xx,xxx,xxxxxx,hhmmss.ss,xxxxxx,hhmmss.ss,x*hh
 * Example: $--TRL,10,5,125,240615,120530,240615,143025,3*hh
 *
 * Fields:
 * 0: Sentence ID (TRL)
 * 1: Total log entries
 * 2: Log entry number
 * 3: Sequential message ID
 * 4: Switch off date (DDMMYY)
 * 5: Switch off time (UTC)
 * 6: Switch on date (DDMMYY)
 * 7: Switch on time (UTC)
 * 8: Reason code
 */
nmea_result_t nmea_parse_trl(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse total entries (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    int32_t total = 0;
    if (nmea_parse_int(tokens->tokens[1], &total) == NMEA_OK) {
      misc->trl_total_entries = (uint8_t)total;
    }
  }

  /* Parse entry number (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    int32_t entry = 0;
    if (nmea_parse_int(tokens->tokens[2], &entry) == NMEA_OK) {
      misc->trl_entry_number = (uint8_t)entry;
    }
  }

  /* Parse message ID (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    int32_t msg_id = 0;
    if (nmea_parse_int(tokens->tokens[3], &msg_id) == NMEA_OK) {
      misc->trl_message_id = (uint8_t)msg_id;
    }
  }

  /* Parse switch off date (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    strncpy(misc->trl_switch_off_date, tokens->tokens[4],
            sizeof(misc->trl_switch_off_date) - 1);
  }

  /* Parse switch off time (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    nmea_parse_time(tokens->tokens[5], &misc->trl_switch_off_time);
  }

  /* Parse switch on date (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    strncpy(misc->trl_switch_on_date, tokens->tokens[6],
            sizeof(misc->trl_switch_on_date) - 1);
  }

  /* Parse switch on time (field 7) */
  if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
    nmea_parse_time(tokens->tokens[7], &misc->trl_switch_on_time);
  }

  /* Parse reason code (field 8) */
  if (tokens->token_count >= 9 && !nmea_is_empty_token(tokens->tokens[8])) {
    /* Last field may contain checksum */
    char temp[8];
    strncpy(temp, tokens->tokens[8], sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    nmea_strip_checksum(temp);
    int32_t reason = 0;
    if (nmea_parse_int(temp, &reason) == NMEA_OK) {
      misc->trl_reason_code = (uint8_t)reason;
    }
  }

  misc->trl_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_TRL_ENABLED */

/* ========================================================================== */
/*                      WAT - WATER LEVEL DETECTION                           */
/* ========================================================================== */

#if NMEA_SENTENCE_WAT_ENABLED
/**
 * @brief Parse WAT - Water Level Detection
 *
 * Format: $--WAT,a,hhmmss.ss,ccc,ccc,ccc,xxxx,a,a,c--c*hh
 * Example: $--WAT,S,120530,FIR,ENG,RM1,1234,H,O,WATER_DETECTED*hh
 *
 * Fields:
 * 0: Sentence ID (WAT)
 * 1: Message type (S/E/F)
 * 2: Timestamp
 * 3: Alarm system type
 * 4: First location indicator
 * 5: Second location indicator
 * 6: Detection point number
 * 7: Alarm condition (N/H/J/L/K/X)
 * 8: Override (O/N)
 * 9: Message description
 */
nmea_result_t nmea_parse_wat(nmea_context_t* ctx, const nmea_tokens_t* tokens) {
  if (ctx == NULL || tokens == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  nmea_misc_state_t* misc = &ctx->misc;

  /* Parse message type (field 1) */
  if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
    misc->wat_message_type = tokens->tokens[1][0];
  }

  /* Parse timestamp (field 2) */
  if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
    nmea_parse_time(tokens->tokens[2], &misc->wat_timestamp);
  }

  /* Parse alarm system type (field 3) */
  if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
    strncpy(misc->wat_alarm_type, tokens->tokens[3],
            sizeof(misc->wat_alarm_type) - 1);
  }

  /* Parse location 1 (field 4) */
  if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
    strncpy(misc->wat_location1, tokens->tokens[4],
            sizeof(misc->wat_location1));
  }

  /* Parse location 2 (field 5) */
  if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
    strncpy(misc->wat_location2, tokens->tokens[5],
            sizeof(misc->wat_location2));
  }

  /* Parse detection point (field 6) */
  if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
    int32_t point = 0;
    if (nmea_parse_int(tokens->tokens[6], &point) == NMEA_OK) {
      misc->wat_detection_point = (uint16_t)point;
    }
  }

  /* Parse alarm condition (field 7) */
  if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
    misc->wat_alarm_condition = tokens->tokens[7][0];
  }

  /* Parse override (field 8) */
  if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
    misc->wat_override = tokens->tokens[8][0];
  }

  /* Parse description (field 9) */
  if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
    char temp[sizeof(misc->wat_description)];
    strncpy(temp, tokens->tokens[9], sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    nmea_strip_checksum(temp);
    strncpy(misc->wat_description, temp, sizeof(misc->wat_description) - 1);
    misc->wat_description[sizeof(misc->wat_description) - 1] = '\0';
  }

  misc->wat_valid = true;
  return NMEA_OK;
}
#endif /* NMEA_SENTENCE_WAT_ENABLED */

#endif /* NMEA_MODULE_MISC_ENABLED */
