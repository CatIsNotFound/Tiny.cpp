/*************************************************************************************
 * MIT License                                                                       *
 *                                                                                   *
 * Copyright (c) 2026 CatIsNotFound                                                  *
 *                                                                                   *
 * Permission is hereby granted, free of charge, to any person obtaining a copy      *
 * of this software and associated documentation files (the "Software"), to deal     *
 * in the Software without restriction, including without limitation the rights      *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         *
 * copies of the Software, and to permit persons to whom the Software is             *
 * furnished to do so, subject to the following conditions:                          *
 *                                                                                   *
 * The above copyright notice and this permission notice shall be included in all    *
 * copies or substantial portions of the Software.                                   *
 *                                                                                   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     *
 * SOFTWARE.                                                                         *
 *                                                                                   *
 *************************************************************************************/

#include "../src/DateTime/DateTime.hpp"
#include <gtest/gtest.h>
using namespace Tiny::DT;

// =============================================================================
// DateTime Enum Tests
// =============================================================================

TEST(MonthEnumTest, Values) {
    EXPECT_EQ(static_cast<int>(January), 1);
    EXPECT_EQ(static_cast<int>(Jan), 1);
    EXPECT_EQ(static_cast<int>(February), 2);
    EXPECT_EQ(static_cast<int>(Feb), 2);
    EXPECT_EQ(static_cast<int>(June), 6);
    EXPECT_EQ(static_cast<int>(Jun), 6);
    EXPECT_EQ(static_cast<int>(July), 7);
    EXPECT_EQ(static_cast<int>(Jul), 7);
    EXPECT_EQ(static_cast<int>(December), 12);
    EXPECT_EQ(static_cast<int>(Dec), 12);
}

TEST(WeekdayEnumTest, Values) {
    EXPECT_EQ(static_cast<int>(Sunday), 0);
    EXPECT_EQ(static_cast<int>(Sun), 0);
    EXPECT_EQ(static_cast<int>(Monday), 1);
    EXPECT_EQ(static_cast<int>(Mon), 1);
    EXPECT_EQ(static_cast<int>(Saturday), 6);
    EXPECT_EQ(static_cast<int>(Sat), 6);
}

// =============================================================================
// DateTime Helper Function Tests
// =============================================================================

TEST(MonthNameTest, FullNames) {
    EXPECT_STREQ(monthName(January), "January");
    EXPECT_STREQ(monthName(February), "February");
    EXPECT_STREQ(monthName(March), "March");
    EXPECT_STREQ(monthName(April), "April");
    EXPECT_STREQ(monthName(May), "May");
    EXPECT_STREQ(monthName(June), "June");
    EXPECT_STREQ(monthName(July), "July");
    EXPECT_STREQ(monthName(August), "August");
    EXPECT_STREQ(monthName(September), "September");
    EXPECT_STREQ(monthName(October), "October");
    EXPECT_STREQ(monthName(November), "November");
    EXPECT_STREQ(monthName(December), "December");
}

TEST(MonthNameTest, ShortNames) {
    EXPECT_STREQ(monthName(January, true), "Jan");
    EXPECT_STREQ(monthName(February, true), "Feb");
    EXPECT_STREQ(monthName(March, true), "Mar");
    EXPECT_STREQ(monthName(April, true), "Apr");
    EXPECT_STREQ(monthName(May, true), "May");
    EXPECT_STREQ(monthName(June, true), "Jun");
    EXPECT_STREQ(monthName(July, true), "Jul");
    EXPECT_STREQ(monthName(August, true), "Aug");
    EXPECT_STREQ(monthName(September, true), "Sep");
    EXPECT_STREQ(monthName(October, true), "Oct");
    EXPECT_STREQ(monthName(November, true), "Nov");
    EXPECT_STREQ(monthName(December, true), "Dec");
}

TEST(WeekDayNameTest, FullNames) {
    EXPECT_STREQ(weekDayName(Sunday), "Sunday");
    EXPECT_STREQ(weekDayName(Monday), "Monday");
    EXPECT_STREQ(weekDayName(Tuesday), "Tuesday");
    EXPECT_STREQ(weekDayName(Wednesday), "Wednesday");
    EXPECT_STREQ(weekDayName(Thursday), "Thursday");
    EXPECT_STREQ(weekDayName(Friday), "Friday");
    EXPECT_STREQ(weekDayName(Saturday), "Saturday");
}

