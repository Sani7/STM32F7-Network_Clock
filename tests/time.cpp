#include "time.hpp"

static const uint8_t monthDays[] =
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // API starts months from 1, this array starts from 0

/**
 * @fn uint8_t IsDST(ts)
 * @brief
 *
 * @param tm
 * @return
 */
uint8_t IsDST(ts *tm)
{
    uint8_t nextSunday;
    uint16_t y, m, d;
    //number of days of each month
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // January, february, and december are out.
    if (tm->Month < 3 || tm->Month > 10)
    {
        tm->IsDST = 0;
        return tm->IsDST;
    }
    // April to september are in
    if (tm->Month > 3 && tm->Month < 10)
    {
        tm->IsDST = 1;
        return tm->IsDST;
    }
    
    m = tm->Month;
    y = tm->Year + 1970;
    days[1] -= (y % 4) || (!(y % 100) && (y % 400));
    d = days[m - 1];
    
    /* dow is in normal format*/
    nextSunday = days[m-1] - ((d += m < 3 ? y-- : y - 2, 23*m/9 + d + 4 + y/4- y/100 + y/400)%7);
    // Start: Last Sunday in March
    if (tm->Month == 3)
    {
        tm->IsDST = tm->Day >= nextSunday ? (tm->Day == nextSunday ? (tm->Hour >= 0) : 1) : 0;
        return tm->IsDST;
    }

    // End: Last Sunday in October
    tm->IsDST = tm->Day >= nextSunday ? (tm->Day == nextSunday ? (tm->Hour < 0) : 0) : 1;
    return tm->IsDST;
}

/*============================================================================*/
/* functions to convert to and from system time */
/* These are for interfacing with time services and are not normally needed in a sketch */

void breakTime(uint32_t timeInput, ts *tm)
{
    // break the given time_t into time components
    // this is a more compact version of the C library localtime function
    // note that year is offset from 1970 !!!

    uint8_t year;
    uint8_t month, monthLength;
    unsigned long days;

    tm->unixtime = timeInput;
    tm->Second = timeInput % 60;
    timeInput /= 60; // now it is minutes
    tm->Minute = timeInput % 60;
    timeInput /= 60; // now it is hours
    tm->Hour = timeInput % 24;
    timeInput /= 24;                      // now it is days
    tm->Wday = ((timeInput + 4) % 7) + 1; // Sunday is day 1

    year = 0;
    days = 0;
    while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= timeInput)
    {
        year++;
    }
    tm->Year = year; // year is offset from 1970

    days -= LEAP_YEAR(year) ? 366 : 365;
    timeInput -= days; // now it is days in this year, starting at 0

    days = 0;
    month = 0;
    monthLength = 0;
    for (month = 0; month < 12; month++)
    {
        if (month == 1)
        { // february
            if (LEAP_YEAR(year))
            {
                monthLength = 29;
            }
            else
            {
                monthLength = 28;
            }
        }
        else
        {
            monthLength = monthDays[month];
        }

        if (timeInput >= monthLength)
        {
            timeInput -= monthLength;
        }
        else
        {
            break;
        }
    }
    tm->Month = month + 1; // jan is month 1
    tm->Day = timeInput + 1;    // day of month

    IsDST(tm);
}

uint32_t makeTime(ts *tm)
{   
// assemble time elements into time_t 
// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9
  
  int i;
  uint32_t seconds;

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds = tm->Year*(NUMBEROFSECONDSPERDAY * 365);
  for (i = 0; i < tm->Year; i++) {
    if (LEAP_YEAR(i)) {
      seconds += NUMBEROFSECONDSPERDAY;   // add extra days for leap years
    }
  }
  
  // add days for this year, months start from 1
  for (i = 1; i < tm->Month; i++) {
    if ( (i == 2) && LEAP_YEAR(tm->Year)) { 
      seconds += NUMBEROFSECONDSPERDAY * 29;
    } else {
      seconds += NUMBEROFSECONDSPERDAY * monthDays[i-1];  //monthDay array starts from 0
    }
  }
  seconds+= (tm->Day-1) * NUMBEROFSECONDSPERDAY;
  seconds+= tm->Hour * NUMBEROFSECONDSPERHOUR;
  seconds+= tm->Minute * NUMBEROFSECONDSPERMINUTE;
  seconds+= tm->Second;
  
  tm->unixtime = seconds;
  return seconds; 
}

void toTimeZone(ts *utc, ts* local, int8_t timeZone)
{
    // convert UTC time to local time
    // timeZone is in hours
    // local is a pointer to a tm structure
    // this function is not reentrant

    uint32_t localTime = makeTime(utc) + timeZone * NUMBEROFSECONDSPERHOUR;
    breakTime(localTime, local);
}