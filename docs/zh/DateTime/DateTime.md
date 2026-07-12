# DateTime 模块

命名空间: `Tiny::DT`

---

## 目录

1. [模块简介](#1-模块简介)
2. [头文件](#2-头文件)
3. [类型定义](#3-类型定义)
4. [枚举](#4-枚举)
5. [辅助函数](#5-辅助函数)
6. [DateTime 类](#6-datetime-类)
7. [时间单位字面量](#7-时间单位字面量)
8. [时间转换函数](#8-时间转换函数)
9. [使用示例](#9-使用示例)
10. [注意事项](#10-注意事项)

---

## 1. 模块简介

`DateTime` 模块提供轻量级的日期时间处理功能，包括：

- 通过日期时间分量或毫秒时间戳构造日期时间
- 使用自定义格式字符串进行格式化
- 比较和算术运算符
- 时间单位用户定义字面量
- 获取当前时间戳

所有时间戳均以 Unix 纪元以来的毫秒数表示。

---

## 2. 头文件

```cpp
// CMake 方式
#include <Tiny/DateTime/DateTime.hpp>
// 直接复制源代码方式
#include "DateTime/DateTime.hpp"
```

---

## 3. 类型定义

```cpp
using Duration = std::int64_t;
```

**说明**: 有符号 64 位整数，表示毫秒数。

---

## 4. 枚举

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

| 枚举值 | 数值 | 简称 |
|--------|------|------|
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

| 枚举值 | 数值 | 简称 |
|--------|------|------|
| `Sunday` | 0 | `Sun` |
| `Monday` | 1 | `Mon` |
| `Tuesday` | 2 | `Tue` |
| `Wednesday` | 3 | `Wed` |
| `Thursday` | 4 | `Thur` |
| `Friday` | 5 | `Fri` |
| `Saturday` | 6 | `Sat` |

---

## 5. 辅助函数

### 5.1 monthName

```cpp
static const char* monthName(Month month, bool short_name = false) noexcept;
```

- **功能**: 获取月份名称
- **参数**:
  - `month` - 月份枚举值
  - `short_name` - 为 `true` 时返回缩写
- **返回值**: 月份名称字符串（例如 "January" 或 "Jan"）

### 5.2 weekDayName

```cpp
static const char* weekDayName(Weekday weekday, bool short_name = false) noexcept;
```

- **功能**: 获取星期名称
- **参数**:
  - `weekday` - 星期枚举值
  - `short_name` - 为 `true` 时返回缩写
- **返回值**: 星期名称字符串（例如 "Thursday" 或 "Thur"）

### 5.3 currentTimestamps

```cpp
Duration currentTimestamps() noexcept;
```

- **功能**: 获取当前系统时间戳（毫秒）
- **返回值**: 自 Unix 纪元以来的毫秒数

---

## 6. DateTime 类

### 6.1 类简介

表示本地时间或 UTC 时间的某个具体时刻。

### 6.2 构造函数

```cpp
DateTime(uint32_t year, uint8_t month, uint8_t day,
         uint8_t hour, uint8_t minute, uint8_t second,
         uint16_t millisecond = 0, uint8_t weekday = 0,
         bool use_local_time = true);

DateTime(Duration timestamps, bool use_local_time = true);
```

| 参数 | 类型 | 说明 |
|------|------|------|
| `year` | `uint32_t` | 年份（如 2026） |
| `month` | `uint8_t` | 月份（1-12） |
| `day` | `uint8_t` | 日期（1-31） |
| `hour` | `uint8_t` | 小时（0-23） |
| `minute` | `uint8_t` | 分钟（0-59） |
| `second` | `uint8_t` | 秒（0-59） |
| `millisecond` | `uint16_t` | 毫秒（0-999），默认 0 |
| `weekday` | `uint8_t` | 星期（0-6，周日=0），默认 0 |
| `use_local_time` | `bool` | `true` 表示本地时间，`false` 表示 UTC |
| `timestamps` | `Duration` | 毫秒时间戳 |

### 6.3 成员函数

#### 获取器

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

#### 状态查询

```cpp
bool isValid() const;
bool isLocalTime() const;
```

| 函数 | 返回值 |
|------|--------|
| `isValid()` | 日期时间转换成功则返回 `true` |
| `isLocalTime()` | 使用本地时间则返回 `true` |

### 6.4 比较运算符

```cpp
bool operator==(const DateTime& date_time) const;
bool operator!=(const DateTime& date_time) const;
bool operator<(const DateTime& date_time) const;
bool operator<=(const DateTime& date_time) const;
bool operator>(const DateTime& date_time) const;
bool operator>=(const DateTime& date_time) const;
```

**说明**: 比较运算要求两个对象使用相同的时间模式（本地/UTC）。

### 6.5 算术运算符

```cpp
DateTime  operator+(Duration other) const noexcept;
DateTime  operator-(Duration other) const noexcept;
DateTime& operator+=(Duration other) noexcept;
DateTime& operator-=(Duration other) noexcept;
Duration  operator-(const DateTime& date_time) const noexcept;
```

| 运算符 | 说明 |
|--------|------|
| `+` | 返回前进 `other` 毫秒后的新 `DateTime` |
| `-` (Duration) | 返回后退 `other` 毫秒后的新 `DateTime` |
| `+=` | 将此对象前进 `other` 毫秒 |
| `-=` | 将此对象后退 `other` 毫秒 |
| `-` (DateTime) | 返回两个时间戳之间的毫秒差 |

### 6.6 reset 方法

```cpp
bool reset(Duration timestamps, bool use_local_time = true);
bool reset(uint32_t year, uint8_t month, uint8_t day,
           uint8_t hour, uint8_t minute, uint8_t second,
           uint16_t millisecond = 0, uint8_t weekday = 0,
           bool use_local_time = true);
```

- **功能**: 使用新值重新初始化对象
- **返回值**: 新值有效则返回 `true`

### 6.7 格式化方法

#### formatString（成员函数）

```cpp
std::string formatString(const char* format) const;
```

- **功能**: 使用格式字符串格式化此 `DateTime`
- **参数**:
  - `format` - 格式说明符字符串
- **返回值**: 格式化后的字符串

**格式说明符**:

| 说明符 | 含义 | 示例 |
|--------|------|------|
| `y` | 年份 | `yy` -> "26"，`yyyy` -> "2026" |
| `M` | 月份 | `MM` -> "07"，`MMM` -> "Jul"，`MMMM` -> "July" |
| `d` | 日期 | `dd` -> "02" |
| `H` | 小时（24 小时制） | `HH` -> "14" |
| `h` | 小时（12 小时制） | `hh` -> "02" |
| `m` | 分钟 | `mm` -> "30" |
| `s` | 秒 | `ss` -> "45" |
| `a` | 上午/下午 | `a` -> "PM" |
| `c` | 星期 | `cc` -> "04"，`ccc` -> "Thur"，`cccc` -> "Thursday" |
| `S` | 毫秒 | `SSS` -> "500" |

其他字符将原样输出。

### 6.8 静态方法

#### now

```cpp
static DateTime now(bool use_local_time = true);
```

- **功能**: 获取当前日期时间
- **参数**: `use_local_time` - `true` 为本地时间，`false` 为 UTC
- **返回值**: 表示当前时刻的 `DateTime` 对象

#### formatString（静态函数）

```cpp
static std::string formatString(const char* format, const DateTime& date_time);
```

- **功能**: 使用格式字符串格式化 `DateTime`
- **参数**:
  - `format` - 格式说明符字符串
  - `date_time` - 要格式化的日期时间
- **返回值**: 格式化后的字符串

---

## 7. 时间单位字面量

```cpp
Duration operator""_ms(unsigned long long milliseconds) noexcept;
Duration operator""_s(unsigned long long seconds) noexcept;
Duration operator""_m(unsigned long long minutes) noexcept;
Duration operator""_h(unsigned long long hours) noexcept;
Duration operator""_d(unsigned long long days) noexcept;
Duration operator""_w(unsigned long long weeks) noexcept;
```

| 字面量 | 单位 | 示例 |
|--------|------|------|
| `_ms` | 毫秒 | `500_ms` |
| `_s` | 秒 | `30_s` |
| `_m` | 分钟 | `5_m` |
| `_h` | 小时 | `2_h` |
| `_d` | 天 | `1_d` |
| `_w` | 周 | `2_w` |

---

## 8. 时间转换函数

```cpp
Duration weeks(Duration timestamps) noexcept;
Duration days(Duration timestamps) noexcept;
Duration hours(Duration timestamps) noexcept;
Duration minutes(Duration timestamps) noexcept;
Duration seconds(Duration timestamps) noexcept;
Duration milliseconds(Duration timestamps) noexcept;
```

- **功能**: 将毫秒时间戳转换为指定的整数单位
- **返回值**: 指定单位的整数数量

---

## 9. 使用示例

### 9.1 基本构造

```cpp
#include "DateTime/DateTime.hpp"
#include <iostream>

int main() {
    using namespace Tiny::DT;

    DateTime dt(2026, 7, 2, 12, 30, 0);
    std::cout << formatString("yyyy-MM-dd HH:mm:ss", dt) << std::endl;
    // 输出: 2026-07-02 12:30:00

    return 0;
}
```

### 9.2 当前时间

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

### 9.3 算术运算

```cpp
#include "DateTime/DateTime.hpp"
#include <iostream>

int main() {
    using namespace Tiny::DT;

    DateTime dt = DateTime::now();
    auto tomorrow = dt + 1_d;
    auto diff = tomorrow - dt;

    std::cout << "相差小时数: " << hours(diff) << std::endl;

    return 0;
}
```

### 9.4 时间戳转换

```cpp
#include "DateTime/DateTime.hpp"
#include <iostream>

int main() {
    using namespace Tiny::DT;

    Duration ts = currentTimestamps();
    std::cout << "秒数: " << seconds(ts) << std::endl;
    std::cout << "自纪元以来的天数: " << days(ts) << std::endl;

    return 0;
}
```

---

## 10. 注意事项

### 10.1 时间戳基准

- 所有内部时间戳均为自 Unix 纪元（1970-01-01 00:00:00 UTC）以来的毫秒数。

### 10.2 本地时间与 UTC

- `use_local_time = true` 使用系统本地时区。
- `use_local_time = false` 使用 UTC。
- 比较不同时间模式的 `DateTime` 对象可能产生意外结果。

### 10.3 有效性

- 如果构造或 `reset()` 失败，`isValid()` 返回 `false`，获取器返回默认值或零。

### 10.4 溢出

- 算术运算使用有符号 64 位数学。大数值的减法可能会下溢；请使用合适的 `Duration` 值。
