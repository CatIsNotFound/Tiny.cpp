# DateTime Module

Namespace: `Tiny::DT`

---

## Table of Contents

1. [Module Overview](#1-module-overview)
2. [Header File](#2-header-file)
3. [Type Definitions](#3-type-definitions)
4. [Enums](#4-enums)
5. [Helper Functions](#5-helper-functions)
6. [DateTime Class](#6-datetime-class)
7. [Time Unit Literals](#7-time-unit-literals)
8. [Time Conversion Functions](#8-time-conversion-functions)
9. [Usage Examples](#9-usage-examples)
10. [Notes](#10-notes)

---

## 1. Module Overview

The `DateTime` module provides lightweight date/time handling, including:

- Date/time construction from components or millisecond timestamps
- Formatting via custom format strings
- Comparison and arithmetic operators
- Time unit user-defined literals
- Current timestamp retrieval

All timestamps are represented as milliseconds since the Unix epoch.

---

## 2. Header File

```cpp
// CMake method
#include <Tiny/DateTime/DateTime.hpp>
// Direct source copy method
#include "DateTime/DateTime.hpp"
```

---

## 3. Type Definitions

```cpp
using Duration = std::int64_t;
```

**Description**: Signed 64-bit integer representing time in milliseconds.

---

## 4. Enums

### 4.1 Month

```cpp
enum Month : uint8_t {
    January = 1, Jan = 1,
    February,    Feb = 2,
    March,       Mar = 3,
    April,       Apr = 4,
    May,
    June,        Jun = 6,
    July,        Jul = 7,
    August = 8,  Aug = 8,
    September = 9, Sep = 9,
    October = 10,  Oct = 10,
    November = 11, Nov = 11,
    December = 12, Dec = 12
};
```

| Enum Value | Value | Short Alias |
|------------|-------|-------------|
| `January` | 1 | `Jan` |
| `February` | 2 | `Feb` |
| `March` | 3 | `Mar` |
| `April` | 4 | `Apr` |
| `May` | 5 | - |
| `June` | 6 | `Jun` |
| `July` | 7 | `Jul` |
| `August` | 8 | `Aug` |
| `September` | 9 | `Sep` |
| `October` | 10 | `Oct` |
| `November` | 11 | `Nov` |
| `December` | 12 | `Dec` |

### 4.2 Weekday

```cpp
enum Weekday : uint8_t {
    Sunday,    Sun = 0,
    Monday,    Mon = 1,
    Tuesday,   Tue = 2,
    Wednesday, Wed = 3,
    Thursday,  Thur = 4,
    Friday,    Fri = 5,
    Saturday,  Sat = 6
};
```

| Enum Value | Value | Short Alias |
|------------|-------|-------------|
| `Sunday` | 0 | `Sun` |
| `Monday` | 1 | `Mon` |
| `Tuesday` | 2 | `Tue` |
| `Wednesday` | 3 | `Wed` |
| `Thursday` | 4 | `Thur` |
| `Friday` | 5 | `Fri` |
| `Saturday` | 6 | `Sat` |

---

## 5. Helper Functions

### 5.1 monthName

```cpp
static const char* monthName(Month month, bool short_name = false) noexcept;
```

- **Function**: Get month name
- **Parameters**:
  - `month` - Month enum value
  - `short_name` - Return abbreviated name if `true`
- **Return Value**: Month name string (e.g., "January" or "Jan")

### 5.2 weekDayName

```cpp
static const char* weekDayName(Weekday weekday, bool short_name = false) noexcept;
```

- **Function**: Get weekday name
- **Parameters**:
  - `weekday` - Weekday enum value
  - `short_name` - Return abbreviated name if `true`
- **Return Value**: Weekday name string (e.g., "Thursday" or "Thur")

### 5.3 currentTimestamps

```cpp
Duration currentTimestamps() noexcept;
```

- **Function**: Get current system timestamp in milliseconds
- **Return Value**: Milliseconds since Unix epoch

---

## 6. DateTime Class

### 6.1 Class Overview

Represents a specific point in time using local or UTC time.

### 6.2 Constructors

```cpp
DateTime(uint32_t year, uint8_t month, uint8_t day,
         uint8_t hour, uint8_t minute, uint8_t second,
         uint16_t millisecond = 0, uint8_t weekday = 0,
         bool use_local_time = true);

DateTime(Duration timestamps, bool use_local_time = true);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `year` | `uint32_t` | Year (e.g., 2026) |
| `month` | `uint8_t` | Month (1-12) |
| `day` | `uint8_t` | Day (1-31) |
| `hour` | `uint8_t` | Hour (0-23) |
| `minute` | `uint8_t` | Minute (0-59) |
| `second` | `uint8_t` | Second (0-59) |
| `millisecond` | `uint16_t` | Milliseconds (0-999), default 0 |
| `weekday` | `uint8_t` | Weekday (0-6, Sunday=0), default 0 |
| `use_local_time` | `bool` | Use local time if `true`, UTC otherwise |
| `timestamps` | `Duration` | Millisecond timestamp |

### 6.3 Member Functions

#### Getters

```cpp
Duration timestamps() const;
uint32_t year() const;
uint8_t  month() const;
uint8_t  day() const;
uint8_t  hour() const;
uint8_t  minute() const;
uint8_t  second() const;
uint16_t millisecond() const;
uint8_t  weekday() const;
```

#### Status Queries

```cpp
bool isValid() const;
bool isLocalTime() const;
```

| Function | Return Value |
|----------|--------------|
| `isValid()` | `true` if the date/time was successfully converted |
| `isLocalTime()` | `true` if the object uses local time |

### 6.4 Comparison Operators

```cpp
bool operator==(const DateTime& date_time) const;
bool operator!=(const DateTime& date_time) const;
bool operator<(const DateTime& date_time) const;
bool operator<=(const DateTime& date_time) const;
bool operator>(const DateTime& date_time) const;
bool operator>=(const DateTime& date_time) const;
```

**Note**: Comparisons require both objects to use the same time mode (local/UTC).

### 6.5 Arithmetic Operators

```cpp
DateTime  operator+(Duration other) const noexcept;
DateTime  operator-(Duration other) const noexcept;
DateTime& operator+=(Duration other) noexcept;
DateTime& operator-=(Duration other) noexcept;
Duration  operator-(const DateTime& date_time) const noexcept;
```

| Operator | Description |
|----------|-------------|
| `+` | Returns a new `DateTime` advanced by `other` milliseconds |
| `-` (Duration) | Returns a new `DateTime` moved back by `other` milliseconds |
| `+=` | Advances this object by `other` milliseconds |
| `-=` | Moves this object back by `other` milliseconds |
| `-` (DateTime) | Returns the difference between two timestamps in milliseconds |

### 6.6 Reset Methods

```cpp
bool reset(Duration timestamps, bool use_local_time = true);
bool reset(uint32_t year, uint8_t month, uint8_t day,
           uint8_t hour, uint8_t minute, uint8_t second,
           uint16_t millisecond = 0, uint8_t weekday = 0,
           bool use_local_time = true);
```

- **Function**: Reinitialize the object with new values
- **Return Value**: `true` if the new values are valid

### 6.7 Format Methods

#### formatString (member)

```cpp
std::string formatString(const char* format) const;
```

- **Function**: Format this `DateTime` using a format string
- **Parameter**:
  - `format` - Format specifier string
- **Return Value**: Formatted string

**Format Specifiers**:

| Specifier | Meaning | Example |
|-----------|---------|---------|
| `y` | Year | `yy` -> "26", `yyyy` -> "2026" |
| `M` | Month | `MM` -> "07", `MMM` -> "Jul", `MMMM` -> "July" |
| `d` | Day | `dd` -> "02" |
| `H` | Hour (24-hour) | `HH` -> "14" |
| `h` | Hour (12-hour) | `hh` -> "02" |
| `m` | Minute | `mm` -> "30" |
| `s` | Second | `ss` -> "45" |
| `a` | AM/PM | `a` -> "PM" |
| `c` | Weekday | `cc` -> "04", `ccc` -> "Thur", `cccc` -> "Thursday" |
| `S` | Millisecond | `SSS` -> "500" |

Any other characters are copied literally.

### 6.8 Static Methods

#### now

```cpp
static DateTime now(bool use_local_time = true);
```

- **Function**: Get current date/time
- **Parameter**: `use_local_time` - Use local time if `true`, UTC otherwise
- **Return Value**: `DateTime` object representing now

#### formatString (static)

```cpp
static std::string formatString(const char* format, const DateTime& date_time);
```

- **Function**: Format a `DateTime` using a format string
- **Parameters**:
  - `format` - Format specifier string
  - `date_time` - Date/time to format
- **Return Value**: Formatted string

---

## 7. Time Unit Literals

```cpp
Duration operator""_ms(unsigned long long milliseconds) noexcept;
Duration operator""_s(unsigned long long seconds) noexcept;
Duration operator""_m(unsigned long long minutes) noexcept;
Duration operator""_h(unsigned long long hours) noexcept;
Duration operator""_d(unsigned long long days) noexcept;
Duration operator""_w(unsigned long long weeks) noexcept;
```

| Literal | Unit | Example |
|---------|------|---------|
| `_ms` | Milliseconds | `500_ms` |
| `_s` | Seconds | `30_s` |
| `_m` | Minutes | `5_m` |
| `_h` | Hours | `2_h` |
| `_d` | Days | `1_d` |
| `_w` | Weeks | `2_w` |

---

## 8. Time Conversion Functions

```cpp
Duration weeks(Duration timestamps) noexcept;
Duration days(Duration timestamps) noexcept;
Duration hours(Duration timestamps) noexcept;
Duration minutes(Duration timestamps) noexcept;
Duration seconds(Duration timestamps) noexcept;
Duration milliseconds(Duration timestamps) noexcept;
```

- **Function**: Convert a millisecond timestamp into whole units
- **Return Value**: Whole number of the requested unit

---

## 9. Usage Examples

### 9.1 Basic Construction

```cpp
#include "DateTime/DateTime.hpp"
#include <iostream>

int main() {
    using namespace Tiny::DT;

    DateTime dt(2026, 7, 2, 12, 30, 0);
    std::cout << formatString("yyyy-MM-dd HH:mm:ss", dt) << std::endl;
    // Output: 2026-07-02 12:30:00

    return 0;
}
```

### 9.2 Current Time

```cpp
#include "DateTime/DateTime.hpp"
#include <iostream>

int main() {
    using namespace Tiny::DT;

    auto now_dt = DateTime::now();
    std::cout << formatString("yyyy-MM-dd HH:mm:ss.SSS", now_dt) << std::endl;

    return 0;
}
```

### 9.3 Arithmetic

```cpp
#include "DateTime/DateTime.hpp"
#include <iostream>

int main() {
    using namespace Tiny::DT;

    DateTime dt = DateTime::now();
    auto tomorrow = dt + 1_d;
    auto diff = tomorrow - dt;

    std::cout << "Diff in hours: " << hours(diff) << std::endl;

    return 0;
}
```

### 9.4 Timestamp Conversion

```cpp
#include "DateTime/DateTime.hpp"
#include <iostream>

int main() {
    using namespace Tiny::DT;

    Duration ts = currentTimestamps();
    std::cout << "Seconds: " << seconds(ts) << std::endl;
    std::cout << "Days since epoch: " << days(ts) << std::endl;

    return 0;
}
```

---

## 10. Notes

### 10.1 Timestamp Base

- All internal timestamps are milliseconds relative to the Unix epoch (1970-01-01 00:00:00 UTC).

### 10.2 Local vs UTC

- `use_local_time = true` uses the system local timezone.
- `use_local_time = false` uses UTC.
- Comparing `DateTime` objects with different time modes may yield unexpected results.

### 10.3 Validity

- If construction or `reset()` fails, `isValid()` returns `false` and getters return default/zero values.

### 10.4 Overflow

- Arithmetic uses signed 64-bit math. Large subtractions may underflow; use appropriate `Duration` values.
