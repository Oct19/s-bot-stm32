#include "config.h"

uint8_t USB_Tx[USB_TX_SIZE];
uint8_t USB_Rx[USB_RX_SIZE];

uint16_t oldPos = 0;
uint16_t newPos = 0;

int isOK = 0;

extern osThreadId_t USBserialTaskHandle;
extern osTimerId_t USB_HelloHandle;

extern DMA_HandleTypeDef hdma_usart2_rx;

void USBserial(void *argument)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, USB_Rx, USB_RX_SIZE);
    /* Disable half-transferred interrupt */
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    for (;;)
    {
        // USBserial_Hello(50);

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
        /* Echo back Rx */
        if (USB_ECHO)
            HAL_UART_Transmit(&huart2, USB_Rx, Size, 10);

        /* Update OLED Message */
        OLED_Update_Rx(USB_Rx, Size);

        /* Read and Execute */
        USBserial_Execute_Command();

        /* Clear Rx buffer */
        memset(USB_Rx, '\0', Size);

        /* start the DMA again */
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, USB_Rx, USB_RX_SIZE);
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    }
}

/**
 * @brief Slice, read and execute
 * @todo Use a new slicing function that slice by ' ' instead
 * 
 */
void USBserial_Execute_Command(void)
{
    /* Step 1: Slice Rx into 2D arrays */
    ListWithLength *instruction = getWords((char *)USB_Rx);

    /* Use OLED.Info to show number of words */
    memset(OLED.Info, '\0', OLED_INFO_SIZE);
    itoa(instruction->length, (char *)OLED.Info, 10);
}