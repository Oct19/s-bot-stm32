/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBSERIAL_H__
#define __USBSERIAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "robot.h"

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;

extern uint8_t USB_Tx[USB_TX_SIZE];
extern uint8_t USB_Rx[USB_RX_SIZE];
extern uint8_t USB_ECHO; //0 to disable, 1 to enable

void USBserial_Init(void);
void USB_Hello(uint32_t delay_ms);

#ifdef __cplusplus
}
#endif

#endif /* __USBSERIAL_H__ */