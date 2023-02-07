#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif
/* Debug */

/* Select only one to show */
// #define  OLED_SHOW_HASH
// #define OLED_DEBUG

// STATE
#define STATE_UPDATE_DELAY_MS 10

// Command
#define COMMAND_DELIMITER " ," // space or comma
#define COMMAND_MAX_LENGTH 20

// USB
#define USB_BAUD_RATE 115200
#define USB_TX_SIZE 20
#define USB_RX_SIZE 20
#define USB_ECHO_DEFAULT 0 // UART Transmit in interrupt mode

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
#define OLED_FPS 10                 // 1 ~ 10

// STEPPER
// hardware setup
#define STEP_NUM 2
#define STEP_PER_REV 1000 // Set by step driver, 200 can be microstep into 400,800, or up to 20000
// position
#define STEP_POS_MAX (STEP_PER_REV * 100)
#define STEP_POS_MIN (-STEP_PER_REV * 100)
// speed
#define STEP_RPM_MAX 300
#define STEP_RPM_MIN 1
#define STEP_RPM_DEFAULT 100 // has to be integer since precompiler cannot compare float
#define STEP_RPM_LEVEL 10    // integer only. speed level resolution during accel and decel
// acceleration
#define STEP_ACC_MAX 600
#define STEP_ACC_MIN 1
#define STEP_ACC_DEFAULT 600 // has to be integer since precompiler cannot compare float
// stepper rest and dwell after motion complete
#define STEP_REST // disable stepper when motion is finished
#ifdef STEP_REST
#define STEP_DWELL_MS 10 // keep powered for sometime to prevent shifting after stop
#endif // STEP_REST
// varify stepper config values
#if STEP_RPM_DEFAULT > STEP_RPM_MAX || STEP_RPM_DEFAULT < STEP_RPM_MIN
#error "STEP_RPM_DEFAULT value invalid"
#endif
#if STEP_ACC_DEFAULT > STEP_ACC_MAX || STEP_ACC_DEFAULT < STEP_ACC_MIN
#error "STEP_ACC_DEFAULT value invalid"
#endif

// MOVE
#define MM_PER_REV 1.2 // mm

// Upon power up, robot will perform homing procedure before taking any commands
#define HOMING_INIT_LOCK // Comment to disable

#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H__ */