/**
 * @file    switch.h
 * @author  Cai Dingkun <caidingkun@outlook.com>
 * @brief   This file contains all the function prototypes for
 *          the stepper.c file (Limit switch part number: FC-SPX307Z)
 * @version 0.1
 * @date    2022-11-10
 *
 * @copyright Copyright (c) 2022
 *
 * ****************************************************************
 * Stepper Motor
 * Part number: 28HS4401-65N2-50
 * Current rating: 0.7A/phase
 * Step angle: 1.8 degrees
 * Lead screw increment: 0.01mm/step
 * **************************************************************
 * Stepper Motor Driver
 * Part number: UM242 (TB6600 family)
 * Half motor current during idel state
 * Voltage high: >3.5V
 * ENA: at least 5μs before DIR signal
 * DIR: at least 2μs before PUL signal
 * PUL: Both high and low signal should be longer than 2μs
 */
#ifndef __switch_H__
#define __switch_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "stdbool.h"

#define ENABLE_MOTORS       HAL_GPIO_WritePin(ENA_GPIO_Port, ENA_Pin, SET);
#define DISABLE_MOTORS      HAL_GPIO_WritePin(ENA_GPIO_Port, ENA_Pin, RESET);


typedef struct
{
    // externally defined parameters
    float acceleration;
    volatile unsigned int minStepInterval; // ie. max speed, smaller is faster
    void (*dirFunc)(int);
    void (*stepFunc)();

    // derived parameters
    unsigned int c0;   // step interval for first step, determines acceleration
    long stepPosition; // current position of stepper (total of all movements taken so far)

    // per movement variables (only changed once per movement)
    volatile int dir;                      // current direction of movement, used to keep track of position
    volatile unsigned int totalSteps;      // number of steps requested for current movement
    volatile bool movementDone;            // true if the current movement has been completed (used by main program to wait for completion)
    volatile unsigned int rampUpStepCount; // number of steps taken to reach either max speed, or half-way to the goal (will be zero until this number is known)

    // per iteration variables (potentially changed every interrupt)
    volatile unsigned int n;         // index in acceleration curve, used to calculate next interval
    volatile float d;                // current interval length
    volatile unsigned long di;       // above variable truncated
    volatile unsigned int stepCount; // number of steps completed in current movement
} stepperInfo;

#define NUM_STEPPERS 2
extern volatile stepperInfo steppers[NUM_STEPPERS];
extern volatile uint8_t remainingSteppersFlag;
extern volatile uint8_t nextStepperFlag;

void step_simplest(void);
void step_constantSpeed(int steps, uint8_t direction, uint8_t delay);
void step_simpleAccel(int steps);
void step_constantAccel();

void resetStepperInfo(stepperInfo si);
void resetStepper(volatile stepperInfo si);
void step_ISR();

#ifdef __cplusplus
}
#endif
#endif /*__ switch_H__ */
