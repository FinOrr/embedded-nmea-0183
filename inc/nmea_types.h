/**
 * @file nmea_types.h
 * @brief Common types, enums, and definitions for NMEA-0183 parser
 *
 * This header defines all fundamental types, error codes, module identifiers,
 * and data structures used throughout the NMEA-0183 parser library.
 *
 */

#ifndef NMEA_TYPES_H
#define NMEA_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/*                              VERSION INFO                                  */
/* ========================================================================== */

#define NMEA_VERSION_MAJOR 1
#define NMEA_VERSION_MINOR 0
#define NMEA_VERSION_PATCH 0

/* ========================================================================== */
/*                              RESULT CODES                                  */
/* ========================================================================== */

/**
 * @brief Result codes returned by NMEA parser functions
 */
typedef enum {
  NMEA_OK = 0,                       /**< Success */
  NMEA_ERROR_NULL_PARAM = -1,        /**< Null parameter provided */
  NMEA_ERROR_INVALID_CONTEXT = -2,   /**< Invalid context handle */
  NMEA_ERROR_INVALID_CONFIG = -3,    /**< Invalid configuration */
  NMEA_ERROR_BUFFER_TOO_SMALL = -4,  /**< Provided buffer too small */
  NMEA_ERROR_INVALID_SENTENCE = -5,  /**< Malformed NMEA sentence */
  NMEA_ERROR_CHECKSUM_FAILED = -6,   /**< Checksum validation failed */
  NMEA_ERROR_UNKNOWN_SENTENCE = -7,  /**< Sentence type not recognized */
  NMEA_ERROR_SENTENCE_DISABLED = -8, /**< Sentence type disabled in config */
  NMEA_ERROR_MODULE_DISABLED = -9,   /**< Module disabled in config */
  NMEA_ERROR_TOO_MANY_FIELDS = -10,  /**< Too many fields in sentence */
  NMEA_ERROR_TOO_FEW_FIELDS = -11,   /**< Too few fields in sentence */
  NMEA_ERROR_PARSE_FAILED = -12,     /**< Generic parsing error */
  NMEA_ERROR_NO_DATA = -13,          /**< No data available */
  NMEA_ERROR_ALREADY_INIT = -14,     /**< Context already initialized */
  NMEA_ERROR_NOT_INIT = -15,         /**< Context not initialized */
} nmea_result_t;

/* ========================================================================== */
/*                              ERROR TYPES                                   */
/* ========================================================================== */

/**
 * @brief Detailed error classification for error callbacks
 */
typedef enum {
  NMEA_ERROR_TYPE_PARAMETER, /**< Invalid function parameter */
  NMEA_ERROR_TYPE_CHECKSUM,  /**< Checksum validation error */
  NMEA_ERROR_TYPE_SYNTAX,    /**< Sentence syntax error */
  NMEA_ERROR_TYPE_SEMANTIC,  /**< Semantic parsing error */
  NMEA_ERROR_TYPE_BUFFER,    /**< Buffer management error */
  NMEA_ERROR_TYPE_CONFIG,    /**< Configuration error */
  NMEA_ERROR_TYPE_STATE,     /**< State machine error */
  NMEA_ERROR_TYPE_UNKNOWN,   /**< Unknown/unclassified error */
} nmea_error_type_t;

/* ========================================================================== */
/*                              MODULE TYPES                                  */
/* ========================================================================== */

/**
 * @brief NMEA sentence module categories
 *
 * Sentences are organized into functional modules for better code organization
 * and selective compilation.
 */
typedef enum {
  NMEA_MODULE_GNSS = 0, /**< GNSS/GPS positioning (12 sentences) */
  NMEA_MODULE_AIS = 1,  /**< Automatic Identification System (15 sentences) */
  NMEA_MODULE_NAVIGATION = 2, /**< Navigation & routing (9 sentences) */
  NMEA_MODULE_WAYPOINT = 3,   /**< Waypoint management (4 sentences) */
  NMEA_MODULE_HEADING = 4,    /**< Heading & compass (11 sentences) */
  NMEA_MODULE_SENSOR = 5,     /**< Environmental sensors (9 sentences) */
  NMEA_MODULE_RADAR = 6,      /**< Radar systems (5 sentences) */
  NMEA_MODULE_SAFETY = 7,     /**< Safety & alarms (11 sentences) */
  NMEA_MODULE_COMM = 8,       /**< Communications (10 sentences) */
  NMEA_MODULE_SYSTEM = 9,     /**< System status (12 sentences) */
  NMEA_MODULE_ATTITUDE = 10,  /**< Attitude & motion (5 sentences) */
  NMEA_MODULE_MISC = 11,      /**< Miscellaneous (15+ sentences) */
  NMEA_MODULE_COUNT = 12,     /**< Total number of modules */
} nmea_module_t;