TEST(WeekDayNameTest, ShortNames) {
    EXPECT_STREQ(weekDayName(Sunday, true), "Sun");
    EXPECT_STREQ(weekDayName(Monday, true), "Mon");
    EXPECT_STREQ(weekDayName(Tuesday, true), "Tue");
    EXPECT_STREQ(weekDayName(Wednesday, true), "Wed");
    EXPECT_STREQ(weekDayName(Thursday, true), "Thur");
    EXPECT_STREQ(weekDayName(Friday, true), "Fri");
    EXPECT_STREQ(weekDayName(Saturday, true), "Sat");
}

// =============================================================================
// DateTime Constructor Tests
// =============================================================================

TEST(DateTimeConstructorTest, FromComponents) {
    DateTime dt(2026, 6, 29, 12, 30, 45, 500, 1);
    EXPECT_TRUE(dt.isValid());
    EXPECT_EQ(dt.year(), 2026);
    EXPECT_EQ(dt.month(), 6);
    EXPECT_EQ(dt.day(), 29);
    EXPECT_EQ(dt.hour(), 12);
    EXPECT_EQ(dt.minute(), 30);
    EXPECT_EQ(dt.second(), 45);
    EXPECT_EQ(dt.millisecond(), 500);
    EXPECT_EQ(dt.weekday(), 1);
}

TEST(DateTimeConstructorTest, DefaultMillisAndWeekday) {
    DateTime dt(2026, 1, 1, 0, 0, 0);
    EXPECT_TRUE(dt.isValid());
    EXPECT_EQ(dt.millisecond(), 0);
    EXPECT_EQ(dt.weekday(), 0);
}

TEST(DateTimeConstructorTest, FromTimestamp) {
    DateTime dt(0);
    EXPECT_TRUE(dt.isValid());
}

TEST(DateTimeConstructorTest, EpochTimestamp) {
    DateTime dt(1000);
    EXPECT_TRUE(dt.isValid());
}

// =============================================================================
// DateTime Getter Tests
// =============================================================================

TEST(DateTimeGetterTest, AllGetters) {
    DateTime dt(2026, 6, 29, 14, 30, 45, 123, 1);
    EXPECT_EQ(dt.year(), 2026);
    EXPECT_EQ(dt.month(), 6);
    EXPECT_EQ(dt.day(), 29);
    EXPECT_EQ(dt.hour(), 14);
    EXPECT_EQ(dt.minute(), 30);
    EXPECT_EQ(dt.second(), 45);
    EXPECT_EQ(dt.millisecond(), 123);
    EXPECT_EQ(dt.weekday(), 1);
}

TEST(DateTimeGetterTest, TimestampsPositive) {
    DateTime dt(2026, 6, 29, 12, 0, 0);
    EXPECT_GT(dt.timestamps(), 0);
}

// =============================================================================
// DateTime Comparison Operator Tests
// =============================================================================

TEST(DateTimeComparisonTest, Equality) {
    DateTime dt1(2026, 6, 29, 12, 0, 0);
    DateTime dt2(2026, 6, 29, 12, 0, 0);
    DateTime dt3(2026, 6, 29, 13, 0, 0);

    EXPECT_TRUE(dt1 == dt2);
    EXPECT_FALSE(dt1 == dt3);
    EXPECT_FALSE(dt1 != dt2);
    EXPECT_TRUE(dt1 != dt3);
}

TEST(DateTimeComparisonTest, LessThan) {
    DateTime dt1(2026, 6, 29, 12, 0, 0);
    DateTime dt2(2026, 6, 29, 13, 0, 0);

    EXPECT_TRUE(dt1 < dt2);
    EXPECT_FALSE(dt2 < dt1);
    EXPECT_FALSE(dt1 < dt1);
}

TEST(DateTimeComparisonTest, LessThanOrEqual) {
    DateTime dt1(2026, 6, 29, 12, 0, 0);
    DateTime dt2(2026, 6, 29, 13, 0, 0);

    EXPECT_TRUE(dt1 <= dt2);
    EXPECT_TRUE(dt1 <= dt1);
    EXPECT_FALSE(dt2 <= dt1);
}

TEST(DateTimeComparisonTest, GreaterThan) {
    DateTime dt1(2026, 6, 29, 12, 0, 0);
    DateTime dt2(2026, 6, 29, 13, 0, 0);

    EXPECT_TRUE(dt2 > dt1);
    EXPECT_FALSE(dt1 > dt2);
    EXPECT_FALSE(dt1 > dt1);
}

