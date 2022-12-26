/**
 * @file state.h
 * @author Cai Dingkun <caidingkun@outlook.com>
 * @brief
 * @version 0.1
 * @date 2022-12-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __STATE_H__
#define __STATE_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "robot.h"

#define STATE_UPDATE_DELAY_MS 10

// Define robot status codes. Valid values (0-255)
#define STATUS_OK 0
#define STATUS_EXPECTED_COMMAND_LETTER 1
#define STATUS_BAD_NUMBER_FORMAT 2
#define STATUS_INVALID_STATEMENT 3
#define STATUS_NEGATIVE_VALUE 4
#define STATUS_SETTING_DISABLED 5
#define STATUS_SETTING_STEP_PULSE_MIN 6
#define STATUS_SETTING_READ_FAIL 7
#define STATUS_IDLE_ERROR 8
#define STATUS_SYSTEM_GC_LOCK 9
#define STATUS_SOFT_LIMIT_ERROR 10
#define STATUS_OVERFLOW 11
#define STATUS_MAX_STEP_RATE_EXCEEDED 12
#define STATUS_CHECK_DOOR 13
#define STATUS_LINE_LENGTH_EXCEEDED 14
#define STATUS_TRAVEL_EXCEEDED 15
#define STATUS_INVALID_JOG_COMMAND 16
#define STATUS_SETTING_DISABLED_LASER 17

    extern uint8_t robot_status;

    void State_Init(void);
    void status_report(uint8_t status);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __STATE_H__
