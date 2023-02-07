/**
 * @brief Various functions for stepper control
 * Stepper GPIO output pin label handled by STM32MX:
 * ENA: Connects all stepper ENA-
 * PUL: PUL1 for stepper 1
 * DIR: DIR1 for stepper 1
 *
 */

#include "robot.h"

static void Stepper_PostInit(Stepper_HandleTypeDef *s);
static void Stepper_StationaryUpdate(Stepper_HandleTypeDef *s);
static void Stepper_RunningUpdate(Stepper_HandleTypeDef *s);
static void Stepper_GetStepPositionIndex(int32_t *num_of_steps, uint8_t count,
                                         int32_t step_offset, int32_t *stepPos_index);

Stepper_HandleTypeDef stepper[STEP_NUM];

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

#ifdef OLED_DEBUG

    uint8_t count = 3;
    char msg[OLED_INFO_SIZE];
    memset(msg, '\0', sizeof(msg));
    for (uint8_t i = 0; i < min(count, 3); i++)
    {
      char buf[8];
      snprintf(buf, sizeof(buf), "%.1f,", stepper[1].stepRPM_level[i]);
      // snprintf(buf, sizeof(buf), "%d,", stepLevels[i]);

      strcat(msg, buf);
    }
    OLED_Update_Info(msg);
#endif // OLED_DEBUG
  }
}

void Stepper_Init(void)
{
  for (int i = 0; i < STEP_NUM; i++)
  {
    stepper[i].Index = i;
    Stepper_Reset(&stepper[i]);
    Stepper_PostInit(&stepper[i]);
  }
}
static void Stepper_PostInit(Stepper_HandleTypeDef *s)
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

void Stepper_Reset(Stepper_HandleTypeDef *s)
{

  s->stepAcc = STEP_ACC_DEFAULT;
  s->stepRPMLimit = STEP_RPM_DEFAULT;
  s->stepPos_target = 0;

  s->stepPos = 0;

  memset(s->stepPos_index, 0, sizeof(s->stepPos_index));

  memset(s->stepARR_level, STEP_ARR_DEFAULT, sizeof(s->stepARR_level));
}

/**
 * @brief
 *
 * @param s
 * @TODO: write callback function
 */
void Stepper_Start(Stepper_HandleTypeDef *s)
{
  // the pwm should toggle near the end of the pwm cycle,
  // give engouth time for ENA and DIR pin to update
  s->update_level = 0;
  HAL_GPIO_WritePin(s->ENA_Port, s->ENA_Pin, STEP_ENABLE);
  HAL_GPIO_WritePin(s->DIR_Port, s->DIR_Pin, s->stepRPM_level[0] > 0);
  HAL_TIM_PWM_Start_IT(&s->timer_handle, s->timer_channel);
}

