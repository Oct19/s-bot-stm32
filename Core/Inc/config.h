#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Debug */

/* Select only one to show */
// #define  OLED_SHOW_HASH
#define OLED_SHOW_FORCE_READING
// #define  OLED_SHOW_STATE

// Command
#define COMMAND_DELIMITER " ," // space or comma
#define COMMAND_MAX_LENGTH 20

// USB
#define USB_BAUD_RATE 115200
#define USB_TX_SIZE 20
#define USB_RX_SIZE 20
#define USB_ECHO_DEFAULT 1 //


// Force sensor
#define FORCE_SENSOR_NUM 9


// OLED
#define OLED_MIRROR          // Comment to disable
#define OLED_DISPLAY_WARNING // Comment to disable
#define OLED_DISPLAY_TX      // Comment to disable
#define OLED_DISPLAY_RX      // Comment to disable
#define OLED_SHORT_MESSAGE_LENGTH 10
#define OLED_LONG_MESSAGE_LENGTH 20
#define OLED_MESSAGE_LINGER_MS 1000 // if no new message to display in current priority level
#define OLED_FPS 24                 // 1 ~ 10

// STEPPER
#define STEP_NUM 2
#define STEP_PER_REV 200 // Microstep, 200,400,800,ect
#define STEP_MAX_RPM 100
#define STEP_MIN_STEP 100 // us
#define STEP_REST 1       // disable stepper when motion is finished
#ifdef STEP_REST
#define STEP_DWELL_MS 10 // keep powered for sometime to prevent shifting after stop
#endif                   // STEP_REST
#define STEP_TEST

// MOVE
#define MM_PER_REV 1.2 // mm

// Upon power up, robot will perform homing procedure before taking any commands
#define HOMING_INIT_LOCK // Comment to disable

#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H__ */