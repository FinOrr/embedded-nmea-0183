/**
 * @file nmea_attitude.h
 * @brief Attitude and motion module
 */

#ifndef NMEA_ATTITUDE_H
#define NMEA_ATTITUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../nmea_types.h"
#include "../nmea_config.h"

#if NMEA_MODULE_ATTITUDE_ENABLED

typedef struct {
    float roll_angle;
    float pitch_angle;
    float heel_angle;
    float roll_period;
    bool attitude_valid;
} nmea_attitude_state_t;

/* Parser declarations */
#if NMEA_SENTENCE_HRM_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_hrm(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif
/* Additional attitude sentence parsers... */

#endif /* NMEA_MODULE_ATTITUDE_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_ATTITUDE_H */
