/**
 * @brief force sensor and multi-channel transmitter
 * For sensor zeroing, calibration, and protocal, please refer 
 * to manual and user note in doc/FORCE_SENSOR
 ****************************************************************
 * Force sensor
 * Part number: DYMH-106
 * Axis: 1 axis push/pull
 * Range: 10kg
 * Voltage: 5-15V
 * Signal: +-20mV
 * Rated output: 0.9mV/V
 * Error: 0.3%F.S
 * Zero shifting: +-1%F.S
 * Connection:
 * Power:       +Red, -Black,
 * Singnal:     +Green, -White (pushing)
 * Singnal:     -Green, +White (pulling)
 * Shielding:   Yellow/Silver
 ***************************************************************
 * Force sensor transmitter
 * Part number: DY094
 * No. of channels: 9
 * Communication: RS232/RS485 modbus-RTU
 * Sampling rate: 10,20,80,320Hz, Default is 80
 * ****************************
 *
 * @version 0.1
 * @date 2022-12-09
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __FORCE_SENSOR_H__
#define __FORCE_SENSOR_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "robot.h"

#define FORCE_SENSOR_UARTx huart1
#define FORCE_SENSOR_DMA hdma_usart1_rx


#define FORCE_SENSOR_RX_SIZE (FORCE_SENSOR_NUM * 2 + 1) // 1 crc byte

    extern int16_t force_readings[FORCE_SENSOR_NUM];
    void Force_Sensor_Init(void);
    void Force_Sensor_Rx_Callback(void);
    void Force_Sensor_Read_Rx(void);
    void Force_Sensor_Zeroing(uint8_t channel_number);
    void Force_Sensor_Set_Mode(uint8_t mode);

// #define FORCE_SENSOR_REQUEST_MODE
#ifdef FORCE_SENSOR_REQUEST_MODE
#define FORCE_SENSOR_REQUEST_TIMEOUT 50
#endif

#ifdef __cplusplus
}
#endif

#endif