TEST(DateTimeComparisonTest, GreaterThanOrEqual) {
    DateTime dt1(2026, 6, 29, 12, 0, 0);
    DateTime dt2(2026, 6, 29, 13, 0, 0);

    EXPECT_TRUE(dt2 >= dt1);
    EXPECT_TRUE(dt1 >= dt1);
    EXPECT_FALSE(dt1 >= dt2);
}

// =============================================================================
// DateTime Arithmetic Operator Tests (with DateTime)
// =============================================================================

TEST(DateTimeArithmeticTest, AddDateTime) {
    DateTime dt1(1970, 1, 1, 0, 0, 0);
    DateTime dt2(1970, 1, 1, 1, 0, 0);

    DateTime result = dt1 + dt2;
    EXPECT_GT(result.timestamps(), dt1.timestamps());
    EXPECT_EQ(dt1.timestamps(), 0ULL);
}

TEST(DateTimeArithmeticTest, SubtractDateTime) {
    DateTime dt1(1970, 1, 1, 2, 0, 0);
    DateTime dt2(1970, 1, 1, 1, 0, 0);

    DateTime result = dt1 - dt2;
    EXPECT_EQ(result.timestamps(), 3600000ULL);
}

TEST(DateTimeArithmeticTest, SubtractDateTimeUnderflow) {
    DateTime dt1(1970, 1, 1, 0, 0, 0);
    DateTime dt2(1970, 1, 1, 1, 0, 0);

    uint64_t ts1 = dt1.timestamps();
    uint64_t ts2 = dt2.timestamps();
    DateTime result = dt1 - dt2;
    EXPECT_EQ(result.timestamps(), ts1 - ts2);
}

TEST(DateTimeArithmeticTest, AddAssignDateTime) {
    DateTime dt1(1970, 1, 1, 0, 0, 0);
    DateTime dt2(1970, 1, 1, 1, 0, 0);

    dt1 += dt2;
    EXPECT_EQ(dt1.timestamps(), 3600000ULL);
}

TEST(DateTimeArithmeticTest, SubtractAssignDateTime) {
    DateTime dt1(1970, 1, 1, 2, 0, 0);
    DateTime dt2(1970, 1, 1, 1, 0, 0);

    dt1 -= dt2;
    EXPECT_EQ(dt1.timestamps(), 3600000ULL);
}

TEST(DateTimeArithmeticTest, SubtractAssignDateTimeUnderflow) {
    DateTime dt1(1970, 1, 1, 0, 0, 0);
    DateTime dt2(1970, 1, 1, 1, 0, 0);

    dt1 -= dt2;
    EXPECT_EQ(dt1.timestamps(), 0ULL);
}

// =============================================================================
// DateTime Arithmetic Operator Tests (with uint64_t)
// =============================================================================

TEST(DateTimeArithmeticTest, AddUint64) {
    DateTime dt(1970, 1, 1, 0, 0, 0);

    DateTime result = dt + 3600000ULL;
    EXPECT_EQ(result.timestamps(), 3600000ULL);
    EXPECT_EQ(dt.timestamps(), 0ULL);
}

TEST(DateTimeArithmeticTest, SubtractUint64) {
    DateTime dt(1970, 1, 1, 2, 0, 0);

    DateTime result = dt - 3600000ULL;
    EXPECT_EQ(result.timestamps(), 3600000ULL);
}

TEST(DateTimeArithmeticTest, SubtractUint64Underflow) {
    DateTime dt(1970, 1, 1, 0, 0, 0);
    uint64_t original_ts = dt.timestamps();

    DateTime result = dt - 3600000ULL;
    EXPECT_EQ(dt.timestamps(), original_ts);
    EXPECT_EQ(result.timestamps(), original_ts - 3600000ULL);
}

TEST(DateTimeArithmeticTest, AddAssignUint64) {
    DateTime dt(1970, 1, 1, 0, 0, 0);

    dt += 3600000ULL;
    EXPECT_EQ(dt.timestamps(), 3600000ULL);
}

TEST(DateTimeArithmeticTest, SubtractAssignUint64) {
    DateTime dt(1970, 1, 1, 2, 0, 0);

    dt -= 3600000ULL;
    EXPECT_EQ(dt.timestamps(), 3600000ULL);
}

