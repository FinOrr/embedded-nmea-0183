#ifndef INC_NMEA_SENTENCES_H_
#define INC_NMEA_SENTENCES_H_

#include <stdint.h>

#define AAM_WAYPOINT_MAX_LENGTH 64
#define ABM_DATA_MAX_LENGTH 60

/**
 * @brief Enumeration of NMEA 0183 Talker IDs.
 *
 * This enumeration represents the different talker IDs used in NMEA 0183
 * communication. Talker IDs identify the source or type of the NMEA data. Each
 * talker ID is associated with a specific category or device that generates
 * NMEA sentences.
 *
 * Use this enum to interpret and handle NMEA sentences based on their talker
 * ID.
 */

typedef enum TalkerID {
  AUTOPILOT_GENERAL = 'AG',         /**< Heading/track controller (autopilot): \
                                                 general */
  AUTOPILOT_MAGNETIC = 'AP',        /**< Heading/track controller (autopilot): \
                                         magnetic */
  AIS = 'AI',                       /**< Automatic identification system */
  BILGE_SYSTEM = 'BI',              /**< Bilge system */
  BRIDGE_NAVIGATIONAL_ALARM = 'BN', /**< Bridge navigational watch alarm \
                                         system */
  ALERT_MANAGEMENT = 'CA',        /**< BAM central alert management function */
  DSC_COMMUNICATIONS = 'CD',      /**< Comms: digital selective calling (DSC) */
  DATA_RECEIVER = 'CR',           /**< Comms: data receiver */
  SATELLITE_COMMUNICATION = 'CS', /**< Comms: satellite */
  VHF_RADIO_TELEPHONE = 'CV',     /**< Comms: radio-telephone (VHF) */
  SCANNING_RECEIVER = 'CX',       /**< Comms: scanning receiver */
  DIRECTION_FINDER = 'DF',        /**< Direction finder */
  DUPLEX_REPEATER_STATION = 'DU', /**< Duplex repeater station */
  ELECTRONIC_CHART_SYSTEM = 'EC', /**< Electronic chart system (ECS) */
  ELECTRONIC_CHART_DISPLAY = 'EI',  /**< Electronic chart display and \
                                         information system (ECDIS) */
  EMERGENCY_LOCATOR = 'EL',         /**< Emergency locator */
  EMERGENCY_POSITION_BEACON = 'EP', /**< Emergency position indicating \
                                         radio beacon (EPIRB) */
  ENGINE_ROOM_MONITORING = 'ER',    /**< Engine room monitoring system */
  FIRE_DOOR_CONTROLLER = 'FD', /**< Fire door controller/monitoring system */
  FIRE_EXTINGUISHER_SYSTEM = 'FE', /**< Fire extinguisher system */
  FIRE_DETECTION_SYSTEM = 'FR',    /**< Fire detection system */
  FIRE_SPRINKLER_SYSTEM = 'FS',    /**< Fire sprinkler system */
  GALILEO_POSITIONING = 'GA',      /**< Galileo positioning system */
  BEIDOU_POSITIONING = 'GB',       /**< BeiDou positioning system */
  IRNSS_POSITIONING = 'GI',        /**< IRNSS positioning system */
  GLONASS_POSITIONING = 'GL',      /**< GLONASS positioning system */
  GPS_POSITIONING = 'GP',          /**< Global positioning system (GPS) */
  QZSS_POSITIONING = 'GQ',         /**< QZSS positioning system */
  GNSS_POSITIONING = 'GN',   /**< Global navigation satellite system (GNSS) */
  COMPASS_HEADING = 'HC',    /**< Heading sensors: compass, magnetic */
  GYRO_NORTH_SEEKING = 'HE', /**< Heading sensors: gyro, north seeking */
  FLUXGATE_HEADING = 'HF',   /**< Heading sensors: fluxgate */
  GYRO_NON_NORTH_SEEKING = 'HN', /**< Heading sensors: gyro, \
                                      on-north seeking */
  HULL_DOOR_CONTROLLER = 'HD',   /**< Hull door controller/monitoring system */
  HULL_STRESS_MONITORING = 'HS', /**< Hull stress monitoring */
  INTEGRATED_COMMUNICATION = 'IC',     /**< Integrated communication system */
  INTEGRATED_INSTRUMENTATION = 'II',   /**< Integrated instrumentation */
  INTEGRATED_NAVIGATION = 'IN',        /**< Integrated navigation */
  LORAN = 'LC',                        /**< LORAN: LORAN-C */
  NETWORK_DEVICE = 'ND',               /**< Network device */
  NAVIGATION_LIGHT_CONTROLLER = 'NL',  /**< Navigation light controller */
  PROPRIETARY_CODE = 'P',              /**< Proprietary code */
  RADAR_AND_PLOTTING = 'RA',           /**< Radar and/or radar plotting */
  PROPULSION_REMOTE_CONTROL = 'RC',    /**< Propulsion machinery including \
                                            remote control */
  SOUNDING_DEPTH = 'SD',               /**< Sounder, depth */
  STEERING_ENGINE = 'SG',              /**< Steering gear/steering engine */
  SERIAL_TO_NETWORK_GATEWAY = 'SI',    /**< Serial to network gateway \
                                            function (SNGF) */
  ELECTRONIC_POSITIONING_OTHER = 'SN', /**< Electronic positioning system, \
                                            other/general */
  SOUNDING_SCANNING = 'SS',            /**< Sounder, scanning */
  TURN_RATE_INDICATOR = 'TI',          /**< Turn rate indicator */
  MICROPROCESSOR_CONTROLLER = 'UP',    /**< Microprocessor controller */
  USER_TALKER_0 = 'U0',    /**< (0<=#<=9) User configured talker ID */
  USER_TALKER_1 = 'U1',    /**< (0<=#<=9) User configured talker ID */
  USER_TALKER_2 = 'U2',    /**< (0<=#<=9) User configured talker ID */
  USER_TALKER_3 = 'U3',    /**< (0<=#<=9) User configured talker ID */
  USER_TALKER_4 = 'U4',    /**< (0<=#<=9) User configured talker ID */
  USER_TALKER_5 = 'U5',    /**< (0<=#<=9) User configured talker ID */
  USER_TALKER_6 = 'U6',    /**< (0<=#<=9) User configured talker ID */
  USER_TALKER_7 = 'U7',    /**< (0<=#<=9) User configured talker ID */
  USER_TALKER_8 = 'U8',    /**< (0<=#<=9) User configured talker ID */
  USER_TALKER_9 = 'U9',    /**< (0<=#<=9) User configured talker ID */
  VELOCITY_DOPPLER = 'VD', /**< Velocity sensors: Doppler, other/general */
  VELOCITY_WATER_MAGNETIC = 'VM',    /**< Velocity sensors: speed log, water, \
                                          magnetic */
  VELOCITY_WATER_MECHANICAL = 'VW',  /**< Velocity sensors: speed log, water, \
                                          mechanical */
  VOYAGE_DATA_RECORDER = 'VR',       /**< Voyage data recorder */
  WATERTIGHT_DOOR_CONTROLLER = 'WD', /**< Watertight door controller / \
                                          monitoring system */
  WATER_LEVEL_DETECTION = 'WL',      /**< Water level detection system */
  TRANSDUCER = 'YX',                 /**< Transducer */
  ATOMIC_CLOCK = 'ZA',               /**< Timekeeper, time/date: atomic clock */
  CHRONOMETER = 'ZC',                /**< Timekeeper, time/date: chronometer */
  QUARTZ_CLOCK = 'ZQ',               /**< Timekeeper, time/date: quartz */
  RADIO_UPDATE = 'ZV',               /**< Timekeeper, time/date: radio update */
  WEATHER_INSTRUMENT = 'WI'          /**< Weather instrument */
} TalkerID;

