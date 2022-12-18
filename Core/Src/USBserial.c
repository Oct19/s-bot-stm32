#include "config.h"

uint8_t USB_Tx[USB_TX_SIZE];
uint8_t USB_Rx[USB_RX_SIZE];
uint8_t USB_Rx_Buf[USB_RX_SIZE];

uint16_t oldPos = 0;
uint16_t newPos = 0;

int isOK = 0;

extern osThreadId_t USBserialTaskHandle;
extern osTimerId_t USB_HelloHandle;

extern DMA_HandleTypeDef hdma_usart2_rx;

void USBserial(void *argument)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, USB_Rx_Buf, USB_RX_SIZE);
    /* Disable half-transferred interrupt */
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    for (;;)
    {
        USBserial_Hello(5000);
        USBserial_Execute_Command();
        osDelay(1);
    }
}

/**
 * @brief USB transmit hellos
 *
 * @param delay_ms delay between hellos in milliseconds
 */
void USBserial_Hello(uint32_t delay_ms)
{
    if (!xTimerIsTimerActive(USB_HelloHandle))
        osTimerStart(USB_HelloHandle, delay_ms);
}

/* USB_Hello_Callback function */
void USB_Hello_Callback(void *argument)
{
    /* USER CODE BEGIN USB_Hello_Callback */
    static int count = 0;
    sprintf((char *)USB_Tx, "Hello %lu\r\n", count++);
    HAL_UART_Transmit(&huart2, USB_Tx, USB_TX_SIZE, 10);
    OLED_Update_Tx(USB_Tx, ARRAY_LEN(USB_Tx));
    /* USER CODE END USB_Hello_Callback */
}

/**
 * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
 * @param  huart UART handle
 * @param  Size  Number of data available in application reception buffer (indicates a position in
 *               reception buffer until which, data are available)
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

    if (huart->Instance == USART2)
    {
        memcpy(USB_Rx, USB_Rx_Buf, Size);
        memset(USB_Rx_Buf, '\0', Size);

        /* start the DMA again */
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *)USB_Rx_Buf, USB_RX_SIZE);
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);

        /****************** PROCESS (Little) THE DATA HERE *********************/

        for (int i = 0; i < Size; i++)
        {
            if ((USB_Rx[i] == 'M') && (USB_Rx[i + 1] == 'O') && (USB_Rx[i + 2] == 'T'))
            {
                isOK = 1;
            }
        }

        if (USB_ECHO)
            HAL_UART_Transmit(&huart2, USB_Rx, Size, 10);
    }
}

void USBserial_Execute_Command(void)
{
    if (!USB_Rx[0])
    {
        return;
    }

    /* Update OLED Message */
    OLED_Update_Rx(USB_Rx, USB_RX_SIZE);

    memset(USB_Rx, '\0', USB_RX_SIZE);
}

// void _putchar(char character)
// {
//     HAL_UART_Transmit(&huart2, (uint8_t) & character, 1, 100);
// }