TEST(DateTimeArithmeticTest, SubtractAssignUint64Underflow) {
    DateTime dt(1970, 1, 1, 0, 0, 0);

    dt -= 3600000ULL;
    EXPECT_EQ(dt.timestamps(), 0ULL);
}

// =============================================================================
// DateTime Reset Method Tests
// =============================================================================

TEST(DateTimeResetTest, ResetWithTimestamp) {
    DateTime dt(2026, 6, 29, 12, 0, 0);

    bool result = dt.reset(0);
    EXPECT_TRUE(result);
    EXPECT_EQ(dt.timestamps(), 0);
}

TEST(DateTimeResetTest, ResetWithComponents) {
    DateTime dt(2026, 6, 29, 12, 0, 0);

    bool result = dt.reset(2025, 1, 1, 0, 0, 0);
    EXPECT_TRUE(result);
    EXPECT_EQ(dt.year(), 2025);
    EXPECT_EQ(dt.month(), 1);
    EXPECT_EQ(dt.day(), 1);
}

TEST(DateTimeResetTest, ResetWithComponentsAndWeekday) {
    DateTime dt(2026, 6, 29, 12, 0, 0);

    bool result = dt.reset(2025, 1, 1, 0, 0, 0, 0, 3);
    EXPECT_TRUE(result);
    EXPECT_EQ(dt.year(), 2025);
    EXPECT_EQ(dt.month(), 1);
    EXPECT_EQ(dt.day(), 1);
    EXPECT_EQ(dt.weekday(), 3);
}

// =============================================================================
// DateTime Static Method Tests
// =============================================================================

TEST(DateTimeStaticTest, NowLocalTime) {
    DateTime local_now = DateTime::now(true);

    EXPECT_TRUE(local_now.isValid());
    EXPECT_GT(local_now.year(), 2020);
}

TEST(DateTimeStaticTest, NowUtcTime) {
    DateTime utc_now = DateTime::now(false);

    EXPECT_TRUE(utc_now.isValid());
    EXPECT_GT(utc_now.year(), 2020);
}

TEST(DateTimeFormatTest, DateFormats) {
    DateTime dt(2026, 6, 29, 14, 30, 45, 123);

    EXPECT_EQ(DateTime::formatString("yyyy-MM-dd", dt), "2026-06-29");
    EXPECT_EQ(DateTime::formatString("yy-MM-dd", dt), "26-06-29");
    EXPECT_EQ(DateTime::formatString("yyyy/MM/dd", dt), "2026/06/29");
    EXPECT_EQ(DateTime::formatString("yyyy.MM.dd", dt), "2026.06.29");
}

TEST(DateTimeFormatTest, TimeFormats) {
    DateTime dt(2026, 6, 29, 14, 30, 45, 123);

    EXPECT_EQ(DateTime::formatString("HH:mm:ss", dt), "14:30:45");
    EXPECT_EQ(DateTime::formatString("HH-mm-ss", dt), "14-30-45");
}

TEST(DateTimeFormatTest, MonthNameFormats) {
    DateTime dt(2026, 6, 29, 12, 0, 0);

    EXPECT_EQ(DateTime::formatString("MMM dd, yyyy", dt), "Jun 29, 2026");
    EXPECT_EQ(DateTime::formatString("MMMM dd, yyyy", dt), "June 29, 2026");
}

TEST(DateTimeFormatTest, MillisecondFormat) {
    DateTime dt(2026, 6, 29, 12, 0, 0, 123);

    EXPECT_EQ(DateTime::formatString("S", dt), "123");
    EXPECT_EQ(DateTime::formatString("SS", dt), "123");
    EXPECT_EQ(DateTime::formatString("SSS", dt), "123");
}

TEST(DateTimeFormatTest, WeekdayFormats) {
    DateTime dt(2026, 6, 29, 12, 0, 0, 0, 1);

    EXPECT_EQ(DateTime::formatString("cc", dt), "01");
    EXPECT_EQ(DateTime::formatString("ccc", dt), "Mon");
    EXPECT_EQ(DateTime::formatString("cccc", dt), "Monday");
}

// =============================================================================
// Time Unit Literal Tests
// =============================================================================

TEST(TimeLiteralTest, Milliseconds) {
    EXPECT_EQ(100_ms, 100ULL);
    EXPECT_EQ(0_ms, 0ULL);
    EXPECT_EQ(1000_ms, 1000ULL);
}

TEST(TimeLiteralTest, Seconds) {
    EXPECT_EQ(1_s, 1000ULL);
    EXPECT_EQ(60_s, 60000ULL);
    EXPECT_EQ(0_s, 0ULL);
}

