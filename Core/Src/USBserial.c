#include "robot.h"

uint8_t USB_Tx[USB_TX_SIZE];
uint8_t USB_Rx[USB_RX_SIZE];

uint8_t USB_ECHO;

void USBserial_Init(void)
{
    /* USB start Rx */
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, USB_Rx, USB_RX_SIZE);
    /* Disable half-transferred interrupt */
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
}

void USB_Hello(uint32_t delay_ms)
{
    if (!xTimerIsTimerActive(USB_HelloHandle))
        osTimerStart(USB_HelloHandle, delay_ms);
}

/* USB_Hello_Callback function */
void USB_Hello_Callback(void *argument)
{
    /* USER CODE BEGIN USB_Hello_Callback */
    static int count = 0;
    char msg[16];
    snprintf(msg, sizeof(msg), "Hello %lu", count++);
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)msg, sizeof(msg));
    OLED_Update_Tx(msg);
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
    if (huart->Instance == USART1) // Force sensor reading
    {
        Force_Sensor_Rx_Callback();
    }
    if (huart->Instance == USART2) // USB
    {
        /* Echo back Rx */
        if (USB_ECHO)
            HAL_UART_Transmit_DMA(&huart2, USB_Rx, Size);

        /* Copy to command_line to execute */
        memset(command_line, '\0', COMMAND_MAX_LENGTH);
        memcpy(command_line, USB_Rx, min(COMMAND_MAX_LENGTH, Size));

        /* Clear Rx */
        // memset(USB_Rx, '\0', USB_RX_SIZE);

        /* start the DMA again */
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, USB_Rx, USB_RX_SIZE);
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    }
}
