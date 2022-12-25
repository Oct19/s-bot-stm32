#include "robot.h"

void Stepper(void *argument)
{
  /* Infinite loop */
  for (;;)
  {
    step_simpleAccel(400);
    osDelay(1);
  }
}
/****************************************************************
 * Functions below are simple, using osDelay
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
  int steps = 100;
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
 * Functions below are basic, use interrupts
 * Stepper GPIO output pin label handled by STM32MX:
 * ENA: Connects all motor ENA-
 * PUL: PUL1 for steppers[1], PULn for steppers[n]
 * DIR: DIR1 for steppers[1], DIRn for steppers[n]
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
void step_ISR()
{
  // HAL_TIM_Base_Start_IT(&htim1);
  //  TIM1->CCR3 = 50;
}

void resetStepperInfo(stepperInfo si)
{
  si.n = 0;
  si.d = 0;
  si.di = 0;
  si.stepCount = 0;
  si.rampUpStepCount = 0;
  si.totalSteps = 0;
  si.stepPosition = 0;
  si.movementDone = false;
}

void resetStepper(volatile stepperInfo si)
{
  si.c0 = si.acceleration;
  si.d = si.c0;
  si.di = si.d;
  si.stepCount = 0;
  si.n = 0;
  si.rampUpStepCount = 0;
  si.movementDone = false;
}

void prepareMovement(int whichMotor, int steps)
{
  volatile stepperInfo si = steppers[whichMotor];
  si.dirFunc(steps < 0 ? 1 : 0);
  si.dir = steps > 0 ? 1 : -1;
  si.totalSteps = abs(steps);
  resetStepper(si);
  remainingSteppersFlag |= (1 << whichMotor);
}

void setNextInterruptInterval()
{
  bool movementComplete = true;
  UNUSED(movementComplete);

  unsigned int mind = 999999;
  for (int i = 0; i < NUM_STEPPERS; i++)
  {
    if (((1 << i) & remainingSteppersFlag) && steppers[i].di < mind)
    {
      mind = steppers[i].di;
    }
  }

  nextStepperFlag = 0;
  for (int i = 0; i < NUM_STEPPERS; i++)
  {
    if (!steppers[i].movementDone)
      movementComplete = false;

    if (((1 << i) & remainingSteppersFlag) && steppers[i].di == mind)
      nextStepperFlag |= (1 << i);
  }

  if (remainingSteppersFlag == 0)
  {
    // OCR1A = 65500;
  }

  // OCR1A = mind;
}

/**
 * @brief Generate S-curve for stepper motor speed:
 * FStart < FStop, curve goes up;
 * FStart > FStop, curve goes down;
 * FStart = FStop, curve is flat horizontal line
 * @param len number of samples
 * @param FStart
 * @param FStop
 * @param flexible define curvature, larger means curvy. recommend 4-6
 * @param index
 * @return float
 */
float motorPower_PowerSLine(float len, float FStart, float FStop, float flexible, uint8_t index)
{
  float deno;
  float melo;
  float num;
  float Fcurrent;

  if (index > len)
    index = len;
  num = len / 2;
  melo = flexible * (index - num) / num;
  deno = 1.0 / (1 + expf(-melo));
  Fcurrent = FStart - (FStart - FStop) * deno;

  return Fcurrent;
}