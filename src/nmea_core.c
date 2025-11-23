/**
 * @file nmea_core.c
 * @brief Core NMEA parser implementation
 *
 * Implements the main parsing API, dispatch system, and state management.
 */

#include "nmea.h"
#include "nmea_internal.h"
#include <string.h>

/* ========================================================================== */
/*                          DISPATCH TABLE                                    */
/* ========================================================================== */

/**
 * @brief Static dispatch table
 *
 * Maps sentence IDs to parser functions. Only includes enabled sentences.
 */
static const nmea_dispatch_entry_t g_dispatch_table[] = {
#if NMEA_MODULE_GNSS_ENABLED
#if NMEA_SENTENCE_GGA_ENABLED
    {"GPGGA", nmea_parse_gga, NMEA_MODULE_GNSS, 14},
    {"GNGGA", nmea_parse_gga, NMEA_MODULE_GNSS, 14},
#endif
#if NMEA_SENTENCE_RMC_ENABLED
    {"GPRMC", nmea_parse_rmc, NMEA_MODULE_GNSS, 12},
    {"GNRMC", nmea_parse_rmc, NMEA_MODULE_GNSS, 12},
#endif
#if NMEA_SENTENCE_GLL_ENABLED
    {"GPGLL", nmea_parse_gll, NMEA_MODULE_GNSS, 7},
    {"GNGLL", nmea_parse_gll, NMEA_MODULE_GNSS, 7},
#endif
#if NMEA_SENTENCE_GSA_ENABLED
    {"GPGSA", nmea_parse_gsa, NMEA_MODULE_GNSS, 17},
    {"GNGSA", nmea_parse_gsa, NMEA_MODULE_GNSS, 17},
#endif
#if NMEA_SENTENCE_GSV_ENABLED
    {"GPGSV", nmea_parse_gsv, NMEA_MODULE_GNSS, 4},
    {"GNGSV", nmea_parse_gsv, NMEA_MODULE_GNSS, 4},
#endif
#if NMEA_SENTENCE_GST_ENABLED
    {"GPGST", nmea_parse_gst, NMEA_MODULE_GNSS, 9},
    {"GNGST", nmea_parse_gst, NMEA_MODULE_GNSS, 9},
#endif
#if NMEA_SENTENCE_GNS_ENABLED
    {"GPGNS", nmea_parse_gns, NMEA_MODULE_GNSS, 13},
    {"GNGNS", nmea_parse_gns, NMEA_MODULE_GNSS, 13},
#endif
#if NMEA_SENTENCE_GBS_ENABLED
    {"GPGBS", nmea_parse_gbs, NMEA_MODULE_GNSS, 9},
    {"GNGBS", nmea_parse_gbs, NMEA_MODULE_GNSS, 9},
#endif
#if NMEA_SENTENCE_GRS_ENABLED
    {"GPGRS", nmea_parse_grs, NMEA_MODULE_GNSS, 14},
    {"GNGRS", nmea_parse_grs, NMEA_MODULE_GNSS, 14},
#endif
#if NMEA_SENTENCE_GFA_ENABLED
    {"GPGFA", nmea_parse_gfa, NMEA_MODULE_GNSS, 14},
    {"GNGFA", nmea_parse_gfa, NMEA_MODULE_GNSS, 14},
#endif
#if NMEA_SENTENCE_RMA_ENABLED
    {"GPRMA", nmea_parse_rma, NMEA_MODULE_GNSS, 12},
    {"GNRMA", nmea_parse_rma, NMEA_MODULE_GNSS, 12},
#endif
#if NMEA_SENTENCE_RMB_ENABLED
    {"GPRMB", nmea_parse_rmb, NMEA_MODULE_GNSS, 14},
    {"GNRMB", nmea_parse_rmb, NMEA_MODULE_GNSS, 14},
#endif
#endif /* NMEA_MODULE_GNSS_ENABLED */

    /* ====================================================================== */
    /*                          SENSOR MODULE                                 */
    /* ====================================================================== */

#if NMEA_MODULE_SENSOR_ENABLED
#if NMEA_SENTENCE_DBT_ENABLED
    {"SDDBT", nmea_parse_dbt, NMEA_MODULE_SENSOR, 6},
    {"IIDBT", nmea_parse_dbt, NMEA_MODULE_SENSOR, 6},
#endif
#if NMEA_SENTENCE_DPT_ENABLED
    {"SDDPT", nmea_parse_dpt, NMEA_MODULE_SENSOR, 3},
    {"IIDPT", nmea_parse_dpt, NMEA_MODULE_SENSOR, 3},
#endif
#if NMEA_SENTENCE_MTW_ENABLED
    {"IIMTW", nmea_parse_mtw, NMEA_MODULE_SENSOR, 2},
    {"SDMTW", nmea_parse_mtw, NMEA_MODULE_SENSOR, 2},
#endif
#if NMEA_SENTENCE_MWD_ENABLED
    {"IIMWD", nmea_parse_mwd, NMEA_MODULE_SENSOR, 8},
    {"WIMWD", nmea_parse_mwd, NMEA_MODULE_SENSOR, 8},
#endif
#if NMEA_SENTENCE_MWV_ENABLED
    {"IIMWV", nmea_parse_mwv, NMEA_MODULE_SENSOR, 5},
    {"WIMWV", nmea_parse_mwv, NMEA_MODULE_SENSOR, 5},
#endif
#if NMEA_SENTENCE_VBW_ENABLED
    {"IIVBW", nmea_parse_vbw, NMEA_MODULE_SENSOR, 10},
    {"VWVBW", nmea_parse_vbw, NMEA_MODULE_SENSOR, 10},
#endif
#if NMEA_SENTENCE_VHW_ENABLED
    {"IIVHW", nmea_parse_vhw, NMEA_MODULE_SENSOR, 8},
    {"VWVHW", nmea_parse_vhw, NMEA_MODULE_SENSOR, 8},
#endif
#if NMEA_SENTENCE_VLW_ENABLED
    {"IIVLW", nmea_parse_vlw, NMEA_MODULE_SENSOR, 8},
    {"VWVLW", nmea_parse_vlw, NMEA_MODULE_SENSOR, 8},
#endif
#if NMEA_SENTENCE_VPW_ENABLED
    {"IIVPW", nmea_parse_vpw, NMEA_MODULE_SENSOR, 4},
    {"VWVPW", nmea_parse_vpw, NMEA_MODULE_SENSOR, 4},
#endif
#endif /* NMEA_MODULE_SENSOR_ENABLED */

    /* ====================================================================== */
    /*                         HEADING MODULE                                 */
    /* ====================================================================== */

#if NMEA_MODULE_HEADING_ENABLED
#if NMEA_SENTENCE_HDG_ENABLED
    {"HEHDG", nmea_parse_hdg, NMEA_MODULE_HEADING, 5},
    {"IIHDG", nmea_parse_hdg, NMEA_MODULE_HEADING, 5},
#endif
#if NMEA_SENTENCE_HDT_ENABLED
    {"HEHDT", nmea_parse_hdt, NMEA_MODULE_HEADING, 2},
    {"IIHDT", nmea_parse_hdt, NMEA_MODULE_HEADING, 2},
#endif
#if NMEA_SENTENCE_ROT_ENABLED
    {"HEROT", nmea_parse_rot, NMEA_MODULE_HEADING, 2},
    {"IIROT", nmea_parse_rot, NMEA_MODULE_HEADING, 2},
#endif
#if NMEA_SENTENCE_VTG_ENABLED
    {"GPVTG", nmea_parse_vtg, NMEA_MODULE_HEADING, 9},
    {"GNVTG", nmea_parse_vtg, NMEA_MODULE_HEADING, 9},
#endif
#if NMEA_SENTENCE_VDR_ENABLED
    {"IIVDR", nmea_parse_vdr, NMEA_MODULE_HEADING, 6},
    {"VWVDR", nmea_parse_vdr, NMEA_MODULE_HEADING, 6},
#endif
#if NMEA_SENTENCE_OSD_ENABLED
    {"RAOSD", nmea_parse_osd, NMEA_MODULE_HEADING, 9},
    {"IIOSD", nmea_parse_osd, NMEA_MODULE_HEADING, 9},
#endif
#if NMEA_SENTENCE_HMR_ENABLED
    {"HEHMR", nmea_parse_hmr, NMEA_MODULE_HEADING, 17},
    {"IIHMR", nmea_parse_hmr, NMEA_MODULE_HEADING, 17},
#endif
#if NMEA_SENTENCE_HTC_ENABLED
    {"HEHTC", nmea_parse_htc, NMEA_MODULE_HEADING, 14},
    {"IIHTC", nmea_parse_htc, NMEA_MODULE_HEADING, 14},
    {"HEHTD", nmea_parse_htc, NMEA_MODULE_HEADING, 17},
    {"IIHTD", nmea_parse_htc, NMEA_MODULE_HEADING, 17},
#endif
#endif /* NMEA_MODULE_HEADING_ENABLED */

    /* ====================================================================== */
    /*                       NAVIGATION MODULE                                */
    /* ====================================================================== */

#if NMEA_MODULE_NAVIGATION_ENABLED
#if NMEA_SENTENCE_APB_ENABLED
    {"GPAPB", nmea_parse_apb, NMEA_MODULE_NAVIGATION, 15},
    {"GNAPB", nmea_parse_apb, NMEA_MODULE_NAVIGATION, 15},
#endif
#if NMEA_SENTENCE_BWC_ENABLED
    {"GPBWC", nmea_parse_bwc, NMEA_MODULE_NAVIGATION, 13},
    {"GNBWC", nmea_parse_bwc, NMEA_MODULE_NAVIGATION, 13},
#endif
#if NMEA_SENTENCE_BWR_ENABLED
    {"GPBWR", nmea_parse_bwr, NMEA_MODULE_NAVIGATION, 13},
    {"GNBWR", nmea_parse_bwr, NMEA_MODULE_NAVIGATION, 13},
#endif
#if NMEA_SENTENCE_BEC_ENABLED
    {"GPBEC", nmea_parse_bec, NMEA_MODULE_NAVIGATION, 12},
    {"GNBEC", nmea_parse_bec, NMEA_MODULE_NAVIGATION, 12},
#endif
#if NMEA_SENTENCE_BOD_ENABLED
    {"GPBOD", nmea_parse_bod, NMEA_MODULE_NAVIGATION, 6},
    {"GNBOD", nmea_parse_bod, NMEA_MODULE_NAVIGATION, 6},
#endif
#if NMEA_SENTENCE_BWW_ENABLED
    {"GPBWW", nmea_parse_bww, NMEA_MODULE_NAVIGATION, 6},
    {"GNBWW", nmea_parse_bww, NMEA_MODULE_NAVIGATION, 6},
#endif
#if NMEA_SENTENCE_RTE_ENABLED
    {"GPRTE", nmea_parse_rte, NMEA_MODULE_NAVIGATION, 4},
    {"GNRTE", nmea_parse_rte, NMEA_MODULE_NAVIGATION, 4},
#endif
#if NMEA_SENTENCE_AAM_ENABLED
    {"GPAAM", nmea_parse_aam, NMEA_MODULE_NAVIGATION, 5},
    {"GNAAM", nmea_parse_aam, NMEA_MODULE_NAVIGATION, 5},
#endif
#endif /* NMEA_MODULE_NAVIGATION_ENABLED */

    /* ====================================================================== */
    /*                         WAYPOINT MODULE                                */
    /* ====================================================================== */

#if NMEA_MODULE_WAYPOINT_ENABLED
#if NMEA_SENTENCE_WPL_ENABLED
    {"GPWPL", nmea_parse_wpl, NMEA_MODULE_WAYPOINT, 5},
    {"GNWPL", nmea_parse_wpl, NMEA_MODULE_WAYPOINT, 5},
#endif
#if NMEA_SENTENCE_WNC_ENABLED
    {"GPWNC", nmea_parse_wnc, NMEA_MODULE_WAYPOINT, 6},
    {"GNWNC", nmea_parse_wnc, NMEA_MODULE_WAYPOINT, 6},
#endif
#if NMEA_SENTENCE_WCV_ENABLED
    {"GPWCV", nmea_parse_wcv, NMEA_MODULE_WAYPOINT, 4},
    {"GNWCV", nmea_parse_wcv, NMEA_MODULE_WAYPOINT, 4},
#endif
#endif /* NMEA_MODULE_WAYPOINT_ENABLED */

    /* ====================================================================== */
    /*                           AIS MODULE                                   */
    /* ====================================================================== */

#if NMEA_MODULE_AIS_ENABLED
#if NMEA_SENTENCE_VDM_ENABLED
    {"AIVDM", nmea_parse_vdm, NMEA_MODULE_AIS, 6},
#endif
#if NMEA_SENTENCE_VDO_ENABLED
    {"AIVDO", nmea_parse_vdo, NMEA_MODULE_AIS, 6},
#endif
#if NMEA_SENTENCE_ABK_ENABLED
    {"AIABK", nmea_parse_abk, NMEA_MODULE_AIS, 5},
#endif
#if NMEA_SENTENCE_ABM_ENABLED
    {"AIABM", nmea_parse_abm, NMEA_MODULE_AIS, 8},
#endif
#if NMEA_SENTENCE_BBM_ENABLED
    {"AIBBM", nmea_parse_bbm, NMEA_MODULE_AIS, 7},
#endif
#if NMEA_SENTENCE_AIR_ENABLED
    {"AIAIR", nmea_parse_air, NMEA_MODULE_AIS, 12},
#endif
#if NMEA_SENTENCE_ACA_ENABLED
    {"AIACA", nmea_parse_aca, NMEA_MODULE_AIS, 19},
#endif
#if NMEA_SENTENCE_ACS_ENABLED
    {"AIACS", nmea_parse_acs, NMEA_MODULE_AIS, 6},
#endif
#if NMEA_SENTENCE_LRF_ENABLED
    {"AILRF", nmea_parse_lrf, NMEA_MODULE_AIS, 5},
#endif
#if NMEA_SENTENCE_LRI_ENABLED
    {"AILRI", nmea_parse_lri, NMEA_MODULE_AIS, 12},
#endif
#if NMEA_SENTENCE_SSD_ENABLED
    {"AISSD", nmea_parse_ssd, NMEA_MODULE_AIS, 8},
#endif
#if NMEA_SENTENCE_VSD_ENABLED
    {"AIVSD", nmea_parse_vsd, NMEA_MODULE_AIS, 9},
#endif
#endif /* NMEA_MODULE_AIS_ENABLED */

    /* Additional modules will be added as they are implemented */

    /* Sentinel entry */
    {NULL, NULL, 0, 0}
};

