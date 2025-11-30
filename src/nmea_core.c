/**
 * @file nmea_core.c
 * @brief Core NMEA parser implementation
 *
 * Implements the main parsing API, dispatch system, and state management.
 */

#include <string.h>

#include "modules/nmea_comm.h"
#include "modules/nmea_radar.h"
#include "modules/nmea_safety.h"
#include "modules/nmea_system.h"
#include "nmea.h"
#include "nmea_internal.h"

/* ========================================================================== */
/*                          DISPATCH TABLE                                    */
/* ========================================================================== */

/**
 * @brief Static dispatch table
 *
 * Maps sentence types (without talker IDs) to parser functions.
 * Talker IDs are validated separately at runtime.
 * Only includes enabled sentences.
 */
static const nmea_dispatch_entry_t g_dispatch_table[] = {
#if NMEA_MODULE_GNSS_ENABLED
#if NMEA_SENTENCE_GGA_ENABLED
    {"GGA", nmea_parse_gga, NMEA_MODULE_GNSS, 14},
#endif
#if NMEA_SENTENCE_RMC_ENABLED
    {"RMC", nmea_parse_rmc, NMEA_MODULE_GNSS, 12},
#endif
#if NMEA_SENTENCE_GLL_ENABLED
    {"GLL", nmea_parse_gll, NMEA_MODULE_GNSS, 7},
#endif
#if NMEA_SENTENCE_GSA_ENABLED
    {"GSA", nmea_parse_gsa, NMEA_MODULE_GNSS, 17},
#endif
#if NMEA_SENTENCE_GSV_ENABLED
    {"GSV", nmea_parse_gsv, NMEA_MODULE_GNSS, 4},
#endif
#if NMEA_SENTENCE_GST_ENABLED
    {"GST", nmea_parse_gst, NMEA_MODULE_GNSS, 9},
#endif
#if NMEA_SENTENCE_GNS_ENABLED
    {"GNS", nmea_parse_gns, NMEA_MODULE_GNSS, 13},
#endif
#if NMEA_SENTENCE_GBS_ENABLED
    {"GBS", nmea_parse_gbs, NMEA_MODULE_GNSS, 9},
#endif
#if NMEA_SENTENCE_GRS_ENABLED
    {"GRS", nmea_parse_grs, NMEA_MODULE_GNSS, 14},
#endif
#if NMEA_SENTENCE_GFA_ENABLED
    {"GFA", nmea_parse_gfa, NMEA_MODULE_GNSS, 14},
#endif
#if NMEA_SENTENCE_RMA_ENABLED
    {"RMA", nmea_parse_rma, NMEA_MODULE_GNSS, 12},
#endif
#if NMEA_SENTENCE_RMB_ENABLED
    {"RMB", nmea_parse_rmb, NMEA_MODULE_GNSS, 14},
#endif
#endif /* NMEA_MODULE_GNSS_ENABLED */

/* ====================================================================== */
/*                           SYSTEM MODULE                                */
/* ====================================================================== */

#if NMEA_MODULE_SYSTEM_ENABLED
#if NMEA_SENTENCE_HBT_ENABLED
    {"HBT", nmea_parse_hbt, NMEA_MODULE_SYSTEM, 4},
#endif
#if NMEA_SENTENCE_HSS_ENABLED
    {"HSS", nmea_parse_hss, NMEA_MODULE_SYSTEM, 4},
#endif
#if NMEA_SENTENCE_EVE_ENABLED
    {"EVE", nmea_parse_eve, NMEA_MODULE_SYSTEM, 4},
#endif
#if NMEA_SENTENCE_ETL_ENABLED
    {"ETL", nmea_parse_etl, NMEA_MODULE_SYSTEM, 7},
#endif
#if NMEA_SENTENCE_NRM_ENABLED
    {"NRM", nmea_parse_nrm, NMEA_MODULE_SYSTEM, 6},
#endif
#if NMEA_SENTENCE_NRX_ENABLED
    {"NRX", nmea_parse_nrx, NMEA_MODULE_SYSTEM, 14},
#endif
#if NMEA_SENTENCE_NSR_ENABLED
    {"NSR", nmea_parse_nsr, NMEA_MODULE_SYSTEM, 14},
#endif
#if NMEA_SENTENCE_POS_ENABLED
    {"POS", nmea_parse_pos, NMEA_MODULE_SYSTEM, 11},
#endif
#if NMEA_SENTENCE_MSK_ENABLED
    {"MSK", nmea_parse_msk, NMEA_MODULE_SYSTEM, 8},
#endif
#if NMEA_SENTENCE_MSS_ENABLED
    {"MSS", nmea_parse_mss, NMEA_MODULE_SYSTEM, 6},
#endif
#endif /* NMEA_MODULE_SYSTEM_ENABLED */
/*                          SENSOR MODULE                                 */
/* ====================================================================== */

#if NMEA_MODULE_SENSOR_ENABLED
#if NMEA_SENTENCE_DBT_ENABLED
    {"DBT", nmea_parse_dbt, NMEA_MODULE_SENSOR, 6},
#endif
#if NMEA_SENTENCE_DPT_ENABLED
    {"DPT", nmea_parse_dpt, NMEA_MODULE_SENSOR, 3},
#endif
#if NMEA_SENTENCE_MTW_ENABLED
    {"MTW", nmea_parse_mtw, NMEA_MODULE_SENSOR, 2},
#endif
#if NMEA_SENTENCE_MWD_ENABLED
    {"MWD", nmea_parse_mwd, NMEA_MODULE_SENSOR, 8},
#endif
#if NMEA_SENTENCE_MWV_ENABLED
    {"MWV", nmea_parse_mwv, NMEA_MODULE_SENSOR, 5},
#endif
#if NMEA_SENTENCE_VBW_ENABLED
    {"VBW", nmea_parse_vbw, NMEA_MODULE_SENSOR, 4},
#endif
#if NMEA_SENTENCE_VHW_ENABLED
    {"VHW", nmea_parse_vhw, NMEA_MODULE_SENSOR, 8},
#endif
#if NMEA_SENTENCE_VLW_ENABLED
    {"VLW", nmea_parse_vlw, NMEA_MODULE_SENSOR, 4},
#endif
#if NMEA_SENTENCE_VPW_ENABLED
    {"VPW", nmea_parse_vpw, NMEA_MODULE_SENSOR, 2},
#endif
#endif /* NMEA_MODULE_SENSOR_ENABLED */

/* ====================================================================== */
/*                         HEADING MODULE                                 */
/* ====================================================================== */

#if NMEA_MODULE_HEADING_ENABLED
#if NMEA_SENTENCE_HDG_ENABLED
    {"HDG", nmea_parse_hdg, NMEA_MODULE_HEADING, 5},
#endif
#if NMEA_SENTENCE_HDT_ENABLED
    {"HDT", nmea_parse_hdt, NMEA_MODULE_HEADING, 2},
#endif
#if NMEA_SENTENCE_ROT_ENABLED
    {"ROT", nmea_parse_rot, NMEA_MODULE_HEADING, 2},
#endif
#if NMEA_SENTENCE_VTG_ENABLED
    {"VTG", nmea_parse_vtg, NMEA_MODULE_HEADING, 9},
#endif
#if NMEA_SENTENCE_VDR_ENABLED
    {"VDR", nmea_parse_vdr, NMEA_MODULE_HEADING, 6},
#endif
#if NMEA_SENTENCE_OSD_ENABLED
    {"OSD", nmea_parse_osd, NMEA_MODULE_HEADING, 9},
#endif
#if NMEA_SENTENCE_HMR_ENABLED
    {"HMR", nmea_parse_hmr, NMEA_MODULE_HEADING, 17},
#endif
#if NMEA_SENTENCE_HTC_ENABLED
    {"HTC", nmea_parse_htc, NMEA_MODULE_HEADING, 14},
    {"HTD", nmea_parse_htc, NMEA_MODULE_HEADING, 17},
#endif
#endif /* NMEA_MODULE_HEADING_ENABLED */

/* ====================================================================== */
/*                       NAVIGATION MODULE                                */
/* ====================================================================== */

#if NMEA_MODULE_NAVIGATION_ENABLED
#if NMEA_SENTENCE_APB_ENABLED
    {"APB", nmea_parse_apb, NMEA_MODULE_NAVIGATION, 15},
#endif
#if NMEA_SENTENCE_BWC_ENABLED
    {"BWC", nmea_parse_bwc, NMEA_MODULE_NAVIGATION, 13},
#endif
#if NMEA_SENTENCE_BWR_ENABLED
    {"BWR", nmea_parse_bwr, NMEA_MODULE_NAVIGATION, 13},
#endif
#if NMEA_SENTENCE_BEC_ENABLED
    {"BEC", nmea_parse_bec, NMEA_MODULE_NAVIGATION, 12},
#endif
#if NMEA_SENTENCE_BOD_ENABLED
    {"BOD", nmea_parse_bod, NMEA_MODULE_NAVIGATION, 6},
#endif
#if NMEA_SENTENCE_BWW_ENABLED
    {"BWW", nmea_parse_bww, NMEA_MODULE_NAVIGATION, 6},
#endif
#if NMEA_SENTENCE_RTE_ENABLED
    {"RTE", nmea_parse_rte, NMEA_MODULE_NAVIGATION, 4},
#endif
#if NMEA_SENTENCE_AAM_ENABLED
    {"AAM", nmea_parse_aam, NMEA_MODULE_NAVIGATION, 5},
#endif
#endif /* NMEA_MODULE_NAVIGATION_ENABLED */

/* ====================================================================== */
/*                         WAYPOINT MODULE                                */
/* ====================================================================== */

#if NMEA_MODULE_WAYPOINT_ENABLED
#if NMEA_SENTENCE_WPL_ENABLED
    {"WPL", nmea_parse_wpl, NMEA_MODULE_WAYPOINT, 5},
#endif
#if NMEA_SENTENCE_WNC_ENABLED
    {"WNC", nmea_parse_wnc, NMEA_MODULE_WAYPOINT, 6},
#endif
#if NMEA_SENTENCE_WCV_ENABLED
    {"WCV", nmea_parse_wcv, NMEA_MODULE_WAYPOINT, 4},
#endif
#endif /* NMEA_MODULE_WAYPOINT_ENABLED */

/* ====================================================================== */
/*                           AIS MODULE                                   */
/* ====================================================================== */

#if NMEA_MODULE_AIS_ENABLED
#if NMEA_SENTENCE_VDM_ENABLED
    {"VDM", nmea_parse_vdm, NMEA_MODULE_AIS, 7},
#endif
#if NMEA_SENTENCE_VDO_ENABLED
    {"VDO", nmea_parse_vdo, NMEA_MODULE_AIS, 7},
#endif
#if NMEA_SENTENCE_ABK_ENABLED
    {"ABK", nmea_parse_abk, NMEA_MODULE_AIS, 6},
#endif
#if NMEA_SENTENCE_ABM_ENABLED
    {"ABM", nmea_parse_abm, NMEA_MODULE_AIS, 9},
#endif
#if NMEA_SENTENCE_BBM_ENABLED
    {"BBM", nmea_parse_bbm, NMEA_MODULE_AIS, 8},
#endif
#if NMEA_SENTENCE_AIR_ENABLED
    {"AIR", nmea_parse_air, NMEA_MODULE_AIS, 13},
#endif
#if NMEA_SENTENCE_ACA_ENABLED
    {"ACA", nmea_parse_aca, NMEA_MODULE_AIS, 20},
#endif
#if NMEA_SENTENCE_ACS_ENABLED
    {"ACS", nmea_parse_acs, NMEA_MODULE_AIS, 7},
#endif
#if NMEA_SENTENCE_LRF_ENABLED
    {"LRF", nmea_parse_lrf, NMEA_MODULE_AIS, 6},
#endif
#if NMEA_SENTENCE_LRI_ENABLED
    {"LRI", nmea_parse_lri, NMEA_MODULE_AIS, 13},
#endif
#if NMEA_SENTENCE_SSD_ENABLED
    {"SSD", nmea_parse_ssd, NMEA_MODULE_AIS, 9},
#endif
#if NMEA_SENTENCE_VSD_ENABLED
    {"VSD", nmea_parse_vsd, NMEA_MODULE_AIS, 10},
#endif
#endif /* NMEA_MODULE_AIS_ENABLED */

/* ====================================================================== */
/*                           COMM MODULE                                  */
/* ====================================================================== */

#if NMEA_MODULE_COMM_ENABLED
#if NMEA_SENTENCE_DSC_ENABLED
    {"DSC", nmea_parse_dsc, NMEA_MODULE_COMM, 11},
#endif
#if NMEA_SENTENCE_DSE_ENABLED
    {"DSE", nmea_parse_dse, NMEA_MODULE_COMM, 5},
#endif
#if NMEA_SENTENCE_VER_ENABLED
    {"VER", nmea_parse_ver, NMEA_MODULE_COMM, 10},
#endif
#if NMEA_SENTENCE_UID_ENABLED
    {"UID", nmea_parse_uid, NMEA_MODULE_COMM, 2},
#endif
#if NMEA_SENTENCE_SPW_ENABLED
    {"SPW", nmea_parse_spw, NMEA_MODULE_COMM, 4},
#endif
#if NMEA_SENTENCE_TUT_ENABLED
    {"TUT", nmea_parse_tut, NMEA_MODULE_COMM, 6},
#endif
#if NMEA_SENTENCE_TXT_ENABLED
    {"TXT", nmea_parse_txt, NMEA_MODULE_COMM, 4},
#endif
#if NMEA_SENTENCE_NAK_ENABLED
    {"NAK", nmea_parse_nak, NMEA_MODULE_COMM, 5},
#endif
#if NMEA_SENTENCE_RLM_ENABLED
    {"RLM", nmea_parse_rlm, NMEA_MODULE_COMM, 4},
#endif
#if NMEA_SENTENCE_STN_ENABLED
    {"STN", nmea_parse_stn, NMEA_MODULE_COMM, 1},
#endif
#endif /* NMEA_MODULE_COMM_ENABLED */

/* ====================================================================== */
/*                           SAFETY MODULE                                */
/* ====================================================================== */

#if NMEA_MODULE_SAFETY_ENABLED
#if NMEA_SENTENCE_ALR_ENABLED
    {"ALR", nmea_parse_alr, NMEA_MODULE_SAFETY, 6},
#endif
#if NMEA_SENTENCE_ALF_ENABLED
    {"ALF", nmea_parse_alf, NMEA_MODULE_SAFETY, 14},
#endif
#if NMEA_SENTENCE_ALC_ENABLED
    {"ALC", nmea_parse_alc, NMEA_MODULE_SAFETY, 7},
#endif
#if NMEA_SENTENCE_ALA_ENABLED
    {"ALA", nmea_parse_ala, NMEA_MODULE_SAFETY, 9},
#endif
#if NMEA_SENTENCE_AKD_ENABLED
    {"AKD", nmea_parse_akd, NMEA_MODULE_SAFETY, 9},
#endif
#if NMEA_SENTENCE_ACN_ENABLED
    {"ACN", nmea_parse_acn, NMEA_MODULE_SAFETY, 6},
#endif
#if NMEA_SENTENCE_ACK_ENABLED
    {"ACK", nmea_parse_ack, NMEA_MODULE_SAFETY, 2},
#endif
#if NMEA_SENTENCE_ARC_ENABLED
    {"ARC", nmea_parse_arc, NMEA_MODULE_SAFETY, 6},
#endif
#if NMEA_SENTENCE_FIR_ENABLED
    {"FIR", nmea_parse_fir, NMEA_MODULE_SAFETY, 10},
#endif
#if NMEA_SENTENCE_MOB_ENABLED
    {"MOB", nmea_parse_mob, NMEA_MODULE_SAFETY, 15},
#endif
#if NMEA_SENTENCE_DOR_ENABLED
    {"DOR", nmea_parse_dor, NMEA_MODULE_SAFETY, 10},
#endif
#endif /* NMEA_MODULE_SAFETY_ENABLED */

/* ====================================================================== */
/*                           RADAR MODULE                                 */
/* ====================================================================== */

#if NMEA_MODULE_RADAR_ENABLED
#if NMEA_SENTENCE_RSD_ENABLED
    {"RSD", nmea_parse_rsd, NMEA_MODULE_RADAR, 14},
#endif
#if NMEA_SENTENCE_TTM_ENABLED
    {"TTM", nmea_parse_ttm, NMEA_MODULE_RADAR, 16},
#endif
#if NMEA_SENTENCE_TLL_ENABLED
    {"TLL", nmea_parse_tll, NMEA_MODULE_RADAR, 9},
#endif
#if NMEA_SENTENCE_TLB_ENABLED
    {"TLB", nmea_parse_tlb, NMEA_MODULE_RADAR, 2},
#endif
#if NMEA_SENTENCE_TTD_ENABLED
    {"TTD", nmea_parse_ttd, NMEA_MODULE_RADAR, 6},
#endif
#endif /* NMEA_MODULE_RADAR_ENABLED */

/* ====================================================================== */
/*                          MISC MODULE                                   */
/* ====================================================================== */

#if NMEA_MODULE_MISC_ENABLED
#if NMEA_SENTENCE_XDR_ENABLED
    {"XDR", nmea_parse_xdr, NMEA_MODULE_MISC, 4},
#endif
#if NMEA_SENTENCE_XTE_ENABLED
    {"XTE", nmea_parse_xte, NMEA_MODULE_MISC, 6},
#endif
#if NMEA_SENTENCE_XTR_ENABLED
    {"XTR", nmea_parse_xtr, NMEA_MODULE_MISC, 2},
#endif
#if NMEA_SENTENCE_ZDA_ENABLED
    {"ZDA", nmea_parse_zda, NMEA_MODULE_MISC, 6},
#endif
#if NMEA_SENTENCE_ZDL_ENABLED
    {"ZDL", nmea_parse_zdl, NMEA_MODULE_MISC, 3},
#endif
#if NMEA_SENTENCE_ZFO_ENABLED
    {"ZFO", nmea_parse_zfo, NMEA_MODULE_MISC, 3},
#endif
#if NMEA_SENTENCE_ZTG_ENABLED
    {"ZTG", nmea_parse_ztg, NMEA_MODULE_MISC, 3},
#endif
#if NMEA_SENTENCE_DTM_ENABLED
    {"DTM", nmea_parse_dtm, NMEA_MODULE_MISC, 8},
#endif
#if NMEA_SENTENCE_CUR_ENABLED
    {"CUR", nmea_parse_cur, NMEA_MODULE_MISC, 11},
#endif
#if NMEA_SENTENCE_FSI_ENABLED
    {"FSI", nmea_parse_fsi, NMEA_MODULE_MISC, 5},
#endif
#if NMEA_SENTENCE_GEN_ENABLED
    {"GEN", nmea_parse_gen, NMEA_MODULE_MISC, 4},
#endif
#if NMEA_SENTENCE_RPM_ENABLED
    {"RPM", nmea_parse_rpm, NMEA_MODULE_MISC, 5},
#endif
#if NMEA_SENTENCE_SFI_ENABLED
    {"SFI", nmea_parse_sfi, NMEA_MODULE_MISC, 4},
#endif
#if NMEA_SENTENCE_DDC_ENABLED
    {"DDC", nmea_parse_ddc, NMEA_MODULE_MISC, 4},
#endif
#if NMEA_SENTENCE_EPV_ENABLED
    {"EPV", nmea_parse_epv, NMEA_MODULE_MISC, 5},
#endif
#if NMEA_SENTENCE_TRL_ENABLED
    {"TRL", nmea_parse_trl, NMEA_MODULE_MISC, 9},
#endif
#if NMEA_SENTENCE_WAT_ENABLED
    {"WAT", nmea_parse_wat, NMEA_MODULE_MISC, 10},
#endif
#endif /* NMEA_MODULE_MISC_ENABLED */

/* ====================================================================== */
/*                        ATTITUDE MODULE                                 */
/* ====================================================================== */

#if NMEA_MODULE_ATTITUDE_ENABLED
#if NMEA_SENTENCE_HRM_ENABLED
    {"HRM", nmea_parse_hrm, NMEA_MODULE_ATTITUDE, 11},
#endif
#if NMEA_SENTENCE_PRC_ENABLED
    {"PRC", nmea_parse_prc, NMEA_MODULE_ATTITUDE, 9},
#endif
#if NMEA_SENTENCE_TRC_ENABLED
    {"TRC", nmea_parse_trc, NMEA_MODULE_ATTITUDE, 9},
#endif
#if NMEA_SENTENCE_TRD_ENABLED
    {"TRD", nmea_parse_trd, NMEA_MODULE_ATTITUDE, 7},
#endif
#endif /* NMEA_MODULE_ATTITUDE_ENABLED */

    /* Sentinel entry */
    {NULL, NULL, 0, 0}};