TEST(TimeLiteralTest, Minutes) {
    EXPECT_EQ(1_m, 60000ULL);
    EXPECT_EQ(5_m, 300000ULL);
    EXPECT_EQ(0_m, 0ULL);
}

TEST(TimeLiteralTest, Hours) {
    EXPECT_EQ(1_h, 3600000ULL);
    EXPECT_EQ(24_h, 86400000ULL);
    EXPECT_EQ(0_h, 0ULL);
}

TEST(TimeLiteralTest, Days) {
    EXPECT_EQ(1_d, 86400000ULL);
    EXPECT_EQ(7_d, 604800000ULL);
    EXPECT_EQ(0_d, 0ULL);
}

TEST(TimeLiteralTest, Weeks) {
    EXPECT_EQ(1_w, 604800000ULL);
    EXPECT_EQ(2_w, 1209600000ULL);
    EXPECT_EQ(0_w, 0ULL);
}

// =============================================================================
// Time Conversion Function Tests
// =============================================================================

TEST(TimeConversionTest, Weeks) {
    EXPECT_EQ(weeks(604800000ULL), 1ULL);
    EXPECT_EQ(weeks(1209600000ULL), 2ULL);
    EXPECT_EQ(weeks(1000ULL), 0ULL);
}

TEST(TimeConversionTest, Days) {
    EXPECT_EQ(days(86400000ULL), 1ULL);
    EXPECT_EQ(days(172800000ULL), 2ULL);
    EXPECT_EQ(days(1000ULL), 0ULL);
}

TEST(TimeConversionTest, Hours) {
    EXPECT_EQ(hours(3600000ULL), 1ULL);
    EXPECT_EQ(hours(7200000ULL), 2ULL);
    EXPECT_EQ(hours(1000ULL), 0ULL);
}

TEST(TimeConversionTest, Minutes) {
    EXPECT_EQ(minutes(60000ULL), 1ULL);
    EXPECT_EQ(minutes(120000ULL), 2ULL);
    EXPECT_EQ(minutes(1000ULL), 0ULL);
}

TEST(TimeConversionTest, Seconds) {
    EXPECT_EQ(seconds(1000ULL), 1ULL);
    EXPECT_EQ(seconds(2000ULL), 2ULL);
    EXPECT_EQ(seconds(500ULL), 0ULL);
}

TEST(TimeConversionTest, Milliseconds) {
    EXPECT_EQ(milliseconds(1000ULL), 1000ULL);
    EXPECT_EQ(milliseconds(0ULL), 0ULL);
}

// =============================================================================
// currentTimestamps Tests
// =============================================================================

TEST(CurrentTimestampsTest, ReturnsPositive) {
    uint64_t ts = currentTimestamps();
    EXPECT_GT(ts, 0ULL);
}

TEST(CurrentTimestampsTest, MonotonicallyIncreasing) {
    uint64_t ts1 = currentTimestamps();
    uint64_t ts2 = currentTimestamps();
    EXPECT_GE(ts2, ts1);
}

// =============================================================================
// Integration Tests
// =============================================================================

TEST(DateTimeIntegrationTest, LiteralsWithArithmetic) {
    DateTime dt(1970, 1, 1, 0, 0, 0);

    dt += 1_h;
    EXPECT_EQ(dt.hour(), 1);

    dt += 30_m;
    EXPECT_EQ(dt.minute(), 30);

    dt += 45_s;
    EXPECT_EQ(dt.second(), 45);
}

TEST(DateTimeIntegrationTest, TimestampRoundTrip) {
    DateTime dt1(2026, 6, 29, 12, 30, 45, 500);
    uint64_t ts = dt1.timestamps();

    DateTime dt2(ts);
    EXPECT_TRUE(dt2.isValid());
    EXPECT_EQ(dt1.year(), dt2.year());
    EXPECT_EQ(dt1.month(), dt2.month());
    EXPECT_EQ(dt1.day(), dt2.day());
}

TEST(DateTimeIntegrationTest, FullDateTimeOperations) {
    DateTime dt(1970, 1, 1, 0, 0, 0);

    dt += 1_d;
    EXPECT_EQ(dt.day(), 2);

    dt += 24_h;
    EXPECT_EQ(dt.day(), 3);

    dt -= 1_d;
    EXPECT_EQ(dt.day(), 2);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}