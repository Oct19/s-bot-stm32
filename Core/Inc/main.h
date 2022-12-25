/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "robot.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

  extern osThreadId_t LEDblinkTaskHandle;
  extern osThreadId_t OLEDdisplayTaskHandle;
  extern osThreadId_t UARTparserTaskHandle;
  extern osThreadId_t StepperTaskHandle;
  extern osTimerId_t OLED_Tx_TimeoutHandle;
  extern osTimerId_t OLED_Warning_TimeoutHandle;
  extern osTimerId_t OLED_Rx_TimeoutHandle;
  extern osTimerId_t USB_HelloHandle;
  extern osTimerId_t Buzzer_TimeoutHandle;
  extern osTimerId_t Force_Sensor_Request_TimeoutHandle;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define USART2_TX_Pin GPIO_PIN_2
#define USART2_TX_GPIO_Port GPIOA
#define USART2_RX_Pin GPIO_PIN_3
#define USART2_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_6
#define BUZZER_GPIO_Port GPIOA
#define ENA_Pin GPIO_PIN_12
#define ENA_GPIO_Port GPIOB
#define DIR1_Pin GPIO_PIN_13
#define DIR1_GPIO_Port GPIOB
#define PUL1_Pin GPIO_PIN_14
#define PUL1_GPIO_Port GPIOB
#define DIR2_Pin GPIO_PIN_15
#define DIR2_GPIO_Port GPIOB
#define PUL2_Pin GPIO_PIN_6
#define PUL2_GPIO_Port GPIOC
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define LIMIT_B_Pin GPIO_PIN_6
#define LIMIT_B_GPIO_Port GPIOB
#define LIMIT_B_EXTI_IRQn EXTI9_5_IRQn
#define LIMIT_A_Pin GPIO_PIN_7
#define LIMIT_A_GPIO_Port GPIOB
#define LIMIT_A_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
