/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBSERIAL_H__
#define __USBSERIAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "config.h"

#define USB_TX_SIZE 20
#define USB_RX_SIZE 20

extern uint8_t USB_Tx[USB_TX_SIZE];
extern uint8_t USB_Rx[USB_RX_SIZE];
extern uint8_t USB_ECHO; //0 to disable, 1 to enable

void USBserial_Hello(uint32_t delay_ms);

#ifdef __cplusplus
}
#endif

#endif /* __USBSERIAL_H__ */