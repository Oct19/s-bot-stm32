#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Debug */
// #define CMD_RETURN_HASH
// #define CMD_DEBUG // Comment to disable

// Command
#define COMMAND_DELIMITER " ," // space or comma
#define COMMAND_MAX_LENGTH 20

// USB
#define USE_USB // Comment to disable
#ifdef USE_USB
#define USB_BAUD_RATE 115200
#define USB_TX_SIZE 20
#define USB_RX_SIZE 20
#define USB_ECHO_DEFAULT 1 // 
#endif // USE_USB

// Force sensor
#define USE_FORCE_SENSOR // Comment to disable
#ifdef USE_FORCE_SENSOR
// Force sensor box baudrate default 19200, can be configured as: 19200, 115200
#define FORCE_SENSOR_BAUD_RATE 115200
// Force sensor request frequency
// Force sensor box sampling frequency can be configured as 10, 20, 80, 320Hz
#define FORCE_SENSOR_REQUEST_FREQUENCY 100
#endif // USE_FORCE_SENSOR

// OLED
#define USE_OLED
#ifdef USE_OLED
#define OLED_MIRROR // Comment to disable
#define OLED_DISPLAY_WARNING // Comment to disable
#define OLED_DISPLAY_TX // Comment to disable
#define OLED_DISPLAY_RX // Comment to disable
#define OLED_SHORT_MESSAGE_LENGTH 10
#define OLED_LONG_MESSAGE_LENGTH 20
#define OLED_MESSAGE_LINGER_MS 1000 // if no new message to display in current priority level
#define OLED_FPS 10                 // 1 ~ 10
#endif // USE_OLED



// Upon power up, robot will perform homing procedure before taking any commands
#define HOMING_INIT_LOCK // Comment to disable

#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H__ */