/**
 * @brief Get dispatch table and size
 */
const nmea_dispatch_entry_t* nmea_get_dispatch_table(size_t* table_size) {
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
nmea_result_t nmea_init(nmea_context_t* ctx, const nmea_config_t* config) {
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
void nmea_cleanup(nmea_context_t* ctx) {
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
nmea_result_t nmea_parse(nmea_context_t* ctx, const char* sentence,
                         size_t length, void* buffer, size_t buffer_size) {
  if (ctx == NULL || sentence == NULL || buffer == NULL) {
    return NMEA_ERROR_NULL_PARAM;
  }

  if (!ctx->initialized) {
    return NMEA_ERROR_NOT_INIT;
  }

  if (buffer_size < NMEA_MAX_SENTENCE_LENGTH) {
    nmea_report_error(ctx, NMEA_ERROR_TYPE_BUFFER, NMEA_ERROR_BUFFER_TOO_SMALL,
                      "Buffer size insufficient for parsing");
    return NMEA_ERROR_BUFFER_TOO_SMALL;
  }

  /* Validate sentence length */
  if (length == 0 || length > NMEA_MAX_SENTENCE_LENGTH) {
    nmea_report_error(ctx, NMEA_ERROR_TYPE_SYNTAX, NMEA_ERROR_INVALID_SENTENCE,
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
  ((char*)buffer)[length] = '\0';

  /* Tokenize */
  nmea_tokens_t tokens;
  nmea_result_t result = nmea_tokenize((char*)buffer, buffer_size, &tokens);
  if (result != NMEA_OK) {
    nmea_report_error(ctx, NMEA_ERROR_TYPE_SYNTAX, result,
                      "Tokenization failed");
    return result;
  }

  /* Validate minimum token count */
  if (tokens.token_count == 0) {
    return NMEA_ERROR_INVALID_SENTENCE;
  }

  /* First token is sentence ID (e.g., "GPGGA", "GNGLL", "AIVDM") */
  const char* sentence_id = tokens.tokens[0];

  /* Extract talker ID and sentence type */
  char talker_id[3];
  char sentence_type[4];
  result = nmea_extract_sentence_parts(sentence_id, talker_id, sentence_type);
  if (result != NMEA_OK) {
    nmea_report_error(ctx, NMEA_ERROR_TYPE_SYNTAX, NMEA_ERROR_INVALID_SENTENCE,
                      "Invalid sentence ID format");
    return NMEA_ERROR_INVALID_SENTENCE;
  }

  /* Validate talker ID */
  nmea_talker_id_t talker = nmea_validate_talker_id(talker_id);
  if (talker == NMEA_TALKER_UNKNOWN) {
    nmea_report_error(ctx, NMEA_ERROR_TYPE_SYNTAX, NMEA_ERROR_INVALID_SENTENCE,
                      "Invalid or unknown talker ID");
    return NMEA_ERROR_INVALID_SENTENCE;
  }

  /* Find matching parser in dispatch table using sentence type only */
  size_t table_size;
  const nmea_dispatch_entry_t* table = nmea_get_dispatch_table(&table_size);

  for (size_t i = 0; i < table_size; i++) {
    if (strcmp(sentence_type, table[i].sentence_id) == 0) {
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
        nmea_report_error(ctx, NMEA_ERROR_TYPE_SEMANTIC, result,
                          "Sentence parsing failed");
      }
      return result;
    }
  }

  /* Sentence type not found */
  nmea_report_error(ctx, NMEA_ERROR_TYPE_SYNTAX, NMEA_ERROR_UNKNOWN_SENTENCE,
                    "Unknown sentence type");
  return NMEA_ERROR_UNKNOWN_SENTENCE;
}

/* ========================================================================== */
/*                       MODULE DATA ACCESSORS                                */
/* ========================================================================== */

#if NMEA_MODULE_GNSS_ENABLED
nmea_result_t nmea_get_gnss_data(const nmea_context_t* ctx,
                                 nmea_gnss_state_t* data) {
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
nmea_result_t nmea_get_ais_data(const nmea_context_t* ctx,
                                nmea_ais_state_t* data) {
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
nmea_result_t nmea_get_navigation_data(const nmea_context_t* ctx,
                                       nmea_navigation_state_t* data) {
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
nmea_result_t nmea_get_waypoint_data(const nmea_context_t* ctx,
                                     nmea_waypoint_state_t* data) {
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
nmea_result_t nmea_get_heading_data(const nmea_context_t* ctx,
                                    nmea_heading_state_t* data) {
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
nmea_result_t nmea_get_sensor_data(const nmea_context_t* ctx,
                                   nmea_sensor_state_t* data) {
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
nmea_result_t nmea_get_radar_data(const nmea_context_t* ctx,
                                  nmea_radar_state_t* data) {
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
nmea_result_t nmea_get_safety_data(const nmea_context_t* ctx,
                                   nmea_safety_state_t* data) {
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
nmea_result_t nmea_get_comm_data(const nmea_context_t* ctx,
                                 nmea_comm_state_t* data) {
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
nmea_result_t nmea_get_system_data(const nmea_context_t* ctx,
                                   nmea_system_state_t* data) {
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
nmea_result_t nmea_get_attitude_data(const nmea_context_t* ctx,
                                     nmea_attitude_state_t* data) {
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
nmea_result_t nmea_get_misc_data(const nmea_context_t* ctx,
                                 nmea_misc_state_t* data) {
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

bool nmea_is_sentence_enabled(const nmea_context_t* ctx,
                              const char* sentence_id) {
  if (ctx == NULL || sentence_id == NULL) {
    return false;
  }

  /* Extract sentence type from full sentence ID */
  char talker_id[3];
  char sentence_type[4];
  if (nmea_extract_sentence_parts(sentence_id, talker_id, sentence_type) !=
      NMEA_OK) {
    return false;
  }

  /* Validate talker ID */
  if (nmea_validate_talker_id(talker_id) == NMEA_TALKER_UNKNOWN) {
    return false;
  }

  /* Check if sentence type exists and its module is enabled */
  size_t table_size;
  const nmea_dispatch_entry_t* table = nmea_get_dispatch_table(&table_size);

  for (size_t i = 0; i < table_size; i++) {
    if (strcmp(sentence_type, table[i].sentence_id) == 0) {
      uint32_t module_bit = (1U << table[i].module);
      return (ctx->config.enabled_modules & module_bit) != 0;
    }
  }

  return false;
}

bool nmea_is_module_enabled(const nmea_context_t* ctx, nmea_module_t module) {
  if (ctx == NULL || module >= NMEA_MODULE_COUNT) {
    return false;
  }

  uint32_t module_bit = (1U << module);
  return (ctx->config.enabled_modules & module_bit) != 0;
}
