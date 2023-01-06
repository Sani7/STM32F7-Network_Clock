# NTP Clock
### By Sander Speetjens

## Brief description
This project uses UDP to get the time from a NTP server and stored in the DS3231 RTC module. The time is then displayed on the LCD screen. The lcd analog clock updates every second. The time is also displayed on the serial port. The time is refreshed at 0:00:00 UTC every day.

## Used Libs, compiler and apps
 
 - lwip version 2.1.2
 - freeRTOS version 10.2.1
 - CMSIS-RTOS version 1.02.1
 - CubeIDE version 1.10.1
 - Firmware Lib (stm32f7) 1.17.0

## Created files / modified files

  ### Created:
  - `Core/Src/app_main.c`
  - `Core/Src/clock.c`
    - `Core/Inc/clock.h`
  - `Core/Src/debug.c`
    - `Core/Inc/debug.h`
  - `Core/Src/ds3231_for_stm32_hal.c`
    - `Core/Inc/ds3231_for_stm32_hal.h`
  - `Core/Src/NTP.c`
    - `Core/Inc/NTP.h`
  - `Core/Src/RTC.c`
    - `Core/Inc/RTC.h`
  - `Core/Src/Time.c`
    - `Core/Inc/Time.h`

  ### Modified:
  - `Core/Src/main.c`
  - `Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.h` (Color transparent define)

## Remarks 
I first used the internal RTC with the internal low frequency RC oscillator, which has a deviation of almost 20 minutes per day. Then I switched to DS3231 and to the external LF xtal oscillator and they stay in sync. 

## Source references 
https://www.meinbergglobal.com/english/info/ntp-packet.htm \
https://github.com/Sani7/DS3231_for_STM32_HAL \
based on: \
https://github.com/eepj/DS3231_for_STM32_HAL \
https://www.nongnu.org/lwip/2_0_x/sntp_8c.html \
Time.c used make and break time functions of: \
https://github.com/michaelmargolis/arduino_time \
Ansi escape codes: \
https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

## Optional: Extras
- Switch back to the internal RTC and use the external LF xtal oscillator wirhout the DS3231 RTC module.
- Remove serial port debug messages.
- Use the NTP struct instead of a for loop.