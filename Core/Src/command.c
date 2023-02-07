#include "robot.h"

uint8_t command_line[USB_RX_SIZE];

/**
 * @brief After CMD_ERROR_FLAG has been updated, goto end of
 * the function to free memories
 *
 * @param line
 * @return uint8_t
 */
uint8_t Execute_Command(uint8_t *line)
{
    uint8_t CMD_ERROR_FLAG = CMD_OK;
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
                {
                    CMD_ERROR_FLAG = CMD_VALUE_NOT_INTEGER;
                    goto free;
                }
                uint8_t num = atoi((char *)words[2]);
                if (num < 0 || num > FORCE_SENSOR_NUM)
                {
                    CMD_ERROR_FLAG = CMD_VALUE_OUT_OF_RANGE;
                    goto free;
                }
                /* Stop periodic force reading request */
                xTimerStopFromISR(Force_Sensor_Request_TimeoutHandle, pdFALSE);

                /* wait for sometime to avoid conflict */
                osDelay(FORCE_SENSOR_REQUEST_TIMEOUT * 2);
                Force_Sensor_Zeroing(num);

                xTimerChangePeriodFromISR(Force_Sensor_Request_TimeoutHandle, FORCE_SENSOR_REQUEST_TIMEOUT, pdFALSE);
                break;
#endif

            default:
                CMD_ERROR_FLAG = CMD_INVALID_TARGET;
                goto free;
            }
            break;
        default:;
            CMD_ERROR_FLAG = CMD_TOO_MANY_PARAMETERS;
            goto free;
        }
        break;
    case CMD_BEEP:;
        switch (word_count)
        {
        case 1:;
            CMD_ERROR_FLAG = CMD_VALUE_WORD_MISSING;
            goto free;
        case 2:;
            if (string_number_type(words[1]) != IsInteger)
            {
                CMD_ERROR_FLAG = CMD_VALUE_NOT_INTEGER;
                goto free;
            }
            uint8_t num = atoi((char *)words[1]);
            if (num < 0 || num > 255)
            {
                CMD_ERROR_FLAG = CMD_VALUE_OUT_OF_RANGE;
                goto free;
            }
            beep(num);
            break;
        default:;
            CMD_ERROR_FLAG = CMD_TOO_MANY_PARAMETERS;
            goto free;
        }
        break;
    case CMD_ECHO:;
        switch (word_count)
        {
        case 1:;
            CMD_ERROR_FLAG = CMD_VALUE_WORD_MISSING;
            goto free;
        case 2:;
            if (string_number_type(words[1]) != IsInteger)
            {
                CMD_ERROR_FLAG = CMD_VALUE_NOT_INTEGER;
                goto free;
            }
            uint8_t num = atoi((char *)words[1]);
            if (num < 0 || num > 1)
            {
                CMD_ERROR_FLAG = CMD_VALUE_OUT_OF_RANGE;
                goto free;
            }
            USB_ECHO = num;
            break;
        default:;
            CMD_ERROR_FLAG = CMD_TOO_MANY_PARAMETERS;
            goto free;
        }
        break;
    case CMD_LED:;
        switch (word_count)
        {
        case 1:;
            CMD_ERROR_FLAG = CMD_VALUE_WORD_MISSING;
            goto free;
        case 2:;
            if (string_number_type(words[1]) != IsInteger)
            {
                CMD_ERROR_FLAG = CMD_VALUE_NOT_INTEGER;
                goto free;
            }
            uint8_t num = atoi((char *)words[1]);
            if (num < 0 || num > 1)
            {
                CMD_ERROR_FLAG = CMD_VALUE_OUT_OF_RANGE;
                goto free;
            }
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, num);
            break;
        default:;
            CMD_ERROR_FLAG = CMD_TOO_MANY_PARAMETERS;
            goto free;
        }
        break;
    case CMD_STOP:;
        switch (word_count)
        {
        case 1:;
            for (int i = 0; i < STEP_NUM; i++)
            {
                // stepper_stop(s[i]);
            }
            break;
        default:
            CMD_ERROR_FLAG = CMD_INVALID_TARGET;
            goto free;
        }
        break;
    case CMD_MOT:;
        /**
         * @brief Command for motor position control
         * e.g. MOT 1 5000 10
         * Means Motor #1, move to step position 5000,
         * with speed limit 10 REV/min (optional)
         * When speed limit is not provided,
         * default values are used.
         *
         */
        switch (word_count)
        {
        case 1:;
        case 2:;
            CMD_ERROR_FLAG = CMD_VALUE_WORD_MISSING;
            goto free;
        case 3:; // posiiton control
        case 4:; // specifying speed limit
            // check MOT number
            if (string_number_type(words[1]) != IsInteger)
            {
                CMD_ERROR_FLAG = CMD_VALUE_NOT_INTEGER;
                goto free;
            }
            uint8_t index = atoi((char *)words[1]);
            if (index < 1 || index > STEP_NUM)
            {
                CMD_ERROR_FLAG = CMD_VALUE_OUT_OF_RANGE;
                goto free;
            }

            // check position
            if (string_number_type(words[2]) != IsInteger)
            {
                CMD_ERROR_FLAG = CMD_VALUE_NOT_INTEGER;
                goto free;
            }
            int32_t pos = atoi((char *)words[2]);
            if (pos < STEP_POS_MIN || pos > STEP_POS_MAX)
            {
                CMD_ERROR_FLAG = CMD_VALUE_OUT_OF_RANGE;
                goto free;
            }
            stepper[index].stepPos_target = pos;
            if (word_count == 3)
            {
                stepper[index].stepRPMLimit = STEP_RPM_DEFAULT;
                if (!stepper[index].stepUpdating)
                    Stepper_Update(&stepper[index]);
                goto free;
            }

            // check speed
            if (string_number_type(words[3]) == IsNotNumber)
            {
                CMD_ERROR_FLAG = CMD_VALUE_NOT_NUMBER;
                goto free;
            }

            float rpm = atof((char *)words[3]);
            if (abs(rpm) < STEP_RPM_MIN || rpm > STEP_RPM_MAX)
            {
                CMD_ERROR_FLAG = CMD_VALUE_OUT_OF_RANGE;
                goto free;
            }
            stepper[index].stepRPMLimit = rpm;
            if (!stepper[index].stepUpdating)
            {
                Stepper_Update(&stepper[index]);
            }
            goto free;
        default:;
            CMD_ERROR_FLAG = CMD_TOO_MANY_PARAMETERS;
            goto free;
        }
        break;

    default:;
        CMD_ERROR_FLAG = CMD_UNSUPPORTED_KEY;
        goto free;
    }

free:
    /* Step 4: Free memory */
    for (int i = 0; i < word_count; i++)
        free(words[i]); // free the dynamically allocated space for each string

    free(words); // free the dynamically allocated space for the array of pointers to strings

    /* Step 5: Clear buf */
    memset(line, '\0', strlen((char *)line));

    return CMD_ERROR_FLAG;
}