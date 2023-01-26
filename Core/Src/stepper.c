/**
 * @brief Various functions for stepper control
 * Stepper GPIO output pin label handled by STM32MX:
 * ENA: Connects all stepper ENA-
 * PUL: PUL1 for stepper 1
 * DIR: DIR1 for stepper 1
 *
 */

#include "robot.h"

void Stepper_PostInit(Stepper_HandleTypeDef * s);

Stepper_HandleTypeDef stepper[STEP_NUM];

void Stepper_Init(void)
{
  for (int i = 0; i < STEP_NUM; i++)
  {
    stepper[i].Index = i;
    Stepper_Reset(&stepper[i]);
    Stepper_PostInit(&stepper[i]);
  }
}
void Stepper_Reset(Stepper_HandleTypeDef *s)
{

  s->stepAcc = STEP_ACC_DEFAULT;
  s->stepSpeedLimit = STEP_RPM_DEFAULT;
  s->stepPos_target = 0;

  s->stepPos = 0;

  memset(s->step_map_index, 0, sizeof(s->step_map_index));

  memset(s->stepENA_map, STEP_DISABLE, sizeof(s->stepENA_map));
  memset(s->stepDIR_map, STEP_CW, sizeof(s->stepDIR_map));
  memset(s->stepARR_map, STEP_ARR_DEFAULT, sizeof(s->stepARR_map));
}

void Stepper_PostInit(Stepper_HandleTypeDef * s)
{
  switch (s->Index)
  {
  case 0:
    s->timer_handle = htim1;
    s->timer_channel = TIM_CHANNEL_1;
    s->ENA_Port = ENA1_GPIO_Port;
    s->ENA_Pin = ENA1_Pin;
    s->DIR_Port = DIR1_GPIO_Port;
    s->DIR_Pin = DIR1_Pin;
    break;
  case 1:
    s->timer_handle = htim2;
    s->timer_channel = TIM_CHANNEL_1;
    s->ENA_Port = ENA2_GPIO_Port;
    s->ENA_Pin = ENA2_Pin;
    s->DIR_Port = DIR2_GPIO_Port;
    s->DIR_Pin = DIR2_Pin;
    break;
  default:
    break;
  }

#ifdef STEP_REST
  HAL_GPIO_WritePin(s->ENA_Port, s->ENA_Pin, STEP_DISABLE);
#else
  HAL_GPIO_WritePin(s->ENA_Port, s->ENA_Pin, STEP_ENABLE);
#endif // STEP_REST
}

// void Stepper_GetSteps(float SpeedLevel, float stepACC)

void Stepper_Start(Stepper_HandleTypeDef s)
{
  HAL_TIM_PWM_Start_IT(&s.timer_handle, s.timer_channel);
}

void Stepper(void *argument)
{
  // HAL_TIM_PWM_Start_DMA();
  for (;;)
  {
    // timer_value = __HAL_TIM_GET_COUNTER(&htim1);

    osDelay(1);
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    HAL_TIM_OnePulse_Start(&htim2, TIM_CHANNEL_1);
    // HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_1);
  }
}

/**
 * @brief free(stepSpeedLevel)
 *
 * @param speed_start
 * @param speed_end
 * @param count
 * @return float*
 */
float *Stepper_GetSpeedLevel(float speed_start, float speed_end, uint8_t *count)
{
  uint8_t size;
  if (speed_start == speed_end)
    size = 1;
  else
    size = 2 + (int)abs(((speed_start - speed_end) / STEP_RPM_LEVEL));


  float *stepSpeedLevel = malloc(size);

  if (speed_start == speed_end)
    stepSpeedLevel[0] = speed_start;
  else
  {
    // linear series
    float speed_level = (speed_end - speed_start) / (size - 1);
    for (int i = 0; i < size; i++)
      stepSpeedLevel[i] = speed_start + i * speed_level;
  }
  *count = size;
  return stepSpeedLevel;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1)
  {

    TIM1->ARR = TIM1->ARR + 100;
  }
}

/****************************************************************
 * Functions below using osDelay between each step
 *
 ****************************************************************
 */

/**
 * @brief Toggle Pulse pin with osDelay
 */
void step_simplest(void)
{
  ENABLE_MOTORS;
  HAL_GPIO_TogglePin(PUL1_GPIO_Port, PUL1_Pin);
  osDelay(1);
}

/**
 * @brief
 *
 * @param steps number of steps
 * @param direction 0/1 CCL/CW
 * @param delay milliseconds delay between each pulse. 1 ms is ok
 */
void step_constantSpeed(int steps, uint8_t direction, uint8_t delay)
{
  ENABLE_MOTORS;
  HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR1_Pin, direction);
  for (int i = 0; i < steps; i++)
  {
    HAL_GPIO_WritePin(PUL1_GPIO_Port, PUL1_Pin, SET);
    osDelay(delay);
    HAL_GPIO_WritePin(PUL1_GPIO_Port, PUL1_Pin, RESET);
    osDelay(delay);
  }
  osDelay(1000);
}

/**
 * @brief Changing Step speed via changing osDelay delay
 * Interval change is linear, while speed change is not linear
 *
 * @param steps Number of steps
 */
