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
    namespace OS {
        class DateTime {
        public:
            DateTime(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
            DateTime(uint64_t timestamps);

            uint64_t timestamps() const;
            uint32_t year() const;
            uint8_t month() const;
            uint8_t day() const;
            uint8_t hour() const;
            uint8_t minute() const;
            uint8_t second() const;
            uint16_t millisecond() const;

            bool operator==(const DateTime &date_time) const;
            bool operator!=(const DateTime &date_time) const;
            bool operator<(const DateTime &date_time) const;
            bool operator<=(const DateTime &date_time) const;
            bool operator>(const DateTime &date_time) const;
            bool operator>=(const DateTime &date_time) const;
            static DateTime operator_ms(uint32_t milliseconds) noexcept;
            static DateTime operator_s(uint32_t seconds) noexcept;
            static DateTime operator_m(uint32_t minutes) noexcept;
            static DateTime operator_h(uint32_t hours) noexcept;
            static DateTime operator_d(uint32_t days) noexcept;
            DateTime& operator+(const DateTime& other) noexcept;
            DateTime& operator-(const DateTime& other) noexcept;

            static DateTime now(bool use_local_time = true);
            static uint64_t currentLocalTimeTS();
            static uint64_t currentUTCTimeTS();
            static std::string formatDateTime(const char* format, const DateTime &date_time);
        private:
            uint32_t _year{1970};
            uint8_t _month{1};
            uint8_t _day{1};
            uint8_t _hour{1};
            uint8_t _minute{};
            uint8_t _second{};
            uint16_t _milliseconds{};
            uint64_t _timestamps{};
        };
    }
}

#endif //TINY_CPP_OS_TIME_HPP