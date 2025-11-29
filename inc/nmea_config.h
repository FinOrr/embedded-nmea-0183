/**
 * @file nmea_config.h
 * @brief Compile-time configuration for NMEA-0183 parser
 *
 * This header controls which modules and sentences are included in the build.
 * Edit this file to customize the parser for your specific needs.
 *
 * Disabling unused modules/sentences significantly reduces:
 * - Code size (binary footprint)
 * - RAM usage (state structures)
 * - Compilation time
 *
 * @note Changes to this file require recompilation of the library.
 */

#ifndef NMEA_CONFIG_H
#define NMEA_CONFIG_H

/* ========================================================================== */
/*                         MODULE-LEVEL CONFIGURATION                         */
/* ========================================================================== */

/**
 * @defgroup ModuleConfig Module Enable/Disable
 * @brief Master switches for entire module categories
 *
 * Set to 1 to enable a module, 0 to completely exclude it from compilation.
 * @{
 */

/** @brief GNSS/GPS positioning module (12 sentences)
 *  Sentences: GGA, GLL, GNS, RMC, RMB, RMA, GSA, GSV, GBS, GST, GRS, GFA */
#define NMEA_MODULE_GNSS_ENABLED        1

/** @brief Automatic Identification System module (15 sentences)
 *  Sentences: VDM, VDO, ABK, ABM, ACA, ACS, AIR, BBM, LR1, LR2, LR3, LRF, LRI, SSD, VSD */
#define NMEA_MODULE_AIS_ENABLED         1

/** @brief Navigation & routing module (9 sentences)
 *  Sentences: APB, BEC, BOD, BWC, BWR, BWW, RTE, AAM, VTG */
#define NMEA_MODULE_NAVIGATION_ENABLED  1

/** @brief Waypoint management module (4 sentences)
 *  Sentences: WPL, WCV, WNC, RTE */
#define NMEA_MODULE_WAYPOINT_ENABLED    1

/** @brief Heading & compass module (11 sentences)
 *  Sentences: HDG, HDT, HSC, HTC, HTD, ROT, RSA, THS, HCR, HMS, HMR */
#define NMEA_MODULE_HEADING_ENABLED     1

/** @brief Environmental sensors module (9 sentences)
 *  Sentences: DBT, DPT, MTW, MWD, MWV, VBW, VHW, VLW, VPW */
#define NMEA_MODULE_SENSOR_ENABLED      1

/** @brief Radar systems module (5 sentences)
 *  Sentences: RSD, TTD, TTM, TLL, TLB */
#define NMEA_MODULE_RADAR_ENABLED       1

/** @brief Safety & alarms module (11 sentences)
 *  Sentences: ALR, ALF, ALC, ALA, AKD, ACN, ACK, ARC, FIR, MOB, DOR */
#define NMEA_MODULE_SAFETY_ENABLED      1

/** @brief Communications module (10 sentences)
 *  Sentences: DSC, DSE, VER, UID, SPW, TUT, TXT, NAK, RLM, STN */
#define NMEA_MODULE_COMM_ENABLED        1

/** @brief System status module (11 sentences)
 *  Sentences: HBT, HSS, OSD, EVE, ETL, NRM, NRX, NSR, POS, MSK, MSS */
#define NMEA_MODULE_SYSTEM_ENABLED      1

/** @brief Attitude & motion module (5 sentences)
 *  Sentences: HRM, VDR, PRC, TRC, TRD */
#define NMEA_MODULE_ATTITUDE_ENABLED    1

/** @brief Miscellaneous module (15+ sentences)
 *  Sentences: XDR, XTE, XTR, ZDA, ZDL, ZFO, ZTG, DTM, CUR, FSI, GEN, RPM, SFI, DDC, EPV, etc. */
#define NMEA_MODULE_MISC_ENABLED        1

/** @} */ /* End of ModuleConfig */

/* ========================================================================== */
/*                   GNSS MODULE - SENTENCE CONFIGURATION                     */
/* ========================================================================== */

#if NMEA_MODULE_GNSS_ENABLED

/**
 * @defgroup GNSSConfig GNSS Sentence Configuration
 * @brief Individual sentence enables for GNSS module
 * @{
 */