static void Stepper_StationaryUpdate(Stepper_HandleTypeDef *s)
{
  int32_t start_pos = s->stepPos;
  if (s->update_level)
    start_pos = s->stepPos_index[s->update_level - 1];

  int32_t step_diff = s->stepPos_target - start_pos;
  float delta_t = STEP_RPM_LEVEL / s->stepAcc;

  if (step_diff == 0)
  {
    /**
     * Case 0: No need to rotate
     */
    s->stepRPM_level[s->update_level] = 0;
    return;
  }

  if (abs(step_diff) <= STEP_RPM_MIN * delta_t)
  {
    /**
     * Case 1: small travel distance, rotate at constant speed: STEP_RPM_MIN
     * ^
     * |
     * |
     * |
     * |____
     * *------------------->
     */
    s->stepRPM_level[s->update_level] = step_diff > 0 ? STEP_RPM_MIN : -STEP_RPM_MIN;
    s->stepPos_index[s->update_level] = s->stepPos_index[s->update_level - 1] + step_diff;
    s->update_level++;
    s->stepRPM_level[s->update_level] = 0;
    return;
  }

  /**
   * Case 2: Stepper will accelerate, cruise, and decelrate to stop;
   *  stepRPMLimit maynot reached if travel distance is small
   * ^
   * |
   * |   _
   * |  / \
   * | /   \
   * *------------------->
   * */
  float speed_level[STEP_LEVEL_SIZE];
  uint8_t levels_to_max_speed;
  Stepper_GetSpeedLevels(s, 0, s->stepRPMLimit, &levels_to_max_speed, speed_level);

  int32_t num_of_steps[levels_to_max_speed];
  Stepper_GetStepLevels(s, levels_to_max_speed, speed_level, num_of_steps);

  int32_t stepPos_index[levels_to_max_speed];
  Stepper_GetStepPositionIndex(num_of_steps, levels_to_max_speed, start_pos, stepPos_index);

  for (uint8_t i = levels_to_max_speed; i >= 2; i--)
  {
    int32_t stepSumAccel = stepPos_index[levels_to_max_speed - 1] - start_pos;
    int32_t stepSumDecel = stepPos_index[levels_to_max_speed - 2] - start_pos;
    int32_t stepSum = stepSumAccel + stepSumDecel;
    if (abs(stepSum) < abs(step_diff))
      break;
    levels_to_max_speed--;
  }

  // Acceleration
  memcpy(s->stepRPM_level + s->update_level, speed_level, levels_to_max_speed * sizeof(float));
  memcpy(s->stepPos_index + s->update_level, stepPos_index, levels_to_max_speed * sizeof(num_of_steps[0]));
  for (int i = 0; i < levels_to_max_speed; i++)
  {
    s->stepPos_index[s->update_level + i] += start_pos;
  }

  s->update_level += levels_to_max_speed - 1; // Overlap 1 speed level with deceleration phase

  // Deceleration
  reverse_array(speed_level, levels_to_max_speed);
  reverse_array(num_of_steps, levels_to_max_speed);
  Stepper_GetStepPositionIndex(num_of_steps, levels_to_max_speed, s->stepPos_index[s->update_level - 1], stepPos_index);
  memcpy(s->stepRPM_level + s->update_level, speed_level, (levels_to_max_speed) * sizeof(float));
  memcpy(s->stepPos_index + s->update_level, stepPos_index, (levels_to_max_speed) * sizeof(num_of_steps[0]));
  // Get cruise distance to offset deceleration phase
  step_diff = s->stepPos_target - s->stepPos_index[s->update_level + levels_to_max_speed - 1];
  for (int i = 0; i < levels_to_max_speed; i++)
  {
    s->stepPos_index[s->update_level + i] += step_diff;
  }

  s->update_level += levels_to_max_speed;
}

/**
 * @brief
 *
 * @param s
 * @TODO: Case 1
 */
static void Stepper_RunningUpdate(Stepper_HandleTypeDef *s)
{
  float speed_level[STEP_LEVEL_SIZE];
  int32_t num_of_steps[STEP_LEVEL_SIZE];
  uint8_t levels_to_stop;

  int32_t step_diff = s->stepPos_target - s->stepPos;

  Stepper_GetSpeedLevels(s, s->stepRPM, 0, &levels_to_stop, speed_level);
  Stepper_GetStepLevels(s, levels_to_stop, speed_level, num_of_steps);
  int32_t stepPos_index[levels_to_stop];
  Stepper_GetStepPositionIndex(num_of_steps, levels_to_stop, s->stepPos, stepPos_index);
  int32_t steps_to_stop = stepPos_index[levels_to_stop] - s->stepPos;
  bool initial_moving_towards_target = ((steps_to_stop < 0) == (step_diff < 0));
  if (abs(steps_to_stop) < abs(step_diff) && initial_moving_towards_target)
  {
    // Case 1: Decel to RPM_Limit, or accel to RPM_Limit or lower
  }
  else
  {
    // Case 1: decelrate to stop, then proceed to stationary update
    memcpy(s->stepRPM_level, speed_level, levels_to_stop * sizeof(float));
    s->update_level = levels_to_stop;
    Stepper_StationaryUpdate(s);
  }
}

/**
 * @brief stepPos may update during calculation, but it is ok!
 * stepPos is absolute, the generated step_level_index is also based on absolute position
 * @param s stepper handle
 * @TODO: Update ARR, DIR, ENA after generate stepRPM and stepPos_index
 */
void Stepper_Update(Stepper_HandleTypeDef *s)
{
  s->stepUpdating = true;
  s->update_level = 0;

  if (s->stepRPM == 0)
    Stepper_StationaryUpdate(s);
  else
    Stepper_RunningUpdate(s);

  s->update_level--;
  // update arr, dir, ena array
  // ...

  int c = F_CPU / (T_PRESCALER * STEP_PER_REV);
  for (int i = 0; i < s->update_level; i++)
  {
    if (s->stepRPM_level[i] == 0)
    {
      s->stepARR_level[i] = c / s->stepRPM_level[i];
    }
  }
  s->stepUpdating = false;

  Stepper_Start(s);
}