/**
 * @brief Enumeration of NMEA 0183 Sentence IDs.
 *
 * This enumeration represents the different types of NMEA 0183 sentences.
 * Each sentence ID corresponds to a specific type of information or command.
 * Use this enum to identify and work with different NMEA 0183 sentences.
 */
typedef enum SentenceID {
  AAM = 'AAM', /**< Waypoint arrival alarm */
  ABK = 'ABK', /**< AIS addressed and binary broadcast acknowledgement */
  ABM = 'ABM', /**< AIS addressed binary and safety related message */
  ACA = 'ACA', /**< AIS channel assignment message */
  ACK = 'ACK', /**< Acknowledge alarm */
  ACN = 'ACN', /**< Alert command */
  ACS = 'ACS', /**< AIS channel management information source */
  AIR = 'AIR', /**< AIS interrogation request */
  AKD = 'AKD', /**< Acknowledge detail alarm condition */
  ALA = 'ALA', /**< Report detailed alarm condition */
  ALC = 'ALC', /**< Cyclic alert list */
  ALF = 'ALF', /**< Alert sentence */
  ALR = 'ALR', /**< Set alarm state */
  APB = 'APB', /**< Heading/track controller (autopilot) sentence B */
  ARC = 'ARC', /**< Alert command refused */
  BBM = 'BBM', /**< AIS broadcast binary message */
  BEC = 'BEC', /**< Bearing and distance to waypoint - Dead reckoning */
  BOD = 'BOD', /**< Bearing origin to destination */
  BWC = 'BWC', /**< Bearing and distance to waypoint - Great circle */
  BWR = 'BWR', /**< Bearing and distance to waypoint - Rhumb line */
  BWW = 'BWW', /**< Bearing waypoint to waypoint */
  CUR = 'CUR', /**< Water current layer - Multi-layer water current data */
  DBT = 'DBT', /**< Depth below transducer */
  DDC = 'DDC', /**< Display dimming control */
  DOR = 'DOR', /**< Door status detection */
  DPT = 'DPT', /**< Depth */
  DSC = 'DSC', /**< Digital selective calling information */
  DSE = 'DSE', /**< Expanded digital selective calling */
  DTM = 'DTM', /**< Datum reference */
  EPV = 'EPV', /**< Command or report equipment property value */
  ETL = 'ETL', /**< Engine telegraph operation status */
  EVE = 'EVE', /**< General event message */
  FIR = 'FIR', /**< Fire detection */
  FSI = 'FSI', /**< Frequency set information */
  GBS = 'GBS', /**< GNSS satellite fault detection */
  GEN = 'GEN', /**< Generic binary information */
  GFA = 'GFA', /**< GNSS fix accuracy and integrity */
  GGA = 'GGA', /**< Global positioning system (GPS) fix data */
  GLL = 'GLL', /**< Geographic position - Latitude/longitude */
  GNS = 'GNS', /**< GNSS fix data */
  GRS = 'GRS', /**< GNSS range residuals */
  GSA = 'GSA', /**< GNSS DOP and active satellites */
  GST = 'GST', /**< GNSS pseudorange noise statistics */
  GSV = 'GSV', /**< GNSS satellites in view */
  HBT = 'HBT', /**< Heartbeat supervision sentence */
  HCR = 'HCR', /**< Heading correction report */
  HDG = 'HDG', /**< Heading, deviation and variation */
  HDT = 'HDT', /**< Heading true */
  HMR = 'HMR', /**< Heading monitor receive */
  HMS = 'HMS', /**< Heading monitor set */
  HRM = 'HRM', /**< Heel angle, roll period and roll amplitude measurement
                  device */
  HSC = 'HSC', /**< Heading/track control command */
  HTD = 'HTD', /**< Heading/track control data */
  LR1 = 'LR1', /**< AIS long-range reply sentence 1 */
  LR2 = 'LR2', /**< AIS long-range reply sentence 2 */
  LR3 = 'LR3', /**< AIS long-range reply sentence 3 */
  LRF = 'LRF', /**< AIS long-range function */
  LRI = 'LRI', /**< AIS long-range interrogation */
  MOB = 'MOB', /**< Man over board notification */
  MSK = 'MSK', /**< MSK receiver interface */
  MSS = 'MSS', /**< MSK receiver signal status */
  MTW = 'MTW', /**< Water temperature */
  MWD = 'MWD', /**< Wind direction and speed */
  MWV = 'MWV', /**< Wind speed and angle */
  NAK = 'NAK', /**< Negative acknowledgement */
  NRM = 'NRM', /**< NAVTEX receiver mask */
  NRX = 'NRX', /**< NAVTEX received message */
  NSR = 'NSR', /**< Navigation status report */
  OSD = 'OSD', /**< Own ship data */
  POS = 'POS', /**< Device position & ship dimensions report or configuration
                  command */
  PRC = 'PRC', /**< Propulsion remote control status */
  RLM = 'RLM', /**< Return link message */
  RMA = 'RMA', /**< Recommended minimum specific LORAN-C data */
  RMB = 'RMB', /**< Recommended minimum navigation information */
  RMC = 'RMC', /**< Recommended minimum specific GNSS data */
  ROR = 'ROR', /**< Rudder order status */
  ROT = 'ROT', /**< Rate of turn */
  RRT = 'RRT', /**< Report route transfer */
  RPM = 'RPM', /**< Revolutions */
  RSA = 'RSA', /**< Rudder sensor angle */
  RSD = 'RSD', /**< Radar system data */
  RTE = 'RTE', /**< Routes */
  SFI = 'SFI', /**< Scanning frequency information */
  SMI = 'SMI', /**< SafetyNET Message, All Ships/NavArea */
  SM2 = 'SM2', /**< SafetyNET Message, Coastal Warning Area */
  SM3 = 'SM3', /**< SafetyNET Message, Circular Area address */
  SM4 = 'SM4', /**< SafetyNET Message, Rectangular Area Address */
  SMB = 'SMB', /**< IMO SafetyNET Message Body */
  SPW = 'SPW', /**< Security password sentence */
  SSD = 'SSD', /**< AIS ship static data */
  STN = 'STN', /**< Multiple data ID */
  THS = 'THS', /**< True heading and status */
  TLB = 'TLB', /**< Target label */
  TLL = 'TLL', /**< Target latitude and longitude */
  TRC = 'TRC', /**< Thruster control data */
  TRL = 'TRL', /**< AIS transmitter-non-functioning log */
  TRD = 'TRD', /**< Thruster response data */
  TTD = 'TTD', /**< Tracked target data */
  TTM = 'TTM', /**< Tracked target message */
  TUT = 'TUT', /**< Transmission of multi-language text */
  TXT = 'TXT', /**< Text transmission */
  UID = 'UID', /**< User identification code transmission */
  VBW = 'VBW', /**< Dual ground/water speed */
  VDM = 'VDM', /**< AIS VHF data-link message */
  VDO = 'VDO', /**< AIS VHF data-link own-vessel report */
  VDR = 'VDR', /**< Set and drift */
  VER = 'VER', /**< Version */
  VHW = 'VHW', /**< Water speed and heading */
  VLW = 'VLW', /**< Dual ground/water distance */
  VPW = 'VPW', /**< Speed measured parallel to wind */
  VSD = 'VSD', /**< AIS voyage static data */
  VTG = 'VTG', /**< Course over ground and ground speed */
  WAT = 'WAT', /**< Water level detection */
  WCV = 'WCV', /**< Waypoint closure velocity */
  WNC = 'WNC', /**< Distance waypoint to waypoint */
  WPL = 'WPL', /**< Waypoint location */
  XDR = 'XDR', /**< Transducer measurements */
  XTE = 'XTE', /**< Cross-track error, measured */
  XTR = 'XTR', /**< Cross-track error, dead reckoning */
  ZDA = 'ZDA', /**< Time and date */
  ZDL = 'ZDL', /**< Time and distance to variable point */
  ZFO = 'ZFO', /**< UTC and time from origin waypoint */
  ZTG = 'ZTG'  /**< UTC and time to destination waypoint */
} SentenceID;