#define NMEA_SENTENCE_GGA_ENABLED   1  /**< Global Positioning System Fix Data */
#define NMEA_SENTENCE_GLL_ENABLED   1  /**< Geographic Position - Latitude/Longitude */
#define NMEA_SENTENCE_GNS_ENABLED   1  /**< GNSS Fix Data */
#define NMEA_SENTENCE_RMC_ENABLED   1  /**< Recommended Minimum Specific GNSS Data */
#define NMEA_SENTENCE_RMB_ENABLED   1  /**< Recommended Minimum Navigation Information */
#define NMEA_SENTENCE_RMA_ENABLED   1  /**< Recommended Minimum LORAN-C Data */
#define NMEA_SENTENCE_GSA_ENABLED   1  /**< GNSS DOP and Active Satellites */
#define NMEA_SENTENCE_GSV_ENABLED   1  /**< GNSS Satellites in View */
#define NMEA_SENTENCE_GBS_ENABLED   1  /**< GNSS Satellite Fault Detection */
#define NMEA_SENTENCE_GST_ENABLED   1  /**< GNSS Pseudorange Noise Statistics */
#define NMEA_SENTENCE_GRS_ENABLED   1  /**< GNSS Range Residuals */
#define NMEA_SENTENCE_GFA_ENABLED   1  /**< GNSS Fix Accuracy and Integrity */

/** @} */ /* End of GNSSConfig */

#endif /* NMEA_MODULE_GNSS_ENABLED */

/* ========================================================================== */
/*                    AIS MODULE - SENTENCE CONFIGURATION                     */
/* ========================================================================== */

#if NMEA_MODULE_AIS_ENABLED

/**
 * @defgroup AISConfig AIS Sentence Configuration
 * @brief Individual sentence enables for AIS module
 * @{
 */

#define NMEA_SENTENCE_VDM_ENABLED   1  /**< AIS VHF Data-Link Message */
#define NMEA_SENTENCE_VDO_ENABLED   1  /**< AIS VHF Data-Link Own-Vessel Report */
#define NMEA_SENTENCE_ABK_ENABLED   1  /**< AIS Addressed and Binary Broadcast Acknowledgement */
#define NMEA_SENTENCE_ABM_ENABLED   1  /**< AIS Addressed Binary and Safety Related Message */
#define NMEA_SENTENCE_ACA_ENABLED   1  /**< AIS Regional Channel Assignment */
#define NMEA_SENTENCE_ACS_ENABLED   1  /**< AIS Channel Management Information Source */
#define NMEA_SENTENCE_AIR_ENABLED   1  /**< AIS Interrogation Request */
#define NMEA_SENTENCE_BBM_ENABLED   1  /**< AIS Broadcast Binary Message */
#define NMEA_SENTENCE_LR1_ENABLED   1  /**< AIS Long Range Function 1 */
#define NMEA_SENTENCE_LR2_ENABLED   1  /**< AIS Long Range Function 2 */
#define NMEA_SENTENCE_LR3_ENABLED   1  /**< AIS Long Range Function 3 */
#define NMEA_SENTENCE_LRF_ENABLED   1  /**< AIS Long Range Function */
#define NMEA_SENTENCE_LRI_ENABLED   1  /**< AIS Long Range Interrogation */
#define NMEA_SENTENCE_SSD_ENABLED   1  /**< AIS Ship Static Data */
#define NMEA_SENTENCE_VSD_ENABLED   1  /**< AIS Voyage Static Data */

/** @} */ /* End of AISConfig */

#endif /* NMEA_MODULE_AIS_ENABLED */

/* ========================================================================== */
/*                NAVIGATION MODULE - SENTENCE CONFIGURATION                  */
/* ========================================================================== */

#if NMEA_MODULE_NAVIGATION_ENABLED

/**
 * @defgroup NavConfig Navigation Sentence Configuration
 * @brief Individual sentence enables for Navigation module
 * @{
 */

#define NMEA_SENTENCE_APB_ENABLED   1  /**< Autopilot Sentence B */
#define NMEA_SENTENCE_BEC_ENABLED   1  /**< Bearing and Distance to Waypoint - Dead Reckoning */
#define NMEA_SENTENCE_BOD_ENABLED   1  /**< Bearing - Origin to Destination */
#define NMEA_SENTENCE_BWC_ENABLED   1  /**< Bearing and Distance to Waypoint - Great Circle */
#define NMEA_SENTENCE_BWR_ENABLED   1  /**< Bearing and Distance to Waypoint - Rhumb Line */
#define NMEA_SENTENCE_BWW_ENABLED   1  /**< Bearing - Waypoint to Waypoint */
#define NMEA_SENTENCE_RTE_ENABLED   1  /**< Routes */
#define NMEA_SENTENCE_AAM_ENABLED   1  /**< Waypoint Arrival Alarm */
#define NMEA_SENTENCE_VTG_ENABLED   1  /**< Course Over Ground and Ground Speed */

