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

SDT fromTimestamp(Tiny::DT::Duration timestamp, bool use_local_time, bool& ok);

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
    auto ok = localtime_r(&now, &now_tm);
    struct timespec tm_spec{};
    if (ok) {
        clock_gettime(CLOCK_REALTIME, &tm_spec);
        return {
            static_cast<uint32_t>(now_tm.tm_year + 1900), 
            static_cast<uint8_t>(now_tm.tm_mon + 1),
            static_cast<uint8_t>(now_tm.tm_mday), 
            static_cast<uint8_t>(now_tm.tm_hour),
            static_cast<uint8_t>(now_tm.tm_min), 
            static_cast<uint8_t>(now_tm.tm_sec), 
            static_cast<uint16_t>(tm_spec.tv_nsec / 1000 % 1000), 
            static_cast<uint8_t>(now_tm.tm_wday)
        };
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
        static_cast<uint16_t>(sys.wMilliseconds), static_cast<uint8_t>(sys.wDayOfWeek), false};
#elif defined(TINY_CPP_MY_OS_UNIX)
    struct tm now_tm{};
    struct timespec tm_spec{};
    clock_gettime(CLOCK_REALTIME, &tm_spec);
    time_t now = tm_spec.tv_sec;
    const tm* OK = gmtime_r(&now, &now_tm);
    if (OK) {
        return {
            static_cast<uint32_t>(now_tm.tm_year + 1900), 
            static_cast<uint8_t>(now_tm.tm_mon + 1),
            static_cast<uint8_t>(now_tm.tm_mday), 
            static_cast<uint8_t>(now_tm.tm_hour),
            static_cast<uint8_t>(now_tm.tm_min), 
            static_cast<uint8_t>(now_tm.tm_sec), 
            static_cast<uint16_t>(tm_spec.tv_nsec / 1000 % 1000), 
            static_cast<uint8_t>(now_tm.tm_wday),
            false
        };
    }
    return {0, false};
#endif
}