/**
 * @brief Enumeration for polarities.
 *
 * This enumeration represents the possible values for a polarity field.
 */
typedef enum Polarity {
  NORTH = 'N', /**< Northern polarity */
  EAST = 'E',  /**< Eastern polarity */
  SOUTH = 'S', /**< Southern polarity */
  WEST = 'W'   /**< Western polarity */
} Polarity;

/**
 * @brief Enumeration for status field.
 *
 * This enumeration represents the possible values of a status field,
 * where 'A' indicates "Yes, data valid, warning flag clear,"
 * and 'V' indicates "No, data invalid, warning flag set."
 */
typedef enum StatusField {
  STATUS_VALID = 'A',  /**< Yes, data valid, warning flag clear */
  STATUS_INVALID = 'V' /**< No, data invalid, warning flag set */
} StatusField;

#ifdef SENTENCE_AAM_ENABLED
/**
 * @brief Waypoint arrival alarm (AAM) sentence structure.
 *
 * Status of arrival (entering the arrival circle, or passing the perpendicular
 * of the course line) at waypoint c--c.
 * *
 * @var TalkerID talkerId
 * @brief The talker ID associated with the sentence.
 *
 * @var SentenceID sentenceId
 * @brief The unique identifier for the sentence (AAM).
 *
 * @var StatusField arrivalCircledEntered
 * @brief Single character field indicating if the vessel has entered the
 * arrival circle (A = Yes, data valid, warning flag clear; V = No, data
 * invalid, warning flag set).
 *
 * @var StatusField perpendicularPassedAtWaypoint
 * @brief Single character field indicating if the vessel has passed the
 * waypoint perpendicularly (A = Yes, data valid, warning flag clear; V = No,
 * data invalid, warning flag set).
 *
 * @var float arrivalCircleRadius
 * @brief The radius of the arrival circle.
 *
 * @var uint8_t radiusUnits
 * @brief The units of the radius (e.g., Nautical Miles).
 *
 * @var uint8_t *waypointID
 * @brief Pointer to the waypoint identifier.
 */
