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
#include <iomanip>
#include <ctime>

#include "../OS/File.hpp"
#ifdef TINY_CPP_MY_OS_WINDOWS
#include <windows.h>
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <sys/time.h>
#endif

struct SDT {
    uint32_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
    uint8_t weekday;
};

inline Tiny::DT::DateTime localTime() {
#ifdef TINY_CPP_MY_OS_WINDOWS
    SYSTEMTIME local{};
    GetLocalTime(&local);
    return {static_cast<uint32_t>(local.wYear), static_cast<uint8_t>(local.wMonth),
        static_cast<uint8_t>(local.wDay), static_cast<uint8_t>(local.wHour),
        static_cast<uint8_t>(local.wMinute), static_cast<uint8_t>(local.wSecond),
        static_cast<uint16_t>(local.wMilliseconds), static_cast<uint8_t>(local.wDayOfWeek)};
#elif defined(TINY_CPP_MY_OS_UNIX)
    struct tm now_tm{};
    time_t now = time(nullptr);
    if (!localtime_s(&now_tm, &now)) {
        return {now_tm.tm_year + 1900, now_tm.tm_mon + 1,
            now_tm.tm_mday, now_tm.tm_hour,
            now_tm.tm_min, now_tm.tm_sec, 0, now_tm.tm_wday};
    }
    return {0};
#endif
}

inline Tiny::DT::DateTime systemTime() {
#ifdef TINY_CPP_MY_OS_WINDOWS
    SYSTEMTIME sys{};
    GetSystemTime(&sys);
    return {static_cast<uint32_t>(sys.wYear), static_cast<uint8_t>(sys.wMonth),
        static_cast<uint8_t>(sys.wDay), static_cast<uint8_t>(sys.wHour),
        static_cast<uint8_t>(sys.wMinute), static_cast<uint8_t>(sys.wSecond),
        static_cast<uint16_t>(sys.wMilliseconds), static_cast<uint8_t>(sys.wDayOfWeek)};
#elif defined(TINY_CPP_MY_OS_UNIX)
    struct tm now_tm{};
    time_t now = time(nullptr);
    if (!gmtime_s(&now_tm, &now)) {
        return {now_tm.tm_year + 1900, now_tm.tm_mon + 1,
            now_tm.tm_mday, now_tm.tm_hour,
            now_tm.tm_min, now_tm.tm_sec, 0, now_tm.tm_wday};
    }
    return {0};
#endif
}

inline SDT fromTimestamp(uint64_t timestamp, bool& ok) {
#ifdef TINY_CPP_MY_OS_WINDOWS
    const uint64_t EPOCH_DIFF = 116444736000000000;
    uint64_t ts = timestamp * 10000 + EPOCH_DIFF;

    ULARGE_INTEGER ull{};
    ull.QuadPart = ts;

    FILETIME ft{};
    ft.dwLowDateTime = ull.LowPart;
    ft.dwHighDateTime = ull.HighPart;

    SYSTEMTIME utc{};
    if (FileTimeToSystemTime(&ft, &utc)) {
        ok = true;
        return {static_cast<uint32_t>(utc.wYear), static_cast<uint8_t>(utc.wMonth),
            static_cast<uint8_t>(utc.wDay), static_cast<uint8_t>(utc.wHour),
            static_cast<uint8_t>(utc.wMinute), static_cast<uint8_t>(utc.wSecond),
            static_cast<uint16_t>(utc.wMilliseconds), static_cast<uint8_t>(utc.wDayOfWeek)};
    }
#elif defined(TINY_CPP_MY_OS_UNIX)
    time_t sec = timestamp / 1000;
    uint16_t milliseconds = timestamp % 1000;

    struct tm local{};
    if (!localtime_s(&local, &sec)) {
        uint32_t year = local.tm_year + 1900;
        uint8_t month = local.tm_mon + 1;
        uint8_t day = local.tm_mday;
        uint8_t hour = local.tm_hour;
        uint8_t minute = local.tm_min;
        uint8_t second = local.tm_sec;
        uint8_t weekday = local.tm_wday;
        ok = true;
        return {year, month, day, hour, minute, second, milliseconds, weekday};
    }
#endif
    ok = false;
    return {};
}

