/**
 * @file nmea_talker.c
 * @brief NMEA-0183 talker ID validation and parsing
 *
 * Implements talker ID validation against the 34 standard NMEA-0183 talker IDs.
 */

#include "nmea_internal.h"
#include <string.h>
#include <ctype.h>

/* ========================================================================== */
/*                          TALKER ID LOOKUP TABLE                            */
/* ========================================================================== */

/**
 * @brief Talker ID lookup table entry
 */
typedef struct {
    const char *id;           /**< 2-character talker ID string */
    nmea_talker_id_t value;   /**< Corresponding enum value */
} nmea_talker_entry_t;

/**
 * @brief Static talker ID lookup table
 *
 * Maps 2-character talker ID strings to their enum values.
 * Ordered alphabetically for readability.
 */
static const nmea_talker_entry_t g_talker_table[] = {
    {"AG", NMEA_TALKER_AG},
    {"AI", NMEA_TALKER_AI},
    {"AP", NMEA_TALKER_AP},
    {"CD", NMEA_TALKER_CD},
    {"CR", NMEA_TALKER_CR},
    {"CS", NMEA_TALKER_CS},
    {"CT", NMEA_TALKER_CT},
    {"CV", NMEA_TALKER_CV},
    {"CX", NMEA_TALKER_CX},
    {"DF", NMEA_TALKER_DF},
    {"DM", NMEA_TALKER_DM},
    {"EC", NMEA_TALKER_EC},
    {"EP", NMEA_TALKER_EP},
    {"ER", NMEA_TALKER_ER},
    {"GL", NMEA_TALKER_GL},
    {"GN", NMEA_TALKER_GN},
    {"GP", NMEA_TALKER_GP},
    {"HC", NMEA_TALKER_HC},
    {"HE", NMEA_TALKER_HE},
    {"HN", NMEA_TALKER_HN},
    {"II", NMEA_TALKER_II},
    {"IN", NMEA_TALKER_IN},
    {"LC", NMEA_TALKER_LC},
    {"P",  NMEA_TALKER_P},   /* Note: Single character */
    {"RA", NMEA_TALKER_RA},
    {"SD", NMEA_TALKER_SD},
    {"SN", NMEA_TALKER_SN},
    {"SS", NMEA_TALKER_SS},
    {"TI", NMEA_TALKER_TI},
    {"VD", NMEA_TALKER_VD},
    {"VW", NMEA_TALKER_VW},
    {"WI", NMEA_TALKER_WI},
    {"YX", NMEA_TALKER_YX},
    {"ZA", NMEA_TALKER_ZA},
    {"ZC", NMEA_TALKER_ZC},
    {"ZQ", NMEA_TALKER_ZQ},
    {"ZV", NMEA_TALKER_ZV},
};

#define TALKER_TABLE_SIZE (sizeof(g_talker_table) / sizeof(g_talker_table[0]))

/* ========================================================================== */
/*                          TALKER ID VALIDATION                              */
/* ========================================================================== */

/**
 * @brief Validate and parse a 2-character talker ID
 */
nmea_talker_id_t nmea_validate_talker_id(const char *talker_id)
{
    if (talker_id == NULL) {
        return NMEA_TALKER_UNKNOWN;
    }

    /* Check if it's a single character (special case: 'P' for proprietary) */
    if (talker_id[0] == 'P' && (talker_id[1] == '\0' || !isupper(talker_id[1]))) {
        return NMEA_TALKER_P;
    }

    /* Validate length and format (must be exactly 2 uppercase characters) */
    if (talker_id[0] == '\0' || talker_id[1] == '\0') {
        return NMEA_TALKER_UNKNOWN;
    }

    if (!isupper(talker_id[0]) || !isupper(talker_id[1])) {
        return NMEA_TALKER_UNKNOWN;
    }

    /* Linear search through talker table */
    for (size_t i = 0; i < TALKER_TABLE_SIZE; i++) {
        if (talker_id[0] == g_talker_table[i].id[0] &&
            talker_id[1] == g_talker_table[i].id[1]) {
            return g_talker_table[i].value;
        }
    }

    return NMEA_TALKER_UNKNOWN;
}

/**
 * @brief Extract talker ID and sentence type from full sentence ID
 */
nmea_result_t nmea_extract_sentence_parts(
    const char *sentence_id,
    char *talker_id,
    char *sentence_type)
{
    if (sentence_id == NULL || talker_id == NULL || sentence_type == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    size_t len = strlen(sentence_id);

    /* Most NMEA sentences are 5 characters: 2-char talker + 3-char type */
    /* But some are 6 characters for AIS (e.g., "AIVDM") or others */
    /* Handle the standard 5-character case first */
    if (len == 5) {
        /* Extract talker ID (first 2 characters) */
        talker_id[0] = sentence_id[0];
        talker_id[1] = sentence_id[1];
        talker_id[2] = '\0';

        /* Extract sentence type (last 3 characters) */
        sentence_type[0] = sentence_id[2];
        sentence_type[1] = sentence_id[3];
        sentence_type[2] = sentence_id[4];
        sentence_type[3] = '\0';

        return NMEA_OK;
    }

    /* Handle proprietary sentences with 'P' talker (4+ characters) */
    /* Example: "PGRM" -> talker="P", type="GRM" */
    if (len >= 4 && sentence_id[0] == 'P') {
        talker_id[0] = 'P';
        talker_id[1] = '\0';
        talker_id[2] = '\0';

        /* Rest is the sentence type */
        sentence_type[0] = sentence_id[1];
        sentence_type[1] = sentence_id[2];
        sentence_type[2] = sentence_id[3];
        sentence_type[3] = '\0';

        return NMEA_OK;
    }

    /* Handle 6-character sentences (some variations exist) */
    if (len == 6) {
        /* Extract talker ID (first 2 characters) */
        talker_id[0] = sentence_id[0];
        talker_id[1] = sentence_id[1];
        talker_id[2] = '\0';

        /* Extract sentence type (last 4 characters, but we'll take first 3) */
        sentence_type[0] = sentence_id[2];
        sentence_type[1] = sentence_id[3];
        sentence_type[2] = sentence_id[4];
        sentence_type[3] = '\0';

        return NMEA_OK;
    }

    /* Invalid format */
    return NMEA_ERROR_INVALID_SENTENCE;
}
