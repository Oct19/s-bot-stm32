/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "stdint.h"
#include "math.h"
#include "printf.h"
#include "main.h"
#include "cmsis_os.h"
#include "nuts_bolts.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "ssd1306.h"
#include "USBserial.h"
#include "OLEDdisplay.h"

    typedef enum _ROBOT_STATE_HandleTypeDef
    {
        STATE_RESET, // 0 ignore Rx, homing procedure
        STATE_IDLE,  // 1 stepper off
        STATE_MOVE,  // 2 stepper destination != current position
        STATE_ERROR, // 3 cant homing,
    } ROBOT_STATE_HandleTypeDef;

    extern ROBOT_STATE_HandleTypeDef ROBOT_STATE;

#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H__ */
