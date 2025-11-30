/**
 * @file nmea_safety.c
 * @brief Safety and alarm systems module implementation
 *
 * Implements parsers for safety-related NMEA sentences including
 * alarm conditions, alert management, fire detection, man overboard,
 * and door status monitoring.
 */

#include "modules/nmea_safety.h"
#include "nmea_internal.h"
#include <string.h>

#if NMEA_MODULE_SAFETY_ENABLED

/* ========================================================================== */
/*                          ALR - SET ALARM STATE                             */
/* ========================================================================== */

#if NMEA_SENTENCE_ALR_ENABLED
/**
 * @brief Parse ALR - Set Alarm State
 *
 * Format: $--ALR,hhmmss.ss,xxx,A,A,c--c*hh
 * Example: $IIALR,120530.00,001,A,A,HEADING ALARM*hh
 *
 * Fields:
 * 0: Sentence ID (IIALR)
 * 1: Time of alarm condition change, UTC
 * 2: Unique alarm number (identifier) at alarm source
 * 3: Alarm condition (A = threshold exceeded, V = not exceeded)
 * 4: Alarm's acknowledge state (A = acknowledged, V = unacknowledged)
 * 5: Alarm's description text
 */
nmea_result_t nmea_parse_alr(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse time of alarm (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &safety->alarm_time);
    }

    /* Parse alarm ID (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        int32_t alarm_id = 0;
        if (nmea_parse_int(tokens->tokens[2], &alarm_id) == NMEA_OK) {
            safety->alarm_id = (uint16_t)alarm_id;
        }
    }

    /* Parse alarm condition (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        safety->alarm_active = (tokens->tokens[3][0] == 'A' || tokens->tokens[3][0] == 'a');
    }

    /* Parse acknowledge state (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        safety->alarm_state = tokens->tokens[4][0];
    }

    /* Parse alarm description (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        strncpy(safety->alarm_condition, tokens->tokens[5], sizeof(safety->alarm_condition) - 1);
        safety->alarm_condition[sizeof(safety->alarm_condition) - 1] = '\0';
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ALR_ENABLED */

/* ========================================================================== */
/*                          ALF - ALERT SENTENCE                              */
/* ========================================================================== */

#if NMEA_SENTENCE_ALF_ENABLED
/**
 * @brief Parse ALF - Alert Sentence
 *
 * Format: $--ALF,x,x,x,hhmmss.ss,a,a,a,aaa,x.x,x.x,x.x,x,c---c*hh
 * Example: $IIALF,1,1,0,124304.50,A,W,A,,192,1,1,0,LOST TARGET*hh
 *
 * Fields:
 * 0: Sentence ID (IIALF)
 * 1: Total number of ALF sentences for this message, 1 to 2
 * 2: Sentence number, 1 to 2
 * 3: Sequential message identifier, 0 to 9
 * 4: Time of last change
 * 5: Alert category (A, B or C)
 * 6: Alert priority (E=Emergency, A=Alarm, W=Warning, C=Caution)
 * 7: Alert state (A=Acknowledged, S=Silenced, N=Normal, O=Transferred, U=Rectified unack, V=Active unack)
 * 8: Manufacturer mnemonic code
 * 9: Alert identifier
 * 10: Alert instance, 1 to 999999
 * 11: Revision counter, 1 to 99
 * 12: Escalation counter, 0 to 9
 * 13: Alert text
 */
