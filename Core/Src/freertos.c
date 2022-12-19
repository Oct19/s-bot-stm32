/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "config.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for LEDblinkTask */
osThreadId_t LEDblinkTaskHandle;
const osThreadAttr_t LEDblinkTask_attributes = {
  .name = "LEDblinkTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for OLEDdisplayTask */
osThreadId_t OLEDdisplayTaskHandle;
const osThreadAttr_t OLEDdisplayTask_attributes = {
  .name = "OLEDdisplayTask",
  .stack_size = 300 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for UARTparserTask */
osThreadId_t UARTparserTaskHandle;
const osThreadAttr_t UARTparserTask_attributes = {
  .name = "UARTparserTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for StepperTask */
osThreadId_t StepperTaskHandle;
const osThreadAttr_t StepperTask_attributes = {
  .name = "StepperTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for USBserialTask */
osThreadId_t USBserialTaskHandle;
const osThreadAttr_t USBserialTask_attributes = {
  .name = "USBserialTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for OLED_Tx_Timeout */
osTimerId_t OLED_Tx_TimeoutHandle;
const osTimerAttr_t OLED_Tx_Timeout_attributes = {
  .name = "OLED_Tx_Timeout"
};
/* Definitions for OLED_Warning_Timeout */
osTimerId_t OLED_Warning_TimeoutHandle;
const osTimerAttr_t OLED_Warning_Timeout_attributes = {
  .name = "OLED_Warning_Timeout"
};
/* Definitions for OLED_Rx_Timeout */
osTimerId_t OLED_Rx_TimeoutHandle;
const osTimerAttr_t OLED_Rx_Timeout_attributes = {
  .name = "OLED_Rx_Timeout"
};
/* Definitions for USB_Hello */
osTimerId_t USB_HelloHandle;
const osTimerAttr_t USB_Hello_attributes = {
  .name = "USB_Hello"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void LEDblink(void *argument);
void OLEDdisplay(void *argument);
void UARTparser(void *argument);
void Stepper(void *argument);
void USBserial(void *argument);
void OLED_Tx_Timeout_Callback(void *argument);
void OLED_Warning_Timeout_Callback(void *argument);
void OLED_Rx_Timeout_Callback(void *argument);
void USB_Hello_Callback(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  OLED_Init();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of OLED_Tx_Timeout */
  OLED_Tx_TimeoutHandle = osTimerNew(OLED_Tx_Timeout_Callback, osTimerOnce, NULL, &OLED_Tx_Timeout_attributes);

  /* creation of OLED_Warning_Timeout */
  OLED_Warning_TimeoutHandle = osTimerNew(OLED_Warning_Timeout_Callback, osTimerOnce, NULL, &OLED_Warning_Timeout_attributes);

  /* creation of OLED_Rx_Timeout */
  OLED_Rx_TimeoutHandle = osTimerNew(OLED_Rx_Timeout_Callback, osTimerOnce, NULL, &OLED_Rx_Timeout_attributes);

  /* creation of USB_Hello */
  USB_HelloHandle = osTimerNew(USB_Hello_Callback, osTimerPeriodic, NULL, &USB_Hello_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of LEDblinkTask */
  LEDblinkTaskHandle = osThreadNew(LEDblink, NULL, &LEDblinkTask_attributes);

  /* creation of OLEDdisplayTask */
  OLEDdisplayTaskHandle = osThreadNew(OLEDdisplay, NULL, &OLEDdisplayTask_attributes);

  /* creation of UARTparserTask */
  UARTparserTaskHandle = osThreadNew(UARTparser, NULL, &UARTparserTask_attributes);

  /* creation of StepperTask */
  StepperTaskHandle = osThreadNew(Stepper, NULL, &StepperTask_attributes);

  /* creation of USBserialTask */
  USBserialTaskHandle = osThreadNew(USBserial, NULL, &USBserialTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_LEDblink */
/**
 * @brief  Function implementing the LEDblinkTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_LEDblink */
void LEDblink(void *argument)
{
  /* USER CODE BEGIN LEDblink */
  /* Infinite loop */
  for (;;)
  {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    osDelay(1000);
  }
  /* USER CODE END LEDblink */
}

/* USER CODE BEGIN Header_OLEDdisplay */
/**
 * @brief Function implementing the OLEDdisplayTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_OLEDdisplay */
__weak void OLEDdisplay(void *argument)
{
  /* USER CODE BEGIN OLEDdisplay */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END OLEDdisplay */
}

/* USER CODE BEGIN Header_UARTparser */
/**
 * @brief Function implementing the UARTparserTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_UARTparser */
__weak void UARTparser(void *argument)
{
  /* USER CODE BEGIN UARTparser */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END UARTparser */
}

/* USER CODE BEGIN Header_Stepper */
/**
 * @brief Function implementing the StepperTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Stepper */
__weak void Stepper(void *argument)
{
  /* USER CODE BEGIN Stepper */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END Stepper */
}

/* USER CODE BEGIN Header_USBserial */
/**
 * @brief Function implementing the USBserialTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_USBserial */
__weak void USBserial(void *argument)
{
  /* USER CODE BEGIN USBserial */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END USBserial */
}

/* OLED_Tx_Timeout_Callback function */
__weak void OLED_Tx_Timeout_Callback(void *argument)
{
  /* USER CODE BEGIN OLED_Tx_Timeout_Callback */

  /* USER CODE END OLED_Tx_Timeout_Callback */
}

/* OLED_Warning_Timeout_Callback function */
__weak void OLED_Warning_Timeout_Callback(void *argument)
{
  /* USER CODE BEGIN OLED_Warning_Timeout_Callback */

  /* USER CODE END OLED_Warning_Timeout_Callback */
}

/* OLED_Rx_Timeout_Callback function */
__weak void OLED_Rx_Timeout_Callback(void *argument)
{
  /* USER CODE BEGIN OLED_Rx_Timeout_Callback */

  /* USER CODE END OLED_Rx_Timeout_Callback */
}

/* USB_Hello_Callback function */
__weak void USB_Hello_Callback(void *argument)
{
  /* USER CODE BEGIN USB_Hello_Callback */

  /* USER CODE END USB_Hello_Callback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

