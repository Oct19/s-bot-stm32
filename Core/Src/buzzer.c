#include "robot.h"

uint8_t beep_count = 0;
bool beep_wait = true;
void beep(uint8_t times)
{
    if (times == 0)
        return;
    beep_count = times - 1;
    HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1);
    xTimerChangePeriodFromISR(Buzzer_TimeoutHandle, 150, pdFALSE);
}

void Buzzer_Timeout_Callback(void *argument)
{
    /* USER CODE BEGIN BuzzerCallback */
    HAL_TIM_PWM_Stop(&htim13, TIM_CHANNEL_1);
    if (beep_count > 0)
    {
        xTimerStart(Buzzer_TimeoutHandle, 150);
        if (!beep_wait)
        {
            HAL_TIM_PWM_Start(&htim13, TIM_CHANNEL_1);
            beep_count--;
        }
        beep_wait = !beep_wait;
    }
    /* USER CODE END BuzzerCallback */
}