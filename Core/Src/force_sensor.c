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
#endif
#ifdef _forceSensor_VALUE_FLOAT // response in 9ms
uint8_t Force_Sensor_Request_Tx[8] = {0x01, 0x03, 0x01, 0x00, 0x00, 0x12, 0xC4, 0x3B};
#endif

void Force_Sensor_Request_Timeout_Callback(void *argument)
{
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    HAL_UART_Transmit(&huart1, Force_Sensor_Request_Tx, sizeof(Force_Sensor_Request_Tx), 10);
}