/** @} */ /* End of NavConfig */

#endif /* NMEA_MODULE_NAVIGATION_ENABLED */

/* ========================================================================== */
/*                WAYPOINT MODULE - SENTENCE CONFIGURATION                    */
/* ========================================================================== */

#if NMEA_MODULE_WAYPOINT_ENABLED

/**
 * @defgroup WaypointConfig Waypoint Sentence Configuration
 * @brief Individual sentence enables for Waypoint module
 * @{
 */

#define NMEA_SENTENCE_WPL_ENABLED   1  /**< Waypoint Location */
#define NMEA_SENTENCE_WCV_ENABLED   1  /**< Waypoint Closure Velocity */
#define NMEA_SENTENCE_WNC_ENABLED   1  /**< Distance - Waypoint to Waypoint */

/** @} */ /* End of WaypointConfig */

#endif /* NMEA_MODULE_WAYPOINT_ENABLED */

/* ========================================================================== */
/*                 HEADING MODULE - SENTENCE CONFIGURATION                    */
/* ========================================================================== */

#if NMEA_MODULE_HEADING_ENABLED

/**
 * @defgroup HeadingConfig Heading Sentence Configuration
 * @brief Individual sentence enables for Heading module
 * @{
 */

#define NMEA_SENTENCE_HDG_ENABLED   1  /**< Heading, Deviation and Variation */
#define NMEA_SENTENCE_HDT_ENABLED   1  /**< Heading - True */
#define NMEA_SENTENCE_HSC_ENABLED   1  /**< Heading Steering Command */
#define NMEA_SENTENCE_HTC_ENABLED   1  /**< Heading/Track Control Command */
#define NMEA_SENTENCE_HTD_ENABLED   1  /**< Heading/Track Control Data */
#define NMEA_SENTENCE_ROT_ENABLED   1  /**< Rate of Turn */
#define NMEA_SENTENCE_RSA_ENABLED   1  /**< Rudder Sensor Angle */
#define NMEA_SENTENCE_THS_ENABLED   1  /**< True Heading and Status */
#define NMEA_SENTENCE_HCR_ENABLED   1  /**< Heading Correction Report */
#define NMEA_SENTENCE_HMS_ENABLED   1  /**< Heading Monitor Set */
#define NMEA_SENTENCE_HMR_ENABLED   1  /**< Heading Monitor Receive */

/** @} */ /* End of HeadingConfig */

#endif /* NMEA_MODULE_HEADING_ENABLED */

/* ========================================================================== */
/*                  SENSOR MODULE - SENTENCE CONFIGURATION                    */
/* ========================================================================== */

#if NMEA_MODULE_SENSOR_ENABLED

/**
 * @defgroup SensorConfig Sensor Sentence Configuration
 * @brief Individual sentence enables for Sensor module
 * @{
 */

#define NMEA_SENTENCE_DBT_ENABLED   1  /**< Depth Below Transducer */
#define NMEA_SENTENCE_DPT_ENABLED   1  /**< Depth */
#define NMEA_SENTENCE_MTW_ENABLED   1  /**< Water Temperature */
#define NMEA_SENTENCE_MWD_ENABLED   1  /**< Wind Direction and Speed */
#define NMEA_SENTENCE_MWV_ENABLED   1  /**< Wind Speed and Angle */
#define NMEA_SENTENCE_VBW_ENABLED   1  /**< Dual Ground/Water Speed */
#define NMEA_SENTENCE_VHW_ENABLED   1  /**< Water Speed and Heading */
#define NMEA_SENTENCE_VLW_ENABLED   1  /**< Dual Ground/Water Distance */
#define NMEA_SENTENCE_VPW_ENABLED   1  /**< Speed - Measured Parallel to Wind */

/** @} */ /* End of SensorConfig */

#endif /* NMEA_MODULE_SENSOR_ENABLED */

/* ========================================================================== */
/*                  RADAR MODULE - SENTENCE CONFIGURATION                     */
/* ========================================================================== */

#if NMEA_MODULE_RADAR_ENABLED

/**
 * @defgroup RadarConfig Radar Sentence Configuration
 * @brief Individual sentence enables for Radar module
 * @{
 */

