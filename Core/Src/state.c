#include "robot.h"

uint8_t robot_status;
char state_report_message[10];

void status_report(uint8_t status);

void StateUpdate(void *argument)
{

  for (;;)
  {
    if (*command_line)
    {
#ifdef OLED_DISPLAY_RX
      /* Update OLED Message */
      OLED_Update_Rx((char *)command_line);
#endif
      status_report(Execute_Command(command_line));
      // robot_status_report(robot_status);

      memset(command_line, '\0', sizeof(command_line));
    }
    char msg[20];
    snprintf(msg, sizeof(msg),"%ld", force_sensor_readings[0]);
    OLED_Update_Info(msg);

    osDelay(STATE_UPDATE_DELAY_MS);
  }
}

void State_Init(void)
{
  robot_status = STATUS_OK;
  memset(Execute_Command, '\0', sizeof(Execute_Command));
  memset(USB_Rx, '\0', sizeof(USB_Rx));
  memset(USB_Tx, '\0', sizeof(USB_Tx));
  USB_ECHO = USB_ECHO_DEFAULT;

  
}

void status_report(uint8_t status)
{
  switch (status)
  {
  case STATUS_OK: // STATUS_OK
    snprintf(state_report_message, sizeof(state_report_message), "OK\n");
    break;
  default:
    snprintf(state_report_message, sizeof(state_report_message), "ERROR:%d", status);
  }
#ifdef OLED_DISPLAY_WARNING
  if (status)
    OLED_Update_Warning(state_report_message);
#endif // OLED_DISPLAY_WARNING
#ifdef USE_USB
    if (status)
      HAL_UART_Transmit_DMA(&huart2, (uint8_t *)state_report_message, strlen(state_report_message));
#endif // USE_USB
}