typedef struct SENTENCE_AAM {
  TalkerID talkerId;
  SentenceID sentenceId = AAM;
  StatusField arrivalCircledEntered;
  StatusField perpendicularPassedAtWaypoint;
  float arrivalCircleRadius;
  uint8_t radiusUnits;
  uint8_t waypointID[AAM_WAYPOINT_MAX_LENGTH];
} SENTENCE_AAM;
#endif

#ifdef SENTENCE_ABK_ENABLED
/**
 * @brief AIS addressed and binary broadcast acknowledgement (ABK) sentence
 * structure.
 *
 * This structure represents information related to the AIS addressed and binary
 * broadcast acknowledgement (ABK) sentence. The ABK sentence is generated upon
 * the completion or termination of a transaction initiated by the reception of
 * ABM, AIR, or BBM sentences. It provides information about the success or
 * failure of an ABM broadcast, specifically ITU-R M.1371 Messages 6 or 12.
 *
 * The ABK sentence utilises information from ITU-R M.1371 Messages 7 and 13. It
 * is delivered upon the reception of VHF Data-link Message 7 or 13, or in case
 * of failure of Messages 6 or 12. The sentence reports the AIS unit's handling
 * of AIR (ITU-R M.1371 Message 15) and BBM (ITU-R M.1371 Messages 8, 14, 25,
 * 26) sentences to the external application.
 *
 * The external application can initiate an interrogation through the
 * AIR-sentence or a broadcast through the BBM sentence. The AIS unit generates
 * the ABK sentence to report the outcome of the ABM, AIR, or BBM broadcast
 * process.
 *
 * @var TalkerID talkerId
 * @brief The talker ID associated with the sentence.
 *
 * @var SentenceID sentenceId
 * @brief The unique identifier for the sentence (ABK).
 *
 * @var uint32_t mmsiAddress
 * @brief The Maritime Mobile Service Identity (MMSI) address.
 *
 * @var uint8_t mmsiChannel
 * @brief The MMSI channel.
 *
 * @var float m1373MessageId
 * @brief The ID of ITU-R M.1373 message.
 *
 * @var uint8_t messageSequenceNumber
 * @brief The sequence number of the message.
 *
 * @var uint8_t acknowledment
 * @brief The acknowledgement status (A = Yes, data valid, warning flag clear;
 * V = No, data invalid, warning flag set).
 */
