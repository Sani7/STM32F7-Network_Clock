#pragma once
#include <stdint.h>
#include "debug.h"

// to transform a number of seconds into a current time you need to do some maths
#define NUMBEROFSECONDSPERDAY 86400UL
#define NUMBEROFSECONDSPERHOUR 3600UL
#define NUMBEROFSECONDSPERMINUTE 60UL

// leap year calculator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) )

// you might not live in the UTC time zone, set up your delta time in hours
#define UTC_DELTA_HOURS 1   // CET
//#define UTC_DELTA_HOURS 2     // CEST
/*Set offset time to adjust for your timezone, for example:
  * GMT +1
  * GMT +2
  * GMT +8
  * GMT -1
  * GMT  0
 */
#define UTC_DELTA_MINUTES 0

typedef struct {
    uint8_t Second;         /* seconds */
    uint8_t Minute;         /* minutes */
    uint8_t Hour;           /* hours */
    uint8_t Day;            /* day of the month */
    uint8_t Month;          /* month */
    int16_t Year;           /* year offset from 1970 */
    uint8_t Wday;           /* day of the week */
    uint8_t Yday;           /* day in the year */
    uint8_t IsDST;          /* daylight saving time */

    uint32_t unixtime;      /* seconds since 01.01.1970 00:00:00 UTC*/
} ts;

enum Wday_e {Sun = 1, Mon, Tue, Wed, Thu, Fri, Sat};

uint8_t IsDST(ts* tm);
void breakTime(uint32_t timeInput, ts *tm, uint8_t runIsDST);
uint32_t makeTime(ts *tm, uint8_t runIsDST);
void toTimeZone(ts *utc, ts* local, int8_t timeZone, uint8_t IsDST);
void TimeDiff(ts* time1, ts* time2, ts* diff);
