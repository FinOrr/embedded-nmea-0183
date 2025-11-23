/**
 * @file nmea_safety.h
 * @brief Safety and alarm systems module
 */

#ifndef NMEA_SAFETY_H
#define NMEA_SAFETY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_types.h"
#include "nmea_config.h"

#if NMEA_MODULE_SAFETY_ENABLED

typedef struct {
    uint16_t alarm_id;
    char alarm_condition[32];
    char alarm_state;
    bool alarm_active;
    nmea_time_t alarm_time;
} nmea_safety_state_t;

/* Parser declarations */
#if NMEA_SENTENCE_ALR_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_alr(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif
/* Additional safety sentence parsers... */

#endif /* NMEA_MODULE_SAFETY_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_SAFETY_H */
