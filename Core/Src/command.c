#include "robot.h"

uint8_t command_line[USB_RX_SIZE];

uint8_t Execute_Command(uint8_t *line)
{
    /* Step 1: String to uppercase*/
    upperString(line);

    /* Step 2: Slice line into 2D arrays */
    int word_count = 0;
    char **words = split((char *)line, COMMAND_DELIMITER, &word_count);

    /* Step 3: First word is key, followed by parameters */
    uint32_t key = hash(words[0]);

/* debug */
#ifdef OLED_SHOW_HASH
    char msg[USB_TX_SIZE];
    snprintf(msg, sizeof(msg), "%d", hash((uint8_t *)words[0]));
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)msg, strlen(msg));
    OLED_Update_Info(msg);
#endif // OLED_SHOW_HASH

    switch (key)
    {
    case CMD_RESET:;
        switch (word_count)
        {
        case 1:;
            NVIC_SystemReset();
            break;
        case 3:;
            uint32_t target = hash(words[1]);
            switch (target)
            {
#ifdef FORCE_SENSOR_REQUEST_MODE
            case CMD_FS:;
                if (string_number_type(words[2]) != IsInteger)
                    return CMD_VALUE_NOT_INTEGER;
                uint8_t num = atoi((char *)words[2]);
                if (num < 0 || num > FORCE_SENSOR_NUM)
                    return CMD_VALUE_OUT_OF_RANGE;

                /* Stop periodic force reading request */
                xTimerStopFromISR(Force_Sensor_Request_TimeoutHandle, pdFALSE);

                /* wait for sometime to avoid conflict */
                osDelay(FORCE_SENSOR_REQUEST_TIMEOUT * 2);
                Force_Sensor_Zeroing(num);

                xTimerChangePeriodFromISR(Force_Sensor_Request_TimeoutHandle, FORCE_SENSOR_REQUEST_TIMEOUT, pdFALSE);
                break;
#endif

            default:
                return CMD_INVALID_TARGET;
                break;
            }
            break;
        default:;
            return CMD_TOO_MANY_PARAMETERS;
        }
        break;
    case CMD_BEEP:;
        switch (word_count)
        {
        case 1:;
            return CMD_VALUE_WORD_MISSING;
        case 2:;
            if (string_number_type(words[1]) != IsInteger)
                return CMD_VALUE_NOT_INTEGER;
            uint8_t num = atoi((char *)words[1]);
            if (num < 0 || num > 255)
                return CMD_VALUE_OUT_OF_RANGE;
            beep(num);
            break;
        default:;
            return CMD_TOO_MANY_PARAMETERS;
        }
        break;
    case CMD_ECHO:;
        switch (word_count)
        {
        case 1:;
            return CMD_VALUE_WORD_MISSING;
        case 2:;
            if (string_number_type(words[1]) != IsInteger)
                return CMD_VALUE_NOT_INTEGER;
            uint8_t num = atoi((char *)words[1]);
            if (num < 0 || num > 1)
                return CMD_VALUE_OUT_OF_RANGE;
            USB_ECHO = num;
            break;
        default:;
            return CMD_TOO_MANY_PARAMETERS;
        }
        break;
    case CMD_LED:;
        switch (word_count)
        {
        case 1:;
            return CMD_VALUE_WORD_MISSING;
        case 2:;
            if (string_number_type(words[1]) != IsInteger)
                return CMD_VALUE_NOT_INTEGER;
            uint8_t num = atoi((char *)words[1]);
            if (num < 0 || num > 1)
                return CMD_VALUE_OUT_OF_RANGE;
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, num);
            break;
        default:;
            return CMD_TOO_MANY_PARAMETERS;
        }
        break;
    case CMD_ENA:;
        switch (word_count)
        {
        case 1:;
            return CMD_VALUE_WORD_MISSING;
        case 2:;
            if (string_number_type(words[1]) != IsInteger)
                return CMD_VALUE_NOT_INTEGER;
            uint8_t num = atoi((char *)words[1]);
            if (num < 0 || num > 1)
                return CMD_VALUE_OUT_OF_RANGE;
            HAL_GPIO_WritePin(ENA_GPIO_Port, ENA_Pin, num);
            break;
        default:;
            return CMD_TOO_MANY_PARAMETERS;
        }
        break;
    case CMD_STOP:;
        switch (word_count)
        {
        case 2:;
            uint32_t target = hash(words[1]);
            switch (target)
            {
            default:
                return CMD_INVALID_TARGET;
                break;
            }
            break;
        default:
            return CMD_INVALID_TARGET;
            break;
        }
        break;
    case CMD_START:;
        switch (word_count)
        {
        case 2:;
            uint32_t target = hash(words[1]);
            switch (target)
            {
            default:
                return CMD_INVALID_TARGET;
                break;
            }
            break;
        default:
            return CMD_INVALID_TARGET;
            break;
        }
        break;
    default:;
        return CMD_UNSUPPORTED_KEY;
    }
    /* Step 4: Free memory */
    for (int i = 0; i < word_count; i++)
        free(words[i]); // free the dynamically allocated space for each string

    free(words); // free the dynamically allocated space for the array of pointers to strings

    /* Step 5: Clear buf */
    memset(line, '\0', strlen((char *)line));

    return CMD_OK;
}