typedef struct SENTENCE_ABK {
  TalkerID talkerId;
  SentenceID sentenceId = ABK;
  uint32_t mmsiAddress;
  uint8_t mmsiChannel;
  float m1373MessageId;
  uint8_t messageSequenceNumber;
  uint8_t acknowledment;
} SENTENCE_ABK;
#endif

#ifdef SENTENCE_ABM_ENABLED

/**
 * @brief AIS addressed binary and safety related message (ABM) sentence
 * structure.
 *
 * This structure represents information related to the AIS addressed binary and
 * safety related message (ABM) sentence. The ABM sentence supports ITU-R M.1371
 * Messages 6, 12, 25, and 26, providing an external application with a means to
 * exchange data via an AIS transponder.
 *
 * Data encapsulated in the ABM sentence is defined by the application, offering
 * great flexibility for implementing system functions that use the transponder
 * as a communications device.
 *
 * Upon receiving the ABM sentence via the IEC 61162-2 interface, the AIS
 * transponder initiates a VDL broadcast of Message 6, 12, 25, or 26. For
 * Messages 6 and 12, the AIS unit makes up to four broadcasts, the actual
 * number depending on the reception of an acknowledgement from the addressed
 * "destination" AIS unit.
 *
 * The success or failure of reception of the transmission by the addressed AIS
 * unit for Messages 6 and 12 is confirmed through the use of the "Addressed
 * binary and safety related message acknowledgement" (ABK) sentence formatter,
 * and the processes that support the generation of an ABK sentence.
 *
 * The AIS transponder determines the appropriate communications state for
 * transmission of Message 26 over the VHF data link.
 *
 * @var TalkerID talkerId
 * @brief The talker ID associated with the sentence.
 *
 * @var SentenceID sentenceId
 * @brief The unique identifier for the sentence (ABM).
 *
 * @var uint8_t totalSentenceNumber
 * @brief The total number of sentences in the message sequence.
 *
 * @var uint8_t sentenceNumber
 * @brief The sequence number of the current sentence.
 *
 * @var uint8_t sequentialMessageId
 * @brief The sequential message ID.
 *
 * @var uint32_t mmsiAddress
 * @brief The Maritime Mobile Service Identity (MMSI) address.
 *
 * @var uint8_t aisChannel
 * @brief The AIS channel.
 *
 * @var uint8_t m1373MessageId
 * @brief The ID of ITU-R M.1373 message.
 *
 * @var uint8_t encapsulatedData[ABM_DATA_MAX_LENGTH]
 * @brief The encapsulated data in the ABM sentence.
 *
 * @var uint8_t numberFillBits
 * @brief The number of fill bits in the sentence.
 */