nmea_result_t nmea_parse_alf(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse time of last change (field 4) */
    if (tokens->token_count > 4 && !nmea_is_empty_token(tokens->tokens[4])) {
        nmea_parse_time(tokens->tokens[4], &safety->alarm_time);
    }

    /* Parse alert state (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        safety->alarm_state = tokens->tokens[7][0];
        /* Set active flag based on state */
        char state = tokens->tokens[7][0];
        safety->alarm_active = (state == 'A' || state == 'a' || state == 'S' || state == 's' ||
                               state == 'O' || state == 'o' || state == 'V' || state == 'v' ||
                               state == 'U' || state == 'u');
    }

    /* Parse alert identifier (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        int32_t alert_id = 0;
        if (nmea_parse_int(tokens->tokens[9], &alert_id) == NMEA_OK) {
            safety->alarm_id = (uint16_t)alert_id;
        }
    }

    /* Parse alert text (field 13) */
    if (tokens->token_count > 13 && !nmea_is_empty_token(tokens->tokens[13])) {
        strncpy(safety->alarm_condition, tokens->tokens[13], sizeof(safety->alarm_condition) - 1);
        safety->alarm_condition[sizeof(safety->alarm_condition) - 1] = '\0';
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ALF_ENABLED */

/* ========================================================================== */
/*                          ALC - CYCLIC ALERT LIST                           */
/* ========================================================================== */

#if NMEA_SENTENCE_ALC_ENABLED
/**
 * @brief Parse ALC - Cyclic Alert List
 *
 * Format: $--ALC,xx,xx,xx,x.x,aaa,x.x,x.x,x.x,…,aaa,x.x,x.x,x.x*hh
 * Example: $IIALC,01,01,00,2,ABC,123,1,5,XYZ,456,2,3*hh
 *
 * Fields:
 * 0: Sentence ID (IIALC)
 * 1: Total number of sentences for this message, 01 to 99
 * 2: Sentence number, 01 to 99
 * 3: Sequential message identifier, 00 to 99
 * 4: Number of alert entries
 * Repeating groups (each alert entry has 4 fields):
 * - Manufacturer mnemonic code
 * - Alert identifier
 * - Alert instance
 * - Revision counter
 *
 * Note: This sentence provides condensed alert information.
 * The current safety state structure can only store the first alert entry.
 */
nmea_result_t nmea_parse_alc(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse first alert identifier (field 6) if present */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        int32_t alert_id = 0;
        if (nmea_parse_int(tokens->tokens[6], &alert_id) == NMEA_OK) {
            safety->alarm_id = (uint16_t)alert_id;
        }
    }

    /* ALC provides a cyclic list of alerts */
    /* The current structure can only store limited information */
    /* A full implementation would store multiple alert entries */

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ALC_ENABLED */

/* ========================================================================== */
/*                       ALA - REPORT DETAILED ALARM CONDITION                */
/* ========================================================================== */

#if NMEA_SENTENCE_ALA_ENABLED
/**
 * @brief Parse ALA - Report Detailed Alarm Condition
 *
 * Format: $--ALA,hhmmss.ss,aa,aa,xx,xxx,a,a,c--c*hh
 * Example: $IIALA,120530.00,AB,CD,01,123,H,A,HIGH TEMPERATURE*hh
 *
 * Fields:
 * 0: Sentence ID (IIALA)
 * 1: Event time
 * 2: System indicator of original alarm source (2 chars)
 * 3: Sub-system equipment indicator of original alarm source (2 chars)
 * 4: Instance number of equipment/unit/item (2 digits)
 * 5: Type of alarm (3 digits, see Annex D)
 * 6: Alarm condition (N=normal, H=high, J=extreme high, L=low, K=extreme low, X=other)
 * 7: Alarm's acknowledged state (A=acknowledged, V=not acknowledged, B=broadcast, H=harbour, O=override)
 * 8: Alarm's description text
 */
nmea_result_t nmea_parse_ala(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse event time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &safety->alarm_time);
    }

    /* Parse type of alarm (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        int32_t alarm_type = 0;
        if (nmea_parse_int(tokens->tokens[5], &alarm_type) == NMEA_OK) {
            safety->alarm_id = (uint16_t)alarm_type;
        }
    }

    /* Parse alarm condition (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        char condition = tokens->tokens[6][0];
        /* Set active flag if not normal */
        safety->alarm_active = (condition != 'N' && condition != 'n');
    }

    /* Parse acknowledge state (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        safety->alarm_state = tokens->tokens[7][0];
    }

    /* Parse alarm description (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        strncpy(safety->alarm_condition, tokens->tokens[8], sizeof(safety->alarm_condition) - 1);
        safety->alarm_condition[sizeof(safety->alarm_condition) - 1] = '\0';
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ALA_ENABLED */

/* ========================================================================== */
/*                    AKD - ACKNOWLEDGE DETAIL ALARM CONDITION                */
/* ========================================================================== */

