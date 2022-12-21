#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "config.h"

#define COMMAND_DELIMITER " " // space

/**
 * @brief
 * Type A: <Command>
 * Example: RESET // Reset STM32 board
 * Type B: <Command><SPACE><Parameter>
 * Example: LED 0 // Suspend LED blinking
 * Type C: <Command><SPACE><Parameter1><SPACE><Parameter2>...<ParameterN>
 * Example: MOV x +3.5 y -1.5 z 0 // Mov towards x_pos by 3.5, y_neg by 1.5, move to z origin
 *
 */

/* Hash for commands */
#define HASH_LED 193462106 //
#define HASH_RESET 233970152
#define HASH_ECHO 2089028676
#define HASH_ENA 193454777
#define HASH_HOME 2089149710
#define HASH_SET 193469745
#define HASH_GET 193456677
#define HASH_X 177661
#define HASH_Y 177662
#define HASH_Z 177663
#define HASH_MOT 193463541
#define HASH_BEEP 2088922945
#define HASH_START 235675955
#define HASH_STOP 2089550539

#define CMD_ErrorCode_NoErr ((uint8_t)0x00)          /*!< No Error */
#define CMD_ErrorCode_InvalidCMD ((uint8_t)0x01)       /*!< Invalid Command */
#define CMD_ErrorCode_NotEnoughParam ((uint8_t)0x02) /*!< Not enough parameters */
#define CMD_ErrorCode_InvalidParam ((uint8_t)0x03)     /*!< Invalid Parameter */


uint8_t Execute_Command(uint8_t * line);

#ifdef __cplusplus
}
#endif

#endif // _COMMANDS_H_