#define NMEA_SENTENCE_RSD_ENABLED   1  /**< Radar System Data */
#define NMEA_SENTENCE_TTD_ENABLED   1  /**< Tracked Target Data */
#define NMEA_SENTENCE_TTM_ENABLED   1  /**< Tracked Target Message */
#define NMEA_SENTENCE_TLL_ENABLED   1  /**< Target Latitude and Longitude */
#define NMEA_SENTENCE_TLB_ENABLED   1  /**< Target Label */

/** @} */ /* End of RadarConfig */

#endif /* NMEA_MODULE_RADAR_ENABLED */

/* ========================================================================== */
/*                  SAFETY MODULE - SENTENCE CONFIGURATION                    */
/* ========================================================================== */

#if NMEA_MODULE_SAFETY_ENABLED

/**
 * @defgroup SafetyConfig Safety Sentence Configuration
 * @brief Individual sentence enables for Safety module
 * @{
 */

#define NMEA_SENTENCE_ALR_ENABLED   1  /**< Set Alarm State */
#define NMEA_SENTENCE_ALF_ENABLED   1  /**< Alert Sentence */
#define NMEA_SENTENCE_ALC_ENABLED   1  /**< Cyclic Alert List */
#define NMEA_SENTENCE_ALA_ENABLED   1  /**< Report Detailed Alarm Condition */
#define NMEA_SENTENCE_AKD_ENABLED   1  /**< Acknowledge Detail Alarm Condition */
#define NMEA_SENTENCE_ACN_ENABLED   1  /**< Alert Command */
#define NMEA_SENTENCE_ACK_ENABLED   1  /**< Acknowledge Alarm */
#define NMEA_SENTENCE_ARC_ENABLED   1  /**< Alert Command Refused */
#define NMEA_SENTENCE_FIR_ENABLED   1  /**< Fire Detection */
#define NMEA_SENTENCE_MOB_ENABLED   1  /**< Man Over Board */
#define NMEA_SENTENCE_DOR_ENABLED   1  /**< Door Status Detection */

/** @} */ /* End of SafetyConfig */

#endif /* NMEA_MODULE_SAFETY_ENABLED */

/* ========================================================================== */
/*                   COMM MODULE - SENTENCE CONFIGURATION                     */
/* ========================================================================== */

#if NMEA_MODULE_COMM_ENABLED

/**
 * @defgroup CommConfig Communication Sentence Configuration
 * @brief Individual sentence enables for Communication module
 * @{
 */

#define NMEA_SENTENCE_DSC_ENABLED   1  /**< Digital Selective Calling Information */
#define NMEA_SENTENCE_DSE_ENABLED   1  /**< Expanded Digital Selective Calling */
#define NMEA_SENTENCE_VER_ENABLED   1  /**< Version */
#define NMEA_SENTENCE_UID_ENABLED   1  /**< User Identification */
#define NMEA_SENTENCE_SPW_ENABLED   1  /**< Security Password Sentence */
#define NMEA_SENTENCE_TUT_ENABLED   1  /**< Transmission of Multi-language Text */
#define NMEA_SENTENCE_TXT_ENABLED   1  /**< Text Transmission */
#define NMEA_SENTENCE_NAK_ENABLED   1  /**< Negative Acknowledgement */
#define NMEA_SENTENCE_RLM_ENABLED   1  /**< Return Link Message */
#define NMEA_SENTENCE_STN_ENABLED   1  /**< Multiple Data ID */

/** @} */ /* End of CommConfig */

#endif /* NMEA_MODULE_COMM_ENABLED */

/* ========================================================================== */
/*                  SYSTEM MODULE - SENTENCE CONFIGURATION                    */
/* ========================================================================== */

#if NMEA_MODULE_SYSTEM_ENABLED

/**
 * @defgroup SystemConfig System Status Sentence Configuration
 * @brief Individual sentence enables for System Status module
 * @{
 */

#define NMEA_SENTENCE_HBT_ENABLED   1  /**< Heartbeat Supervision Sentence */
#define NMEA_SENTENCE_HSS_ENABLED   1  /**< Hull Stress Surveillance Systems */
#define NMEA_SENTENCE_OSD_ENABLED   1  /**< Own Ship Data */
#define NMEA_SENTENCE_EVE_ENABLED   1  /**< General Event Message */
#define NMEA_SENTENCE_ETL_ENABLED   1  /**< Engine Telegraph Operation Status */
#define NMEA_SENTENCE_NRM_ENABLED   1  /**< NAVTEX Receiver Mask */
#define NMEA_SENTENCE_NRX_ENABLED   1  /**< NAVTEX Received Message */
#define NMEA_SENTENCE_NSR_ENABLED   1  /**< Navigation Status Report */
#define NMEA_SENTENCE_POS_ENABLED   1  /**< Device Position and Dimension Report */
#define NMEA_SENTENCE_MSK_ENABLED   1  /**< MSK Receiver Interface */
#define NMEA_SENTENCE_MSS_ENABLED   1  /**< MSK Receiver Signal Status */