void step_simpleAccel(int steps)
{
  int lowSpeed = 30;
  int highSpeed = 1;
  int change = 1;

  int rampUpStop = (lowSpeed - highSpeed) / change;
  if (rampUpStop > steps / 2)
    rampUpStop = steps / 2; // movement finished before reach highSpeed
  int rampDownStart = steps - rampUpStop;

  int delay = lowSpeed;

  ENABLE_MOTORS;
  for (int i = 0; i < steps; i++)
  {
    HAL_GPIO_WritePin(PUL1_GPIO_Port, PUL1_Pin, SET);
    osDelay(delay);
    HAL_GPIO_WritePin(PUL1_GPIO_Port, PUL1_Pin, RESET);
    osDelay(delay);

    if (i < rampUpStop)
      delay -= change;
    else if (i > rampDownStart)
      delay += change;
  }
  osDelay(1000);
}

/**
 * @brief Interstep delay with Taylor series approximation
 *
 * For detailed calculation, refer to documentation:
 * AVR446: Linear speed control of stepper motor,
 * 2.3.1 Exact calculations of the inter-step delay
 *
 * @param steps number of steps
 */
void step_constantAccel()
{
  /* Calculate delay for each step */
  int steps = 50;
  int delays[steps];
  int spr = 200;                  // steps per round
  float angle = 2 * pi / spr;     // angle per step (rad)
  float accel = 30;               // rad/s2
  float counterFreequency = 1000; // Hz
  float c0 = counterFreequency * sqrt(2 * angle / accel) * 0.67703;
  float lastDelay = 0;
  int highSpeed = 1; // minimum osDelay 1 milliseconds
  for (int i = 0; i < steps; i++)
  {
    float d = c0;
    if (i > 0)
    {
      d = lastDelay - 2 * lastDelay / (4 * i - 1);
      angle++;
    }
    if ((int)d <= highSpeed)
    {
      d = highSpeed;
      angle++;
    }
    delays[i] = d;
    lastDelay = d;
  }

  ENABLE_MOTORS;
  /* Acceleration */
  for (int i = 0; i < steps; i++)
  {
    HAL_GPIO_WritePin(PUL1_GPIO_Port, PUL1_Pin, RESET);
    osDelay(delays[i]);
    HAL_GPIO_WritePin(PUL1_GPIO_Port, PUL1_Pin, SET);
    osDelay(delays[i]);
  }
  /* Deceleration */
  for (int i = 0; i < steps; i++)
  {
    HAL_GPIO_WritePin(PUL1_GPIO_Port, PUL1_Pin, RESET);
    osDelay(delays[steps - i - 1]);
    HAL_GPIO_WritePin(PUL1_GPIO_Port, PUL1_Pin, SET);
    osDelay(delays[steps - i - 1]);
  }
  osDelay(1000);
}

/****************************************************************
 * Functions below use timer interrupts
 * ********************************
 * Timer configuration in SMT32MX:
 * Clock source: internal clock
 * Prescaler: HCLK(MHz) - 1 (for STM32F103 = 72-1)
 * Counter period: handled by function below
 * ****************************************************************
 */
volatile uint8_t remainingSteppersFlag = 0;
volatile uint8_t nextStepperFlag = 0;
/**
 * @brief Inside HAL_TIM_PeriodElapsedCallback
 *
 */

// void resetStepperInfo(stepperInfo si)
// {
//   si.n = 0;
//   si.d = 0;
//   si.di = 0;
//   si.stepCount = 0;
//   si.rampUpStepCount = 0;
//   si.totalSteps = 0;
//   si.stepPos = 0;
//   si.movementDone = false;
// }

// void resetStepper(volatile stepperInfo si)
// {
//   si.c0 = si.acceleration;
//   si.d = si.c0;
//   si.di = si.d;
//   si.stepCount = 0;
//   si.n = 0;
//   si.rampUpStepCount = 0;
//   si.movementDone = false;
// }

// void prepareMovement(int whichMotor, int steps)
// {
//   volatile stepperInfo si = steppers[whichMotor];
//   si.dirFunc(steps < 0 ? 1 : 0);
//   si.dir = steps > 0 ? 1 : -1;
//   si.totalSteps = abs(steps);
//   resetStepper(si);
//   remainingSteppersFlag |= (1 << whichMotor);
// }

// void setNextInterruptInterval()
// {
//   bool movementComplete = true;
//   UNUSED(movementComplete);

//   unsigned int mind = 999999;
//   for (int i = 0; i < STEP_NUM; i++)
//   {
//     if (((1 << i) & remainingSteppersFlag) && steppers[i].di < mind)
//     {
//       mind = steppers[i].di;
//     }
//   }

//   nextStepperFlag = 0;
//   for (int i = 0; i < STEP_NUM; i++)
//   {
//     if (!steppers[i].movementDone)
//       movementComplete = false;

//     if (((1 << i) & remainingSteppersFlag) && steppers[i].di == mind)
//       nextStepperFlag |= (1 << i);
//   }

//   if (remainingSteppersFlag == 0)
//   {
//     // OCR1A = 65500;
//   }

//   // OCR1A = mind;
// }

// /**
//  * @brief Generate S-curve for stepper motor speed:
//  * FStart < FStop, curve goes up;
//  * FStart > FStop, curve goes down;
//  * FStart = FStop, curve is flat horizontal line
//  * @param len number of samples
//  * @param FStart
//  * @param FStop
//  * @param flexible define curvature, larger means curvy. recommend 4-6
//  * @param index
//  * @return float
//  */
// float motorPower_PowerSLine(float len, float FStart, float FStop, float flexible, uint8_t index)
// {
//   float deno;
//   float melo;
//   float num;
//   float Fcurrent;

//   if (index > len)
//     index = len;
//   num = len / 2;
//   melo = flexible * (index - num) / num;
//   deno = 1.0 / (1 + expf(-melo));
//   Fcurrent = FStart - (FStart - FStop) * deno;

//   return Fcurrent;
// }