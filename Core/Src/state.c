#include "robot.h"

uint8_t robot_status;
char state_report_message[30];

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

    Force_Sensor_Read_Rx();

#ifdef OLED_SHOW_FORCE_READING
    char msg[20];
    snprintf(msg, sizeof(msg), "%d", force_readings[0]);
    OLED_Update_Info(msg);
#endif // OLED_SHOW_FORCE_READING

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
  case STATUS_OK:; // STATUS_OK
    snprintf(state_report_message, sizeof(state_report_message), "\nOK\n");
    break;
  case CMD_UNSUPPORTED_KEY:;
    snprintf(state_report_message, sizeof(state_report_message), "\nCMD_UNSUPPORTED_KEY\n");
    break;
  case CMD_VALUE_WORD_MISSING:;
    snprintf(state_report_message, sizeof(state_report_message), "\nCMD_VALUE_WORD_MISSING\n");
    break;
  case CMD_VALUE_NOT_INTEGER:;
    snprintf(state_report_message, sizeof(state_report_message), "\nCMD_VALUE_NOT_INTEGER\n");
    break;
  case CMD_INVALID_TARGET:;
    snprintf(state_report_message, sizeof(state_report_message), "\nCMD_INVALID_TARGET\n");
    break;
  case CMD_NO_AXIS_WORDS:;
    snprintf(state_report_message, sizeof(state_report_message), "\nCMD_NO_AXIS_WORDS\n");
    break;
  case CMD_VALUE_OUT_OF_RANGE:;
    snprintf(state_report_message, sizeof(state_report_message), "\nCMD_VALUE_OUT_OF_RANGE\n");
    break;
  case CMD_TOO_MANY_PARAMETERS:;
    snprintf(state_report_message, sizeof(state_report_message), "\nCMD_TOO_MANY_PARAMETERS\n");
    break;
  default:
    return;
  }
    HAL_UART_Transmit(&huart2, (uint8_t *)state_report_message, strlen(state_report_message),10);
#ifdef OLED_DISPLAY_WARNING
  if (status)
  {
    snprintf(state_report_message, sizeof(state_report_message), "ERR:%d", status);
    OLED_Update_Warning(state_report_message);
  }
#endif // OLED_DISPLAY_WARNING
}