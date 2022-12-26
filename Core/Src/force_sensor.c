/**
 * @file force_sensor.c
 * @author Cai Dingkun <caidingkun@outlook.com>
 * @brief Code for force sensor and multi-channel transmitter
 * @version 0.1
 * @date 2022-12-10
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "robot.h"

char force_sensor_Rx[FORCE_SENSOR_RX_SIZE];

/* Request reading for 9 channels, including CRC */
#ifdef _forceSensor_VALUE_LONG
uint8_t Force_Sensor_Request_Tx[8] = {0x01, 0x03, 0x03, 0x00, 0x00, 0x12, 0xC5, 0x83};
int32_t force_sensor_readings[NUM_FORCE_SENSORS];
#endif
#ifdef _forceSensor_VALUE_FLOAT // response in 9ms
uint8_t Force_Sensor_Request_Tx[8] = {0x01, 0x03, 0x01, 0x00, 0x00, 0x12, 0xC4, 0x3B};
float force_sensor_readings[NUM_FORCE_SENSORS];
#endif

void Force_Sensor_Init(void)
{
    /* Clear Rx */
    memset(force_sensor_Rx, '\0', FORCE_SENSOR_RX_SIZE);

    /* Force sensor start Rx */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)force_sensor_Rx, FORCE_SENSOR_RX_SIZE);

    /* Disable half-transferred interrupt */
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}

void Force_Sensor_start(void)
{
    // /* Force sensor periodic serial request */
    xTimerChangePeriodFromISR(Force_Sensor_Request_TimeoutHandle, FORCE_SENSOR_REQUEST_TIMEOUT, pdFALSE);
}

void Force_Sensor_Request_Timeout_Callback(void *argument)
{
    HAL_UART_Transmit_DMA(&huart1, Force_Sensor_Request_Tx, sizeof(Force_Sensor_Request_Tx));
}

/* Run inside HAL_UARTEx_RxEventCallback */
void Force_Sensor_Rx_Callback(void)
{
    for (uint8_t i = 0; i < NUM_FORCE_SENSORS; i++)
    {
        uint8_t buf[4];
        memcpy(buf, force_sensor_Rx + 3 + i * 4, 4);
        force_sensor_readings[i] = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    }

    /* start the DMA again */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)force_sensor_Rx, FORCE_SENSOR_RX_SIZE);
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}