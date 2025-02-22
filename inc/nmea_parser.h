#ifndef NMEA_PARSER_H
#define NMEA_PARSER_H

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Define a POSIX-like ssize_t if not already defined.
#ifndef _SSIZE_T_DEFINED
typedef int ssize_t;
#define _SSIZE_T_DEFINED
#endif

// -----------------------------------------------------------------------------
// Enable / Disable Sentence Parsing to Reduce Code Size
// -----------------------------------------------------------------------------
/** CFG_SENTENCE_ENABLED_AAM - Waypoint arrival alarm */
#define CFG_SENTENCE_ENABLED_AAM true
/** CFG_SENTENCE_ENABLED_ABK - AIS addressed and binary broadcast
 * acknowledgement */
#define CFG_SENTENCE_ENABLED_ABK true
/** CFG_SENTENCE_ENABLED_ABM - AIS addressed binary & safety related message */
#define CFG_SENTENCE_ENABLED_ABM true
/** CFG_SENTENCE_ENABLED_ACA - AIS channel assignment message */
#define CFG_SENTENCE_ENABLED_ACA true
/** CFG_SENTENCE_ENABLED_ACK - Acknowledge alarm */
#define CFG_SENTENCE_ENABLED_ACK true
/** CFG_SENTENCE_ENABLED_ACN - Alert command */
#define CFG_SENTENCE_ENABLED_ACN true
/** CFG_SENTENCE_ENABLED_ACS - AIS channel management information source */
#define CFG_SENTENCE_ENABLED_ACS true
/** CFG_SENTENCE_ENABLED_AIR - AIS interrogation request */
#define CFG_SENTENCE_ENABLED_AIR true
/** CFG_SENTENCE_ENABLED_AKD - Acknowledge detail alarm condition */
#define CFG_SENTENCE_ENABLED_AKD true
/** CFG_SENTENCE_ENABLED_ALA - Report detailed alarm condition */
#define CFG_SENTENCE_ENABLED_ALA true
/** CFG_SENTENCE_ENABLED_ALC - Cyclic alert list */
#define CFG_SENTENCE_ENABLED_ALC true
/** CFG_SENTENCE_ENABLED_ALF - Alert sentence */
#define CFG_SENTENCE_ENABLED_ALF true
/** CFG_SENTENCE_ENABLED_ALR - Set alarm state */
#define CFG_SENTENCE_ENABLED_ALR true
/** CFG_SENTENCE_ENABLED_APB - Heading/track control (autopilot) sentence B */
#define CFG_SENTENCE_ENABLED_APB true
/** CFG_SENTENCE_ENABLED_ARC - Alert command refused */
#define CFG_SENTENCE_ENABLED_ARC true
/** CFG_SENTENCE_ENABLED_BBM - AIS broadcast binary message */
#define CFG_SENTENCE_ENABLED_BBM true
/** CFG_SENTENCE_ENABLED_BEC - Bearing & distance to waypoint: Dead reckoning */
#define CFG_SENTENCE_ENABLED_BEC true
/** CFG_SENTENCE_ENABLED_BOD - Bearing origin to destination */
#define CFG_SENTENCE_ENABLED_BOD true
/** CFG_SENTENCE_ENABLED_BWC - Bearing and distance to waypoint: Great circle */
#define CFG_SENTENCE_ENABLED_BWC true
/** CFG_SENTENCE_ENABLED_BWR - Bearing and distance to waypoint: Rhumb line */
#define CFG_SENTENCE_ENABLED_BWR true
/** CFG_SENTENCE_ENABLED_BWW - Bearing waypoint to waypoint */
#define CFG_SENTENCE_ENABLED_BWW true
/** CFG_SENTENCE_ENABLED_CUR - Water current layer: Multi-layer water current */
#define CFG_SENTENCE_ENABLED_CUR true
/** CFG_SENTENCE_ENABLED_DBT - Depth below transducer */
#define CFG_SENTENCE_ENABLED_DBT true
/** CFG_SENTENCE_ENABLED_DDC - Display dimming control */
#define CFG_SENTENCE_ENABLED_DDC true
/** CFG_SENTENCE_ENABLED_DOR - Door status detection */
#define CFG_SENTENCE_ENABLED_DOR true
/** CFG_SENTENCE_ENABLED_DPT - Depth */
#define CFG_SENTENCE_ENABLED_DPT true
/** CFG_SENTENCE_ENABLED_DSC - Digital selective calling information */
#define CFG_SENTENCE_ENABLED_DSC true
/** CFG_SENTENCE_ENABLED_DSE - Expanded digital selective calling */
#define CFG_SENTENCE_ENABLED_DSE true
/** CFG_SENTENCE_ENABLED_DTM - Datum reference */
#define CFG_SENTENCE_ENABLED_DTM true
/** CFG_SENTENCE_ENABLED_EPV - Command or report equipment property value */
#define CFG_SENTENCE_ENABLED_EPV true
/** CFG_SENTENCE_ENABLED_ETL - Engine telegraph operation status */
#define CFG_SENTENCE_ENABLED_ETL true
/** CFG_SENTENCE_ENABLED_EVE - General event message */
#define CFG_SENTENCE_ENABLED_EVE true
/** CFG_SENTENCE_ENABLED_FIR - Fire detection */
#define CFG_SENTENCE_ENABLED_FIR true
/** CFG_SENTENCE_ENABLED_FSI - Frequency set information */
#define CFG_SENTENCE_ENABLED_FSI true
/** CFG_SENTENCE_ENABLED_GBS - GNSS satellite fault detection */
#define CFG_SENTENCE_ENABLED_GBS true
/** CFG_SENTENCE_ENABLED_GEN - Generic binary information */
#define CFG_SENTENCE_ENABLED_GEN true
/** CFG_SENTENCE_ENABLED_GFA - GNSS fix accuracy and integrity */
#define CFG_SENTENCE_ENABLED_GFA true
/** CFG_SENTENCE_ENABLED_GGA - Global positioning system (GPS) fix data */
#define CFG_SENTENCE_ENABLED_GGA true
/** CFG_SENTENCE_ENABLED_GLL - Geographic position – Latitude/longitude */
#define CFG_SENTENCE_ENABLED_GLL true
/** CFG_SENTENCE_ENABLED_GNS - GNSS fix data */
#define CFG_SENTENCE_ENABLED_GNS true
/** CFG_SENTENCE_ENABLED_GRS - GNSS range residuals */
#define CFG_SENTENCE_ENABLED_GRS true
/** CFG_SENTENCE_ENABLED_GSA - GNSS DOP and active satellites */
#define CFG_SENTENCE_ENABLED_GSA true
/** CFG_SENTENCE_ENABLED_GST - GNSS pseudorange noise statistics */
#define CFG_SENTENCE_ENABLED_GST true
/** CFG_SENTENCE_ENABLED_GSV - GNSS satellites in view */
#define CFG_SENTENCE_ENABLED_GSV true
/** CFG_SENTENCE_ENABLED_HBT - Heartbeat supervision sentence */
#define CFG_SENTENCE_ENABLED_HBT true
/** CFG_SENTENCE_ENABLED_HCR - Heading correction report */
#define CFG_SENTENCE_ENABLED_HCR true
/** CFG_SENTENCE_ENABLED_HDG - Heading, deviation and variation */
#define CFG_SENTENCE_ENABLED_HDG true
/** CFG_SENTENCE_ENABLED_HDT - Heading true */
#define CFG_SENTENCE_ENABLED_HDT true
/** CFG_SENTENCE_ENABLED_HMR - Heading monitor receive */
#define CFG_SENTENCE_ENABLED_HMR true
/** CFG_SENTENCE_ENABLED_HMS - Heading monitor set */
#define CFG_SENTENCE_ENABLED_HMS true
/** CFG_SENTENCE_ENABLED_HRM - heel angle, roll period and roll amplitude
 * measurement device */
