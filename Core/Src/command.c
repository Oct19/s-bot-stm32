#include "robot.h"

uint8_t command_line[USB_RX_SIZE];

uint8_t Execute_Command(uint8_t *line)
{
    /* Step 1: String to uppercase*/
    upperString(line);

    /* Step 2: Slice line into 2D arrays */
    int count = 0;
    char **words = split((char*)line, COMMAND_DELIMITER, &count);

    /* Step 3: First word is key, followed by parameters */
    uint32_t key = hash((uint8_t *)words[0]);

/* debug */
#ifdef CMD_DEBUG
    char info[USB_TX_SIZE];
    uint8_t num_type = string_number_type(words[0]);
    switch (num_type)
    {
    case IsNotNumber:;
        snprintf(info, sizeof(info), "%s is not num", words[0]);
        break;
    case IsInteger:;
        snprintf(info, sizeof(info), "float %f", atof(words[0]));
        break;
    case IsFloat:;
        snprintf(info, sizeof(info), "int %d", atoi(words[0]));
        break;
    default:
        break;
    }
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)info, strlen(info));
    OLED_Update_Info(info);
#endif /* CMD_DEBUG */
    /* Type 1: Command without parameters ****************************************************/
    if (count == 1)
    {
        switch (key)
        {
        case HASH_RESET:;
            NVIC_SystemReset();
            break;
        case HASH_BEEP:;
            beep(2);
            break;
        default:;
            return COMMAND_UNSUPPORTED_KEY;
        }
        return STATUS_OK;
    }

    /* Type 2: Command line with 0/1 parameter ************************************************/
    uint8_t param_num_type = string_number_type(words[1]);
    if (count == 2 && param_num_type != IsNotNumber)
    {
        float value = atof(words[1]);
        bool on_or_off = value > 0 ? 1 : 0;
        switch (key)
        {
        case HASH_ECHO:;
            USB_ECHO = on_or_off;
            break;
        case HASH_LED:;
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, on_or_off);
            break;
        case HASH_ENA:;
            HAL_GPIO_WritePin(ENA_GPIO_Port, ENA_Pin, on_or_off);
            break;
        default:;
            return COMMAND_UNSUPPORTED_KEY;
        }
        return COMMAND_OK;
    }

    /* All other single parameter commands are invalid */
    if (count == 2)
        return COMMAND_UNSUPPORTED_KEY;

    /* Type 3: Command with multiple parameters */
    uint32_t paramA = hash((uint8_t *)words[1]);
    switch (key)
    {
    /* Change value of a variable */
    case HASH_SET:;
        uint32_t value = atoi(words[2]);
        UNUSED(value);
        switch (paramA)
        {
        default:;
            return COMMAND_UNSUPPORTED_KEY;
        }
        break;
    default:;
        return COMMAND_UNSUPPORTED_KEY;
    }

    /* Step 4: Free memory */
    for (int i = 0; i < count; i++)
        free(words[i]); // free the dynamically allocated space for each string

    free(words); // free the dynamically allocated space for the array of pointers to strings

    /* Step 5: Clear buf */
    memset(line, '\0', strlen((char *)line));

    return COMMAND_OK;
}