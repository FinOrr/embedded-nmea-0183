/**
 * @file nmea_system.h
 * @brief System status and monitoring module
 */

#ifndef NMEA_SYSTEM_H
#define NMEA_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_types.h"
#include "nmea_config.h"

#if NMEA_MODULE_SYSTEM_ENABLED

typedef struct {
    uint16_t system_status;
    nmea_time_t heartbeat_time;
    char system_id[32];
    bool system_operational;
} nmea_system_state_t;

/* Parser declarations */
#if NMEA_SENTENCE_HBT_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_hbt(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif
/* Additional system sentence parsers... */

#endif /* NMEA_MODULE_SYSTEM_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_SYSTEM_H */