inline uint64_t toTimestamp(SDT date_time, bool& ok) {
#ifdef TINY_CPP_MY_OS_WINDOWS
    SYSTEMTIME user_time = {
        (WORD)date_time.year, (WORD)date_time.month, (WORD)date_time.weekday,
        (WORD)date_time.day, (WORD)date_time.hour, (WORD)date_time.minute,
        (WORD)date_time.second, (WORD)date_time.millisecond
    };

    FILETIME ftime = {};
    if (SystemTimeToFileTime(&user_time, &ftime)) {
        ULARGE_INTEGER ull = {};
        ull.LowPart = ftime.dwLowDateTime;
        ull.HighPart = ftime.dwHighDateTime;

        const uint64_t EPOCH_DIFF = 116444736000000000;
        ok = true;
        if (ull.QuadPart > EPOCH_DIFF) return (ull.QuadPart - EPOCH_DIFF) / 10000;
        return (EPOCH_DIFF - ull.QuadPart) / 10000;
    }
#elif defined(TINY_CPP_MY_OS_UNIX)
    struct tm t = {};
    t.tm_year = date_time.year - 1900;
    t.tm_mon = date_time.month - 1;
    t.tm_mday = date_time.day;
    t.tm_hour = date_time.hour;
    t.tm_min = date_time.minute;
    t.tm_sec = date_time.second;

    time_t sec = mktime(&t);
    if (sec != -1) {
        ok = true;
        return sec * 1000ULL + date_time.millisecond;
    }
#endif
    ok = false;
    return 0;
}

Tiny::DT::DateTime::DateTime(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second,
    uint16_t millisecond, uint8_t weekday)
        : _year(year), _month(month), _day(day), _hour(hour), _minute(minute), _second(second),
          _milliseconds(millisecond), _w_day(weekday) {
    SDT dt = {year, month, day, hour, minute, second, millisecond, weekday};
    _timestamps = toTimestamp(dt, _valid);
}

Tiny::DT::DateTime::DateTime(uint64_t timestamps) : _timestamps(timestamps) {
    SDT dt = fromTimestamp(timestamps, _valid);
    if (_valid) {
        _year = dt.year;
        _month = dt.month;
        _day = dt.day;
        _hour = dt.hour;
        _minute = dt.minute;
        _second = dt.second;
        _milliseconds = dt.millisecond;
        _w_day = dt.weekday;
    }
}

uint64_t Tiny::DT::DateTime::timestamps() const {
    return _timestamps;
}

uint32_t Tiny::DT::DateTime::year() const {
    return _year;
}

uint8_t Tiny::DT::DateTime::month() const {
    return _month;
}

uint8_t Tiny::DT::DateTime::day() const {
    return _day;
}

uint8_t Tiny::DT::DateTime::hour() const {
    return _hour;
}

uint8_t Tiny::DT::DateTime::minute() const {
    return _minute;
}

uint8_t Tiny::DT::DateTime::second() const {
    return _second;
}

uint16_t Tiny::DT::DateTime::millisecond() const {
    return _milliseconds;
}

uint8_t Tiny::DT::DateTime::weekday() const {
    return _w_day;
}

bool Tiny::DT::DateTime::isValid() const {
    return _valid;
}

bool Tiny::DT::DateTime::operator==(const DateTime &date_time) const {
    return _timestamps == date_time._timestamps;
}

bool Tiny::DT::DateTime::operator!=(const DateTime &date_time) const {
    return _timestamps != date_time._timestamps;
}

bool Tiny::DT::DateTime::operator<(const DateTime &date_time) const {
    return _timestamps < date_time._timestamps;
}

bool Tiny::DT::DateTime::operator<=(const DateTime &date_time) const {
    return _timestamps <= date_time._timestamps;
}

bool Tiny::DT::DateTime::operator>(const DateTime &date_time) const {
    return _timestamps > date_time._timestamps;
}

bool Tiny::DT::DateTime::operator>=(const DateTime &date_time) const {
    return _timestamps >= date_time._timestamps;
}

