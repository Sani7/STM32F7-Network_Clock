#include "Time.h"

/**
 * @fn uint8_t IsDST(ts*)
 * @brief This function calculates if we are in EDST
 *
 * @param utc the time struct in UTC
 * @return returns the value of utc->IsDST which is 0 if not in DST and 1 if in DST
 */
uint8_t IsDST(ts *utc)
{
    uint8_t nextSunday;
    uint16_t y, m, d;
    // number of days of each month
    uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // January, february, and december are out.
    if (utc->Month < 3 || utc->Month > 10)
    {
        utc->IsDST = 0;
        return utc->IsDST;
    }
    // April to september are in
    if (utc->Month > 3 && utc->Month < 10)
    {
        utc->IsDST = 1;
        return utc->IsDST;
    }

    m = utc->Month;
    y = utc->Year + 1970;
    days[1] -= (y % 4) || (!(y % 100) && (y % 400));
    d = days[m - 1];

    /* dow is in normal format*/
    nextSunday = days[m - 1] - ((d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y / 4 - y / 100 + y / 400) % 7);
    // Start: Last Sunday in March
    if (utc->Month == 3)
    {
        utc->IsDST = utc->Day >= nextSunday ? (utc->Day == nextSunday ? (utc->Hour >= 0) : 1) : 0;
        return utc->IsDST;
    }

    // End: Last Sunday in October
    utc->IsDST = utc->Day >= nextSunday ? (utc->Day == nextSunday ? (utc->Hour < 0) : 0) : 1;
    return utc->IsDST;
}

/**
 * @fn void breakTime(uint32_t, ts*, uint8_t)
 * @brief functions to convert from epoch time to our time struct
 *
 * @param timeInput: epoch time
 * @param time: the struct in which this function will return the datetime
 * @param runIsDST: bool if it will run the IsDST function after this function completes
 */
void breakTime(uint32_t timeInput, ts *time, uint8_t runIsDST)
{
    // break the given time_t into time components
    // this is a more compact version of the C library localtime function
    // note that year is offset from 1970 !!!

    uint8_t year;
    uint8_t month, monthLength;
    unsigned long days;
    uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    time->unixtime = timeInput;
    time->Second = timeInput % 60;
    timeInput /= 60; // now it is minutes
    time->Minute = timeInput % 60;
    timeInput /= 60; // now it is hours
    time->Hour = timeInput % 24;
    timeInput /= 24;                      // now it is days
    time->Wday = ((timeInput + 4) % 7) + 1; // Sunday is day 1

    year = 0;
    days = 0;
    while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= timeInput)
    {
        year++;
    }
    time->Year = year; // year is offset from 1970

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
    time->Month = month + 1;   // jan is month 1
    time->Day = timeInput + 1; // day of month

    if (runIsDST)
        IsDST(time);
}

/**
 * @fn uint32_t makeTime(ts*, uint8_t)
 * @brief functions to convert from our time struct to epoch time
 *
 * @param time: our time struct to convert from
 * @param runIsDST: bool if it will run the IsDST function after this function completes
 * @return the epoch time
 */
uint32_t makeTime(ts *time, uint8_t runIsDST)
{
    // assemble time elements into time_t
    // note year argument is offset from 1970 (see macros in time.h to convert to other formats)
    // previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9

    int i;
    uint32_t seconds;
    uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // seconds from 1970 till 1 jan 00:00:00 of the given year
    seconds = time->Year * (NUMBEROFSECONDSPERDAY * 365);
    for (i = 0; i < time->Year; i++)
    {
        if (LEAP_YEAR(i))
        {
            seconds += NUMBEROFSECONDSPERDAY; // add extra days for leap years
        }
    }

    // add days for this year, months start from 1
    for (i = 1; i < time->Month; i++)
    {
        if ((i == 2) && LEAP_YEAR(time->Year))
        {
            seconds += NUMBEROFSECONDSPERDAY * 29;
        }
        else
        {
            seconds += NUMBEROFSECONDSPERDAY * monthDays[i - 1]; // monthDay array starts from 0
        }
    }
    seconds += (time->Day - 1) * NUMBEROFSECONDSPERDAY;
    seconds += time->Hour * NUMBEROFSECONDSPERHOUR;
    seconds += time->Minute * NUMBEROFSECONDSPERMINUTE;
    seconds += time->Second;

    if (runIsDST)
        IsDST(time);
    time->unixtime = seconds;
    return seconds;
}

/**
 * @fn void toTimeZone(ts*, ts*, int8_t)
 * @brief this function converts utc time to local time it automatically calculates if we are in DST or not
 *
 * @param utc: the time struct which contains the time in utc
 * @param local: the time struct in which we want to write our conversion
 * @param timeZone: the offset from UTC ex. UTC+1 => timeZone = 1
 * @param IsDST: bool if we want to take DST in to account
 */
void toTimeZone(ts *utc, ts *local, int8_t timeZone, uint8_t IsDST)
{
	uint32_t localTime;
    makeTime(utc, 1);
    if (IsDST)
    {
    	localTime = utc->unixtime + (timeZone + utc->IsDST) * NUMBEROFSECONDSPERHOUR;
    }
    else
    {
    	localTime = utc->unixtime + timeZone * NUMBEROFSECONDSPERHOUR;
    }

    breakTime(localTime, local, 0);
}

void TimeDiff(ts* time1, ts* time2, ts* diff)
{
	uint16_t totalTime;
	totalTime = (time1->Day - time2->Day) * 86400 + (time1->Hour - time2->Hour) * 3600 + (time1->Minute - time2->Minute) * 60 + (time1->Second - time2->Second);
	diff->Day = totalTime / 86400;
	totalTime %= 86400;
	diff->Hour = totalTime / 3600;
	totalTime %= 3600;
	diff->Minute = totalTime / 60;
	diff->Second = totalTime % 60;
}
