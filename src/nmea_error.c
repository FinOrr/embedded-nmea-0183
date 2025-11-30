/**
 * @file nmea_error.c
 * @brief Error handling and reporting
 */

#include "nmea_internal.h"

/**
 * @brief Trigger error callback if configured
 *
 * @param ctx         Parser context
 * @param error_type  Type of error
 * @param error_code  Result code
 * @param message     Optional error message (can be NULL)
 */
void nmea_report_error(const nmea_context_t* ctx, nmea_error_type_t error_type,
                       nmea_result_t error_code, const char* message) {
  if (ctx == NULL) {
    return;
  }

  /* Only invoke callback if configured */
  if (ctx->config.error_callback != NULL) {
    ctx->config.error_callback(error_type, error_code, message,
                               ctx->config.error_callback_user_data);
  }
}

/**
 * @brief Get human-readable error message
 *
 * @param result  Result code to convert
 * @return Pointer to static error string (never NULL)
 */
const char* nmea_get_error_string(nmea_result_t result) {
  switch (result) {
    case NMEA_OK:
      return "Success";
    case NMEA_ERROR_NULL_PARAM:
      return "Null parameter provided";
    case NMEA_ERROR_INVALID_CONTEXT:
      return "Invalid context handle";
    case NMEA_ERROR_INVALID_CONFIG:
      return "Invalid configuration";
    case NMEA_ERROR_BUFFER_TOO_SMALL:
      return "Buffer too small";
    case NMEA_ERROR_INVALID_SENTENCE:
      return "Invalid NMEA sentence format";
    case NMEA_ERROR_CHECKSUM_FAILED:
      return "Checksum validation failed";
    case NMEA_ERROR_UNKNOWN_SENTENCE:
      return "Unknown sentence type";
    case NMEA_ERROR_SENTENCE_DISABLED:
      return "Sentence type disabled in configuration";
    case NMEA_ERROR_MODULE_DISABLED:
      return "Module disabled in configuration";
    case NMEA_ERROR_TOO_MANY_FIELDS:
      return "Too many fields in sentence";
    case NMEA_ERROR_TOO_FEW_FIELDS:
      return "Too few fields in sentence";
    case NMEA_ERROR_PARSE_FAILED:
      return "Parsing failed";
    case NMEA_ERROR_NO_DATA:
      return "No data available";
    case NMEA_ERROR_ALREADY_INIT:
      return "Context already initialized";
    case NMEA_ERROR_NOT_INIT:
      return "Context not initialized";
    default:
      return "Unknown error";
  }
}
