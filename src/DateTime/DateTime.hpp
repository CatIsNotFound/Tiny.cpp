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

#ifndef TINY_CPP_OS_TIME_HPP
#define TINY_CPP_OS_TIME_HPP

#include <string>
#include <cstdint>

/*******************************************************************************************
 * P.S: - To enabled using Windows32 lib, please use the macro below:                      *
 *      #define TINY_CPP_MY_OS_WINDOWS                                                     *
 *      - To enabled using Unix lib (for Linux, MacOS, etc.), please use the macro below:  *
 *      #define TINY_CPP_MY_OS_UNIX                                                        *
 *******************************************************************************************/

#if !defined(TINY_CPP_MY_OS_WINDOWS) && !defined(TINY_CPP_MY_OS_UNIX)
    #if defined(_WIN32) || defined(_WIN64)
        #define TINY_CPP_MY_OS_WINDOWS
    #elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
        #define TINY_CPP_MY_OS_UNIX
    #endif
#endif

namespace Tiny {
    namespace DT {
        using Duration = std::int64_t;

        enum Month : uint8_t {
            January = 1,
            Jan = 1,
            February,
            Feb = 2,
            March,
            Mar = 3,
            April,
            Apr = 4,
            May,
            June,
            Jun = 6,
            July,
            Jul = 7,
            August = 8,
            Aug = 8,
            September = 9,
            Sep = 9,
            October = 10,
            Oct = 10,
            November = 11,
            Nov = 11,
            December = 12,
            Dec = 12
        };

        enum Weekday : uint8_t {
            Sunday,
            Sun = 0,
            Monday,
            Mon = 1,
            Tuesday,
            Tue = 2,
            Wednesday,
            Wed = 3,
            Thursday,
            Thur = 4,
            Friday,
            Fri = 5,
            Saturday,
            Sat = 6
        };

        static const char* monthName(Month month, bool short_name = false) noexcept {
            switch (month) {
                case January:
                    return short_name ? "Jan" : "January";
                case February:
                    return short_name ? "Feb" : "February";
                case March:
                    return short_name ? "Mar" : "March";
                case April:
                    return short_name ? "Apr" : "April";
                case May:
                    return "May";
                case June:
                    return short_name ? "Jun" : "June";
                case July:
                    return short_name ? "Jul" : "July";
                case August:
                    return short_name ? "Aug" : "August";
                case September:
                    return short_name ? "Sep" : "September";
                case October:
                    return short_name ? "Oct" : "October";
                case November:
                    return short_name ? "Nov" : "November";
                case December:
                    return short_name ? "Dec" : "December";
            }
            return "NaN";
        }

        static const char* weekDayName(Weekday weekday, bool short_name = false) noexcept {
            switch (weekday) {
                case Sunday:
                    return short_name ? "Sun" : "Sunday";
                case Monday:
                    return short_name ? "Mon" : "Monday";
                case Tuesday:
                    return short_name ? "Tue" : "Tuesday";
                case Wednesday:
                    return short_name ? "Wed" : "Wednesday";
                case Thursday:
                    return short_name ? "Thur" : "Thursday";
                case Friday:
                    return short_name ? "Fri" : "Friday";
                case Saturday:
                    return short_name ? "Sat" : "Saturday";
            }
            return "NaN";
        }

        class DateTime {
        public:
            DateTime(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second,
                uint16_t millisecond = 0, uint8_t weekday = 0, bool use_local_time = true);
            DateTime(Duration timestamps, bool use_local_time = true);

            Duration timestamps() const;
            uint32_t year() const;
            uint8_t month() const;
            uint8_t day() const;
            uint8_t hour() const;
            uint8_t minute() const;
            uint8_t second() const;
            uint16_t millisecond() const;
            uint8_t weekday() const;

            bool isValid() const;
            bool isLocalTime() const;

            bool operator==(const DateTime &date_time) const;
            bool operator!=(const DateTime &date_time) const;
            bool operator<(const DateTime &date_time) const;
            bool operator<=(const DateTime &date_time) const;
            bool operator>(const DateTime &date_time) const;
            bool operator>=(const DateTime &date_time) const;
            DateTime operator+(Duration other) const noexcept;
            DateTime operator-(Duration other) const noexcept;
            DateTime& operator+=(Duration other) noexcept;
            DateTime& operator-=(Duration other) noexcept;
            Duration operator-(const DateTime& date_time) const noexcept;

            bool reset(Duration timestamps, bool use_local_time = true);
            bool reset(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second,
                uint16_t millisecond = 0, uint8_t weekday = 0, bool use_local_time = true);

            static DateTime now(bool use_local_time = true);
            static std::string formatString(const char* format, const DateTime &date_time);
        private:
            void generateDateTime(Duration timestamps);
            Duration correctTS(const DateTime& other) const;

            uint32_t _year{1970};
            uint8_t _month{1};
            uint8_t _day{1};
            uint8_t _hour{1};
            uint8_t _minute{};
            uint8_t _second{};
            bool _valid{false};
            bool _local_time{false};
            uint16_t _milliseconds{};
            uint8_t _w_day{};
            Duration _timestamps{};
        };

        Duration operator""_ms(unsigned long long milliseconds) noexcept;
        Duration operator""_s(unsigned long long seconds) noexcept;
        Duration operator""_m(unsigned long long minutes) noexcept;
        Duration operator""_h(unsigned long long hours) noexcept;
        Duration operator""_d(unsigned long long days) noexcept;
        Duration operator""_w(unsigned long long weeks) noexcept;
        Duration weeks(Duration timestamps) noexcept;
        Duration days(Duration timestamps) noexcept;
        Duration hours(Duration timestamps) noexcept;
        Duration minutes(Duration timestamps) noexcept;
        Duration seconds(Duration timestamps) noexcept;
        Duration milliseconds(Duration timestamps) noexcept;
        Duration currentTimestamps() noexcept;
    }
}

#endif //TINY_CPP_OS_TIME_HPP