/* ========================================================================== */
/*                           CONFIGURATION TYPES                              */
/* ========================================================================== */

/**
 * @brief Error callback function signature
 *
 * @param error_type  Type/category of the error
 * @param error_code  Specific result code
 * @param message     Human-readable error message (may be NULL)
 * @param user_data   User-provided context pointer
 */
typedef void (*nmea_error_callback_t)(nmea_error_type_t error_type,
                                      nmea_result_t error_code,
                                      const char* message, void* user_data);

/**
 * @brief Parser configuration structure
 */
typedef struct {
  /**
   * @brief Module enable flags (bit field)
   *
   * Each bit corresponds to a nmea_module_t value. Set bit to 1 to enable
   * the module, 0 to disable. Disabled modules are not compiled in if
   * also disabled at compile-time via nmea_config.h
   */
  uint32_t enabled_modules;

  /**
   * @brief Error callback function (optional)
   *
   * If provided, will be called whenever an error occurs during parsing.
   * Useful for debugging and logging. Set to NULL to disable.
   */
  nmea_error_callback_t error_callback;

  /**
   * @brief User data pointer for error callback
   *
   * This pointer is passed to the error callback function. Can be NULL.
   */
  void* error_callback_user_data;

  /**
   * @brief Validate checksums (default: true)
   *
   * When true, sentences with invalid checksums are rejected.
   * Set to false only if checksum validation is not needed (not recommended).
   */
  bool validate_checksums;

} nmea_config_t;

/* ========================================================================== */
/*                           COMMON DATA TYPES                                */
/* ========================================================================== */

/**
 * @brief Latitude/Longitude coordinate structure
 */
typedef struct {
  float
      degrees; /**< Decimal degrees (-90 to 90 for lat, -180 to 180 for lon) */
  bool valid;  /**< True if coordinate is valid */
} nmea_coordinate_t;

/**
 * @brief Time structure (UTC)
 */
typedef struct {
  uint8_t hour;         /**< Hour (0-23) */
  uint8_t minute;       /**< Minute (0-59) */
  uint8_t second;       /**< Second (0-59) */
  uint16_t millisecond; /**< Millisecond (0-999) */
  bool valid;           /**< True if time is valid */
} nmea_time_t;

/**
 * @brief Date structure
 */
typedef struct {
  uint16_t year; /**< Year (full 4-digit year) */
  uint8_t month; /**< Month (1-12) */
  uint8_t day;   /**< Day (1-31) */
  bool valid;    /**< True if date is valid */
} nmea_date_t;

/**
 * @brief Speed structure with multiple representations
 */
typedef struct {
  float knots; /**< Speed in knots */
  float kmh;   /**< Speed in km/h */
  float mps;   /**< Speed in m/s */
  bool valid;  /**< True if speed is valid */
} nmea_speed_t;

/**
 * @brief Direction/heading structure
 */
typedef struct {
  float degrees; /**< Direction in degrees (0-359.99) */
  bool is_true;  /**< True: true north, False: magnetic north */
  bool valid;    /**< True if direction is valid */
} nmea_direction_t;

/**
 * @brief Generic quality indicator
 */
typedef enum {
  NMEA_QUALITY_INVALID = 0,    /**< No fix / invalid data */
  NMEA_QUALITY_GPS = 1,        /**< GPS fix */
  NMEA_QUALITY_DGPS = 2,       /**< Differential GPS fix */
  NMEA_QUALITY_PPS = 3,        /**< PPS fix */
  NMEA_QUALITY_RTK = 4,        /**< Real-Time Kinematic */
  NMEA_QUALITY_RTK_FLOAT = 5,  /**< Float RTK */
  NMEA_QUALITY_DR = 6,         /**< Dead reckoning */
  NMEA_QUALITY_MANUAL = 7,     /**< Manual input */
  NMEA_QUALITY_SIMULATION = 8, /**< Simulation mode */
} nmea_quality_t;

/**
 * @brief NMEA-0183 Talker ID enumeration
 *
 * Identifies the source device/system that generated the sentence.
 */
