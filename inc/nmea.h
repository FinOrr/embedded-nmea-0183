/**
 * @file nmea.h
 * @brief Main public API for NMEA-0183 parser library
 *
 * This is the primary header file to include when using the NMEA parser.
 *
 * @section usage Basic Usage
 * @code
 * #include "nmea.h"
 *
 * // 1. Create and configure parser
 * nmea_context_t ctx;
 * nmea_config_t config = {
 *     .enabled_modules = NMEA_MODULE_GNSS | NMEA_MODULE_SENSOR,
 *     .error_callback = my_error_handler,
 *     .error_callback_user_data = NULL,
 *     .validate_checksums = true
 * };
 *
 * // 2. Initialize parser
 * if (nmea_init(&ctx, &config) != NMEA_OK) {
 *     // Handle error
 * }
 *
 * // 3. Parse a sentence
 * const char *sentence =
 * "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n"; char
 * buffer[256];  // Working buffer for tokenization
 *
 * nmea_result_t result = nmea_parse(&ctx, sentence, strlen(sentence), buffer,
 * sizeof(buffer)); if (result == NMEA_OK) {
 *     // Access parsed data from context
 *     nmea_gnss_state_t gnss_data;
 *     if (nmea_get_gnss_data(&ctx, &gnss_data) == NMEA_OK) {
 *         printf("Lat: %f, Lon: %f\n", gnss_data.latitude.degrees,
 * gnss_data.longitude.degrees);
 *     }
 * }
 *
 * // 4. Cleanup
 * nmea_cleanup(&ctx);
 * @endcode
 *
 * @version 1.0.0
 */

#ifndef NMEA_H
#define NMEA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nmea_config.h"
#include "nmea_types.h"

/* Include enabled module headers */
#if NMEA_MODULE_GNSS_ENABLED
#include "modules/nmea_gnss.h"
#endif

#if NMEA_MODULE_AIS_ENABLED
#include "modules/nmea_ais.h"
#endif

#if NMEA_MODULE_NAVIGATION_ENABLED
#include "modules/nmea_navigation.h"
#endif

#if NMEA_MODULE_WAYPOINT_ENABLED
#include "modules/nmea_waypoint.h"
#endif

#if NMEA_MODULE_HEADING_ENABLED
#include "modules/nmea_heading.h"
#endif

#if NMEA_MODULE_SENSOR_ENABLED
#include "modules/nmea_sensor.h"
#endif

#if NMEA_MODULE_RADAR_ENABLED
#include "modules/nmea_radar.h"
#endif

#if NMEA_MODULE_SAFETY_ENABLED
#include "modules/nmea_safety.h"
#endif

#if NMEA_MODULE_COMM_ENABLED
#include "modules/nmea_comm.h"
#endif

#if NMEA_MODULE_SYSTEM_ENABLED
#include "modules/nmea_system.h"
#endif

#if NMEA_MODULE_ATTITUDE_ENABLED
#include "modules/nmea_attitude.h"
#endif

#if NMEA_MODULE_MISC_ENABLED
#include "modules/nmea_misc.h"
#endif

/* ========================================================================== */
/*                          CONTEXT STRUCTURE                                 */
/* ========================================================================== */

/**
 * @brief NMEA parser context structure
 *
 * This structure contains all state for a parser instance.
 * All fields are public but should be treated as read-only by users.
 * Use the accessor functions to retrieve parsed data.
 */
struct nmea_context {
  /** Configuration */
  nmea_config_t config;

  /** Initialization flag */
  bool initialized;

  /** Module state storage (defined per enabled module) */
#if NMEA_MODULE_GNSS_ENABLED
  nmea_gnss_state_t gnss;
#endif

#if NMEA_MODULE_AIS_ENABLED
  nmea_ais_state_t ais;
#endif

#if NMEA_MODULE_NAVIGATION_ENABLED
  nmea_navigation_state_t navigation;
#endif

#if NMEA_MODULE_WAYPOINT_ENABLED
  nmea_waypoint_state_t waypoint;
#endif

#if NMEA_MODULE_HEADING_ENABLED
  nmea_heading_state_t heading;
#endif

#if NMEA_MODULE_SENSOR_ENABLED
  nmea_sensor_state_t sensor;
#endif

#if NMEA_MODULE_RADAR_ENABLED
  nmea_radar_state_t radar;
#endif

#if NMEA_MODULE_SAFETY_ENABLED
  nmea_safety_state_t safety;
#endif

#if NMEA_MODULE_COMM_ENABLED
  nmea_comm_state_t comm;
#endif

#if NMEA_MODULE_SYSTEM_ENABLED
  nmea_system_state_t system;
#endif

#if NMEA_MODULE_ATTITUDE_ENABLED
  nmea_attitude_state_t attitude;
#endif

#if NMEA_MODULE_MISC_ENABLED
  nmea_misc_state_t misc;
#endif
};