Tiny::DT::DateTime Tiny::DT::DateTime::operator+(const DateTime &other) const noexcept {
    return {_timestamps + other._timestamps};
}

Tiny::DT::DateTime Tiny::DT::DateTime::operator-(const DateTime &other) const noexcept {
    return {_timestamps - other._timestamps};
}

Tiny::DT::DateTime Tiny::DT::DateTime::operator+(uint64_t other) const noexcept {
    return {_timestamps + other};
}

Tiny::DT::DateTime Tiny::DT::DateTime::operator-(uint64_t other) const noexcept {
    return {_timestamps - other};
}

Tiny::DT::DateTime & Tiny::DT::DateTime::operator+=(const DateTime &other) noexcept {
    _timestamps += other._timestamps;
    generateDateTime(_timestamps);
    return *this;
}

Tiny::DT::DateTime & Tiny::DT::DateTime::operator-=(const DateTime &other) noexcept {
    if (_timestamps < other._timestamps) {
        _timestamps = 0;
    } else {
        _timestamps -= other._timestamps;
    }
    generateDateTime(_timestamps);
    return *this;
}

Tiny::DT::DateTime & Tiny::DT::DateTime::operator+=(uint64_t other) noexcept {
    _timestamps += other;
    generateDateTime(_timestamps);
    return *this;
}

Tiny::DT::DateTime & Tiny::DT::DateTime::operator-=(uint64_t other) noexcept {
    if (_timestamps < other) {
        _timestamps = 0;
    } else {
        _timestamps -= other;
    }
    generateDateTime(_timestamps);
    return *this;
}

bool Tiny::DT::DateTime::reset(uint64_t timestamps) {
    _timestamps = timestamps;
    generateDateTime(_timestamps);
    return _valid;
}

bool Tiny::DT::DateTime::reset(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second,
        uint16_t millisecond, uint8_t weekday) {
    SDT dt = {year, month, day, hour, minute, second, millisecond, weekday};
    _timestamps = toTimestamp(dt, _valid);
    if (_valid) {
        generateDateTime(_timestamps);
    }
    return _valid;
}

Tiny::DT::DateTime Tiny::DT::DateTime::now(bool use_local_time) {
    return use_local_time ? localTime() : systemTime();
}

std::string Tiny::DT::DateTime::formatString(const char* format, const DateTime &date_time) {
    std::ostringstream oss;
    int cnt = 0;
    while (*format) {
        if (*format == 'y') {
            while (*(format + cnt) == 'y') ++cnt;
            if (cnt == 2 || cnt == 3) {
                oss << std::setw(cnt) << std::setfill('0') << date_time.year() % 100;
            } else {
                oss << std::setw(cnt) << std::setfill(' ') << date_time.year();
            }
        }
        else if (*format == 'M') {
            while (*(format + cnt) == 'M') ++cnt;
            if (cnt == 2) {
                oss << std::setw(2) << std::setfill('0') << (int)date_time.month();
            } else if (cnt == 3) {
                oss << monthName(static_cast<Month>(date_time.month()), true);
            } else if (cnt == 4) {
                oss << monthName(static_cast<Month>(date_time.month()));
            } else {
                oss << std::setw(cnt) << std::setfill(' ') << (int)date_time.month();
            }
        }
        else if (*format == 'd') {
            while (*(format + cnt) == 'd') ++cnt;
            if (cnt == 2) {
                oss << std::setw(2) << std::setfill('0') << (int)date_time.day();
            } else {
                oss << std::setw(cnt) << std::setfill(' ') << (int)date_time.day();
            }
        }
        else if (*format == 'H') {
            while (*(format + cnt) == 'H') ++cnt;
            if (cnt == 2) {
                oss << std::setw(2) << std::setfill('0') << (int)date_time.hour();
            } else {
                oss << std::setw(cnt) << std::setfill(' ') << (int)date_time.hour();
            }
        }
        else if (*format == 'h') {
            while (*(format + cnt) == 'h') ++cnt;
            int hh = date_time.hour() % 12;
            if (hh == 0) hh = 12;
            if (cnt == 2) {
                oss << std::setw(2) << std::setfill('0') << hh;
            } else {
                oss << std::setw(cnt) << std::setfill(' ') << hh;
            }
        }
        else if (*format == 'm') {
            while (*(format + cnt) == 'm') ++cnt;
            oss << std::setw(cnt) << std::setfill('0') << (int)date_time.minute();
        }
        else if (*format == 's') {
            while (*(format + cnt) == 's') ++cnt;
            oss << std::setw(cnt) << std::setfill('0') << (int)date_time.second();
        }
        else if (*format == 'a') {
            while (*(format + cnt) == 'a') ++cnt;
            oss << std::setw(cnt) << std::left << std::setfill(' ') << (date_time.hour() >= 12 ? "PM" : "AM");
        }
        else if (*format == 'c') {
            while (*(format + cnt) == 'c') ++cnt;
            if (cnt == 2) {
                oss << std::setw(2) << std::setfill('0') << static_cast<int>(date_time.weekday());
            } else if (cnt == 3) {
                oss << weekDayName(static_cast<Weekday>(date_time.weekday()), true);
            } else if (cnt == 4) {
                oss << weekDayName(static_cast<Weekday>(date_time.weekday()));
            } else {
                oss << std::setw(cnt) << static_cast<int>(date_time.weekday());
            }
        }
        else if (*format == 'S') {
            while (*(format + cnt) == 'S') ++cnt;
            oss << std::setw(cnt) << std::left << std::setfill('0') << date_time.millisecond();
        }
        if (cnt > 0) {
            format += cnt;
            cnt = 0;
        } else {
            oss << *format;
            format += 1;
        }
    }

    return oss.str();
}

