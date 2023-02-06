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

#define ENABLE_MOTORS HAL_GPIO_WritePin(ENA1_GPIO_Port, ENA1_Pin, SET);
#define DISABLE_MOTORS HAL_GPIO_WritePin(ENA1_GPIO_Port, ENA1_Pin, RESET);

// ENA pin high/low
#define STEP_ENABLE 0
#define STEP_DISABLE 1
// DIR pin cw/ccw
#define STEP_CW 0
#define STEP_CCW 1
// ARR default
#define t_prescaler 180
#define STEP_ARR_DEFAULT (F_CPU / (t_prescaler * STEP_RPM_DEFAULT * STEP_PER_REV / 60))

#define STEP_LEVEL_SIZE ((int)(3 * STEP_RPM_MAX / STEP_RPM_LEVEL))

    typedef struct _Stepper_HandleTypeDef
    {
        // index
        uint8_t Index;

        // timer
        TIM_HandleTypeDef timer_handle;
        uint32_t timer_channel;

        // connection
        GPIO_TypeDef *ENA_Port;
        uint32_t ENA_Pin;
        GPIO_TypeDef *DIR_Port;
        uint32_t DIR_Pin;

        // command parameters
        int32_t stepPos_target;
        float stepRPMLimit;
        float stepAcc;

        // variables update at every step
        volatile int32_t stepPos; // current position of stepper (total of all movements taken so far)

        // variables update at every level
        volatile float stepRPM;

        // intermediate values
        uint8_t update_level;
        int32_t stepPos_index[STEP_LEVEL_SIZE]; // step index for mapping
        float stepRPM_level[STEP_LEVEL_SIZE];

        // mapping for driver control
        bool stepENA_level[STEP_LEVEL_SIZE];     // power
        bool stepDIR_level[STEP_LEVEL_SIZE];     // direction
        uint32_t stepARR_level[STEP_LEVEL_SIZE]; // interval

        // for system checking
        bool stepUpdating;

    } Stepper_HandleTypeDef;

    extern Stepper_HandleTypeDef stepper[STEP_NUM];

    void Stepper_Init(void);
    void Stepper_Reset(Stepper_HandleTypeDef *s);
    void Stepper_Update(Stepper_HandleTypeDef *s);
    void Stepper_GetSpeedLevels(Stepper_HandleTypeDef *s, float speed_start, float speed_end,
                                uint8_t *size, float *speed_level);
    void Stepper_GetStepLevels(Stepper_HandleTypeDef *s, uint8_t size, float *speed_level, int32_t *stepnum_level);
    void Stepper_Start(Stepper_HandleTypeDef *s);

    void step_simplest(void);
    void step_constantSpeed(int steps, uint8_t direction, uint8_t delay);
    void step_simpleAccel(int steps);
    void step_constantAccel();

#ifdef __cplusplus
}
#endif
#endif /*__ switch_H__ */
