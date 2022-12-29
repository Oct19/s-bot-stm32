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

#ifdef FORCE_SENSOR_REQUEST_MODE
/* Request reading for 9 channels, including CRC */
uint8_t Force_Sensor_Request_Tx[8] = {0x01, 0x03, 0x03, 0x00, 0x00, 0x12, 0xC5, 0x83};
#endif

/* Unit: grams force, range from -10000(10kgF pull) to 10000(10kgF push) */
int16_t force_readings[FORCE_SENSOR_NUM];

void Force_Sensor_Init(void)
{
#ifndef FORCE_SENSOR_REQUEST_MODE
    // Force_Sensor_Set_Mode(3);
#endif

    /* Clear Rx */
    memset(force_sensor_Rx_Buf, '\0', FORCE_SENSOR_RX_SIZE);
    memset(force_sensor_Rx, '\0', FORCE_SENSOR_RX_SIZE);

#ifdef FORCE_SENSOR_REQUEST_MODE
    /* Force sensor periodic serial request */
    xTimerChangePeriodFromISR(Force_Sensor_Request_TimeoutHandle, FORCE_SENSOR_REQUEST_TIMEOUT, pdFALSE);
#else
    HAL_UARTEx_ReceiveToIdle_DMA(&FORCE_SENSOR_UARTx, (uint8_t *)force_sensor_Rx_Buf, FORCE_SENSOR_RX_SIZE);
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
#endif
}


/* Run inside HAL_UARTEx_RxEventCallback */
void Force_Sensor_Rx_Callback(void)
{
    if (FORCE_RX_CONVERTING) // blocked when reading rx buffer
        return;
    memcpy(force_sensor_Rx, force_sensor_Rx_Buf, FORCE_SENSOR_RX_SIZE);

#ifndef FORCE_SENSOR_REQUEST_MODE
    HAL_UARTEx_ReceiveToIdle_DMA(&FORCE_SENSOR_UARTx, (uint8_t *)force_sensor_Rx_Buf, FORCE_SENSOR_RX_SIZE);
    __HAL_DMA_DISABLE_IT(&FORCE_SENSOR_DMA, DMA_IT_HT);
#endif
}

void Force_Sensor_Read_Rx()
{
    FORCE_RX_CONVERTING = true; // during reading, new Rx ignored and lost
    for (uint8_t i = 0; i < FORCE_SENSOR_NUM; i++)
    {
#ifdef FORCE_SENSOR_REQUEST_MODE
        uint8_t buf[4];
        memcpy(buf, force_sensor_Rx + 3 + i * 4, 4);
        force_readings[i] = (buf[2] << 8) | buf[3];
#else
        uint8_t buf[2];
        memcpy(buf, force_sensor_Rx + i * 2, 2);
        force_readings[i] = (buf[0] << 8) | buf[1];
#endif
    }
    FORCE_RX_CONVERTING = false;
}

/**
 * @brief Initialize the force sensor mode:
 * Send request periodiclly:    0    // Rx is 4 byte long 011003200002040000001E654F
 * Recieve float periodiclly:   1   // 2 byte long
 * Recieve long periodiclly:    2   *not working
 */
void Force_Sensor_Set_Mode(uint8_t mode)
{
    uint8_t buf[13] = {0x01, 0x10, 0x02, 0xBE, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    buf[10] = mode;

    /* Calculate CRC */
    uint16_t crc = ModRTU_CRC(buf, sizeof(buf) - 2);
    buf[11] = (crc & 0x00FF);
    buf[12] = (crc & 0xFF00) >> 8;

    HAL_UART_Transmit(&FORCE_SENSOR_UARTx, buf, sizeof(buf), 1);
}

/**
 * @brief The functions below are disabled because in RS485 auto report mode the box is not responding.
 * See `doc\FORCE_SENSOR\user note.txt` for more info
 * 
 */
#ifdef FORCE_SENSOR_REQUEST_MODE
/**
 * @brief To set baudrate as 115200: 0116022080002040000000601236
 *
 * @param baudrate
 */
void Force_Sensor_Set_BaudRate(uint32_t baudrate)
{
    uint8_t buf[13] = {0x01, 0x10, 0x02, 0xD0, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    switch (baudrate)
    {
    case 19200: // default
        buf[10] = 3;
        break;
    case 115200:
        buf[10] = 6;
        break;
    default:
        break;
    }
    uint16_t crc = ModRTU_CRC(buf, sizeof(buf) - 2);
    buf[11] = (crc & 0x00FF);
    buf[12] = (crc & 0xFF00) >> 8;

    HAL_UART_Transmit(&FORCE_SENSOR_UARTx, buf, sizeof(buf), 1);
}

/**
 * @brief Reset: 011003200002040000001E654F
 * 
 */
void Force_Sensor_Reset(void)
{
    uint8_t buf[13] = {0x01, 0x10, 0x03, 0x20, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x1E, 0x65, 0x4F};
    HAL_UART_Transmit(&FORCE_SENSOR_UARTx, buf, sizeof(buf), 1);
}

/**
 * @brief Reset sensor readings for specific channel
 *
 * @param channel_number 1~9 channel; 0 for all channels
 */
void Force_Sensor_Zeroing(uint8_t channel_number)
{
    uint8_t buf[13] = {0x01, 0x10, 0x03, 0x20, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (channel_number < 0 || channel_number > FORCE_SENSOR_NUM)
        return;
    if (!channel_number)
        buf[10] = 0x0A;
    else
        buf[10] = channel_number;

    /* Calculate CRC */
    uint16_t crc = ModRTU_CRC(buf, sizeof(buf) - 2);
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
#endif