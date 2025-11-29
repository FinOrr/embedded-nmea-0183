/**
 * @file nmea_internal.h
 * @brief Internal helper functions and structures for NMEA parser
 *
 * This header defines internal-only interfaces used within the library.
 * It should NOT be included by user code.
 *
 * @internal
 */

#ifndef NMEA_INTERNAL_H
#define NMEA_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea.h"  /* Includes all necessary headers and defines struct nmea_context */
#include <string.h>

/* Module headers already included via nmea.h */

/* ========================================================================== */
/*                          CONTEXT STRUCTURE                                 */
/* ========================================================================== */

/**
 * @brief Context structure is now defined in nmea.h (public API)
 *
 * This allows users to stack-allocate the context instead of requiring
 * dynamic allocation. The full definition is in nmea.h after all module
 * headers are included.
 */
/* struct nmea_context definition moved to nmea.h */

/* ========================================================================== */
/*                          TOKENIZER INTERFACE                               */
/* ========================================================================== */

/**
 * @brief Tokenization result structure
 */
struct nmea_tokens {
    const char *tokens[NMEA_MAX_FIELDS];  /**< Array of token pointers */
    size_t token_count;                    /**< Number of tokens found */
};

/**
 * @brief Tokenize an NMEA sentence
 *
 * Parses a sentence into comma-separated fields. Uses in-place modification
 * of the buffer (inserts null terminators).
 *
 * @param[in,out] buffer       Sentence buffer (will be modified)
 * @param[in]     buffer_size  Size of buffer
 * @param[out]    tokens       Output token structure
 *
 * @return NMEA_OK on success
 * @return NMEA_ERROR_NULL_PARAM if any parameter is NULL
 * @return NMEA_ERROR_BUFFER_TOO_SMALL if buffer too small
 * @return NMEA_ERROR_TOO_MANY_FIELDS if field count exceeds NMEA_MAX_FIELDS
 * @return NMEA_ERROR_INVALID_SENTENCE if sentence format is invalid
 */
nmea_result_t nmea_tokenize(
    char *buffer,
    size_t buffer_size,
    nmea_tokens_t *tokens
);

/* ========================================================================== */
/*                         CHECKSUM INTERFACE                                 */
/* ========================================================================== */

/**
 * @brief Calculate NMEA sentence checksum
 *
 * Calculates XOR checksum of characters between '$' and '*'.
 *
 * @param[in] sentence  Sentence string (must start with '$')
 * @param[in] length    Length of sentence
 *
 * @return 8-bit checksum value
 */
uint8_t nmea_calculate_checksum(const char *sentence, size_t length);

/**
 * @brief Validate NMEA sentence checksum
 *
 * Checks if the checksum in the sentence matches the calculated value.
 *
 * @param[in] sentence  Complete NMEA sentence with checksum
 * @param[in] length    Length of sentence
 *
 * @return true if checksum is valid, false otherwise
 */
bool nmea_validate_checksum(const char *sentence, size_t length);

/**
 * @brief Extract checksum from sentence
 *
 * Extracts the checksum value from the end of the sentence.
 *
 * @param[in] sentence  NMEA sentence
 * @param[in] length    Length of sentence
 * @param[out] checksum Pointer to store extracted checksum
 *
 * @return NMEA_OK if checksum found and extracted
 * @return NMEA_ERROR_INVALID_SENTENCE if no checksum present
 */
nmea_result_t nmea_extract_checksum(
    const char *sentence,
    size_t length,
    uint8_t *checksum
);

/* ========================================================================== */
/*                          ERROR INTERFACE                                   */
/* ========================================================================== */

/**
 * @brief Trigger error callback if configured
 *
 * Internal helper to invoke the user's error callback.
 *
 * @param[in] ctx         Parser context
 * @param[in] error_type  Type of error
 * @param[in] error_code  Result code
 * @param[in] message     Optional error message (can be NULL)
 */
void nmea_report_error(
    const nmea_context_t *ctx,
    nmea_error_type_t error_type,
    nmea_result_t error_code,
    const char *message
);

/* ========================================================================== */
/*                       PARSER FUNCTION TYPEDEF                              */
/* ========================================================================== */

/**
 * @brief Sentence parser function pointer type
 *
 * All sentence parsers follow this signature.
 *
 * @param[in,out] ctx     Parser context
 * @param[in]     tokens  Parsed tokens from sentence
 *
 * @return NMEA_OK on success, error code otherwise
 */
typedef nmea_result_t (*nmea_parser_func_t)(
    nmea_context_t *ctx,
    const nmea_tokens_t *tokens
);

/* ========================================================================== */
/*                         DISPATCH TABLE ENTRY                               */
/* ========================================================================== */

/**
 * @brief Dispatch table entry structure
 */