void Stepper_GetSpeedLevels(Stepper_HandleTypeDef *s, float speed_start, float speed_end,
                            uint8_t *size, float *speed_level)
{
  float speed_diff = (speed_end - speed_start);
  uint8_t count = 1 + (int)abs(speed_diff / STEP_RPM_LEVEL);
  *size = count;

  speed_level[count - 1] = speed_end;

  for (int i = 0; i < count - 1; i++)
  {
    speed_level[i] = speed_start + (i + 1) * speed_diff / count;
    if (abs(speed_level[i] < STEP_RPM_MIN))
      speed_level[i] = speed_level[i] > 0 ? STEP_RPM_MIN : -STEP_RPM_MIN;
  }
}

void Stepper_GetStepLevels(Stepper_HandleTypeDef *s, uint8_t size, float *speed_level, int32_t *num_of_steps)
{
  float delta_t = STEP_RPM_LEVEL / s->stepAcc;
  for (uint8_t i = 0; i < size; i++)
  {
    num_of_steps[i] = delta_t * speed_level[i] * STEP_PER_REV;
  }
}

static void Stepper_GetStepPositionIndex(int32_t *num_of_steps, uint8_t count,
                                         int32_t step_offset, int32_t *stepPos_index)
{
  stepPos_index[0] = num_of_steps[0] + step_offset;
  for (uint8_t i = 1; i < count; i++)
  {
    stepPos_index[i] = stepPos_index[i - 1] + num_of_steps[i];
  }
}

/**
 * @brief 
 * 
 * @param s 
 * @TODO: delay timer for stepper dwelling
 */
void Stepper_ISR(Stepper_HandleTypeDef *s)
{
  s->stepPos++;
  if (s->stepPos >= s->stepPos_target)
  {
    HAL_TIM_PWM_Stop_IT(&s->timer_handle,s->timer_channel);
    // One pulse delay timer for step dwelling
    HAL_GPIO_WritePin(&s->ENA_Port, s->ENA_Pin, STEP_DISABLE);
  }
  if (s->stepPos >= s->stepPos_index[s->update_level])
  {
    s->update_level++;
    bool dir = s->stepRPM_level[s->update_level] > 0;
    HAL_GPIO_WritePin(s->DIR_Port, s->DIR_Pin, dir);
    s->timer_handle.Instance->ARR = s->stepARR_level[s->update_level];
  }
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  for (int i = 0; i < STEP_NUM; i++)
  {
    if (htim->Instance == stepper[i].timer_handle.Instance)
    {
      Stepper_ISR(&stepper[i]);
    }
  }
  // if (htim->Instance == TIM1)
  // {
  //   TIM1->ARR = TIM1->ARR + 100;
  // }
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

// void Stepper_GetStepLevel(SpeedInfo_HandleTypeDef *si)
// {
//   float delta_t = STEP_RPM_LEVEL / si->acceleration;
//   for (uint8_t i = 0; i < si->size; i++)
//   {
//     si->number_of_steps[i] = delta_t * si->speed_level[i] * STEP_PER_REV;
//   }
// }
// /**
//  * @brief
//  *
//  * if speed_difference < STEP_RPM_LEVEL, return speed_end;
//  * for linear speed map, if abs(speed) < STEP_RPM_MIN , speed set to 0
//  */
// void Stepper_GetSpeedLevel(SpeedInfo_HandleTypeDef *si)
// {
//   float speed_diff = (si->speed_end - si->speed_start);
//   uint8_t count = 1 + (int)abs(speed_diff / STEP_RPM_LEVEL);
//   si->size = count;

//   si->speed_level[count - 1] = si->speed_end;

//   for (int i = 0; i < count - 1; i++)
//   {
//     float buf = si->speed_start + (i + 1) * speed_diff / count;
//     si->speed_level[i] = abs(buf) < STEP_RPM_MIN ? 0 : buf;
//   }
// }

// int32_t Stepper_SumSteps(int32_t *num_of_steps, uint8_t count)
// {
//   int32_t stepSum = 0;
//   for (uint8_t i = 0; i < count; i++)
//     stepSum = stepSum + num_of_steps[count];
//   return stepSum;
// }