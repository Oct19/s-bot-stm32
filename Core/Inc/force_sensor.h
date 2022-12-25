/**
 * @file force_sensor.h
 * @author Cai Dingkun <caidingkun@outlook.com>
 * @brief Code for force sensor and multi-channel transmitter
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
 * Sampling rate: 10,20,80,320Hz
 * ****************************
 * Example1: Read Channel 1 value, Long type
 * Tx: 01 03 03 00 00 02 C4 4F
 * Rx: 01 03 04 00 00 03 A7 BB 79
 * In Tx, 03 00 specify type Long
 * In Rx,  00 00 03 A7 is Long type of 935 in decimal format
 * ****************************
 * Example2: Read Channel 1 value, float type
 * Tx: 01 03 01 00 00 02 C5 F7 //010301000002C5F7
 * Rx: 01 03 04 42 BB 00 00 9E 6E
 * In Tx, 01 00 specify type float
 * In Rx, 42 BB 00 00  is float type of 93.5 in decimal format
 * ****************************
 * Example3: Read all 9 Channel, Long type
 * Tx: 01 03 03 00 00 12 C5 83 //010303000012C583
 * Rx: 01 03 24 AA AA AA AA BB BB BB BB CC CC CC CC DD DD DD DD
 * EE EE EE EE FF FF FF FF GG GG GG GG HH HH HH HH II II II II A1 33
 * In Rx, AA AA AA AA is the long value for channel 1
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

#define FORCE_SENSOR_RX_SIZE 42 // for 9 channels
extern char force_sensor_Rx[FORCE_SENSOR_RX_SIZE];

/* Choose force sensor readings data type: float or long */
#define _forceSensor_VALUE_FLOAT
// #define _forceSensor_VALUE_LONG


#ifdef __cplusplus
}
#endif

#endif