typedef struct {
    const char *sentence_id;            /**< Sentence identifier (e.g., "GPGGA") */
    nmea_parser_func_t parser;          /**< Parser function pointer */
    nmea_module_t module;               /**< Module this sentence belongs to */
    uint8_t min_fields;                 /**< Minimum number of fields required */
} nmea_dispatch_entry_t;

/**
 * @brief Get dispatch table
 *
 * Returns a pointer to the dispatch table and its size.
 *
 * @param[out] table_size  Pointer to store table size
 *
 * @return Pointer to dispatch table
 */
const nmea_dispatch_entry_t *nmea_get_dispatch_table(size_t *table_size);

/* ========================================================================== */
/*                      TALKER ID VALIDATION                                  */
/* ========================================================================== */

/**
 * @brief Validate and parse a 2-character talker ID
 *
 * Checks if the provided talker ID string matches one of the 34 valid
 * NMEA-0183 talker IDs and returns the corresponding enum value.
 *
 * @param[in] talker_id  2-character talker ID string (e.g., "GP", "GL", "GN")
 *
 * @return Talker ID enum value, or NMEA_TALKER_UNKNOWN if invalid
 */
nmea_talker_id_t nmea_validate_talker_id(const char *talker_id);

/**
 * @brief Extract talker ID and sentence type from full sentence ID
 *
 * Parses a full sentence ID (e.g., "GPGGA") into its talker ID ("GP")
 * and sentence type ("GGA") components.
 *
 * @param[in]  sentence_id     Full sentence ID (e.g., "GPGGA", "GNGLL")
 * @param[out] talker_id       Buffer to store 2-char talker ID (min 3 bytes for null)
 * @param[out] sentence_type   Buffer to store 3-char sentence type (min 4 bytes for null)
 *
 * @return NMEA_OK on success, NMEA_ERROR_INVALID_SENTENCE if format invalid
 */
nmea_result_t nmea_extract_sentence_parts(
    const char *sentence_id,
    char *talker_id,
    char *sentence_type
);

/* ========================================================================== */
/*                      PARSING HELPER FUNCTIONS                              */
/* ========================================================================== */

/**
 * @brief Parse a float value from token
 *
 * @param[in]  token   Token string
 * @param[out] value   Pointer to store parsed value
 *
 * @return NMEA_OK on success, NMEA_ERROR_PARSE_FAILED on error
 */
nmea_result_t nmea_parse_float(const char *token, float *value);

/**
 * @brief Parse an integer value from token
 *
 * @param[in]  token   Token string
 * @param[out] value   Pointer to store parsed value
 *
 * @return NMEA_OK on success, NMEA_ERROR_PARSE_FAILED on error
 */
nmea_result_t nmea_parse_int(const char *token, int *value);

/**
 * @brief Parse a coordinate (latitude/longitude) from NMEA format
 *
 * Converts NMEA format (DDMM.MMMM or DDDMM.MMMM) to decimal degrees.
 *
 * @param[in]  coord_token     Coordinate token (e.g., "4807.038")
 * @param[in]  direction_token Direction token (e.g., "N", "S", "E", "W")
 * @param[out] coordinate      Parsed coordinate structure
 *
 * @return NMEA_OK on success, NMEA_ERROR_PARSE_FAILED on error
 */
nmea_result_t nmea_parse_coordinate(
    const char *coord_token,
    const char *direction_token,
    nmea_coordinate_t *coordinate
);

/**
 * @brief Parse time from NMEA format
 *
 * Converts NMEA time format (HHMMSS.SSS) to structured time.
 *
 * @param[in]  token  Time token
 * @param[out] time   Parsed time structure
 *
 * @return NMEA_OK on success, NMEA_ERROR_PARSE_FAILED on error
 */
nmea_result_t nmea_parse_time(const char *token, nmea_time_t *time);

/**
 * @brief Parse date from NMEA format
 *
 * Converts NMEA date format (DDMMYY) to structured date.
 *
 * @param[in]  token  Date token
 * @param[out] date   Parsed date structure
 *
 * @return NMEA_OK on success, NMEA_ERROR_PARSE_FAILED on error
 */
nmea_result_t nmea_parse_date(const char *token, nmea_date_t *date);

/**
 * @brief Check if token is empty
 *
 * @param[in] token  Token to check
 *
 * @return true if token is NULL or empty string, false otherwise
 */
static inline bool nmea_is_empty_token(const char *token)
{
    return (token == NULL || token[0] == '\0');
}

/**
 * @brief Strip checksum from end of token if present
 *
 * Defensively removes "*XX" suffix from a token string in place.
 * This handles edge cases where the tokenizer may not have properly
 * stripped the checksum from the last field.
 *
 * @param[in,out] token  Token string to modify (may be const char* cast to char*)
 */
void nmea_strip_checksum(char *token);

#ifdef __cplusplus
}
#endif

#endif /* NMEA_INTERNAL_H */
