/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "main.h"

#include "stdlib.h"
#include "stdbool.h"
#include "ctype.h"
#include "string.h"
#include "stdint.h"
#include "math.h"

#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"

#include "printf.h"
#include "nuts_bolts.h"
#include "commands.h"
#include "ssd1306.h"
#include "USBserial.h"
#include "OLEDdisplay.h"
#include "stepper.h"

    typedef enum _ROBOT_STATE_HandleTypeDef
    {
        STATE_RESET, // 0 ignore Rx, homing procedure
        STATE_IDLE,  // 1 stepper off
        STATE_MOVE,  // 2 stepper destination != current position
        STATE_ERROR, // 3 cant homing,
    } ROBOT_STATE_HandleTypeDef;

    extern ROBOT_STATE_HandleTypeDef ROBOT_STATE;

extern osThreadId_t LEDblinkTaskHandle;
extern osThreadId_t OLEDdisplayTaskHandle;
extern osThreadId_t UARTparserTaskHandle;
extern osThreadId_t StepperTaskHandle;
extern osThreadId_t USBserialTaskHandle;
extern osTimerId_t OLED_Tx_TimeoutHandle;
extern osTimerId_t OLED_Warning_TimeoutHandle;
extern osTimerId_t OLED_Rx_TimeoutHandle;
extern osTimerId_t USB_HelloHandle;
#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H__ */
