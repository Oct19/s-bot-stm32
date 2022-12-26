/**
 * @file OLEDdisplay.c
 * @author Cai Dingkun <caidingkun@outlook.com>
 * @brief FreeROTS task for OLED display
 * @version 0.1
 * @date 2022-12-15
 *
 * @copyright Copyright (c) 2022
 *
 */
/* Includes ---------------------------------------------------------------- */
#include "robot.h"
#include "ssd1306.h"

OLED_HandleTypeDef OLED;

/* Message to display contains special character */
char OLED_ERROR_MSG[8] = "????????";

void OLED_Init(void);
void OLED_display_Warning(void);
void OLED_display_TxRx(void);
void OLED_display_Info(void);

void OLEDdisplay(void *argument)
{
    for (;;)
    {

        if (OLED.Warning[0])
            OLED_display_Warning();
        else if (OLED.Rx[0] | OLED.Tx[0])
            OLED_display_TxRx();
        else
            OLED_display_Info();

        osDelay(1000 / OLED_FPS);
    }
}

void OLED_Init(void)
{
    ssd1306_Init();
#ifdef OLED_MIRROR
    ssd1306_FlipScreenVertically();
#endif
    memset(OLED.Warning, '\0', OLED_WARNING_SIZE);
    memset(OLED.Tx, '\0', OLED_TX_SIZE);
    memset(OLED.Rx, '\0', OLED_RX_SIZE);
    memset(OLED.Info, '\0', OLED_INFO_SIZE);
}
/**
 * @brief for testing purposes
 *
 */
void OLED_display_welcome(void)
{
    ssd1306_SetColor(Black);
    ssd1306_Fill();
    ssd1306_SetColor(White);
    ssd1306_SetCursor(20, 5);
    ssd1306_WriteString("Welcome!", Font_11x18);
    ssd1306_UpdateScreen();
}

void OLED_display_Warning(void)
{
    ssd1306_SetColor(Black);
    ssd1306_Fill();
    ssd1306_SetColor(White);
    ssd1306_SetCursor(0, 5);
    ssd1306_WriteString((char *)OLED.Warning, Font_16x26);
    ssd1306_UpdateScreen();
}

void OLED_display_TxRx(void)
{
    ssd1306_SetColor(Black);
    ssd1306_Fill();
    ssd1306_SetColor(White);
    // First row: Tx
    ssd1306_SetCursor(0, 0);
    if (ssd1306_WriteString((char *)OLED.Tx, Font_7x10))
    {
        /* Contains special character that cannot display */
        ssd1306_WriteString(OLED_ERROR_MSG, Font_7x10);
    }
    // Second row: Rx
    ssd1306_SetCursor(0, 20);
    if (ssd1306_WriteString((char *)OLED.Rx, Font_7x10))
    {
        /* Contains special character that cannot display */
        ssd1306_WriteString(OLED_ERROR_MSG, Font_7x10);
    }
    ssd1306_UpdateScreen();
}

void OLED_display_Info(void)
{
    ssd1306_SetColor(Black);
    ssd1306_Fill();
    ssd1306_SetColor(White);
    // First row: State
    ssd1306_SetCursor(20, 0);

    char str[OLED_INFO_SIZE] = "";
    snprintf(str, sizeof(str), "Status: %d", robot_status);

    ssd1306_WriteString(str, Font_7x10);
    // Second row: Positions
    ssd1306_SetCursor(0, 20);
    if (ssd1306_WriteString((char *)OLED.Info, Font_7x10))
    {
        /* Contains special character that cannot display */
        ssd1306_WriteString(OLED_ERROR_MSG, Font_7x10);
    }
    ssd1306_UpdateScreen();
}

void OLED_display_off(void)
{
    ssd1306_SetColor(Black);
    ssd1306_Fill();
    ssd1306_SetColor(White);
    ssd1306_UpdateScreen();
}

void OLED_Update_Warning(char *msg)
{
    memset(OLED.Warning, '\0', OLED_WARNING_SIZE);
    memcpy(OLED.Warning, msg, min(OLED_WARNING_SIZE, strlen(msg)));
    xTimerChangePeriodFromISR(OLED_Warning_TimeoutHandle, OLED_MESSAGE_LINGER_MS,pdFALSE);
}

/**
 * @brief Called from HAL_UARTEx_RxEventCallback
 *
 * @param Rx
 * @param size
 */
void OLED_Update_Rx(char *msg)
{
    memset(OLED.Rx, '\0', OLED_RX_SIZE);
    memcpy(OLED.Rx, msg, min(OLED_RX_SIZE, strlen(msg)));
    xTimerChangePeriodFromISR(OLED_Rx_TimeoutHandle, OLED_MESSAGE_LINGER_MS, pdFALSE);
}

void OLED_Update_Tx(char *msg)
{
    memset(OLED.Tx, '\0', OLED_TX_SIZE);
    memcpy(OLED.Tx, msg, min(OLED_TX_SIZE, strlen(msg)));
    xTimerChangePeriodFromISR(OLED_Tx_TimeoutHandle, OLED_MESSAGE_LINGER_MS,pdFALSE);
}

void OLED_Update_Info(char *msg)
{
    memset(OLED.Info, '\0', OLED_TX_SIZE);
    memcpy(OLED.Info, msg, min(OLED_INFO_SIZE, strlen(msg)));
}

void OLED_Tx_Timeout_Callback(void *argument)
{
    memset(OLED.Tx, '\0', OLED_TX_SIZE);
}

void OLED_Warning_Timeout_Callback(void *argument)
{
    memset(OLED.Warning, '\0', OLED_WARNING_SIZE);
}

void OLED_Rx_Timeout_Callback(void *argument)
{
    memset(OLED.Rx, '\0', OLED_RX_SIZE);
}