typedef struct SENTENCE_ABM {
  TalkerID talkerId;
  SentenceID sentenceId = ABM;
  uint8_t totalSentenceNumber;
  uint8_t sentenceNumber;
  uint8_t sequentialMessageId;
  uint32_t mmsiAddress;
  uint8_t aisChannel;
  uint8_t m1373MessageId;
  uint8_t encapsulatedData[ABM_DATA_MAX_LENGTH];
  uint8_t numberFillBits;
} SENTENCE_ABM;
#endif

#ifdef SENTENCE_ACA_ENABLED
/**
 * @brief AIS channel assignment message (ACA) sentence structure.
 *
 * This structure represents information related to the AIS channel assignment
 * message (ACA) sentence. An AIS device can receive regional channel management
 * information in four ways: ITU-R M.1371 Message 22, DSC telecommand received
 * on channel 70, manual operator input, and an ACA sentence. The AIS unit may
 * store channel management information for future use. Channel management
 * information is applied based upon the actual location of the AIS device. An
 * AIS unit is “using” channel management information when the information is
 * being used to manage the operation of the VHF receiver and/or transmitter
 * inside the AIS unit.
 *
 * This sentence is used both to enter and obtain channel management
 * information. When sent to an AIS unit, the ACA sentence provides regional
 * information that the unit stores and uses to manage the internal VHF radio.
 * When sent from an AIS unit, the ACA sentence provides the current channel
 * management information retained by the AIS unit. The information contained in
 * this sentence is similar to the information contained in an ITU-R M.1371
 * Message 22. The information contained in this sentence directly relates to
 * the initialization phase and dual-channel operation and channel management
 * functions of the AIS unit as described in ITU-R M.1371.
 *
 * @var TalkerID talkerId
 * @brief The talker ID associated with the sentence.
 *
 * @var SentenceID sentenceId
 * @brief The unique identifier for the sentence (ACA).
 *
 * @var uint8_t sequenceNumber
 * @brief The sequence number of the ACA sentence.
 *
 * @var float neLatitude
 * @brief The latitude of the northeast corner of the geographic area.
 *
 * @var Polarity neLatitudePolarity
 * @brief The polarity of the latitude of the northeast corner of the geographic
 * area.
 *
 * @var float neLongitude
 * @brief The longitude of the northeast corner of the geographic area.
 *
 * @var Polarity neLongitudePolarity
 * @brief The polarity of the longitude of the northeast corner of the
 * geographic area.
 *
 * @var float swLatitude
 * @brief The latitude of the southwest corner of the geographic area.
 *
 * @var Polarity swLatitudePolarity
 * @brief The polarity of the latitude of the southwest corner of the geographic
 * area.
 *
 * @var float swLongitude
 * @brief The longitude of the southwest corner of the geographic area.
 *
 * @var Polarity swLongitudePolarity
 * @brief The polarity of the longitude of the southwest corner of the
 * geographic area.
 *
 * @var uint8_t transitionZoneSize
 * @brief The size of the transition zone.
 *
 * @var uint16_t channelA
 * @brief The frequency of channel A.
 *
 * @var uint8_t channelABandwidth
 * @brief The bandwidth of channel A. See ITU-R M.1084, Annex 4 for details.
 *
 * @var uint16_t channelB
 * @brief The frequency of channel B.
 *
 * @var uint8_t channelBBandwidth
 * @brief The bandwidth of channel B. See ITU-R M.1084, Annex 4 for details.
 *
 * @var uint8_t txRxMode
 * @brief The transmit/receive mode. See additional comments for details.
 *
 * @var uint8_t powerLevel
 * @brief The power level.
 *
 * @var uint8_t infoSource
 * @brief The information source. See additional comments for details.
 *
 * @var uint8_t inUseFlag
 * @brief The flag indicating if the channel management information is in use.
 * See additional comments for details.
 *
 * @var float inUseChangeTime
 * @brief The time when the channel management information became in use. See
 * additional comments for details.
 * @note This is the UTC time that the “In-use flag” field changed to the
 * indicated state. This field should be null when the sentence is sent to an
 * AIS unit.
 */