/** @} */ /* End of SystemConfig */

#endif /* NMEA_MODULE_SYSTEM_ENABLED */

/* ========================================================================== */
/*                 ATTITUDE MODULE - SENTENCE CONFIGURATION                   */
/* ========================================================================== */

#if NMEA_MODULE_ATTITUDE_ENABLED

/**
 * @defgroup AttitudeConfig Attitude Sentence Configuration
 * @brief Individual sentence enables for Attitude module
 * @{
 */

#define NMEA_SENTENCE_HRM_ENABLED   1  /**< Heel Angle, Roll Period, Roll Amplitude Measurement Device */
#define NMEA_SENTENCE_VDR_ENABLED   1  /**< Set and Drift */
#define NMEA_SENTENCE_PRC_ENABLED   1  /**< Propulsion Remote Control Status */
#define NMEA_SENTENCE_TRC_ENABLED   1  /**< Thruster Control Data */
#define NMEA_SENTENCE_TRD_ENABLED   1  /**< Thruster Response Data */

/** @} */ /* End of AttitudeConfig */

#endif /* NMEA_MODULE_ATTITUDE_ENABLED */

/* ========================================================================== */
/*                   MISC MODULE - SENTENCE CONFIGURATION                     */
/* ========================================================================== */

#if NMEA_MODULE_MISC_ENABLED

/**
 * @defgroup MiscConfig Miscellaneous Sentence Configuration
 * @brief Individual sentence enables for Miscellaneous module
 * @{
 */

#define NMEA_SENTENCE_XDR_ENABLED   1  /**< Transducer Measurement */
#define NMEA_SENTENCE_XTE_ENABLED   1  /**< Cross-Track Error, Measured */
#define NMEA_SENTENCE_XTR_ENABLED   1  /**< Cross-Track Error - Dead Reckoning */
#define NMEA_SENTENCE_ZDA_ENABLED   1  /**< Time and Date */
#define NMEA_SENTENCE_ZDL_ENABLED   1  /**< Time and Distance to Variable Point */
#define NMEA_SENTENCE_ZFO_ENABLED   1  /**< UTC and Time from Origin Waypoint */
#define NMEA_SENTENCE_ZTG_ENABLED   1  /**< UTC and Time to Destination Waypoint */
#define NMEA_SENTENCE_DTM_ENABLED   1  /**< Datum Reference */
#define NMEA_SENTENCE_CUR_ENABLED   1  /**< Water Current Layer */
#define NMEA_SENTENCE_FSI_ENABLED   1  /**< Frequency Set Information */
#define NMEA_SENTENCE_GEN_ENABLED   1  /**< Generic Binary Information */
#define NMEA_SENTENCE_RPM_ENABLED   1  /**< Revolutions */
#define NMEA_SENTENCE_SFI_ENABLED   1  /**< Scanning Frequency Information */
#define NMEA_SENTENCE_DDC_ENABLED   1  /**< Display Dimming Control */
#define NMEA_SENTENCE_EPV_ENABLED   1  /**< Command or Report Equipment Property Value */
#define NMEA_SENTENCE_TRL_ENABLED   1  /**< Thruster/Lateral Transverse Measurement Device */
#define NMEA_SENTENCE_WAT_ENABLED   1  /**< Water Level Detection */
#define NMEA_SENTENCE_TRD_MISC_ENABLED 1 /**< Thruster Response Data */

/** @} */ /* End of MiscConfig */

#endif /* NMEA_MODULE_MISC_ENABLED */

/* ========================================================================== */
/*                         BUFFER SIZE CONFIGURATION                          */
/* ========================================================================== */

/**
 * @defgroup BufferConfig Buffer Size Configuration
 * @brief Configure buffer sizes for parser operation
 * @{
 */

/** @brief Maximum sentence length to accept (NMEA standard = 82) */
#ifndef NMEA_MAX_SENTENCE_LENGTH
#define NMEA_MAX_SENTENCE_LENGTH    82
#endif

/** @brief Maximum number of fields per sentence (default = 32) */
#ifndef NMEA_MAX_FIELDS
#define NMEA_MAX_FIELDS             32
#endif

/** @} */ /* End of BufferConfig */

#endif /* NMEA_CONFIG_H */