inline SDT fromTimestamp(Tiny::DT::Duration timestamp, bool use_local_time, bool& ok) {
#ifdef TINY_CPP_MY_OS_WINDOWS
    const int64_t EPOCH_DIFF = 116444736000000000;
    int64_t ts = timestamp * 10000 + EPOCH_DIFF;

    LARGE_INTEGER ll{};
    ll.QuadPart = ts;

    FILETIME ft{};
    ft.dwLowDateTime = ll.LowPart;
    ft.dwHighDateTime = ll.HighPart;

    SYSTEMTIME utc{}, lc_tm{};
    if (FileTimeToSystemTime(&ft, &utc)) {
        if (use_local_time) {
            FILETIME ft_utc{};
            if (SystemTimeToFileTime(&utc, &ft_utc)) {
                FILETIME ft_local{};
                if (FileTimeToLocalFileTime(&ft_utc, &ft_local)) {
                    if (FileTimeToSystemTime(&ft_local, &lc_tm)) {
                        ok = true;
                        return {static_cast<uint32_t>(lc_tm.wYear), static_cast<uint8_t>(lc_tm.wMonth),
                                    static_cast<uint8_t>(lc_tm.wDay), static_cast<uint8_t>(lc_tm.wHour),
                                    static_cast<uint8_t>(lc_tm.wMinute), static_cast<uint8_t>(lc_tm.wSecond),
                                    lc_tm.wMilliseconds, static_cast<uint8_t>(lc_tm.wDayOfWeek)};
                    }
                }
            }
        } else {
            ok = true;
            return {static_cast<uint32_t>(utc.wYear), static_cast<uint8_t>(utc.wMonth),
                static_cast<uint8_t>(utc.wDay), static_cast<uint8_t>(utc.wHour),
                static_cast<uint8_t>(utc.wMinute), static_cast<uint8_t>(utc.wSecond),
                static_cast<uint16_t>(utc.wMilliseconds), static_cast<uint8_t>(utc.wDayOfWeek)};
        }
    }
#elif defined(TINY_CPP_MY_OS_UNIX)
    int64_t secs = (timestamp / 1000);
    time_t sp_time = secs;
    uint16_t milliseconds = timestamp % 1000;
    
    struct tm local;
    const tm* OK = use_local_time ? localtime_r(&sp_time, &local) : gmtime_r(&sp_time, &local);
    if (OK != nullptr) {
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

inline Tiny::DT::Duration toTimestamp(SDT date_time, bool use_local_time, bool& ok) {
#ifdef TINY_CPP_MY_OS_WINDOWS
    SYSTEMTIME user_time = {
        (WORD)date_time.year, (WORD)date_time.month, (WORD)date_time.weekday,
        (WORD)date_time.day, (WORD)date_time.hour, (WORD)date_time.minute,
        (WORD)date_time.second, (WORD)date_time.millisecond
    };

    FILETIME ftime = {};
    const int64_t EPOCH_DIFF = 116444736000000000;
    if (SystemTimeToFileTime(&user_time, &ftime)) {
        if (use_local_time) {
            FILETIME ft_utc{};
            if (LocalFileTimeToFileTime(&ftime, &ft_utc)) {
                LARGE_INTEGER ll = {};
                ll.LowPart = ft_utc.dwLowDateTime;
                ll.HighPart = ft_utc.dwHighDateTime;

                ok = true;
                return (ll.QuadPart - EPOCH_DIFF) / 10000;
            }
        }
        TIME_ZONE_INFORMATION tzi{};
        DWORD res = GetTimeZoneInformation(&tzi);
        if (res != TIME_ZONE_ID_INVALID) {
            int64_t bias_mins = tzi.Bias;
            if (res == TIME_ZONE_ID_DAYLIGHT) {
                bias_mins += tzi.DaylightBias;
            } else {
                bias_mins += tzi.StandardBias;
            }

            LARGE_INTEGER ll{};
            ll.LowPart = ftime.dwLowDateTime;
            ll.HighPart = ftime.dwHighDateTime;
            ok = true;
            return (ll.QuadPart - EPOCH_DIFF) / 10000;
        }
    }
#elif defined(TINY_CPP_MY_OS_UNIX)
    struct tm tm_now{}, tm_start{};
    int year = date_time.year - 1900;
    tm_now.tm_year = year;
    tm_now.tm_mon = date_time.month - 1;
    tm_now.tm_mday = date_time.day;
    tm_now.tm_hour = date_time.hour;
    tm_now.tm_min = date_time.minute;
    tm_now.tm_sec = date_time.second;
    tm_now.tm_isdst = -1;

    tm_start.tm_year = 70;
    tm_start.tm_mday = 1;
    tm_start.tm_isdst = -1;

    time_t sec = mktime(&tm_now);
    int64_t real = 0;
    if (sec != -1) {
        if (!use_local_time) {
            time_t now = time(nullptr);
            struct tm utc_tm{}, local_tm{};
            gmtime_r(&now, &utc_tm);
            localtime_r(&now, &local_tm);

            time_t utc_as_local = mktime(&utc_tm);
            time_t local_ts = mktime(&local_tm);
            time_t offset = local_ts - utc_as_local;

            sec -= offset;
            real = sec;
        } else {
            real = sec;
        }
        ok = true;
        return real * 1000LL + date_time.millisecond;
    }
#endif
    ok = false;
    return 0;
}

Tiny::DT::DateTime::DateTime(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second,
    uint16_t millisecond, uint8_t weekday, bool use_local_time)
        : _year(year), _month(month), _day(day), _hour(hour), _minute(minute), _second(second),
          _local_time(use_local_time), _milliseconds(millisecond), _w_day(weekday) {
    SDT dt = {year, month, day, hour, minute, second, millisecond, weekday};
    _timestamps = toTimestamp(dt, use_local_time, _valid);
}

Tiny::DT::DateTime::DateTime(Tiny::DT::Duration timestamps, bool use_local_time)
        : _local_time(use_local_time), _timestamps(timestamps) {
    SDT dt = fromTimestamp(timestamps, use_local_time, _valid);
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

Tiny::DT::Duration Tiny::DT::DateTime::timestamps() const {
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

bool Tiny::DT::DateTime::isLocalTime() const {
    return _local_time;
}

bool Tiny::DT::DateTime::operator==(const DateTime &date_time) const {
    return (_local_time == date_time._local_time) && (_timestamps == date_time._timestamps);
}

bool Tiny::DT::DateTime::operator!=(const DateTime &date_time) const {
    return (_local_time != date_time._local_time) || (_timestamps != date_time._timestamps);
}

bool Tiny::DT::DateTime::operator<(const DateTime &date_time) const {
    return (_local_time == date_time._local_time) && (_timestamps < date_time._timestamps);
}

bool Tiny::DT::DateTime::operator<=(const DateTime &date_time) const {
    return (_local_time == date_time._local_time) && (_timestamps <= date_time._timestamps);
}

bool Tiny::DT::DateTime::operator>(const DateTime &date_time) const {
    return (_local_time == date_time._local_time) && (_timestamps > date_time._timestamps);
}

bool Tiny::DT::DateTime::operator>=(const DateTime &date_time) const {
    return (_local_time == date_time._local_time) && (_timestamps >= date_time._timestamps);
}

Tiny::DT::DateTime Tiny::DT::DateTime::operator+(Tiny::DT::Duration other) const noexcept {
    return {_timestamps + other, _local_time};
}

Tiny::DT::DateTime Tiny::DT::DateTime::operator-(Tiny::DT::Duration other) const noexcept {
    return {_timestamps - other, _local_time};
}

Tiny::DT::DateTime & Tiny::DT::DateTime::operator+=(Tiny::DT::Duration other) noexcept {
    _timestamps += other;
    generateDateTime(_timestamps);
    return *this;
}

Tiny::DT::DateTime & Tiny::DT::DateTime::operator-=(Tiny::DT::Duration other) noexcept {
    _timestamps -= other;
    generateDateTime(_timestamps);
    return *this;
}

Tiny::DT::Duration Tiny::DT::DateTime::operator-(const DateTime &date_time) const noexcept {
    return _timestamps - date_time._timestamps;
}

bool Tiny::DT::DateTime::reset(Tiny::DT::Duration timestamps, bool use_local_time) {
    _timestamps = timestamps;
    _local_time = use_local_time;
    generateDateTime(_timestamps);
    return _valid;
}

bool Tiny::DT::DateTime::reset(uint32_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second,
        uint16_t millisecond, uint8_t weekday, bool use_local_time) {
    SDT dt = {year, month, day, hour, minute, second, millisecond, weekday};
    _timestamps = toTimestamp(dt, use_local_time, _valid);
    _local_time = use_local_time;
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
                oss << std::setw(2) << std::right << std::setfill('0') << (int)date_time.month();
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
                oss << std::setw(2) << std::right << std::setfill('0') << (int)date_time.day();
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
                oss << std::setw(2) << std::right << std::setfill('0') << static_cast<int>(date_time.weekday());
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

void Tiny::DT::DateTime::generateDateTime(Tiny::DT::Duration timestamps) {
    SDT dt = fromTimestamp(timestamps, _local_time, _valid);
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

Tiny::DT::Duration Tiny::DT::DateTime::correctTS(const DateTime &other) const {
    if (_local_time != other._local_time) {
        SDT sdt = {other._year, other._month, other._day, other._hour, other._minute, other._second, other._milliseconds};
        bool ok = false;
        return toTimestamp(sdt, _local_time, ok);
    }
    return other._timestamps;
}


Tiny::DT::Duration Tiny::DT::operator""_ms(unsigned long long milliseconds) noexcept {
    return milliseconds;
}

Tiny::DT::Duration Tiny::DT::operator""_s(unsigned long long seconds) noexcept {
    return seconds * 1000;
}

Tiny::DT::Duration Tiny::DT::operator""_m(unsigned long long minutes) noexcept {
    return minutes * 60000;
}

Tiny::DT::Duration Tiny::DT::operator""_h(unsigned long long hours) noexcept {
    return hours * 3600000;
}

Tiny::DT::Duration Tiny::DT::operator""_d(unsigned long long days) noexcept {
    return days * 86400000;
}

Tiny::DT::Duration Tiny::DT::operator""_w(unsigned long long weeks) noexcept {
    return weeks * 604800000;
}

Tiny::DT::Duration Tiny::DT::weeks(Tiny::DT::Duration timestamps) noexcept {
    return timestamps / 604800000;
}

Tiny::DT::Duration Tiny::DT::days(Tiny::DT::Duration timestamps) noexcept {
    return timestamps / 86400000;
}

Tiny::DT::Duration Tiny::DT::hours(Tiny::DT::Duration timestamps) noexcept {
    return timestamps / 3600000;
}

Tiny::DT::Duration Tiny::DT::minutes(Tiny::DT::Duration timestamps) noexcept {
    return timestamps / 60000;
}

Tiny::DT::Duration Tiny::DT::seconds(Tiny::DT::Duration timestamps) noexcept {
    return timestamps / 1000;
}

Tiny::DT::Duration Tiny::DT::milliseconds(Tiny::DT::Duration timestamps) noexcept {
    return timestamps;
}

Tiny::DT::Duration Tiny::DT::currentTimestamps() noexcept {
#ifdef TINY_CPP_MY_OS_WINDOWS
    SYSTEMTIME sys{};
    GetSystemTime(&sys);
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);

    LARGE_INTEGER ll{};
    ll.LowPart = ft.dwLowDateTime;
    ll.HighPart = ft.dwHighDateTime;
    const int64_t EPOCH_DIFF = 116444736000000000;
    return (ll.QuadPart - EPOCH_DIFF) / 10000;
#elif defined(TINY_CPP_MY_OS_UNIX)
    struct timespec tv{};
    clock_gettime(CLOCK_REALTIME, &tv);
    return tv.tv_sec * 1000LL + tv.tv_nsec / 1000000;
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

