#include <gtest/gtest.h>
#include "time.hpp"

// TEST(TestCaseName, IndividualTestName)
TEST(timeLib, isDST1)
{
    ts t;

    // 2022-10-5 00:00:00 UTC DST on
    t.Wday = Wed;
    t.Day = 5;
    t.Month = 10;
    t.Year = 2022 - 1970;
    t.Hour = 0;
    
    EXPECT_EQ(1, IsDST(&t));
}
TEST(timeLib, isDST2)
{
    ts t;

    // 2022-10-29 59:59:59 UTC DST still on
    t.Wday = Sun;
    t.Day = 29;
    t.Month = 10;
    t.Hour = 23;
    t.Minute = 59;
    t.Second = 59;
    t.Year = 2022 - 1970;

    EXPECT_EQ(1 ,IsDST(&t));
}

TEST(timeLib, isDST3)
{
    ts t;

    // 2022-10-30 00:00:00 UTC DST ends
    t.Wday = Sun;
    t.Day = 30;
    t.Month = 10;
    t.Hour = 0;
    t.Year = 2022 - 1970;

    EXPECT_EQ(0 ,IsDST(&t));
}

TEST(timeLib, isDST4)
{
    ts t;

    // 2022-11-23 00:00:00 UTC DST off
    t.Wday = Wed;
    t.Day = 23;
    t.Month = 11;
    t.Hour = 0;
    t.Year = 2022 - 1970;

    EXPECT_EQ(0 ,IsDST(&t));
}

TEST(timeLib, isDST5)
{
    ts t;

    // 2022-03-25 23:59:59 UTC DST off
    t.Wday = Sun;
    t.Day = 25;
    t.Month = 3;
    t.Hour = 23;
    t.Minute = 59;
    t.Second = 59;
    t.Year = 2023 - 1970;

    EXPECT_EQ(0 ,IsDST(&t));
}

TEST(timeLib, isDST6)
{
    ts t;

    // 2022-03-26 00:00:00 UTC DST starts
    t.Wday = Sun;
    t.Day = 26;
    t.Month = 3;
    t.Hour = 0;
    t.Year = 2023 - 1970;

    EXPECT_EQ(1 ,IsDST(&t));
}

TEST(timeLib, isDST7)
{
    ts t;

    // 2022-03-27 00:00:00 UTC DST on
    t.Wday = Mon;
    t.Day = 27;
    t.Month = 3;
    t.Hour = 0;
    t.Year = 2023 - 1970;

    EXPECT_EQ(1 ,IsDST(&t));
}

// TEST(TestCaseName, IndividualTestName)
TEST(timeLib, breakTime)
{
    ts t;

    // 1669233881 = Wed 2022-11-23 20:04:41 UTC
    breakTime(1669233881, &t);

    EXPECT_EQ(Wed, t.Wday);
    EXPECT_EQ(23, t.Day);
    EXPECT_EQ(11, t.Month);
    EXPECT_EQ(20, t.Hour);
    EXPECT_EQ(4, t.Minute);
    EXPECT_EQ(41, t.Second);
    EXPECT_EQ(2022, t.Year + 1970);
    EXPECT_EQ(0, t.IsDST);
    EXPECT_EQ(1669233881, t.unixtime);
}

TEST(timeLib, makeTime)
{
    ts t;
    t.Wday = Wed;
    t.Day = 23;
    t.Month = 11;
    t.Hour = 20;
    t.Minute = 4;
    t.Second = 41;
    t.Year = 2022 - 1970;
    t.IsDST = 0;
    t.unixtime = 1669233881;
    EXPECT_EQ(1669233881, makeTime(&t));
    EXPECT_EQ(1669233881, t.unixtime);
}

TEST(timeLib, toTimeZone)
{
    ts t;
    t.Day = 23;
    t.Month = 11;
    t.Year = 2022 - 1970;
    t.Hour = 20;
    t.Minute = 4;
    t.Second = 41;

    ts local;
    toTimeZone(&t, &local, 1);
    EXPECT_EQ(23, local.Day);
    EXPECT_EQ(11, local.Month);
    EXPECT_EQ(2022, local.Year + 1970);
    EXPECT_EQ(21, local.Hour);
    EXPECT_EQ(4, local.Minute);
    EXPECT_EQ(41, local.Second);
}