#if NMEA_SENTENCE_AKD_ENABLED
/**
 * @brief Parse AKD - Acknowledge Detail Alarm Condition
 *
 * Format: $--AKD,hhmmss.ss,aa,aa,xx,xxx,aa,aa,xx*hh
 * Example: $IIAKD,120530.00,AB,CD,01,123,EF,GH,02*hh
 *
 * Fields:
 * 0: Sentence ID (IIAKD)
 * 1: Time of acknowledgement
 * 2: System indicator of original alarm source (2 chars)
 * 3: Sub system equipment indicator of original alarm source (2 chars)
 * 4: Instance number of equipment/unit/item (2 digits)
 * 5: Type of alarm (3 digits)
 * 6: System indicator of system sending acknowledgement (2 chars)
 * 7: Sub system indicator of system sending acknowledgement (2 chars)
 * 8: Instance of equipment/unit/item sending acknowledgement (2 digits)
 */
nmea_result_t nmea_parse_akd(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse time of acknowledgement (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &safety->alarm_time);
    }

    /* Parse type of alarm (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        int32_t alarm_type = 0;
        if (nmea_parse_int(tokens->tokens[5], &alarm_type) == NMEA_OK) {
            safety->alarm_id = (uint16_t)alarm_type;
        }
    }

    /* AKD is an acknowledgement, so mark as acknowledged */
    safety->alarm_state = 'A';

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_AKD_ENABLED */

/* ========================================================================== */
/*                          ACN - ALERT COMMAND                               */
/* ========================================================================== */

#if NMEA_SENTENCE_ACN_ENABLED
/**
 * @brief Parse ACN - Alert Command
 *
 * Format: $--ACN,hhmmss.ss,aaa,x.x,x.x,c,a*hh
 * Example: $IIACN,120530.00,ABC,123,1,A,C*hh
 *
 * Fields:
 * 0: Sentence ID (IIACN)
 * 1: Time (release time of the alert command)
 * 2: Manufacturer mnemonic code
 * 3: Alert Identifier
 * 4: Alert Instance, 1 to 999999
 * 5: Alert command (A=acknowledge, Q=request/repeat, O=responsibility transfer, S=silence)
 * 6: Sentence status flag (C=command)
 */
nmea_result_t nmea_parse_acn(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &safety->alarm_time);
    }

    /* Parse alert identifier (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        int32_t alert_id = 0;
        if (nmea_parse_int(tokens->tokens[3], &alert_id) == NMEA_OK) {
            safety->alarm_id = (uint16_t)alert_id;
        }
    }

    /* Parse alert command (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        safety->alarm_state = tokens->tokens[5][0];
    }

    /* ACN is a command sentence - we store the command type in alarm_state */

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ACN_ENABLED */

/* ========================================================================== */
/*                          ACK - ACKNOWLEDGE ALARM                           */
/* ========================================================================== */

#if NMEA_SENTENCE_ACK_ENABLED
/**
 * @brief Parse ACK - Acknowledge Alarm
 *
 * Format: $--ACK,xxx*hh
 * Example: $IIACK,001*hh
 *
 * Fields:
 * 0: Sentence ID (IIACK)
 * 1: Unique alarm number (identifier) at alarm source
 */
