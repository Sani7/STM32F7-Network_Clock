#include "RTC.h"

extern RTC_HandleTypeDef hrtc;

void Set_Time (RTC_TimeTypeDef sTime, RTC_DateTypeDef sDate)
{
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    debugErrln("%s:%d Error putting the time in the RTC", __FILE__, __LINE__);
  }

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
	  debugErrln("%s:%d Error putting the date in the RTC", __FILE__, __LINE__);
  }
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2); // backup register just a random value
}

void Get_Time(RTC_DateTypeDef* gDate, RTC_TimeTypeDef* gTime)
{
/* Get the RTC current Time */
 HAL_RTC_GetTime(&hrtc, gTime, RTC_FORMAT_BIN);
/* Get the RTC current Date */
 HAL_RTC_GetDate(&hrtc, gDate, RTC_FORMAT_BIN);
}

void Ts_To_RTC(ts* tm, RTC_TimeTypeDef* sTime, RTC_DateTypeDef* sDate)
{
  sTime->Hours = tm->Hour;
  sTime->Minutes = tm->Minute;
  sTime->Seconds = tm->Second;
  sDate->Date = tm->Day;
  sDate->Month = tm->Month;
  sDate->Year = tm->Year;
}

void RTC_To_Ts(RTC_TimeTypeDef* sTime, RTC_DateTypeDef* sDate, ts* tm)
{
  tm->Hour = sTime->Hours;
  tm->Minute = sTime->Minutes;
  tm->Second = sTime->Seconds;
  tm->Day = sDate->Date;
  tm->Month = sDate->Month;
  tm->Year = sDate->Year;
}