/* ========================================================================== */
/*                         CORE API FUNCTIONS                                 */
/* ========================================================================== */

/**
 * @brief Initialize NMEA parser context
 *
 * Creates and initializes a new parser instance with the given configuration.
 * The context must be initialized before any parsing operations.
 *
 * @param[out] ctx     Pointer to context structure to initialize (must not be
 * NULL)
 * @param[in]  config  Parser configuration (must not be NULL)
 *
 * @return NMEA_OK on success
 * @return NMEA_ERROR_NULL_PARAM if ctx or config is NULL
 * @return NMEA_ERROR_INVALID_CONFIG if configuration is invalid
 * @return NMEA_ERROR_ALREADY_INIT if context already initialized
 *
 * @note This function performs zero heap allocation. All state is in the
 * context structure.
 *
 * @see nmea_cleanup()
 * @see nmea_config_t
 */
nmea_result_t nmea_init(nmea_context_t *ctx, const nmea_config_t *config);

/**
 * @brief Parse a single NMEA sentence
 *
 * Parses one complete NMEA-0183 sentence and updates the context state.
 * The sentence should include the leading '$' and trailing checksum.
 *
 * @param[in,out] ctx          Parser context (must be initialized)
 * @param[in]     sentence     NMEA sentence string (null-terminated)
 * @param[in]     length       Length of sentence (excluding null terminator)
 * @param[in,out] buffer       Working buffer for tokenization (will be
 * modified)
 * @param[in]     buffer_size  Size of working buffer (min:
 * NMEA_MAX_SENTENCE_LENGTH)
 *
 * @return NMEA_OK on successful parse
 * @return NMEA_ERROR_NULL_PARAM if any pointer parameter is NULL
 * @return NMEA_ERROR_INVALID_CONTEXT if context not initialized
 * @return NMEA_ERROR_BUFFER_TOO_SMALL if buffer is insufficient
 * @return NMEA_ERROR_INVALID_SENTENCE if sentence format is invalid
 * @return NMEA_ERROR_CHECKSUM_FAILED if checksum validation fails
 * @return NMEA_ERROR_UNKNOWN_SENTENCE if sentence type not recognized
 * @return NMEA_ERROR_SENTENCE_DISABLED if sentence disabled in config
 * @return NMEA_ERROR_PARSE_FAILED if field parsing fails
 *
 * @warning The provided buffer will be modified during parsing. Pass a copy if
 *          you need to preserve the original sentence.
 *
 * @note This function performs zero heap allocation. All parsing uses the
 * provided buffer.
 *
 * @see nmea_init()
 */
nmea_result_t nmea_parse(nmea_context_t *ctx, const char *sentence,
                         size_t length, void *buffer, size_t buffer_size);

/**
 * @brief Cleanup and reset parser context
 *
 * Resets all parser state and releases any resources. After calling this,
 * the context must be reinitialized with nmea_init() before reuse.
 *
 * @param[in,out] ctx  Parser context to cleanup
 *
 * @note This function is safe to call on already-cleaned or uninitialized
 * contexts.
 * @note This function performs zero heap deallocation (no heap was used).
 *
 * @see nmea_init()
 */
void nmea_cleanup(nmea_context_t *ctx);

/* ========================================================================== */
/*                      MODULE DATA ACCESSOR FUNCTIONS                        */
/* ========================================================================== */

#if NMEA_MODULE_GNSS_ENABLED
/**
 * @brief Get current GNSS/GPS data from parser context
 *
 * Retrieves the latest parsed GNSS data (position, time, quality, satellites,
 * etc.) from the parser context.
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to GNSS data structure to fill
 *
 * @return NMEA_OK on success
 * @return NMEA_ERROR_NULL_PARAM if ctx or data is NULL
 * @return NMEA_ERROR_INVALID_CONTEXT if context not initialized
 * @return NMEA_ERROR_MODULE_DISABLED if GNSS module disabled
 * @return NMEA_ERROR_NO_DATA if no GNSS data has been parsed yet
 *
 * @see nmea_gnss_state_t
 */
nmea_result_t nmea_get_gnss_data(const nmea_context_t *ctx,
                                 nmea_gnss_state_t *data);
#endif

#if NMEA_MODULE_AIS_ENABLED
/**
 * @brief Get current AIS data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to AIS data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_ais_state_t
 */
nmea_result_t nmea_get_ais_data(const nmea_context_t *ctx,
                                nmea_ais_state_t *data);
#endif

#if NMEA_MODULE_NAVIGATION_ENABLED
/**
 * @brief Get current navigation data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to navigation data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_navigation_state_t
 */
nmea_result_t nmea_get_navigation_data(const nmea_context_t *ctx,
                                       nmea_navigation_state_t *data);
#endif

