/**
 * @file nmea_comm.h
 * @brief Communications module
 */

#ifndef NMEA_COMM_H
#define NMEA_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_types.h"
#include "nmea_config.h"

#if NMEA_MODULE_COMM_ENABLED

typedef struct {
    char message_text[256];
    char message_id[16];
    uint8_t message_priority;
    bool message_valid;
} nmea_comm_state_t;

/* Parser declarations */
#if NMEA_SENTENCE_DSC_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_dsc(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif
/* Additional communication sentence parsers... */

#endif /* NMEA_MODULE_COMM_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_COMM_H */
