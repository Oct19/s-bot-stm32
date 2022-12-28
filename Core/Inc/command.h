/**
 * @file commands.h
 * @author Cai Dingkun
 * @brief Line ending None
 * Type A: <Command>
 * Example: RESET // Reset STM32 board
 * Type B: <Command><SPACE><0/1>
 * Example: LED 1 // LED on
 * Type C: <Command><SPACE><Parameter1><SPACE><Parameter2>...<ParameterN>
 * Example: MOV x +3.5 y -1.5 z 0 // Move in x,y,z axis
 */
#ifndef _COMMAND_H_
#define _COMMAND_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "robot.h"

/* CMD Error Code */
#define CMD_OK 0
#define CMD_UNSUPPORTED_KEY 21
#define CMD_VALUE_WORD_MISSING 22
#define CMD_VALUE_NOT_INTEGER 23
#define CMD_INVALID_TARGET 24
#define CMD_NO_AXIS_WORDS 25
#define CMD_VALUE_OUT_OF_RANGE 26
#define CMD_TOO_MANY_PARAMETERS 27

/* Basic */
#define CMD_RESET 233970152
#define CMD_LED 193462106
#define CMD_BEEP 2088922945
#define CMD_ECHO 2089028676
#define CMD_START 235675955
#define CMD_STOP 2089550539
#define CMD_GET 193456677
#define CMD_SET 193469745

 /* Robotics */
#define CMD_ENA 193454777
#define CMD_MOT 193463541

/* Motion */
#define CMD_X 177661
#define CMD_Y 177662
#define CMD_Z 177663
#define CMD_HOMING 3046341607

/**
 * @brief Force sensor
 * RESET FS X : Zeroing sepecific channel. 0 <= int X <= FORCE_SENSOR_NUM, 0 zeroing all channels
 * 
 */
#define CMD_FS 5862302


    extern uint8_t command_line[COMMAND_MAX_LENGTH];

    uint8_t Execute_Command(uint8_t *line);

#ifdef __cplusplus
}
#endif

#endif // _COMMAND_H_