nmea_result_t nmea_parse_ack(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse alarm ID (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        int32_t alarm_id = 0;
        if (nmea_parse_int(tokens->tokens[1], &alarm_id) == NMEA_OK) {
            safety->alarm_id = (uint16_t)alarm_id;
        }
    }

    /* ACK sentence indicates acknowledgement */
    safety->alarm_state = 'A';

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ACK_ENABLED */

/* ========================================================================== */
/*                          ARC - ALERT COMMAND REFUSED                       */
/* ========================================================================== */

#if NMEA_SENTENCE_ARC_ENABLED
/**
 * @brief Parse ARC - Alert Command Refused
 *
 * Format: $--ARC,hhmmss.ss,aaa,x.x,x.x,c*hh
 * Example: $IIARC,120530.00,ABC,123,1,A*hh
 *
 * Fields:
 * 0: Sentence ID (IIARC)
 * 1: Time (release time of Alert Command Refused)
 * 2: Manufacturer mnemonic code
 * 3: Alert identifier
 * 4: Alert instance, 1 to 999999
 * 5: Refused alert command (A=acknowledge, Q=request, O=responsibility transfer, S=silence)
 */
nmea_result_t nmea_parse_arc(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse time (field 1) */
    if (tokens->token_count > 1 && !nmea_is_empty_token(tokens->tokens[1])) {
        nmea_parse_time(tokens->tokens[1], &safety->alarm_time);
    }

    /* Parse alert identifier (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        int32_t alert_id = 0;
        if (nmea_parse_int(tokens->tokens[3], &alert_id) == NMEA_OK) {
            safety->alarm_id = (uint16_t)alert_id;
        }
    }

    /* Parse refused command (field 5) */
    if (tokens->token_count > 5 && !nmea_is_empty_token(tokens->tokens[5])) {
        safety->alarm_state = tokens->tokens[5][0];
    }

    /* ARC indicates a command was refused */

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_ARC_ENABLED */

/* ========================================================================== */
/*                          FIR - FIRE DETECTION                              */
/* ========================================================================== */

#if NMEA_SENTENCE_FIR_ENABLED
/**
 * @brief Parse FIR - Fire Detection
 *
 * Format: $--FIR,a,hhmmss.ss,aa,cc,xxx,xxx,a,a,c--c*hh
 * Example: $IIFIR,E,120530.00,FD,AB,123,001,A,A,FIRE DETECTED ZONE 1*hh
 *
 * Fields:
 * 0: Sentence ID (IIFIR)
 * 1: Message Type (S=Status section, E=Status each detector, F=Fault, D=Disabled)
 * 2: Event time
 * 3: Type of fire detection system (2 chars: FD=Generic, FH=Heat, FS=Smoke, etc.)
 * 4: First division indicator (2 chars)
 * 5: Second Division indicator (3 chars)
 * 6: Fire detector number or activation detection count (3 digits)
 * 7: Condition (A=Activation, V=Non-activation, X=Fault)
 * 8: Alarm's acknowledgement state (A=acknowledged, V=not acknowledged)
 * 9: Message description text
 */
nmea_result_t nmea_parse_fir(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse event time (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_time(tokens->tokens[2], &safety->alarm_time);
    }

    /* Parse fire detector number (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        int32_t detector_num = 0;
        if (nmea_parse_int(tokens->tokens[6], &detector_num) == NMEA_OK) {
            safety->alarm_id = (uint16_t)detector_num;
        }
    }

    /* Parse condition (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        char condition = tokens->tokens[7][0];
        /* A=Activation means fire detected */
        safety->alarm_active = (condition == 'A' || condition == 'a');
    }

    /* Parse acknowledgement state (field 8) */
    if (tokens->token_count > 8 && !nmea_is_empty_token(tokens->tokens[8])) {
        safety->alarm_state = tokens->tokens[8][0];
    }

    /* Parse message description (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        strncpy(safety->alarm_condition, tokens->tokens[9], sizeof(safety->alarm_condition) - 1);
        safety->alarm_condition[sizeof(safety->alarm_condition) - 1] = '\0';
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_FIR_ENABLED */

/* ========================================================================== */
/*                       MOB - MAN OVER BOARD NOTIFICATION                    */
/* ========================================================================== */

#if NMEA_SENTENCE_MOB_ENABLED
/**
 * @brief Parse MOB - Man Over Board Notification
 *
 * Format: $--MOB,hhhhh,a,hhmmss.ss,x,x,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxxxxxxx,x*hh
 * Example: $IIMOB,000FF,A,120530,1,0,120545,3723.46,N,12158.32,W,10,5,123456789,0*hh
 *
 * Fields:
 * 0: Sentence ID (IIMOB)
 * 1: MOB emitter ID (5-digit hex)
 * 2: MOB status (A=Activated, T=Test, M=Manual, V=Not in use, E=Error)
 * 3: UTC of MOB activation (hhmmss - no decimals)
 * 4: MOB position source (0=vessel, 1=emitter, 6=error)
 * 5: Days since activation
 * 6: UTC of position (hhmmss - no decimals)
 * 7-10: Latitude, N/S, Longitude, E/W
 * 11: Course over ground, degrees true (integer)
 * 12: Speed over ground (integer knots)
 * 13: MMSI of vessel
 * 14: Battery status (0=Good, 1=Low, 6=Error)
 */
nmea_result_t nmea_parse_mob(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse MOB status (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        char status = tokens->tokens[2][0];
        /* A=Activated or M=Manual means MOB active */
        safety->alarm_active = (status == 'A' || status == 'a' || status == 'M' || status == 'm');
        safety->alarm_state = status;
    }

    /* Parse UTC of MOB activation (field 3) */
    if (tokens->token_count > 3 && !nmea_is_empty_token(tokens->tokens[3])) {
        nmea_parse_time(tokens->tokens[3], &safety->alarm_time);
    }

    /* MOB emitter ID (field 1) could be stored as alarm_id, but it's hex */
    /* For simplicity, we set a fixed alarm_id for MOB events */
    safety->alarm_id = 999; /* Special ID for MOB */

    /* Set description for MOB */
    strncpy(safety->alarm_condition, "MAN OVERBOARD", sizeof(safety->alarm_condition) - 1);
    safety->alarm_condition[sizeof(safety->alarm_condition) - 1] = '\0';

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_MOB_ENABLED */

/* ========================================================================== */
/*                          DOR - DOOR STATUS DETECTION                       */
/* ========================================================================== */

#if NMEA_SENTENCE_DOR_ENABLED
/**
 * @brief Parse DOR - Door Status Detection
 *
 * Format: $--DOR,a,hhmmss.ss,aa,cc,xxx,xxx,a,a,c--c*hh
 * Example: $IIDOR,E,120530.00,WT,AB,123,001,O,C,DOOR 1 OPEN*hh
 *
 * Fields:
 * 0: Sentence ID (IIDOR)
 * 1: Message Type (S=Status section, E=Status single door, F=Fault)
 * 2: Event time
 * 3: Type of door monitoring system (2 chars: WT=Watertight, WS=Semi-watertight, FD=Fire, HD=Hull, OT=Other)
 * 4: First division indicator (2 chars)
 * 5: Second division indicator (3 chars)
 * 6: Door number or door open count (3 digits)
 * 7: Door status (O=Open, C=Closed, S=Secured, F=Free status, X=Fault)
 * 8: Water tight door switch setting (O=Harbour mode, C=Sea mode)
 * 9: Message description text
 */
nmea_result_t nmea_parse_dor(nmea_context_t *ctx, const nmea_tokens_t *tokens)
{
    if (ctx == NULL || tokens == NULL) {
        return NMEA_ERROR_NULL_PARAM;
    }

    nmea_safety_state_t *safety = &ctx->safety;

    /* Parse event time (field 2) */
    if (tokens->token_count > 2 && !nmea_is_empty_token(tokens->tokens[2])) {
        nmea_parse_time(tokens->tokens[2], &safety->alarm_time);
    }

    /* Parse door number (field 6) */
    if (tokens->token_count > 6 && !nmea_is_empty_token(tokens->tokens[6])) {
        int32_t door_num = 0;
        if (nmea_parse_int(tokens->tokens[6], &door_num) == NMEA_OK) {
            safety->alarm_id = (uint16_t)door_num;
        }
    }

    /* Parse door status (field 7) */
    if (tokens->token_count > 7 && !nmea_is_empty_token(tokens->tokens[7])) {
        char status = tokens->tokens[7][0];
        safety->alarm_state = status;
        /* O=Open or X=Fault could be considered alarm conditions */
        safety->alarm_active = (status == 'O' || status == 'o' || status == 'X' || status == 'x');
    }

    /* Parse message description (field 9) */
    if (tokens->token_count > 9 && !nmea_is_empty_token(tokens->tokens[9])) {
        strncpy(safety->alarm_condition, tokens->tokens[9], sizeof(safety->alarm_condition) - 1);
        safety->alarm_condition[sizeof(safety->alarm_condition) - 1] = '\0';
    }

    return NMEA_OK;
}
#endif /* NMEA_SENTENCE_DOR_ENABLED */

#endif /* NMEA_MODULE_SAFETY_ENABLED */
