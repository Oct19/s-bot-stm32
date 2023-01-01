/**
 * @file buzzer.h
 * @author 
 * @brief Buzzer module, low level trigger
 * @settings:
 * Prescaler: 256 - 1
 * Counter Period: 256 - 1
 * Channel1: PWM Generation CH1 (sound frequency)
 * PWM Pulse: 10 (sound level)
 * @version 0.1
 * @date 2022-12-31
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef _BUZZER_H_
#define _BUZZER_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "robot.h"

#define BUZZER_TIMER htim13
#define BUZZER_DURATION 150
void beep(uint8_t times);

#ifdef __cplusplus
}
#endif

#endif /* _BUZZER_H_ */