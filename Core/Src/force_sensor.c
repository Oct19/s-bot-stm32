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

uint8_t force_sensor_Rx_Buf[FORCE_SENSOR_RX_SIZE];
uint8_t force_sensor_Rx[FORCE_SENSOR_RX_SIZE];
bool FORCE_RX_CONVERTING = false;

/* Request reading for 9 channels, including CRC */

uint8_t Force_Sensor_Request_Tx[8] = {0x01, 0x03, 0x03, 0x00, 0x00, 0x12, 0xC5, 0x83};

/* Unit: grams force, range from -10000(10kgF pull) to 10000(10kgF push) */
int16_t force_readings[FORCE_SENSOR_NUM];

void Force_Sensor_Init(void)
{
    /* Clear Rx */
    memset(force_sensor_Rx_Buf, '\0', FORCE_SENSOR_RX_SIZE);
    memset(force_sensor_Rx, '\0', FORCE_SENSOR_RX_SIZE);

    /* Force sensor periodic serial request */
    xTimerChangePeriodFromISR(Force_Sensor_Request_TimeoutHandle, FORCE_SENSOR_REQUEST_TIMEOUT, pdFALSE);
}

/**
 * @brief Reset sensor readings for specific channel
 *
 * @param channel_number 1~9 channel; 0 for all channels
 */
void Force_Sensor_Reset(uint8_t channel_number)
{
    uint8_t buf[13] = {0x01, 0x10, 0x03, 0x20, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (channel_number < 0 || channel_number > FORCE_SENSOR_NUM)
        return;
    if (!channel_number)
        buf[10] = 0x0A;
    else
        buf[10] = channel_number;

    /* Calculate CRC */
    uint16_t crc = ModRTU_CRC(buf, 11);
    buf[11] = (crc & 0x00FF);
    buf[12] = (crc & 0xFF00) >> 8;

    HAL_UART_Transmit(&FORCE_SENSOR_UARTx, buf, sizeof(buf), 10);
}

void Force_Sensor_Request_Timeout_Callback(void *argument)
{
    HAL_UART_Transmit_DMA(&FORCE_SENSOR_UARTx, Force_Sensor_Request_Tx, sizeof(Force_Sensor_Request_Tx));

    HAL_UARTEx_ReceiveToIdle_DMA(&FORCE_SENSOR_UARTx, (uint8_t *)force_sensor_Rx_Buf, FORCE_SENSOR_RX_SIZE);
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}

/* Run inside HAL_UARTEx_RxEventCallback */
void Force_Sensor_Rx_Callback(void)
{
    if (FORCE_RX_CONVERTING) // blocked when reading rx buffer
        return;
    memcpy(force_sensor_Rx, force_sensor_Rx_Buf, FORCE_SENSOR_RX_SIZE);
}

void Force_Sensor_Read_Rx()
{
    /* Check if Rx already been read and 1-st byte cleared */
    if (!force_sensor_Rx[0])
        return;

    FORCE_RX_CONVERTING = true; // during reading, new Rx ignored and lost
    uint8_t buf[4];
    for (uint8_t i = 0; i < FORCE_SENSOR_NUM; i++)
    {
        memcpy(buf, force_sensor_Rx + 3 + i * 4, 4);
        force_readings[i] = (buf[2] << 8) | buf[3];
    }
    FORCE_RX_CONVERTING = false;

    /* Set first byte to zero means Rx already read */
    memset(force_sensor_Rx, '\0', 1);
}