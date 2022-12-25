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

#define COMMAND_OK 0
#define COMMAND_UNSUPPORTED_KEY 21
#define COMMAND_VALUE_WORD_MISSING 22
#define COMMAND_VALUE_NOT_INTEGER 23
#define COMMAND_INVALID_TARGET 24
#define COMMAND_NO_AXIS_WORDS 25
#define COMMAND_MAX_VALUE_EXCEEDED 26

    /* Hash for commands */
    typedef enum _CMD_HASH_HandleTypeDef
    {
        /* Basic */
        HASH_LED = 193462106,
        HASH_BEEP = 2088922945,
        HASH_ECHO = 2089028676,
        HASH_RESET = 233970152,
        HASH_START = 235675955,
        HASH_STOP = 2089550539,
        HASH_GET = 193456677,
        HASH_SET = 193469745,
        /* Robotics */
        HASH_ENA = 193454777,
        HASH_HOMING = 3046341607,
        HASH_MOT = 193463541,
        HASH_STATE = 235676006,
        HASH_X = 177661,
        HASH_Y = 177662,
        HASH_Z = 177663
    } CMD_HASH_HandleTypeDef;
    extern CMD_HASH_HandleTypeDef CMD_HASH;

    extern uint8_t command_line[COMMAND_MAX_LENGTH];

    uint8_t Execute_Command(uint8_t *line);

#ifdef __cplusplus
}
#endif

#endif // _COMMAND_H_