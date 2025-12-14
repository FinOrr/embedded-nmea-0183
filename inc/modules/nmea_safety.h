/**
 * @file nmea_safety.h
 * @brief Safety and alarm systems module
 */

#ifndef NMEA_SAFETY_H
#define NMEA_SAFETY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_config.h"
#include "nmea_types.h"

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
nmea_result_t nmea_parse_alr(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_ALF_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_alf(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_ALC_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_alc(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_ALA_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_ala(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_AKD_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_akd(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_ACN_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_acn(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_ACK_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_ack(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_ARC_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_arc(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_FIR_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_fir(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_MOB_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_mob(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_DOR_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_dor(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
/* Additional safety sentence parsers... */

#endif /* NMEA_MODULE_SAFETY_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_SAFETY_H */