void Tiny::DT::DateTime::generateDateTime(uint64_t timestamps) {
    SDT dt = fromTimestamp(timestamps, _valid);
    if (_valid) {
        _year = dt.year;
        _month = dt.month;
        _day = dt.day;
        _hour = dt.hour;
        _minute = dt.minute;
        _second = dt.second;
        _milliseconds = dt.millisecond;
        _w_day = dt.weekday;
    }
}


uint64_t Tiny::DT::operator""_ms(uint64_t milliseconds) noexcept {
    return milliseconds;
}

uint64_t Tiny::DT::operator""_s(uint64_t seconds) noexcept {
    return seconds * 1000;
}

uint64_t Tiny::DT::operator""_m(uint64_t minutes) noexcept {
    return minutes * 60000;
}

uint64_t Tiny::DT::operator""_h(uint64_t hours) noexcept {
    return hours * 3600000;
}

uint64_t Tiny::DT::operator""_d(uint64_t days) noexcept {
    return days * 86400000;
}

uint64_t Tiny::DT::operator""_w(uint64_t weeks) noexcept {
    return weeks * 604800000;
}

uint64_t Tiny::DT::weeks(uint64_t timestamps) noexcept {
    return timestamps / 604800000;
}

uint64_t Tiny::DT::days(uint64_t timestamps) noexcept {
    return timestamps / 86400000;
}

uint64_t Tiny::DT::hours(uint64_t timestamps) noexcept {
    return timestamps / 3600000;
}

uint64_t Tiny::DT::minutes(uint64_t timestamps) noexcept {
    return timestamps / 60000;
}

uint64_t Tiny::DT::seconds(uint64_t timestamps) noexcept {
    return timestamps / 1000;
}

uint64_t Tiny::DT::milliseconds(uint64_t timestamps) noexcept {
    return timestamps;
}

uint64_t Tiny::DT::currentTimestamps() noexcept {
#ifdef TINY_CPP_MY_OS_WINDOWS
    SYSTEMTIME sys{};
    GetSystemTime(&sys);
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);

    ULARGE_INTEGER ull{};
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;
    const uint64_t EPOCH_DIFF = 116444736000000000;
    return (ull.QuadPart - EPOCH_DIFF) / 10000;
#elif defined(TINY_CPP_MY_OS_UNIX)
    struct timeval tv{};
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
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

