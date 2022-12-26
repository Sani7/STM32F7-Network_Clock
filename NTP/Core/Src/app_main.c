#include "FreeRTOS.h"
#include "lwip.h"
#include "ip4_addr.h"

#include "NTP.h"
#include "Time.h"
#include "RTC.h"
#include "ds3231_for_stm32_hal.h"
#include "clock.h"
#include "debug.h"

extern RTC_HandleTypeDef hrtc;
extern I2C_HandleTypeDef hi2c1;

void app_main(void *argument)
{
  ts utc;
  ts rtc;
  ts local;
  ts diff;
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  uint8_t sBefore;

  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN 5 */

  debugln("LWIP is initialized");

  DS3231_Init(&hi2c1);
  debugln("DS3231 is initialized");

  // The stored time is always in UTC
  breakTime(NTPToEpochUnix(), &utc, 0);
  toTimeZone(&utc, &local, UTC_DELTA_HOURS, 1);
  debugln("Started: %02d:%02d:%02d", utc.Hour, utc.Minute, utc.Second);
  debugln(" \n \n \n"); // I don't know why, but my uart/Serial monitor doesn't show epoch and packet sent if I remove the spaces
  Ts_To_RTC(&utc, &sTime, &sDate);
  Set_Time(sTime, sDate);
  DS3231_SetTime(&utc);

  BSP_LCD_SelectLayer(0);
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
  BSP_LCD_SetFont(&Font12);
  Clock_Draw_Outline(X_CENTER, Y_CENTER, Radius);
  BSP_LCD_SelectLayer(1);

  /* Infinite loop */
  for (;;)
  {
    Get_Time(&sDate, &sTime);
    DS3231_GetTime(&utc);
    if ((utc.Hour == 0) && (utc.Minute == 0) && (utc.Second == 0) && sBefore == 59)
    {
      debugln(" \nUpdating time");
      breakTime(NTPToEpochUnix(), &utc, 0);
      printf(" \n \n \n \n");
      Ts_To_RTC(&utc, &sTime, &sDate);
      Set_Time(sTime, sDate);
      DS3231_SetTime(&utc);
    }

    RTC_To_Ts(&sTime, &sDate, &rtc);
    toTimeZone(&utc, &local, UTC_DELTA_HOURS, 1);
    if (local.Second != sBefore)
    {
      printf(CURSOR_PREV_N_LINES(3) ERASE_FROM_CURSOR_TO_END);
      debugln("RTC   : %02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
      debugln("DS3231: %02d:%02d:%02d", utc.Hour, utc.Minute, utc.Second);
      TimeDiff(&utc, &rtc, &diff);
      debugln("DIFF  : %02d:%02d:%02d", diff.Hour, diff.Minute, diff.Second);
      BSP_LCD_Clear(LCD_COLOR_TRANSPARENT);

      Clock_Write_Date(0, 0, Y_CENTER, 40, local.Wday, local.Day, local.Month, local.Year + 1970);

      Clock_Draw_Hands(X_CENTER, Y_CENTER, Radius, local.Hour, local.Minute, local.Second);
    }

    sBefore = local.Second;
    osDelay(10);
  }
  /* USER CODE END 5 */
}
