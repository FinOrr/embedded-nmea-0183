#ifndef INC_NMEA_CONFIG_H_
#define INC_NMEA_CONFIG_H_

/* Enabled the sentences and functionlity that you require */
#define CFG_SENTENCE_AAM_ENABLED true
#define CFG_SENTENCE_ABK_ENABLED true
#define CFG_SENTENCE_ABM_ENABLED true
#define CFG_SENTENCE_ACA_ENABLED true
#define CFG_SENTENCE_ACK_ENABLED true
#define CFG_SENTENCE_ACN_ENABLED true
#define CFG_SENTENCE_ACS_ENABLED true
#define CFG_SENTENCE_AIR_ENABLED true
#define CFG_SENTENCE_AKD_ENABLED true
#define CFG_SENTENCE_ALA_ENABLED true
#define CFG_SENTENCE_ALC_ENABLED true
#define CFG_SENTENCE_ALF_ENABLED true
#define CFG_SENTENCE_ALR_ENABLED true
#define CFG_SENTENCE_APB_ENABLED true
#define CFG_SENTENCE_ARC_ENABLED true

/* Sentence configuration parameters */
#define AAM_WAYPOINT_MAX_LENGTH 64
#define ABM_DATA_MAX_LENGTH 60
#define ALA_DETAIL_MAX_LENGTH 64
#define ALC_MAX_ALERT_ENTRIES 128
#define ALR_ALARM_DESCRIPTION_MAX_LENGTH 64
#define APB_WAYPOINT_MAX_LENGTH 32

#endif
