/**
 * @file nmea_radar.h
 * @brief Radar and target tracking module
 */

#ifndef NMEA_RADAR_H
#define NMEA_RADAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_config.h"
#include "nmea_types.h"

#if NMEA_MODULE_RADAR_ENABLED

typedef struct {
  uint16_t target_number;
  nmea_coordinate_t target_position;
  float target_bearing;
  float target_distance_nm;
  float target_speed_knots;
  float target_course;
  bool target_valid;
} nmea_radar_state_t;

/* Parser declarations */
#if NMEA_SENTENCE_RSD_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_rsd(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_TTM_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_ttm(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_TLL_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_tll(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_TLB_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_tlb(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
#if NMEA_SENTENCE_TTD_ENABLED
struct nmea_context;
struct nmea_tokens;
nmea_result_t nmea_parse_ttd(struct nmea_context *ctx,
                             const struct nmea_tokens *tokens);
#endif
/* Additional radar sentence parsers... */

#endif /* NMEA_MODULE_RADAR_ENABLED */

#ifdef __cplusplus
}
#endif

#endif /* NMEA_RADAR_H */
