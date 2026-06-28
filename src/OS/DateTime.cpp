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

#include "DateTime.hpp"
#include <sstream>
#include <ctime>
#ifdef TINY_CPP_MY_OS_WINDOWS
#include <windows.h>
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <sys/time.h>
#endif

Tiny::OS::DateTime::DateTime(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
        : _year(year), _month(month), _day(day), _hour(hour), _minute(minute), _second(second) {}

Tiny::OS::DateTime::DateTime(uint64_t timestamps) : _timestamps(timestamps) {
    // TODO: Parsed timestamp

}

uint64_t Tiny::OS::DateTime::timestamps() const {
    return _timestamps;
}

uint32_t Tiny::OS::DateTime::year() const {
    return _year;
}

uint8_t Tiny::OS::DateTime::month() const {
    return _month;
}

uint8_t Tiny::OS::DateTime::day() const {
    return _day;
}

uint8_t Tiny::OS::DateTime::hour() const {
    return _hour;
}

uint8_t Tiny::OS::DateTime::minute() const {
    return _minute;
}

uint8_t Tiny::OS::DateTime::second() const {
    return _second;
}

uint16_t Tiny::OS::DateTime::millisecond() const {
    return _milliseconds;
}

bool Tiny::OS::DateTime::operator==(const DateTime &date_time) const {
    return _timestamps == date_time._timestamps;
}

bool Tiny::OS::DateTime::operator!=(const DateTime &date_time) const {
    return _timestamps != date_time._timestamps;
}

bool Tiny::OS::DateTime::operator<(const DateTime &date_time) const {
    return _timestamps < date_time._timestamps;
}

bool Tiny::OS::DateTime::operator<=(const DateTime &date_time) const {
    return _timestamps <= date_time._timestamps;
}

bool Tiny::OS::DateTime::operator>(const DateTime &date_time) const {
    return _timestamps > date_time._timestamps;
}

bool Tiny::OS::DateTime::operator>=(const DateTime &date_time) const {
    return _timestamps >= date_time._timestamps;
}

Tiny::OS::DateTime Tiny::OS::DateTime::operator_ms(uint32_t milliseconds) noexcept {
    return {milliseconds};
}

Tiny::OS::DateTime Tiny::OS::DateTime::operator_s(uint32_t seconds) noexcept {
    return {seconds * 1000};
}

Tiny::OS::DateTime Tiny::OS::DateTime::operator_m(uint32_t minutes) noexcept {
    return {minutes * 60000};
}

Tiny::OS::DateTime Tiny::OS::DateTime::operator_h(uint32_t hours) noexcept {
    return {hours * 3600000};
}

Tiny::OS::DateTime Tiny::OS::DateTime::operator_d(uint32_t days) noexcept {
    return {days * 86400000};
}

Tiny::OS::DateTime & Tiny::OS::DateTime::operator+(const DateTime &other) noexcept {
    _timestamps += other._timestamps;
    return *this;
}

Tiny::OS::DateTime & Tiny::OS::DateTime::operator-(const DateTime &other) noexcept {
    _timestamps -= other._timestamps;
    return *this;
}

Tiny::OS::DateTime Tiny::OS::DateTime::now(bool use_local_time) {
    DateTime now_dt(0);


    return now_dt;
}

uint64_t Tiny::OS::DateTime::currentLocalTimeTS() {

}

uint64_t Tiny::OS::DateTime::currentUTCTimeTS() {

}

std::string Tiny::OS::DateTime::formatDateTime(const char* format, const DateTime &date_time) {
    std::ostringstream oss;

}

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

