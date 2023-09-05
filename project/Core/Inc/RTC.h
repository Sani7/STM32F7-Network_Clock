#include "debug.h"
#include "Time.h"

void Set_Time (RTC_TimeTypeDef sTime, RTC_DateTypeDef sDate);
void Get_Time(RTC_DateTypeDef* gDate, RTC_TimeTypeDef* gTime);
void Ts_To_RTC(ts* tm, RTC_TimeTypeDef* sTime, RTC_DateTypeDef* sDate);
void RTC_To_Ts(RTC_TimeTypeDef* sTime, RTC_DateTypeDef* sDate, ts* tm);
