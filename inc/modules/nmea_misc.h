/**
 * @file nmea_misc.h
 * @brief Miscellaneous sensors and data module
 */

#ifndef NMEA_MISC_H
#define NMEA_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../nmea_types.h"
#include "../nmea_config.h"

#if NMEA_MODULE_MISC_ENABLED

typedef struct {
    nmea_time_t zda_time;
    nmea_date_t zda_date;
    int8_t local_zone_hours;
    int8_t local_zone_minutes;
    float transducer_value;
    char transducer_type[8];
    bool data_valid;
} nmea_misc_state_t;

/* Parser declarations */
#if NMEA_SENTENCE_ZDA_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_zda(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_XDR_ENABLED
nmea_result_t nmea_parse_xdr(struct nmea_context *ctx, const struct nmea_tokens *tokens);
#endif
/* Additional miscellaneous sentence parsers... */

#endif /* NMEA_MODULE_MISC_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_MISC_H */
