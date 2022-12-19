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
#include "config.h"
#include "ssd1306.h"

OLED_HandleTypeDef OLED;

extern osTimerId_t OLED_Warning_TimeoutHandle;
extern osTimerId_t OLED_Tx_TimeoutHandle;
extern osTimerId_t OLED_Rx_TimeoutHandle;

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
    ssd1306_Fill(White);
    ssd1306_SetCursor(20, 5);
    ssd1306_WriteString("Welcome!", Font_11x18, Black);
    ssd1306_UpdateScreen();
}

void OLED_display_Warning(void)
{
    ssd1306_Fill(White);
    ssd1306_SetCursor(20, 10);
    ssd1306_WriteString((char *)OLED.Warning, Font_11x18, Black);
    ssd1306_UpdateScreen();
}

void OLED_display_TxRx(void)
{
    ssd1306_Fill(Black);
    // First row: Tx
    ssd1306_SetCursor(0, 0);
    if (ssd1306_WriteString((char *)OLED.Tx, Font_7x10, White))
    {
        /* Contains special character that cannot display */
        ssd1306_WriteString(OLED_ERROR_MSG, Font_7x10, White);
    }
    // Second row: Rx
    ssd1306_SetCursor(0, 20);
    if(ssd1306_WriteString((char *)OLED.Rx, Font_7x10, White))
    {
        /* Contains special character that cannot display */
        ssd1306_WriteString(OLED_ERROR_MSG, Font_7x10, White);
    }
    ssd1306_UpdateScreen();
}

void OLED_display_Info(void)
{
    ssd1306_Fill(Black);
    // First row: State
    ssd1306_SetCursor(30, 0);

    char *str = NULL;
    switch (ROBOT_STATE)
    {
    case STATE_RESET:;
        str = "RESET";
        break;
    case STATE_IDLE:;
        str = "Idle";
        break;
    case STATE_MOVE:;
        str = "Move";
        break;
    case STATE_ERROR:;
        str = "Error";
        break;
    }
    ssd1306_WriteString(str, Font_7x10, White);
    // Second row: Positions
    ssd1306_SetCursor(0, 20);
    if (ssd1306_WriteString((char *)OLED.Info, Font_7x10, White))
    {
        /* Contains special character that cannot display */
        ssd1306_WriteString(OLED_ERROR_MSG, Font_7x10, White);
    }
    ssd1306_UpdateScreen();
}
/**
 * @brief for testing purposes
 *
 */
void OLED_display_off(void)
{
    ssd1306_SetCursor(20, 5);
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
}

void OLED_Update_Warning(uint8_t *msg, size_t size)
{
    memset(OLED.Warning, '\0', OLED_WARNING_SIZE);
    memcpy(OLED.Warning, msg, min(OLED_WARNING_SIZE, size));
    xTimerChangePeriodFromISR(OLED_Warning_TimeoutHandle, OLED_MESSAGE_LINGER_MS, pdFALSE);
}

/**
 * @brief Called from HAL_UARTEx_RxEventCallback
 *
 * @param Rx
 * @param size
 */
void OLED_Update_Rx(uint8_t *Rx, size_t size)
{
    memset(OLED.Rx, '\0', OLED_RX_SIZE);
    memcpy(OLED.Rx, Rx, min(OLED_RX_SIZE, size));
    /* Timer start during ISR */
    xTimerChangePeriodFromISR(OLED_Rx_TimeoutHandle, OLED_MESSAGE_LINGER_MS, pdFALSE);
}

void OLED_Update_Tx(uint8_t *Tx, size_t size)
{
    memset(OLED.Tx, '\0', OLED_TX_SIZE);
    memcpy(OLED.Tx, Tx, min(OLED_TX_SIZE, size));
    xTimerChangePeriodFromISR(OLED_Tx_TimeoutHandle, OLED_MESSAGE_LINGER_MS, pdFALSE);
}

/* OLED_Tx_Timeout_Callback function */
void OLED_Tx_Timeout_Callback(void *argument)
{
    /* USER CODE BEGIN OLED_Tx_Timeout_Callback */
    memset(OLED.Tx, '\0', OLED_TX_SIZE);
    /* USER CODE END OLED_Tx_Timeout_Callback */
}

/* OLED_Warning_Timeout_Callback function */
void OLED_Warning_Timeout_Callback(void *argument)
{
    /* USER CODE BEGIN OLED_Warning_Timeout_Callback */
    memset(OLED.Warning, '\0', OLED_WARNING_SIZE);
    /* USER CODE END OLED_Warning_Timeout_Callback */
}

/* OLED_Rx_Timeout_Callback function */
void OLED_Rx_Timeout_Callback(void *argument)
{
    /* USER CODE BEGIN OLED_Rx_Timeout_Callback */
    memset(OLED.Rx, '\0', OLED_RX_SIZE);
    /* USER CODE END OLED_Rx_Timeout_Callback */
}