#define CFG_SENTENCE_ENABLED_HRM true
/** CFG_SENTENCE_ENABLED_HSC - Heading steering command */
#define CFG_SENTENCE_ENABLED_HSC true
/** CFG_SENTENCE_ENABLED_HSS - Hull stress surveillance systems */
#define CFG_SENTENCE_ENABLED_HSS true
/** CFG_SENTENCE_ENABLED_HTC - Heading/track control command */
#define CFG_SENTENCE_ENABLED_HTC true
/** CFG_SENTENCE_ENABLED_HTD - Heading /track control data */
#define CFG_SENTENCE_ENABLED_HTD true
/** CFG_SENTENCE_ENABLED_LR1 - AIS long-range reply sentence 1 */
#define CFG_SENTENCE_ENABLED_LR1 true
/** CFG_SENTENCE_ENABLED_LR2 - AIS long-range reply sentence 2 */
#define CFG_SENTENCE_ENABLED_LR2 true
/** CFG_SENTENCE_ENABLED_LR3 - AIS long-range reply sentence 3 */
#define CFG_SENTENCE_ENABLED_LR3 true
/** CFG_SENTENCE_ENABLED_LRF - AIS long-range function */
#define CFG_SENTENCE_ENABLED_LRF true
/** CFG_SENTENCE_ENABLED_LRI - AIS long-range interrogation */
#define CFG_SENTENCE_ENABLED_LRI true
/** CFG_SENTENCE_ENABLED_MOB - Man over board notification */
#define CFG_SENTENCE_ENABLED_MOB true
/** CFG_SENTENCE_ENABLED_MSK - MSK receiver interface */
#define CFG_SENTENCE_ENABLED_MSK true
/** CFG_SENTENCE_ENABLED_MSS - MSK receiver signal status */
#define CFG_SENTENCE_ENABLED_MSS true
/** CFG_SENTENCE_ENABLED_MTW - Water temperature */
#define CFG_SENTENCE_ENABLED_MTW true
/** CFG_SENTENCE_ENABLED_MWD - Wind direction and speed */
#define CFG_SENTENCE_ENABLED_MWD true
/** CFG_SENTENCE_ENABLED_MWV - Wind speed and angle */
#define CFG_SENTENCE_ENABLED_MWV true
/** CFG_SENTENCE_ENABLED_NAK - Negative acknowledgement */
#define CFG_SENTENCE_ENABLED_NAK true
/** CFG_SENTENCE_ENABLED_NRM - NAVTEX receiver mask */
#define CFG_SENTENCE_ENABLED_NRM true
/** CFG_SENTENCE_ENABLED_NRX - NAVTEX received message */
#define CFG_SENTENCE_ENABLED_NRX true
/** CFG_SENTENCE_ENABLED_NSR - Navigation status report */
#define CFG_SENTENCE_ENABLED_NSR true
/** CFG_SENTENCE_ENABLED_OSD - Own ship data */
#define CFG_SENTENCE_ENABLED_OSD true
/** CFG_SENTENCE_ENABLED_POS - Device position and ship dimensions report or
 * configuration command */