typedef struct SENTENCE_ACA {
  TalkerID talkerId;
  SentenceID sentenceId = ACA;
  uint8_t sequenceNumber;
  float neLatitude;
  Polarity neLatitudePolarity;
  float neLongitude;
  Polarity neLongitudePolarity;
  float swLatitude;
  Polarity swLatitudePolarity;
  float swLongitude;
  Polarity swLongitudePolarity;
  uint8_t transitionZoneSize;
  uint16_t channelA;
  uint8_t channelABandwidth;
  uint16_t channelB;
  uint8_t channelBBandwidth;
  uint8_t txRxMode;
  uint8_t powerLevel;
  uint8_t infoSource;
  uint8_t inUseFlag;
  float inUseChangeTime;
} SENTENCE_ACA;
#endif

#ifdef SENTENCE_ACK_ENABLED
/**
 * @brief Acknowledge alarm (ACK) sentence structure.
 *
 * This structure represents information related to the Acknowledge alarm (ACK)
 * sentence. The ACK sentence is used to acknowledge an alarm condition reported
 * by a device.
 *
 * @var TalkerID talkerId
 * @brief The talker ID associated with the sentence.
 *
 * @var SentenceID sentenceId
 * @brief The unique identifier for the sentence (ACK).
 *
 * @var uint16_t alarmId
 * @brief The unique identifier (alarm number) of the alarm being acknowledged.
 */
typedef struct SENTENCE_ACK {
  TalkerID talkerId;
  SentenceID sentenceId = ACK;
  uint32_t alarmId;
} SENTENCE_ACK;

#endif

#ifdef SENTENCE_ACN_ENABLED
/**
 * @brief Alert command (ACN) sentence structure.
 *
 * This structure represents information related to the Alert command (ACN)
 * sentence. ACN sentences, along with other related sentences like ALC, ALF,
 * and ARC, are used for alert handling as described in IEC 61924-2.
 *
 * @var TalkerID talkerId
 * @brief The talker ID associated with the sentence.
 *
 * @var SentenceID sentenceId
 * @brief The unique identifier for the sentence (ACN).
 *
 * @var float time
 * @brief The release time of the alert command. Optional field, can be null.
 *
 * @var uint8_t[3] manufacturerCode
 * @brief The manufacturer mnemonic code for proprietary alerts. Should be null
 * for standardized alerts.
 *
 * @var uint32_t alertIdentifier
 * @brief The unique identifier of the alert. Range: 10000-9999999. 0 reserved
 * for command request to all alerts.
 *
 * @var uint32_t alertInstance
 * @brief The alert instance identifies the current instance of an alert. Range:
 * 1 to 999999. 0 for all instances.
 *
 * @var uint8_t alertCommand
 * @brief The alert command:
 * - 'A': Acknowledge
 * - 'Q': Request/Repeat information
 * - 'O': Responsibility transfer
 * - 'S': Silence
 *
 * @var uint8_t sentenceStatusFlag
 * @brief The sentence status flag:
 * - 'C': Command
 * Should not be null, indicates a command. A sentence without 'C' is not a
 * command.
 */
typedef struct SENTENCE_ACN {
  TalkerID talkerId;
  SentenceID sentenceId = ACN;
  float time;
  uint8_t manufacturerMnemonic[3];
  uint32_t alertId;
  uint32_t alertInstance;
  uint8_t alertCommand;
  uint8_t statusFlag;
} SENTENCE_ACN;
#endif

#endif  // Header guard