/**
 * @brief Get dispatch table and size
 */
const nmea_dispatch_entry_t *nmea_get_dispatch_table(size_t *table_size)
{
    if (table_size != NULL) {
        /* Count entries (exclude sentinel) */
        size_t count = 0;
        while (g_dispatch_table[count].sentence_id != NULL) {
            count++;
        }
        *table_size = count;
    }
    return g_dispatch_table;
}

/* ========================================================================== */
/*                          CONTEXT MANAGEMENT                                */
/* ========================================================================== */

/**
 * @brief Initialize NMEA parser context
 */
nmea_result_t nmea_init(nmea_context_t *ctx, const nmea_config_t *config)
{
    if (ctx == NULL || config == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (ctx->initialized) {
        return NMEA_ERROR_ALREADY_INIT;
    }

    /* Clear context */
    memset(ctx, 0, sizeof(nmea_context_t));

    /* Copy configuration */
    memcpy(&ctx->config, config, sizeof(nmea_config_t));

    /* Mark as initialized */
    ctx->initialized = true;

    return NMEA_OK;
}

/**
 * @brief Cleanup parser context
 */
void nmea_cleanup(nmea_context_t *ctx)
{
    if (ctx == NULL) {
        return;
    }

    /* Clear all state */
    memset(ctx, 0, sizeof(nmea_context_t));
}

/* ========================================================================== */
/*                          MAIN PARSE FUNCTION                               */
/* ========================================================================== */

/**
 * @brief Parse a single NMEA sentence
 */
nmea_result_t nmea_parse(
    nmea_context_t *ctx,
    const char *sentence,
    size_t length,
    void *buffer,
    size_t buffer_size)
{
    if (ctx == NULL || sentence == NULL || buffer == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_NOT_INIT;
    }

    if (buffer_size < NMEA_MAX_SENTENCE_LENGTH) {
        nmea_report_error(ctx, NMEA_ERROR_TYPE_BUFFER,
                         NMEA_ERROR_BUFFER_TOO_SMALL,
                         "Buffer size insufficient for parsing");
        return NMEA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Validate sentence length */
    if (length == 0 || length > NMEA_MAX_SENTENCE_LENGTH) {
        nmea_report_error(ctx, NMEA_ERROR_TYPE_SYNTAX,
                         NMEA_ERROR_INVALID_SENTENCE,
                         "Sentence length invalid");
        return NMEA_ERROR_INVALID_SENTENCE;
    }

    /* Validate checksum if enabled */
    if (ctx->config.validate_checksums) {
        if (!nmea_validate_checksum(sentence, length)) {
            nmea_report_error(ctx, NMEA_ERROR_TYPE_CHECKSUM,
                             NMEA_ERROR_CHECKSUM_FAILED,
                             "Checksum validation failed");
            return NMEA_ERROR_CHECKSUM_FAILED;
        }
    }

    /* Copy sentence to buffer for tokenization */
    if (length >= buffer_size) {
        return NMEA_ERROR_BUFFER_TOO_SMALL;
    }
    memcpy(buffer, sentence, length);
    ((char *)buffer)[length] = '\0';

    /* Tokenize */
    nmea_tokens_t tokens;
    nmea_result_t result = nmea_tokenize((char *)buffer, buffer_size, &tokens);
    if (result != NMEA_OK) {
        nmea_report_error(ctx, NMEA_ERROR_TYPE_SYNTAX, result,
                         "Tokenization failed");
        return result;
    }

    /* Validate minimum token count */
    if (tokens.token_count == 0) {
        return NMEA_ERROR_INVALID_SENTENCE;
    }

    /* First token is sentence ID */
    const char *sentence_id = tokens.tokens[0];

    /* Find matching parser in dispatch table */
    size_t table_size;
    const nmea_dispatch_entry_t *table = nmea_get_dispatch_table(&table_size);

    for (size_t i = 0; i < table_size; i++) {
        if (strcmp(sentence_id, table[i].sentence_id) == 0) {
            /* Check if module is enabled */
            uint32_t module_bit = (1U << table[i].module);
            if ((ctx->config.enabled_modules & module_bit) == 0) {
                nmea_report_error(ctx, NMEA_ERROR_TYPE_CONFIG,
                                 NMEA_ERROR_MODULE_DISABLED,
                                 "Module disabled in configuration");
                return NMEA_ERROR_MODULE_DISABLED;
            }

            /* Check minimum field count */
            if (tokens.token_count < table[i].min_fields) {
                nmea_report_error(ctx, NMEA_ERROR_TYPE_SYNTAX,
                                 NMEA_ERROR_TOO_FEW_FIELDS,
                                 "Too few fields for sentence type");
                return NMEA_ERROR_TOO_FEW_FIELDS;
            }

            /* Invoke parser */
            result = table[i].parser(ctx, &tokens);
            if (result != NMEA_OK) {
                nmea_report_error(ctx, NMEA_ERROR_TYPE_SEMANTIC,
                                 result, "Sentence parsing failed");
            }
            return result;
        }
    }

    /* Sentence type not found */
    nmea_report_error(ctx, NMEA_ERROR_TYPE_SYNTAX,
                     NMEA_ERROR_UNKNOWN_SENTENCE,
                     "Unknown sentence type");
    return NMEA_ERROR_UNKNOWN_SENTENCE;
}

/* ========================================================================== */
/*                       MODULE DATA ACCESSORS                                */
/* ========================================================================== */

#if NMEA_MODULE_GNSS_ENABLED
nmea_result_t nmea_get_gnss_data(const nmea_context_t *ctx, nmea_gnss_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    /* Copy GNSS state */
    memcpy(data, &ctx->gnss, sizeof(nmea_gnss_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_AIS_ENABLED
nmea_result_t nmea_get_ais_data(const nmea_context_t *ctx, nmea_ais_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->ais, sizeof(nmea_ais_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_NAVIGATION_ENABLED
nmea_result_t nmea_get_navigation_data(const nmea_context_t *ctx, nmea_navigation_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->navigation, sizeof(nmea_navigation_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_WAYPOINT_ENABLED
nmea_result_t nmea_get_waypoint_data(const nmea_context_t *ctx, nmea_waypoint_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->waypoint, sizeof(nmea_waypoint_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_HEADING_ENABLED
nmea_result_t nmea_get_heading_data(const nmea_context_t *ctx, nmea_heading_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->heading, sizeof(nmea_heading_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_SENSOR_ENABLED
nmea_result_t nmea_get_sensor_data(const nmea_context_t *ctx, nmea_sensor_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->sensor, sizeof(nmea_sensor_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_RADAR_ENABLED
nmea_result_t nmea_get_radar_data(const nmea_context_t *ctx, nmea_radar_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->radar, sizeof(nmea_radar_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_SAFETY_ENABLED
nmea_result_t nmea_get_safety_data(const nmea_context_t *ctx, nmea_safety_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->safety, sizeof(nmea_safety_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_COMM_ENABLED
nmea_result_t nmea_get_comm_data(const nmea_context_t *ctx, nmea_comm_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->comm, sizeof(nmea_comm_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_SYSTEM_ENABLED
nmea_result_t nmea_get_system_data(const nmea_context_t *ctx, nmea_system_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->system, sizeof(nmea_system_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_ATTITUDE_ENABLED
nmea_result_t nmea_get_attitude_data(const nmea_context_t *ctx, nmea_attitude_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->attitude, sizeof(nmea_attitude_state_t));
    return NMEA_OK;
}
#endif

#if NMEA_MODULE_MISC_ENABLED
nmea_result_t nmea_get_misc_data(const nmea_context_t *ctx, nmea_misc_state_t *data)
{
    if (ctx == NULL || data == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (!ctx->initialized) {
        return NMEA_ERROR_INVALID_CONTEXT;
    }

    memcpy(data, &ctx->misc, sizeof(nmea_misc_state_t));
    return NMEA_OK;
}
#endif

/* ========================================================================== */
/*                          UTILITY FUNCTIONS                                 */
/* ========================================================================== */

bool nmea_is_sentence_enabled(const nmea_context_t *ctx, const char *sentence_id)
{
    if (ctx == NULL || sentence_id == NULL) {
        return false;
    }

    size_t table_size;
    const nmea_dispatch_entry_t *table = nmea_get_dispatch_table(&table_size);

    for (size_t i = 0; i < table_size; i++) {
        if (strcmp(sentence_id, table[i].sentence_id) == 0) {
            uint32_t module_bit = (1U << table[i].module);
            return (ctx->config.enabled_modules & module_bit) != 0;
        }
    }

    return false;
}

bool nmea_is_module_enabled(const nmea_context_t *ctx, nmea_module_t module)
{
    if (ctx == NULL || module >= NMEA_MODULE_COUNT) {
        return false;
    }

    uint32_t module_bit = (1U << module);
    return (ctx->config.enabled_modules & module_bit) != 0;
}