#define CFG_SENTENCE_ENABLED_POS true
/** CFG_SENTENCE_ENABLED_PRC - Propulsion remote control status */
#define CFG_SENTENCE_ENABLED_PRC true
/** CFG_SENTENCE_ENABLED_RLM - Return link message */
#define CFG_SENTENCE_ENABLED_RLM true
/** CFG_SENTENCE_ENABLED_RMA - Recommended minimum specific LORAN-C data */
#define CFG_SENTENCE_ENABLED_RMA true
/** CFG_SENTENCE_ENABLED_RMB - Recommended minimum navigation information */
#define CFG_SENTENCE_ENABLED_RMB true
/** CFG_SENTENCE_ENABLED_RMC - Recommended minimum specific GNSS data */
#define CFG_SENTENCE_ENABLED_RMC true
/** CFG_SENTENCE_ENABLED_ROR - Rudder order status */
#define CFG_SENTENCE_ENABLED_ROR true
/** CFG_SENTENCE_ENABLED_ROT - Rate of turn */
#define CFG_SENTENCE_ENABLED_ROT true
/** CFG_SENTENCE_ENABLED_RRT - Report route transfer */
#define CFG_SENTENCE_ENABLED_RRT true
/** CFG_SENTENCE_ENABLED_RPM - Revolutions */
#define CFG_SENTENCE_ENABLED_RPM true
/** CFG_SENTENCE_ENABLED_RSA - Rudder sensor angle */
#define CFG_SENTENCE_ENABLED_RSA true
/** CFG_SENTENCE_ENABLED_RSD - Radar system data */
#define CFG_SENTENCE_ENABLED_RSD true
/** CFG_SENTENCE_ENABLED_RTE - Routes */
#define CFG_SENTENCE_ENABLED_RTE true
/** CFG_SENTENCE_ENABLED_SFI - Scanning frequency information */
#define CFG_SENTENCE_ENABLED_SFI true
/** CFG_SENTENCE_ENABLED_SMI - SafetyNET Message, All Ships/NavArea */
#define CFG_SENTENCE_ENABLED_SMI true
/** CFG_SENTENCE_ENABLED_SM2 - SafetyNET Message, Coastal Warning Area */
#define CFG_SENTENCE_ENABLED_SM2 true
/** CFG_SENTENCE_ENABLED_SM3 - SafetyNET Message, Circular Area address */
#define CFG_SENTENCE_ENABLED_SM3 true
/** CFG_SENTENCE_ENABLED_SM4 - SafetyNET Message, Rectangular Area Address */
#define CFG_SENTENCE_ENABLED_SM4 true
/** CFG_SENTENCE_ENABLED_SMB - IMO SafetyNET Message Body */
#define CFG_SENTENCE_ENABLED_SMB true
/** CFG_SENTENCE_ENABLED_SPW - Security password sentence */
#define CFG_SENTENCE_ENABLED_SPW true
/** CFG_SENTENCE_ENABLED_SSD - AIS ship static data */
#define CFG_SENTENCE_ENABLED_SSD true
/** CFG_SENTENCE_ENABLED_STN - Multiple data ID */
#define CFG_SENTENCE_ENABLED_STN true
/** CFG_SENTENCE_ENABLED_THS - True heading and status */
#define CFG_SENTENCE_ENABLED_THS true
/** CFG_SENTENCE_ENABLED_TLB - Target label */
#define CFG_SENTENCE_ENABLED_TLB true
/** CFG_SENTENCE_ENABLED_TLL - Target latitude and longitude */
#define CFG_SENTENCE_ENABLED_TLL true
/** CFG_SENTENCE_ENABLED_TRC - Thruster control data */
#define CFG_SENTENCE_ENABLED_TRC true
/** CFG_SENTENCE_ENABLED_TRL - AIS transmitter-non-functioning log */
#define CFG_SENTENCE_ENABLED_TRL true
/** CFG_SENTENCE_ENABLED_TRD - Thruster response data */
#define CFG_SENTENCE_ENABLED_TRD true
/** CFG_SENTENCE_ENABLED_TTD - Tracked target data */
#define CFG_SENTENCE_ENABLED_TTD true
/** CFG_SENTENCE_ENABLED_TTM - Tracked target message */
#define CFG_SENTENCE_ENABLED_TTM true
/** CFG_SENTENCE_ENABLED_TUT - Transmission of multi-language text */
#define CFG_SENTENCE_ENABLED_TUT true
/** CFG_SENTENCE_ENABLED_TXT - Text transmission */
#define CFG_SENTENCE_ENABLED_TXT true
/** CFG_SENTENCE_ENABLED_UID - User identification code transmission */
#define CFG_SENTENCE_ENABLED_UID true
/** CFG_SENTENCE_ENABLED_VBW - Dual ground/water speed */
#define CFG_SENTENCE_ENABLED_VBW true
/** CFG_SENTENCE_ENABLED_VDM - AIS VHF data-link message */
#define CFG_SENTENCE_ENABLED_VDM true
/** CFG_SENTENCE_ENABLED_VDO - AIS VHF data-link own-vessel report */
#define CFG_SENTENCE_ENABLED_VDO true
/** CFG_SENTENCE_ENABLED_VDR - Set and drift */
#define CFG_SENTENCE_ENABLED_VDR true
/** CFG_SENTENCE_ENABLED_VER - Version */
#define CFG_SENTENCE_ENABLED_VER true
/** CFG_SENTENCE_ENABLED_VHW - Water speed and heading */
#define CFG_SENTENCE_ENABLED_VHW true
/** CFG_SENTENCE_ENABLED_VLW - Dual ground/water distance */
#define CFG_SENTENCE_ENABLED_VLW true
/** CFG_SENTENCE_ENABLED_VPW - Speed measured parallel to wind */
#define CFG_SENTENCE_ENABLED_VPW true
/** CFG_SENTENCE_ENABLED_VSD - AIS voyage static data */
#define CFG_SENTENCE_ENABLED_VSD true
/** CFG_SENTENCE_ENABLED_VTG - Course over ground and ground speed */
#define CFG_SENTENCE_ENABLED_VTG true
/** CFG_SENTENCE_ENABLED_WAT - Water level detection */
#define CFG_SENTENCE_ENABLED_WAT true
/** CFG_SENTENCE_ENABLED_WCV - Waypoint closure velocity */
#define CFG_SENTENCE_ENABLED_WCV true
/** CFG_SENTENCE_ENABLED_WNC - Distance waypoint to waypoint */
#define CFG_SENTENCE_ENABLED_WNC true
/** CFG_SENTENCE_ENABLED_WPL - Waypoint location */
#define CFG_SENTENCE_ENABLED_WPL true
/** CFG_SENTENCE_ENABLED_XDR - Transducer measurements */
#define CFG_SENTENCE_ENABLED_XDR true
/** CFG_SENTENCE_ENABLED_XTE - Cross-track error, measured */
#define CFG_SENTENCE_ENABLED_XTE true
/** CFG_SENTENCE_ENABLED_XTR - Cross-track error, dead reckoning */
#define CFG_SENTENCE_ENABLED_XTR true
/** CFG_SENTENCE_ENABLED_ZDA - Time and date */
#define CFG_SENTENCE_ENABLED_ZDA true
/** CFG_SENTENCE_ENABLED_ZDL - Time and distance to variable point */
#define CFG_SENTENCE_ENABLED_ZDL true
/** CFG_SENTENCE_ENABLED_ZFO - UTC and time from origin waypoint */
#define CFG_SENTENCE_ENABLED_ZFO true
/** CFG_SENTENCE_ENABLED_ZTG - UTC and time to destination waypoint */
#define CFG_SENTENCE_ENABLED_ZTG true

// -----------------------------------------------------------------------------
// Output Mode Enumeration and Configuration
// -----------------------------------------------------------------------------

/**
 * @brief NMEA output mode enumeration.
 *
 * This enumeration defines the output modes supported by the NMEA parser.
 * The output mode can be set at open() and determines the format of the data
 * returned by the parser.
 */
typedef enum {
  NMEA_OUTPUT_RAW,  //< Return a binary copy of the global structure.
  NMEA_OUTPUT_JSON  //< Return a JSON-formatted string.
} nmea_output_mode_t;

/**
 * @brief NMEA configuration structure.
 *
 * This structure holds the configuration fields for the NMEA parser.
 * The configuration structure is used to set the output mode and other
 * configuration fields as needed.
 */
typedef struct {
  nmea_output_mode_t output_mode;
} nmea_config_t;

static nmea_config_t g_nmea_config = {NMEA_OUTPUT_RAW};  //< Global NMEA config.

/**
 * @brief Global NMEA state structure.
 *
 * This structure holds all the data fields that any NMEA sentence might update.
 * The structure is updated by the sentence-specific parsers and can be used by
 * the application to access the latest data.
 */
typedef struct {
  char time[16];
  char latitude[16];
  char longitude[16];
  float speed;     // e.g. in knots
  float altitude;  // e.g. in meters
                   // Extend with more fields as needed.
} nmea_global_t;

static nmea_global_t g_nmea_state = {0};

// -----------------------------------------------------------------------------
// Dispatch Table Definitions for Sentence Parsers
// -----------------------------------------------------------------------------
/**
 * @brief NMEA sentence parser function pointer type.
 *
 * This function pointer type is used to define the signature of NMEA sentence
 * parsers. The parser function is expected to parse the sentence tokens and
 * update the global NMEA state structure.
 */
typedef int (*nmea_sentence_parser_t)(const char **tokens, size_t token_count);

/**
 * @brief Dispatch table for NMEA sentence parsers.
 *
 * This table is used to dispatch the correct sentence parser based on the
 * sentence identifier.
 */
typedef struct {
  const char *sentence_id;        //< Sentence identifier
  nmea_sentence_parser_t parser;  //< Sentence parser function
} nmea_dispatch_entry_t;

// --- Add these definitions at an appropriate location (e.g. near the dispatch table) ---
#define HASH_TABLE_SIZE 64

static const nmea_dispatch_entry_t* dispatch_hash_table[HASH_TABLE_SIZE] = {0};
static int dispatch_hash_built = 0;

// A simple djb2 hash function.
static unsigned _hash_sentence_id(const char *s) {
    unsigned long hash = 5381;
    while (*s)
      hash = ((hash << 5) + hash) + (unsigned char)(*s++);
    return (unsigned)hash;
}

