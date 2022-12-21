#include "config.h"

/**
 * @brief Slice, read and execute, return error code
 * @todo Use a new slicing function that slice by ' ' instead
 *
 */

uint8_t Execute_Command(uint8_t *line)
{
    /* Step 1: String to uppercase*/
    upperString(line);

    /* Step 2: Slice Rx into 2D arrays */
    int count = 0;
    char **words = split((char *)line, COMMAND_DELIMITER, &count);

    /* Step 3: First word is command, followed by parameters */
    uint32_t command = hash((uint8_t *)words[0]);

    switch (command)
    {
    /* Type 1: Command without parameters ****************************************************/
    case HASH_RESET:;
        NVIC_SystemReset();
        break;

        /* Type 2: Command with a single parameter ************************************************/
        /* Check if parameter exists */
    case HASH_ECHO:;
        if (count != 2)
            return CMD_ErrorCode_NotEnoughParam;
        if (!strcmp(words[1], "0"))
            USB_ECHO = 0;
        else if (!strcmp(words[1], "1"))
            USB_ECHO = 1;
        else
            return CMD_ErrorCode_InvalidParam;
        break;
    case HASH_LED:;
        if (count != 2)
            return CMD_ErrorCode_NotEnoughParam;
        if (!strcmp(words[1], "0"))
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, RESET);
        else if (!strcmp(words[1], "1"))
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, SET);
        // xTaskResumeFromISR(LEDblinkTaskHandle);
        else
            return CMD_ErrorCode_InvalidParam;
        break;
    default:;
        return CMD_ErrorCode_InvalidCMD;
    }

    /* Step 4: Free memory */
    for (int i = 0; i < count; i++)
        free(words[i]); // free the dynamically allocated space for each string

    free(words); // free the dynamically allocated space for the array of pointers to strings

    /* Step 5: Clear buf */
    memset(line, '\0', strlen((char *)line));

    return CMD_ErrorCode_NoErr;
}