#if NMEA_MODULE_WAYPOINT_ENABLED
/**
 * @brief Get current waypoint data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to waypoint data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_waypoint_state_t
 */
nmea_result_t nmea_get_waypoint_data(const nmea_context_t *ctx,
                                     nmea_waypoint_state_t *data);
#endif

#if NMEA_MODULE_HEADING_ENABLED
/**
 * @brief Get current heading data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to heading data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_heading_state_t
 */
nmea_result_t nmea_get_heading_data(const nmea_context_t *ctx,
                                    nmea_heading_state_t *data);
#endif

#if NMEA_MODULE_SENSOR_ENABLED
/**
 * @brief Get current sensor data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to sensor data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_sensor_state_t
 */
nmea_result_t nmea_get_sensor_data(const nmea_context_t *ctx,
                                   nmea_sensor_state_t *data);
#endif

#if NMEA_MODULE_RADAR_ENABLED
/**
 * @brief Get current radar data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to radar data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_radar_state_t
 */
nmea_result_t nmea_get_radar_data(const nmea_context_t *ctx,
                                  nmea_radar_state_t *data);
#endif

#if NMEA_MODULE_SAFETY_ENABLED
/**
 * @brief Get current safety/alarm data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to safety data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_safety_state_t
 */
nmea_result_t nmea_get_safety_data(const nmea_context_t *ctx,
                                   nmea_safety_state_t *data);
#endif

#if NMEA_MODULE_COMM_ENABLED
/**
 * @brief Get current communication data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to communication data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_comm_state_t
 */
nmea_result_t nmea_get_comm_data(const nmea_context_t *ctx,
                                 nmea_comm_state_t *data);
#endif

#if NMEA_MODULE_SYSTEM_ENABLED
/**
 * @brief Get current system status data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to system data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_system_state_t
 */
nmea_result_t nmea_get_system_data(const nmea_context_t *ctx,
                                   nmea_system_state_t *data);
#endif

#if NMEA_MODULE_ATTITUDE_ENABLED
/**
 * @brief Get current attitude/motion data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to attitude data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_attitude_state_t
 */
nmea_result_t nmea_get_attitude_data(const nmea_context_t *ctx,
                                     nmea_attitude_state_t *data);
#endif

#if NMEA_MODULE_MISC_ENABLED
/**
 * @brief Get current miscellaneous data from parser context
 *
 * @param[in]  ctx   Parser context
 * @param[out] data  Pointer to miscellaneous data structure to fill
 *
 * @return NMEA_OK on success
 * @return Error codes as per nmea_get_gnss_data()
 *
 * @see nmea_misc_state_t
 */
nmea_result_t nmea_get_misc_data(const nmea_context_t *ctx,
                                 nmea_misc_state_t *data);
#endif

/* ========================================================================== */
/*                         UTILITY FUNCTIONS                                  */
/* ========================================================================== */

/**
 * @brief Get required buffer size for parsing
 *
 * Returns the minimum buffer size needed for the nmea_parse() function.
 * Use this to allocate appropriately-sized buffers.
 *
 * @return Minimum buffer size in bytes
 *
 * @see nmea_parse()
 */
size_t nmea_get_required_buffer_size(void);

/**
 * @brief Get size of parser context structure
 *
 * Returns the size of nmea_context_t for memory allocation planning.
 *
 * @return Size of context structure in bytes
 *
 * @see nmea_context_t
 */
size_t nmea_get_context_size(void);

/**
 * @brief Get human-readable error message
 *
 * Converts a result code to a descriptive error string.
 *
 * @param[in] result  Result code to convert
 *
 * @return Pointer to static error string (never NULL)
 *
 * @note The returned string is statically allocated and should not be freed.
 */
const char *nmea_get_error_string(nmea_result_t result);

/**
 * @brief Get library version string
 *
 * Returns the version of the NMEA parser library in "major.minor.patch" format.
 *
 * @return Pointer to static version string
 */
const char *nmea_get_version(void);

/**
 * @brief Check if a sentence type is enabled
 *
 * Checks if a specific sentence type is enabled in the current configuration.
 *
 * @param[in] ctx           Parser context
 * @param[in] sentence_id   Sentence ID (e.g., "GPGGA", "GPRMC")
 *
 * @return true if sentence is enabled, false otherwise
 */
bool nmea_is_sentence_enabled(const nmea_context_t *ctx,
                              const char *sentence_id);

/**
 * @brief Check if a module is enabled
 *
 * Checks if a specific module is enabled in the current configuration.
 *
 * @param[in] ctx     Parser context
 * @param[in] module  Module identifier
 *
 * @return true if module is enabled, false otherwise
 */
bool nmea_is_module_enabled(const nmea_context_t *ctx, nmea_module_t module);

#ifdef __cplusplus
}
#endif

#endif /* NMEA_H */