// Build the hash table from the dispatch table.
static void _build_dispatch_hash(void) {
    if (dispatch_hash_built) return;

    for (size_t i = 0; i < DISPATCH_TABLE_SIZE; i++) {
        const nmea_dispatch_entry_t *entry = &g_dispatch_table[i];
        unsigned h = _hash_sentence_id(entry->sentence_id) % HASH_TABLE_SIZE;
        // Use linear probing.
        while (dispatch_hash_table[h] != NULL)
            h = (h + 1) % HASH_TABLE_SIZE;
        dispatch_hash_table[h] = entry;
    }
    dispatch_hash_built = 1;
}

// --- End added definitions ---

// -----------------------------------------------------------------------------
// Forward declarations for sentence-specific parsers
// -----------------------------------------------------------------------------
#if CFG_SENTENCE_ENABLED_AAM
#define TOKEN_COUNT_AAM 5
static inline int _nmea_parse_aam(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ABK
#define TOKEN_COUNT_ABK 5
static inline int _nmea_parse_abk(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ABM
#define TOKEN_COUNT_ABM 8
static inline int _nmea_parse_abm(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ACA
#define TOKEN_COUNT_ACA 19
static inline int _nmea_parse_aca(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ACK
#define TOKEN_COUNT_ACK 1
static inline int _nmea_parse_ack(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ACN
#define TOKEN_COUNT_ACN 6
static inline int _nmea_parse_acn(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ACS
#define TOKEN_COUNT_ACS 6
static inline int _nmea_parse_acs(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_AIR
#define TOKEN_COUNT_AIR 12
static inline int _nmea_parse_air(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_AKD
#define TOKEN_COUNT_AKD 8
static inline int _nmea_parse_akd(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ALA
#define TOKEN_COUNT_ALA 8
static inline int _nmea_parse_ala(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ALC
/* TODO: ALC can have 0 - n alert entries: this must be handled dynamically.
 * This placeholder of 36 tokens represents a MAX 8 alerts in a sentence */
#define TOKEN_COUNT_ALC 36
static inline int _nmea_parse_alc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ALF
#define TOKEN_COUNT_ALF 13
static inline int _nmea_parse_alf(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ALR
#define TOKEN_COUNT_ALR 5
static inline int _nmea_parse_alr(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_APB
#define TOKEN_COUNT_APB 15
static inline int _nmea_parse_apb(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ARC
#define TOKEN_COUNT_ARC 5
static inline int _nmea_parse_arc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_BBM
#define TOKEN_COUNT_BBM 7
static inline int _nmea_parse_bbm(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_BEC
#define TOKEN_COUNT_BEC 12
static inline int _nmea_parse_bec(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_BOD
#define TOKEN_COUNT_BOD 6
static inline int _nmea_parse_bod(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_BWC
#define TOKEN_COUNT_BWC 13
static inline int _nmea_parse_bwc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_BWR
#define TOKEN_COUNT_BWR 13
static inline int _nmea_parse_bwr(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_BWW
#define TOKEN_COUNT_BWW 6
static inline int _nmea_parse_bww(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_CUR
static inline int _nmea_parse_cur(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_DBT
#define TOKEN_COUNT_DBT 6
static inline int _nmea_parse_dbt(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_DDC
#define TOKEN_COUNT_DDC 4
static inline int _nmea_parse_ddc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_DOR
#define TOKEN_COUNT_DOR 9
static inline int _nmea_parse_dor(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_DPT
#define TOKEN_COUNT_DPT 3
static inline int _nmea_parse_dpt(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_DSC
#define TOKEN_COUNT_DSC 11
static inline int _nmea_parse_dsc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_DSE
#define TOKEN_COUNT_DSE 9
static inline int _nmea_parse_dse(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_DTM
#define TOKEN_COUNT_DTM 8
static inline int _nmea_parse_dtm(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_EPV
#define TOKEN_COUNT_EPV 5
static inline int _nmea_parse_epv(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ETL
#define TOKEN_COUNT_ETL 6
static inline int _nmea_parse_etl(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_EVE
#define TOKEN_COUNT_EVE 3
static inline int _nmea_parse_eve(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_FIR
#define TOKEN_COUNT_FIR 9
static inline int _nmea_parse_fir(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_FSI
#define TOKEN_COUNT_FSI 5
static inline int _nmea_parse_fsi(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_GBS
#define TOKEN_COUNT_GBS 10
static inline int _nmea_parse_gbs(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_GEN
#define TOKEN_COUNT_GEN 5
static inline int _nmea_parse_gen(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_GFA
#define TOKEN_COUNT_GFA 9
static inline int _nmea_parse_gfa(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_GGA
#define TOKEN_COUNT_GGA 14
static inline int _nmea_parse_gga(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_GLL
#define TOKEN_COUNT_GLL 7
static inline int _nmea_parse_gll(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_GNS
#define TOKEN_COUNT_GNS 13
static inline int _nmea_parse_gns(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_GRS
#define TOKEN_COUNT_GRS 16
static inline int _nmea_parse_grs(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_GSA
#define TOKEN_COUNT_GSA 18
static inline int _nmea_parse_gsa(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_GST
#define TOKEN_COUNT_GST 8
static inline int _nmea_parse_gst(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_GSV
#define TOKEN_COUNT_GSV 12
static inline int _nmea_parse_gsv(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HBT
#define TOKEN_COUNT_HBT 3
static inline int _nmea_parse_hbt(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HCR
#define TOKEN_COUNT_HCR 4
static inline int _nmea_parse_hcr(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HDG
#define TOKEN_COUNT_HDG 5
static inline int _nmea_parse_hdg(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HDT
#define TOKEN_COUNT_HDT 2
static inline int _nmea_parse_hdt(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HMR
#define TOKEN_COUNT_HMR 17
static inline int _nmea_parse_hmr(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HMS
#define TOKEN_COUNT_HMS 3
static inline int _nmea_parse_hms(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HRM
static inline int _nmea_parse_hrm(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HSC
#define TOKEN_COUNT_HSC 5
static inline int _nmea_parse_hsc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HSS
#define TOKEN_COUNT_HSS 3
static inline int _nmea_parse_hss(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HTC
#define TOKEN_COUNT_HTC 14
static inline int _nmea_parse_htc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_HTD
#define TOKEN_COUNT_HTD 17
static inline int _nmea_parse_htd(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_LR1
#define TOKEN_COUNT_LR1 6
static inline int _nmea_parse_lr1(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_LR2
#define TOKEN_COUNT_LR2 12
static inline int _nmea_parse_lr2(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_LR3
#define TOKEN_COUNT_LR3 11
static inline int _nmea_parse_lr3(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_LRF
#define TOKEN_COUNT_LRF 5
static inline int _nmea_parse_lrf(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_LRI
#define TOKEN_COUNT_LRI 12
static inline int _nmea_parse_lri(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_MOB
#define TOKEN_COUNT_MOB 14
static inline int _nmea_parse_mob(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_MSK
#define TOKEN_COUNT_MSK 7
static inline int _nmea_parse_msk(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_MSS
#define TOKEN_COUNT_MSS 5
static inline int _nmea_parse_mss(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_MTW
#define TOKEN_COUNT_MTW 2
static inline int _nmea_parse_mtw(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_MWD
#define TOKEN_COUNT_MWD 8
static inline int _nmea_parse_mwd(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_MWV
#define TOKEN_COUNT_MWV 5
static inline int _nmea_parse_mwv(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_NAK
#define TOKEN_COUNT_NAK 5
static inline int _nmea_parse_nak(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_NRM
#define TOKEN_COUNT_NRM 5
static inline int _nmea_parse_nrm(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_NRX
#define TOKEN_COUNT_NRX 13
static inline int _nmea_parse_nrx(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_NSR
#define TOKEN_COUNT_NSR 13
static inline int _nmea_parse_nsr(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_OSD
#define TOKEN_COUNT_OSD 9
static inline int _nmea_parse_osd(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_POS
#define TOKEN_COUNT_POS 10
static inline int _nmea_parse_pos(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_PRC
#define TOKEN_COUNT_PRC 8
static inline int _nmea_parse_prc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_RLM
#define TOKEN_COUNT_RLM 4
static inline int _nmea_parse_rlm(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_RMA
#define TOKEN_COUNT_RMA 12
static inline int _nmea_parse_rma(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_RMB
#define TOKEN_COUNT_RMB 14
static inline int _nmea_parse_rmb(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_RMC
#define TOKEN_COUNT_RMC 13
static inline int _nmea_parse_rmc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ROR
#define TOKEN_COUNT_ROR 5
static inline int _nmea_parse_ror(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ROT
#define TOKEN_COUNT_ROT 2
static inline int _nmea_parse_rot(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_RRT
#define TOKEN_COUNT_RRT 6
static inline int _nmea_parse_rrt(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_RPM
#define TOKEN_COUNT_RPM 5
static inline int _nmea_parse_rpm(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_RSA
#define TOKEN_COUNT_RSA 4
static inline int _nmea_parse_rsa(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_RSD
#define TOKEN_COUNT_RSD 13
static inline int _nmea_parse_rsd(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_RTE
#define TOKEN_COUNT_RTE 6
static inline int _nmea_parse_rte(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_SFI
#define TOKEN_COUNT_SFI 7
static inline int _nmea_parse_sfi(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_SMI
static inline int _nmea_parse_smi(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_SM2
static inline int _nmea_parse_sm2(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_SM3
static inline int _nmea_parse_sm3(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_SM4
static inline int _nmea_parse_sm4(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_SMB
static inline int _nmea_parse_smb(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_SPW
#define TOKEN_COUNT_SPW 4
static inline int _nmea_parse_spw(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_SSD
#define TOKEN_COUNT_SSD 8
static inline int _nmea_parse_ssd(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_STN
static inline int _nmea_parse_stn(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_THS
#define TOKEN_COUNT_THS 2
static inline int _nmea_parse_ths(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_TLB
#define TOKEN_COUNT_TLB 6
static inline int _nmea_parse_tlb(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_TLL
#define TOKEN_COUNT_TLL 9
static inline int _nmea_parse_tll(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_TRC
#define TOKEN_COUNT_TRC 8
static inline int _nmea_parse_trc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_TRL
#define TOKEN_COUNT_TRL 8
static inline int _nmea_parse_trl(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_TRD
#define TOKEN_COUNT_TRD 6
static inline int _nmea_parse_trd(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_TTD
#define TOKEN_COUNT_TTD 5
static inline int _nmea_parse_ttd(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_TTM
#define TOKEN_COUNT_TTM 15
static inline int _nmea_parse_ttm(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_TUT
#define TOKEN_COUNT_TUT 6
static inline int _nmea_parse_tut(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_TXT
#define TOKEN_COUNT_TXT 4
static inline int _nmea_parse_txt(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_UID
#define TOKEN_COUNT_UID 2
static inline int _nmea_parse_uid(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_VBW
#define TOKEN_COUNT_VBW 10
static inline int _nmea_parse_vbw(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_VDM
#define TOKEN_COUNT_VDM 6
static inline int _nmea_parse_vdm(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_VDO
#define TOKEN_COUNT_VDO 6
static inline int _nmea_parse_vdo(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_VDR
#define TOKEN_COUNT_VDR 6
static inline int _nmea_parse_vdr(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_VER
#define TOKEN_COUNT_VER 10
static inline int _nmea_parse_ver(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_VHW
#define TOKEN_COUNT_VHW 8
static inline int _nmea_parse_vhw(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_VLW
#define TOKEN_COUNT_VLW 8
static inline int _nmea_parse_vlw(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_VPW
#define TOKEN_COUNT_VPW 4
static inline int _nmea_parse_vpw(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_VSD
#define TOKEN_COUNT_VSD 9
static inline int _nmea_parse_vsd(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_VTG
#define TOKEN_COUNT_VTG 9
static inline int _nmea_parse_vtg(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_WAT
#define TOKEN_COUNT_WAT 9
static inline int _nmea_parse_wat(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_WCV
#define TOKEN_COUNT_WCV 4
static inline int _nmea_parse_wcv(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_WNC
#define TOKEN_COUNT_WNC 6
static inline int _nmea_parse_wnc(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_WPL
#define TOKEN_COUNT_WPL 5
static inline int _nmea_parse_wpl(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_XDR
#define TOKEN_COUNT_XDR 8
static inline int _nmea_parse_xdr(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_XTE
#define TOKEN_COUNT_XTE 6
static inline int _nmea_parse_xte(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_XTR
#define TOKEN_COUNT_XTR 3
static inline int _nmea_parse_xtr(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ZDA
#define TOKEN_COUNT_ZDA 6
static inline int _nmea_parse_zda(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ZDL
#define TOKEN_COUNT_ZDL 3
static inline int _nmea_parse_zdl(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ZFO
#define TOKEN_COUNT_ZFO 3
static inline int _nmea_parse_zfo(const char **tokens, size_t token_count);
#endif
#if CFG_SENTENCE_ENABLED_ZTG
#define TOKEN_COUNT_ZTG 3
static inline int _nmea_parse_ztg(const char **tokens, size_t token_count);
#endif

/**
 * @brief Dispatch table for NMEA sentence parsers.
 *
 * This table is used to dispatch the correct sentence parser based on the
 * sentence identifier. The table is used by the NMEA parser to call the
 * correct parser function for each sentence.
 */
static const nmea_dispatch_entry_t g_dispatch_table[] = {
#if CFG_SENTENCE_ENABLED_AAM
    {"AAM", _nmea_parse_aam},
#endif
#if CFG_SENTENCE_ENABLED_ABK
    {"ABK", _nmea_parse_abk},
#endif
#if CFG_SENTENCE_ENABLED_ABM
    {"ABM", _nmea_parse_abm},
#endif
#if CFG_SENTENCE_ENABLED_ACA
    {"ACA", _nmea_parse_aca},
#endif
#if CFG_SENTENCE_ENABLED_ACK
    {"ACK", _nmea_parse_ack},
#endif
#if CFG_SENTENCE_ENABLED_ACN
    {"ACN", _nmea_parse_acn},
#endif
#if CFG_SENTENCE_ENABLED_ACS
    {"ACS", _nmea_parse_acs},
#endif
#if CFG_SENTENCE_ENABLED_AIR
    {"AIR", _nmea_parse_air},
#endif
#if CFG_SENTENCE_ENABLED_AKD
    {"AKD", _nmea_parse_akd},
#endif
#if CFG_SENTENCE_ENABLED_ALA
    {"ALA", _nmea_parse_ala},
#endif
#if CFG_SENTENCE_ENABLED_ALC
    {"ALC", _nmea_parse_alc},
#endif
#if CFG_SENTENCE_ENABLED_ALF
    {"ALF", _nmea_parse_alf},
#endif
#if CFG_SENTENCE_ENABLED_ALR
    {"ALR", _nmea_parse_alr},
#endif
#if CFG_SENTENCE_ENABLED_APB
    {"APB", _nmea_parse_apb},
#endif
#if CFG_SENTENCE_ENABLED_ARC
    {"ARC", _nmea_parse_arc},
#endif
#if CFG_SENTENCE_ENABLED_BBM
    {"BBM", _nmea_parse_bbm},
#endif
#if CFG_SENTENCE_ENABLED_BEC
    {"BEC", _nmea_parse_bec},
#endif
#if CFG_SENTENCE_ENABLED_BOD
    {"BOD", _nmea_parse_bod},
#endif
#if CFG_SENTENCE_ENABLED_BWC
    {"BWC", _nmea_parse_bwc},
#endif
#if CFG_SENTENCE_ENABLED_BWR
    {"BWR", _nmea_parse_bwr},
#endif
#if CFG_SENTENCE_ENABLED_BWW
    {"BWW", _nmea_parse_bww},
#endif
#if CFG_SENTENCE_ENABLED_CUR
    {"CUR", _nmea_parse_cur},
#endif
#if CFG_SENTENCE_ENABLED_DBT
    {"DBT", _nmea_parse_dbt},
#endif
#if CFG_SENTENCE_ENABLED_DDC
    {"DDC", _nmea_parse_ddc},
#endif
#if CFG_SENTENCE_ENABLED_DOR
    {"DOR", _nmea_parse_dor},
#endif
#if CFG_SENTENCE_ENABLED_DPT
    {"DPT", _nmea_parse_dpt},
#endif
#if CFG_SENTENCE_ENABLED_DSC
    {"DSC", _nmea_parse_dsc},
#endif
#if CFG_SENTENCE_ENABLED_DSE
    {"DSE", _nmea_parse_dse},
#endif
#if CFG_SENTENCE_ENABLED_DTM
    {"DTM", _nmea_parse_dtm},
#endif
#if CFG_SENTENCE_ENABLED_EPV
    {"EPV", _nmea_parse_epv},
#endif
#if CFG_SENTENCE_ENABLED_ETL
    {"ETL", _nmea_parse_etl},
#endif
#if CFG_SENTENCE_ENABLED_EVE
    {"EVE", _nmea_parse_eve},
#endif
#if CFG_SENTENCE_ENABLED_FIR
    {"FIR", _nmea_parse_fir},
#endif
#if CFG_SENTENCE_ENABLED_FSI
    {"FSI", _nmea_parse_fsi},
#endif
#if CFG_SENTENCE_ENABLED_GBS
    {"GBS", _nmea_parse_gbs},
#endif
#if CFG_SENTENCE_ENABLED_GEN
    {"GEN", _nmea_parse_gen},
#endif
#if CFG_SENTENCE_ENABLED_GFA
    {"GFA", _nmea_parse_gfa},
#endif
#if CFG_SENTENCE_ENABLED_GLL
    {"GLL", _nmea_parse_gll},
#endif
#if CFG_SENTENCE_ENABLED_GNS
    {"GNS", _nmea_parse_gns},
#endif
#if CFG_SENTENCE_ENABLED_GRS
    {"GRS", _nmea_parse_grs},
#endif
#if CFG_SENTENCE_ENABLED_GSA
    {"GSA", _nmea_parse_gsa},
#endif
#if CFG_SENTENCE_ENABLED_GST
    {"GST", _nmea_parse_gst},
#endif
#if CFG_SENTENCE_ENABLED_GSV
    {"GSV", _nmea_parse_gsv},
#endif
#if CFG_SENTENCE_ENABLED_HBT
    {"HBT", _nmea_parse_hbt},
#endif
#if CFG_SENTENCE_ENABLED_HCR
    {"HCR", _nmea_parse_hcr},
#endif
#if CFG_SENTENCE_ENABLED_HDG
    {"HDG", _nmea_parse_hdg},
#endif
#if CFG_SENTENCE_ENABLED_HDT
    {"HDT", _nmea_parse_hdt},
#endif
#if CFG_SENTENCE_ENABLED_HMR
    {"HMR", _nmea_parse_hmr},
#endif
#if CFG_SENTENCE_ENABLED_HMS
    {"HMS", _nmea_parse_hms},
#endif
#if CFG_SENTENCE_ENABLED_HRM
    {"HRM", _nmea_parse_hrm},
#endif
#if CFG_SENTENCE_ENABLED_HSC
    {"HSC", _nmea_parse_hsc},
#endif
#if CFG_SENTENCE_ENABLED_HSS
    {"HSS", _nmea_parse_hss},
#endif
#if CFG_SENTENCE_ENABLED_HTC
    {"HTC", _nmea_parse_htc},
#endif
#if CFG_SENTENCE_ENABLED_HTD
    {"HTD", _nmea_parse_htd},
#endif
#if CFG_SENTENCE_ENABLED_LR1
    {"LR1", _nmea_parse_lr1},
#endif
#if CFG_SENTENCE_ENABLED_LR2
    {"LR2", _nmea_parse_lr2},
#endif
#if CFG_SENTENCE_ENABLED_LR3
    {"LR3", _nmea_parse_lr3},
#endif
#if CFG_SENTENCE_ENABLED_LRF
    {"LRF", _nmea_parse_lrf},
#endif
#if CFG_SENTENCE_ENABLED_LRI
    {"LRI", _nmea_parse_lri},
#endif
#if CFG_SENTENCE_ENABLED_MOB
    {"MOB", _nmea_parse_mob},
#endif
#if CFG_SENTENCE_ENABLED_MSK
    {"MSK", _nmea_parse_msk},
#endif
#if CFG_SENTENCE_ENABLED_MSS
    {"MSS", _nmea_parse_mss},
#endif
#if CFG_SENTENCE_ENABLED_MTW
    {"MTW", _nmea_parse_mtw},
#endif
#if CFG_SENTENCE_ENABLED_MWD
    {"MWD", _nmea_parse_mwd},
#endif
#if CFG_SENTENCE_ENABLED_MWV
    {"MWV", _nmea_parse_mwv},
#endif
#if CFG_SENTENCE_ENABLED_NAK
    {"NAK", _nmea_parse_nak},
#endif
#if CFG_SENTENCE_ENABLED_NRM
    {"NRM", _nmea_parse_nrm},
#endif
#if CFG_SENTENCE_ENABLED_NRX
    {"NRX", _nmea_parse_nrx},
#endif
#if CFG_SENTENCE_ENABLED_NSR
    {"NSR", _nmea_parse_nsr},
#endif
#if CFG_SENTENCE_ENABLED_OSD
    {"OSD", _nmea_parse_osd},
#endif
#if CFG_SENTENCE_ENABLED_POS
    {"POS", _nmea_parse_pos},
#endif
#if CFG_SENTENCE_ENABLED_PRC
    {"PRC", _nmea_parse_prc},
#endif
#if CFG_SENTENCE_ENABLED_RLM
    {"RLM", _nmea_parse_rlm},
#endif
#if CFG_SENTENCE_ENABLED_RMA
    {"RMA", _nmea_parse_rma},
#endif
#if CFG_SENTENCE_ENABLED_RMB
    {"RMB", _nmea_parse_rmb},
#endif
#if CFG_SENTENCE_ENABLED_RMC
    {"RMC", _nmea_parse_rmc},
#endif
#if CFG_SENTENCE_ENABLED_ROR
    {"ROR", _nmea_parse_ror},
#endif
#if CFG_SENTENCE_ENABLED_ROT
    {"ROT", _nmea_parse_rot},
#endif
#if CFG_SENTENCE_ENABLED_RRT
    {"RRT", _nmea_parse_rrt},
#endif
#if CFG_SENTENCE_ENABLED_RPM
    {"RPM", _nmea_parse_rpm},
#endif
#if CFG_SENTENCE_ENABLED_RSA
    {"RSA", _nmea_parse_rsa},
#endif
#if CFG_SENTENCE_ENABLED_RSD
    {"RSD", _nmea_parse_rsd},
#endif
#if CFG_SENTENCE_ENABLED_RTE
    {"RTE", _nmea_parse_rte},
#endif
#if CFG_SENTENCE_ENABLED_SFI
    {"SFI", _nmea_parse_sfi},
#endif
#if CFG_SENTENCE_ENABLED_SMI
    {"SMI", _nmea_parse_smi},
#endif
#if CFG_SENTENCE_ENABLED_SM2
    {"SM2", _nmea_parse_sm2},
#endif
#if CFG_SENTENCE_ENABLED_SM3
    {"SM3", _nmea_parse_sm3},
#endif
#if CFG_SENTENCE_ENABLED_SM4
    {"SM4", _nmea_parse_sm4},
#endif
#if CFG_SENTENCE_ENABLED_SMB
    {"SMB", _nmea_parse_smb},
#endif
#if CFG_SENTENCE_ENABLED_SPW
    {"SPW", _nmea_parse_spw},
#endif
#if CFG_SENTENCE_ENABLED_SSD
    {"SSD", _nmea_parse_ssd},
#endif
#if CFG_SENTENCE_ENABLED_STN
    {"STN", _nmea_parse_stn},
#endif
#if CFG_SENTENCE_ENABLED_THS
    {"THS", _nmea_parse_ths},
#endif
#if CFG_SENTENCE_ENABLED_TLB
    {"TLB", _nmea_parse_tlb},
#endif
#if CFG_SENTENCE_ENABLED_TLL
    {"TLL", _nmea_parse_tll},
#endif
#if CFG_SENTENCE_ENABLED_TRC
    {"TRC", _nmea_parse_trc},
#endif
#if CFG_SENTENCE_ENABLED_TRL
    {"TRL", _nmea_parse_trl},
#endif
#if CFG_SENTENCE_ENABLED_TRD
    {"TRD", _nmea_parse_trd},
#endif
#if CFG_SENTENCE_ENABLED_TTD
    {"TTD", _nmea_parse_ttd},
#endif
#if CFG_SENTENCE_ENABLED_TTM
    {"TTM", _nmea_parse_ttm},
#endif
#if CFG_SENTENCE_ENABLED_TUT
    {"TUT", _nmea_parse_tut},
#endif
#if CFG_SENTENCE_ENABLED_TXT
    {"TXT", _nmea_parse_txt},
#endif
#if CFG_SENTENCE_ENABLED_UID
    {"UID", _nmea_parse_uid},
#endif
#if CFG_SENTENCE_ENABLED_VBW
    {"VBW", _nmea_parse_vbw},
#endif
#if CFG_SENTENCE_ENABLED_VDM
    {"VDM", _nmea_parse_vdm},
#endif
#if CFG_SENTENCE_ENABLED_VDO
    {"VDO", _nmea_parse_vdo},
#endif
#if CFG_SENTENCE_ENABLED_VDR
    {"VDR", _nmea_parse_vdr},
#endif
#if CFG_SENTENCE_ENABLED_VER
    {"VER", _nmea_parse_ver},
#endif
#if CFG_SENTENCE_ENABLED_VHW
    {"VHW", _nmea_parse_vhw},
#endif
#if CFG_SENTENCE_ENABLED_VLW
    {"VLW", _nmea_parse_vlw},
#endif
#if CFG_SENTENCE_ENABLED_VPW
    {"VPW", _nmea_parse_vpw},
#endif
#if CFG_SENTENCE_ENABLED_VSD
    {"VSD", _nmea_parse_vsd},
#endif
#if CFG_SENTENCE_ENABLED_VTG
    {"VTG", _nmea_parse_vtg},
#endif
#if CFG_SENTENCE_ENABLED_WAT
    {"WAT", _nmea_parse_wat},
#endif
#if CFG_SENTENCE_ENABLED_WCV
    {"WCV", _nmea_parse_wcv},
#endif
#if CFG_SENTENCE_ENABLED_WNC
    {"WNC", _nmea_parse_wnc},
#endif
#if CFG_SENTENCE_ENABLED_WPL
    {"WPL", _nmea_parse_wpl},
#endif
#if CFG_SENTENCE_ENABLED_XDR
    {"XDR", _nmea_parse_xdr},
#endif
#if CFG_SENTENCE_ENABLED_XTE
    {"XTE", _nmea_parse_xte},
#endif
#if CFG_SENTENCE_ENABLED_XTR
    {"XTR", _nmea_parse_xtr},
#endif
#if CFG_SENTENCE_ENABLED_ZDA
    {"ZDA", _nmea_parse_zda},
#endif
#if CFG_SENTENCE_ENABLED_ZDL
    {"ZDL", _nmea_parse_zdl},
#endif
#if CFG_SENTENCE_ENABLED_ZFO
    {"ZFO", _nmea_parse_zfo},
#endif
#if CFG_SENTENCE_ENABLED_ZTG
    {"ZTG", _nmea_parse_ztg}};

// Number of entries in the dispatch table, duh.
#define DISPATCH_TABLE_SIZE \
  (sizeof(g_dispatch_table) / sizeof(g_dispatch_table[0]))

// -----------------------------------------------------------------------------
// Helper Functions
// -----------------------------------------------------------------------------

/**
 * @brief Calculate the XOR checksum for the sentence (characters between '$'
 * and '*').
 *
 * @param sentence The NMEA sentence string.
 * @return uint8_t The calculated checksum.
 */
static inline uint8_t _nmea_calculate_checksum(const char *sentence) {
  // The checksum is the 8-bit XOR of all characters in the sentence, excluding
  // the start and end markers. This includes "," and "^" characters, but does
  // NOT include "$", "!" and "*" delimiters. We start at "$" or "!", and stop
  // at "*", which marks the end of the sentence.
  uint8_t checksum = 0;
  if (*sentence == '$' || *sentence == '!') {
    sentence++;  // Skip '$' and '!' characters, they're not part of the cksm.
  }
  while (*sentence && *sentence != '*') {
    checksum ^= (uint8_t)*sentence;
    sentence++;
  }
  return checksum;
}

/**
 * @brief Validate the NMEA sentence checksum.
 *
 * @param sentence The NMEA sentence string.
 * @return int 1 if the checksum is valid, 0 otherwise.
 */
static inline int _nmea_validate_checksum(const char *sentence) {
  const char *asterisk = strrchr(sentence, '*');
  if (!asterisk || strlen(asterisk) < 3) {
    return 0;
  }
  uint8_t expected = _nmea_calculate_checksum(sentence);
  uint8_t actual = (uint8_t)strtol(asterisk + 1, NULL, 16);
  return expected == actual;
}

// Tokenise the sentence into an array of tokens (split on commas and asterisk).
/**
 * @brief Tokenise the NMEA sentence into an array of tokens.
 *
 * @param sentence The NMEA sentence string.
 * @param tokens The array of tokens.
 * @param max_tokens The maximum number of tokens.
 * @return size_t The number of tokens.
 *
 * @note The tokens array is populated with pointers to the original sentence.
 *      The sentence is modified in place.
 */
static inline size_t _nmea_tokenise(const char *sentence, const char **tokens,
                                    size_t max_tokens) {
  size_t count = 0;
  char *copy = strdup(sentence);
  if (!copy) return 0;
  char *token = strtok(copy, ",*");
  while (token && count < max_tokens) {
    tokens[count++] = token;
    token = strtok(NULL, ",*");
  }
  free(copy);
  return count;
}

// Add JSON serialiser helper function in the helper functions section
static inline int _nmea_serialise_json(char *buf, size_t len) {
  int written =
      snprintf(buf, len,
               "{ \"time\": \"%s\", \"latitude\": \"%s\", \"longitude\": "
               "\"%s\", \"speed\": %.2f, \"altitude\": %.2f }",
               g_nmea_state.time, g_nmea_state.latitude, g_nmea_state.longitude,
               g_nmea_state.speed, g_nmea_state.altitude);
  if (written < 0 || (size_t)written >= len) return -1;
  return written;
}

// -----------------------------------------------------------------------------
// Sentence-Specific Parser Implementations
// These functions update the global state with data extracted from the tokens.
// -----------------------------------------------------------------------------

static inline int _nmea_parse_aam(const char **tokens, size_t token_count) {
  if (token_count) }

// Example: Parse a GGA sentence (update time, latitude, longitude).
static inline int _nmea_parse_gga(const char **tokens, size_t token_count) {
  if (token_count < 6) return -1;
  strncpy(g_nmea_state.time, tokens[1], sizeof(g_nmea_state.time) - 1);
  strncpy(g_nmea_state.latitude, tokens[2], sizeof(g_nmea_state.latitude) - 1);
  strncpy(g_nmea_state.longitude, tokens[4],
          sizeof(g_nmea_state.longitude) - 1);
  return 0;
}

// Example: Parse an RMC sentence (update speed, for instance).
static inline int _nmea_parse_rmc(const char **tokens, size_t token_count) {
  if (token_count < 8) return -1;
  // Convert the token at index 7 to a float (assuming it's the speed).
  g_nmea_state.speed = (float)atof(tokens[7]);
  return 0;
}

// -----------------------------------------------------------------------------
// POSIX-like Interface Functions
// -----------------------------------------------------------------------------

/**
 * @brief This function initialises the NMEA parser with the given
 * configuration.
 *
 * @param config Pointer to a configuration structure. If NULL, defaults are
 * used.
 * @return int 0 on success, negative error code otherwise.
 *
 * @note This function is called before any other NMEA parser function.
 *
 */
static inline int nmea_open(const nmea_config_t *config) {
  if (config) {
    g_nmea_config = *config;
  } else {
    // Use default configuration
    g_nmea_config.output_mode = NMEA_OUTPUT_RAW;
  }
  memset(&g_nmea_state, 0, sizeof(g_nmea_state));
  _build_dispatch_hash();
  return 0;
}

/**
 * @brief Close the NMEA parser.
 *
 * @return int 0 on success, negative error code otherwise.
 *
 * @note This function is called after all NMEA parser operations are done.
 *       It doesn't yet implement any actual cleanup, it's just for symmetry.
 */
static inline int nmea_close(void) {
  // If any dynamic memory or handles were allocated, free them here.
  return 0;
}

/**
 * @brief Process an incoming NMEA sentence.
 *
 * This function validates the sentence, tokenises it, and then uses
 * a dispatch table to call the appropriate sentence-specific parser.
 *
 * @param buf The input NMEA sentence string.
 * @param len The length of the sentence.
 * @return ssize_t 0 on success or a negative error code.
 *
 * @note This function is called for each incoming NMEA sentence.
 */
static inline ssize_t nmea_write(const char *buf, size_t len) {
  if (!buf || len == 0) return -1;

  // Validate checksum.
  if (!_nmea_validate_checksum(buf)) {
    return -1;
  }

  // Tokenise the sentence.
#define MAX_TOKENS 20
  const char *tokens[MAX_TOKENS] = {0};
  size_t token_count = _nmea_tokenise(buf, tokens, MAX_TOKENS);
  if (token_count == 0) return -2;

  // Extract sentence type from the first token.
  if (strlen(tokens[0]) < 3) return -3;
  const char *sentence_type = tokens[0] + 2;

  // Perform lookup in the hash table.
  unsigned h = _hash_sentence_id(sentence_type) % HASH_TABLE_SIZE;
  for (unsigned i = 0; i < HASH_TABLE_SIZE; i++) {
      unsigned idx = (h + i) % HASH_TABLE_SIZE;
      if (!dispatch_hash_table[idx]) break; // Not found.
      if (strcmp(dispatch_hash_table[idx]->sentence_id, sentence_type) == 0) {
          return dispatch_hash_table[idx]->parser(tokens, token_count);
      }
  }
  return -4;  // Unknown sentence type.
}

/**
 * nmea_read() - Retrieve the latest parsed data.
 *
 * This function returns data in the format configured at open() (raw or JSON).
 * The caller provides a buffer and its size. In raw mode, a binary copy of the
 * global structure is written. In JSON mode, a JSON string is generated.
 *
 * Returns the number of bytes written on success or a negative error code.
 */

/**
 * @brief Retrieve the latest parsed data.
 *
 * @param buf The output buffer.
 * @param len The size of the buffer.
 * @return ssize_t  The number of bytes written on success or a negative error
 *                  code.
 *
 * @note This function returns data in the format configured at open() (raw or
 * JSON). The caller provides a buffer and its size. In raw mode, a binary copy
 * of the global structure is written. In JSON mode, a JSON string is generated.
 */
static inline ssize_t nmea_read(char *buf, size_t len) {
  if (!buf || len == 0) return -1;
  if (g_nmea_config.output_mode == NMEA_OUTPUT_RAW) {
    if (len < sizeof(nmea_global_t)) return -2;
    memcpy(buf, &g_nmea_state, sizeof(nmea_global_t));
    return sizeof(nmea_global_t);
  } else if (g_nmea_config.output_mode == NMEA_OUTPUT_JSON) {
    int ret =
        _nmea_serialise_json(buf, len);
    if (ret < 0) return -3;
    return ret;
  }
  return -1;
}

#ifdef __cplusplus
}
#endif

#endif  // NMEA_PARSER_H