typedef enum {
  NMEA_TALKER_AG = 0, /**< Autopilot - General */
  NMEA_TALKER_AP,     /**< Autopilot - Magnetic */
  NMEA_TALKER_CD,     /**< Communications - Digital Selective Calling (DSC) */
  NMEA_TALKER_CR,     /**< Communications - Receiver / Beacon Receiver */
  NMEA_TALKER_CS,     /**< Communications - Satellite */
  NMEA_TALKER_CT,     /**< Communications - Radio-Telephone (MF/HF) */
  NMEA_TALKER_CV,     /**< Communications - Radio-Telephone (VHF) */
  NMEA_TALKER_CX,     /**< Communications - Scanning Receiver */
  NMEA_TALKER_DF,     /**< Direction Finder */
  NMEA_TALKER_EC, /**< Electronic Chart Display & Information System (ECDIS) */
  NMEA_TALKER_EP, /**< Emergency Position Indicating Beacon (EPIRB) */
  NMEA_TALKER_ER, /**< Engine Room Monitoring Systems */
  NMEA_TALKER_GP, /**< Global Positioning System (GPS) */
  NMEA_TALKER_HC, /**< Heading - Magnetic Compass */
  NMEA_TALKER_HE, /**< Heading - North Seeking Gyro */
  NMEA_TALKER_HN, /**< Heading - Non North Seeking Gyro */
  NMEA_TALKER_II, /**< Integrated Instrumentation */
  NMEA_TALKER_IN, /**< Integrated Navigation */
  NMEA_TALKER_LC, /**< Loran C */
  NMEA_TALKER_P,  /**< Proprietary Code */
  NMEA_TALKER_RA, /**< RADAR and/or ARPA */
  NMEA_TALKER_SD, /**< Sounder, Depth */
  NMEA_TALKER_SN, /**< Electronic Positioning System, other/general */
  NMEA_TALKER_SS, /**< Sounder, Scanning */
  NMEA_TALKER_TI, /**< Turn Rate Indicator */
  NMEA_TALKER_VD, /**< Velocity Sensor, Doppler, other/general */
  NMEA_TALKER_DM, /**< Velocity Sensor, Speed Log, Water, Magnetic */
  NMEA_TALKER_VW, /**< Velocity Sensor, Speed Log, Water, Mechanical */
  NMEA_TALKER_WI, /**< Weather Instruments */
  NMEA_TALKER_YX, /**< Transducer */
  NMEA_TALKER_ZA, /**< Timekeeper - Atomic Clock */
  NMEA_TALKER_ZC, /**< Timekeeper - Chronometer */
  NMEA_TALKER_ZQ, /**< Timekeeper - Quartz */
  NMEA_TALKER_ZV, /**< Timekeeper - Radio Update, WWV or WWVH */
  NMEA_TALKER_AI, /**< AIS (Mobile AIS station) */
  NMEA_TALKER_GL, /**< GLONASS */
  NMEA_TALKER_GN, /**< Global Navigation Satellite System (GNSS) */
  NMEA_TALKER_UNKNOWN, /**< Unknown or invalid talker ID */
  NMEA_TALKER_COUNT    /**< Total number of talker IDs */
} nmea_talker_id_t;

/* ========================================================================== */
/*                           CONSTANTS                                        */
/* ========================================================================== */

/** Maximum NMEA sentence length.
 *
 * The NMEA-0183 specification defines a maximum sentence length of 82
 * characters (including '$'/'!' and trailing CRLF). However, some AIS
 * related test sentences and extended payload use cases exceed this
 * limit. To accommodate unit tests and longer proprietary/AIS payloads
 * while still guarding against pathological input sizes, we raise the
 * limit modestly.
 */
#define NMEA_MAX_SENTENCE_LENGTH 128

/** Maximum number of fields in a sentence */
#define NMEA_MAX_FIELDS 32

/** Maximum sentence ID length (e.g., "GPGGA") */
#define NMEA_MAX_SENTENCE_ID_LENGTH 8

/* ========================================================================== */
/*                           FORWARD DECLARATIONS                             */
/* ========================================================================== */

/**
 * @brief Opaque context handle for NMEA parser
 *
 * This structure contains all parser state and is defined in nmea_internal.h.
 * Users interact with it only through pointers.
 */
typedef struct nmea_context nmea_context_t;

/**
 * @brief Opaque tokenization result structure (forward declaration)
 *
 * This structure contains tokenized sentence fields. Defined in
 * nmea_internal.h. Used internally by parsers.
 */
typedef struct nmea_tokens nmea_tokens_t;

#ifdef __cplusplus
}
#endif

#endif /* NMEA_TYPES_H */
