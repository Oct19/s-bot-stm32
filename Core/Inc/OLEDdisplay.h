/**
 * @file    display.h
 * @author  Dingkun <https://github.com/oct19>
 * @brief   This file contains all the function prototypes for
 *           the display.c file
 * @version 0.1
 * @date    2022-11-09
 *
 * @copyright Copyright (c) 2022
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OLEDDISPLAY_H__
#define __OLEDDISPLAY_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define OLED_SHORT_MESSAGE_SIZE 10
#define OLED_LONG_MESSAGE_SIZE 20
#define OLED_WARNING_SIZE OLED_SHORT_MESSAGE_SIZE
#define OLED_TX_SIZE OLED_LONG_MESSAGE_SIZE
#define OLED_RX_SIZE OLED_LONG_MESSAGE_SIZE
#define OLED_INFO_SIZE OLED_LONG_MESSAGE_SIZE

#define OLED_MESSAGE_LINGER_MS 1000 // if no new message to display in current priority level
#define OLED_FPS 10                 // 1 ~ 10

    typedef struct _OLED_HandleTypeDef
    {
        uint8_t Warning[OLED_WARNING_SIZE]; // priority High
        uint8_t Tx[OLED_TX_SIZE];           // priority Normal
        uint8_t Rx[OLED_RX_SIZE];           // priority Normal
        uint8_t Info[OLED_INFO_SIZE];       // priority Low
    } OLED_HandleTypeDef;

    extern OLED_HandleTypeDef OLED;

    void OLED_Init(void);
    void OLED_display_welcome(void);
    void OLED_display_off(void);
    void OLED_Update_Warning(uint8_t *msg, size_t size);
    void OLED_Update_Rx(uint8_t *Rx, size_t size);
    void OLED_Update_Tx(uint8_t *Tx, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __OLEDDISPLAY_H__ */