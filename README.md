# NEEDLE-PUNCTURE-ROBOT

Cai Dingkun [caidingkun@outlook.com](caidingkun@outlook.com)

[Github](https://github.com/Oct19/needle-puncture-robot)

## Hardware

* Nucleo-F446RE (STM32F446RET6)
* OLED Display SSD1306 (width 128, height 32)
* ...

## Bug

* After system freeze, normal upload will not reset the board correctly, need to press the RESET button
* Sometimes OLED screen stuck, need to power off the screen and restart, seems to be OLED hardware issue

## TODO

* [ ] Stepper control
* [X] Limit switch
* [X] Command execution
* [ ] Robot state
* [ ] Update README
* [ ] Update connection diagram
* [X] Capture return error from ssdWriteString
* [ ] Use FreeRTOS queue, Semaphore, Event, etc
* [ ] Implement tasksuspend and tashresumefromISR

## Fixed Issue

* During debug session, system goes to Hard Fault after osDelay : task stack size too small
* Sometimes PlatformIO need to build twice to build successfully, or after Clean All
* sprintf print use big flash size. Switch to customized printf, see printf.c and printf.h
* [Linux RS485 to USB device cannot connect]([https://unix.stackexchange.com/questions/670636/unable-to-use-usb-dongle-based-on-usb-serial-converter-chip](https://unix.stackexchange.com/questions/670636/unable-to-use-usb-dongle-based-on-usb-serial-converter-chip))
* FreeRTOS Timer cannot be started during interrupt. Use `xTimerStartFromISR` instead. Also check IRQ priority level
* Some serial port monitor software is inconsistent at higher baudrate. Enable USB_ECHO to check communication

## References

* [STM32 project template](https://github.com/Oct19/Bluepill-CubeMX-PlatformIO-Template)
* [STM32 Driver for OLED Display SSD1306]([https://github.com/afiskon/stm32-ssd1306](https://github.com/afiskon/stm32-ssd1306))
* [STM32 timer interrupt]([https://controllerstech.com/pwm-in-stm32/](https://controllerstech.com/pwm-in-stm32/))
* [Coordinated stepper motor control (arduino)]([https://youtu.be/fHAO7SW-SZI](https://youtu.be/fHAO7SW-SZI))
* [STM32 timer]([https://youtu.be/VfbW6nfG4kw](https://youtu.be/VfbW6nfG4kw))
* [Stepper motor configuration]([https://github.com/brentnd/PiPlot](https://github.com/brentnd/PiPlot))
* [GRBL stm32]([https://github.com/dungjk/grbl-stm32](https://github.com/dungjk/grbl-stm32))
* [Stepper S-Curve]([https://github.com/MGDG/SLineControl](https://github.com/MGDG/SLineControl))
* [UART with DMA and FreeRTOS](https://www.devcoons.com/stm32-uart-receive-unknown-size-data-using-dma-and-freertos/)
* [[野火]FreeRTOS 内核实现与应用开发实战—基于RT1052](https://doc.embedfire.com/rtos/freertos/i.mx_rt1052/zh/latest/application/message_queue.html)
* [Split string](https://youtu.be/Vp6OELK4gmo)
