/**
 * @file nmea_tokenizer.c
 * @brief NMEA sentence tokenization
 *
 * Implements zero-allocation tokenization using in-place null terminator insertion.
 */

#include "nmea_internal.h"

/**
 * @brief Tokenize an NMEA sentence
 *
 * Parses a sentence into comma-separated fields by inserting null terminators.
 * The buffer is modified in-place, so pass a copy if you need to preserve the original.
 *
 * @param buffer       Sentence buffer (will be modified!)
 * @param buffer_size  Size of buffer
 * @param tokens       Output token structure
 *
 * @return NMEA_OK on success, error code otherwise
 */
nmea_result_t nmea_tokenize(
    char *buffer,
    size_t buffer_size,
    nmea_tokens_t *tokens)
{
    if (buffer == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    if (buffer_size == 0) {
        return NMEA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Initialize tokens */
    tokens->token_count = 0;
    for (size_t i = 0; i < NMEA_MAX_FIELDS; i++) {
        tokens->tokens[i] = NULL;
    }

    /* Validate sentence starts with '$' */
    if (buffer[0] != '$') {
        return NMEA_ERROR_INVALID_SENTENCE;
    }

    /* Skip leading '$' */
    char *ptr = buffer + 1;

    /* Find end of sentence (before checksum or end markers) */
    char *end = ptr;
    while (*end != '\0' && *end != '*' && *end != '\r' && *end != '\n') {
        end++;
    }

    /* Mark end with null terminator */
    char sentinel = *end;
    *end = '\0';

    /* Tokenize by commas */
    tokens->tokens[tokens->token_count++] = ptr;

    while (ptr < end && tokens->token_count < NMEA_MAX_FIELDS) {
        if (*ptr == ',') {
            /* Replace comma with null terminator */
            *ptr = '\0';
            /* Next token starts after the comma */
            tokens->tokens[tokens->token_count++] = ptr + 1;
        }
        ptr++;
    }

    /* Check if we exceeded maximum field count */
    if (tokens->token_count >= NMEA_MAX_FIELDS && ptr < end) {
        /* There might be more fields beyond our limit */
        while (ptr < end) {
            if (*ptr == ',') {
                return NMEA_ERROR_TOO_MANY_FIELDS;
            }
            ptr++;
        }
    }

    /* Restore sentinel character for checksum extraction if needed */
    *end = sentinel;

    /* Validate minimum field count (at least sentence ID) */
    if (tokens->token_count == 0) {
        return NMEA_ERROR_INVALID_SENTENCE;
    }

    return NMEA_OK;
}
