# Events 模块 - Event 类

命名空间: `Tiny`

---

## 目录

1. [类简介](#1-类简介)
2. [头文件](#2-头文件)
3. [构造函数](#3-构造函数)
4. [析构函数](#4-析构函数)
5. [赋值运算符](#5-赋值运算符)
6. [成员函数](#6-成员函数)
7. [EventsMap 类](#7-eventsmap-类)
8. [使用示例](#8-使用示例)
9. [注意事项](#9-注意事项)

---

## 1. 类简介

`Event` 类提供异步事件管理功能，支持条件触发、定时延迟、重复执行次数控制。内部使用独立线程执行事件循环，适用于定时任务、后台处理等场景。

### 核心特性

- **条件触发**: 支持自定义条件函数，条件满足时才执行
- **定时执行**: 可设置执行间隔（毫秒）
- **重复控制**: 支持设置执行次数，0 表示无限循环
- **线程安全**: 使用原子变量控制运行状态
- **异常处理**: 自动捕获并报告回调中的异常

---

## 2. 头文件

```cpp
// CMake 方式
#include <Tiny/Events/Events.hpp>
// 直接复制源代码方式
#include "Events/Events.hpp"
```

---

## 3. 构造函数

### 3.1 构造带条件的事件

```cpp
Event(uint32_t id, std::string name,
      const std::function<bool()>& condition,
      const std::function<void(const std::atomic<bool>&)>& event);
```

| 参数 | 类型 | 说明 | 是否可为空 |
|------|------|------|------------|
| `id` | `uint32_t` | 事件唯一标识符 | 否 |
| `name` | `std::string` | 事件名称 | 否（可为空字符串） |
| `condition` | `std::function<bool()>` | 执行条件回调 | 是 |
| `event` | `std::function<void(const std::atomic<bool>&)>` | 事件执行回调 | 是 |

**说明**: 当 `condition` 返回 `true` 时执行 `event` 回调。

### 3.2 构造无条件的事件

```cpp
Event(uint32_t id, std::string name,
      const std::function<void(const std::atomic<bool>&)>& event);
```

| 参数 | 类型 | 说明 | 是否可为空 |
|------|------|------|------------|
| `id` | `uint32_t` | 事件唯一标识符 | 否 |
| `name` | `std::string` | 事件名称 | 否 |
| `event` | `std::function<void(const std::atomic<bool>&)>` | 事件执行回调 | 是 |

**说明**: 条件默认为 `[] { return true; }`，即始终执行。

### 3.3 构造空事件

```cpp
Event(uint32_t id, std::string name);
```

| 参数 | 类型 | 说明 | 是否可为空 |
|------|------|------|------------|
| `id` | `uint32_t` | 事件唯一标识符 | 否 |
| `name` | `std::string` | 事件名称 | 否 |

**说明**: 仅设置 ID 和名称，后续需调用 `setCondition()` 和 `setEvent()` 设置回调。

### 3.4 拷贝构造函数

```cpp
Event(const Event& other);
```

**说明**: 复制源对象的所有属性，但重置执行计数器，不复制线程状态。

---

## 4. 析构函数

```cpp
virtual ~Event();
```

**行为**:
- 自动调用 `stop()` 停止事件
- 等待事件线程结束（join）
- 释放线程资源

---

## 5. 赋值运算符

```cpp
Event& operator=(const Event& other);
```

**行为**:
1. 先停止当前事件（如果正在运行）
2. 复制源对象的所有属性
3. 重置执行计数器
4. 不复制线程状态

**返回值**: 自身引用

---

## 6. 成员函数

### 6.1 设置函数

#### setID

```cpp
void setID(uint32_t id);
```
- **功能**: 设置事件 ID
- **参数**: `id` - 新的标识符
- **返回值**: 无

#### setName

```cpp
void setName(const std::string& name);
```
- **功能**: 设置事件名称
- **参数**: `name` - 新的名称
- **返回值**: 无

#### setDelayMS

```cpp
void setDelayMS(uint32_t delay);
```
- **功能**: 设置事件执行间隔（毫秒）
- **参数**: `delay` - 延迟时间，单位毫秒，默认 1000ms
- **返回值**: 无
- **注意事项**: 事件运行中修改会立即生效

#### setRepeatCount

```cpp
void setRepeatCount(uint32_t count);
```
- **功能**: 设置重复执行次数
- **参数**: `count` - 执行次数，0 表示无限循环
- **返回值**: 无
- **默认值**: 1（执行一次）

#### setCondition

```cpp
void setCondition(const std::function<bool()>& condition);
```
- **功能**: 设置执行条件
- **参数**: `condition` - 返回 bool 的回调函数
- **返回值**: 无
- **默认条件**: `[] { return true; }`

#### setAllowedFailedEnabled

```cpp
void setAllowedFailedEnabled(bool enabled);
```
- **功能**: 设置是否允许失败的执行
- **参数**: `enabled` - `true` 表示允许失败，`false` 表示遇到失败时停止
- **返回值**: 无
- **默认值**: `true`（默认允许失败）

#### setEvent

```cpp
void setEvent(const std::function<void(const std::atomic<bool>&)>& callback);
```
- **功能**: 设置事件回调函数
- **参数**: `callback` - 事件执行体，接收运行状态引用
- **返回值**: 无

### 6.2 查询函数

#### eventID

```cpp
[[nodiscard]] uint32_t eventID() const;
```
- **功能**: 获取事件 ID
- **返回值**: 事件标识符

#### eventName

```cpp
[[nodiscard]] const std::string& eventName() const;
```
- **功能**: 获取事件名称
- **返回值**: 事件名称的常量引用

#### eventDelayMS

```cpp
[[nodiscard]] uint32_t eventDelayMS() const;
```
- **功能**: 获取执行延迟
- **返回值**: 延迟毫秒数

#### eventRepeatCount

```cpp
[[nodiscard]] uint32_t eventRepeatCount() const;
```
- **功能**: 获取设置的重复次数
- **返回值**: 重复次数（0 表示无限）

#### executionCount

```cpp
[[nodiscard]] uint32_t executionCount() const;
```
- **功能**: 获取事件已执行的次数
- **返回值**: 执行次数

#### allowedFailedEnabled

```cpp
[[nodiscard]] bool allowedFailedEnabled() const;
```
- **功能**: 检查是否允许失败的执行
- **返回值**: `true` 表示允许失败

#### isRunning

```cpp
[[nodiscard]] bool isRunning() const;
```
- **功能**: 检查事件是否正在运行
- **返回值**: `true` 表示运行中

#### hasEvent

```cpp
[[nodiscard]] bool hasEvent() const;
```
- **功能**: 检查是否设置了事件回调
- **返回值**: `true` 表示已设置回调

### 6.3 控制函数

#### run

```cpp
void run();
```
- **功能**: 启动事件执行
- **返回值**: 无
- **约束条件**:
  - 事件已在运行时不执行任何操作
  - 未设置事件回调时不执行
  - 首次调用创建后台线程
  - 重复调用唤醒等待中的线程

**执行流程**:
1. 检查是否已运行，是则返回
2. 检查是否有事件回调，否则返回
3. 创建后台线程
4. 在线程中循环执行：
   - 检查停止标志
   - 检查执行条件
   - 执行事件回调
   - 等待延迟时间
   - 检查重复次数

#### stop

```cpp
void stop();
```
- **功能**: 停止事件执行
- **返回值**: 无
- **注意事项**: 不会立即终止线程，只是设置停止标志，事件会在下一次循环时退出

---

## 7. EventsMap 类

`EventsMap` 类用于通过事件 ID 管理多个 `Event` 对象。

### 7.1 类型定义

```cpp
using constIter = std::unordered_map<size_t, Event>::const_iterator;
```

### 7.2 构造函数与析构函数

```cpp
EventsMap();
~EventsMap();
```

**析构函数行为**:
- 析构前自动调用 `waitAllEvents()`。

### 7.3 事件管理

#### execEvent

```cpp
bool execEvent(const Event& event);
bool execEvent(size_t event_id);
```
- **功能**: 添加并运行事件，或通过 ID 运行已有事件
- **返回值**: `true` 表示成功

#### addEvent

```cpp
bool addEvent(const Event& event);
```
- **功能**: 添加事件但不运行
- **返回值**: `true` 表示成功（ID 不重复）

#### removeEvent

```cpp
bool removeEvent(size_t event_id);
```
- **功能**: 停止并移除指定 ID 的事件
- **返回值**: `true` 表示成功

#### removeAllFreeEvents

```cpp
bool removeAllFreeEvents();
```
- **功能**: 移除所有未在运行的事件
- **返回值**: `true`

### 7.4 控制函数

#### stopEvent

```cpp
void stopEvent(size_t event_id);
```
- **功能**: 停止指定 ID 的运行中事件

#### waitEvent

```cpp
void waitEvent(size_t event_id);
```
- **功能**: 停止并等待指定事件结束

#### stopAllEvents

```cpp
void stopAllEvents();
```
- **功能**: 停止映射中的所有事件

#### waitAllEvents

```cpp
void waitAllEvents();
```
- **功能**: 停止并等待所有事件结束

### 7.5 设置函数

#### setConditionByID

```cpp
bool setConditionByID(size_t event_id, const std::function<bool()>& condition);
```

#### setEventByID

```cpp
bool setEventByID(size_t event_id, const std::function<void(const std::atomic<bool>&)>& event);
```

#### setDelayByID

```cpp
bool setDelayByID(size_t event_id, uint32_t delay_ms);
```

#### setRepeatByID

```cpp
bool setRepeatByID(size_t event_id, uint32_t repeat_count);
```

#### setAllowedFailedEnabledByID

```cpp
bool setAllowedFailedEnabledByID(size_t event_id, bool enabled);
```

- **功能**: 通过 ID 修改事件属性
- **返回值**: `true` 表示事件存在并已更新

### 7.6 查询函数

#### exist

```cpp
bool exist(size_t event_id) const;
```
- **功能**: 检查事件 ID 是否存在
- **返回值**: `true` 表示存在

#### event

```cpp
const Event& event(size_t event_id) const;
```
- **功能**: 通过 ID 获取事件
- **返回值**: 事件引用
- **异常**: ID 不存在时抛出异常

#### size

```cpp
size_t size() const;
```
- **功能**: 获取映射中事件数量

#### eventIDList

```cpp
std::vector<size_t> eventIDList() const;
```
- **功能**: 获取所有事件 ID 列表

### 7.7 迭代器

```cpp
constIter cbegin() const;
constIter cend() const;
```
- **功能**: 事件映射的常量迭代器

---

## 8. 使用示例

### 8.1 基本定时器

```cpp
#include "Events/Events.hpp"
#include <iostream>

int main() {
    // 创建定时器事件（每秒执行一次，共执行 5 次）
    Tiny::Event timer(1, "Timer",
        [] { return true; },  // 条件：始终执行
        [](const std::atomic<bool>& running) {
            if (running) {
                std::cout << "Timer tick!" << std::endl;
            }
        }
    );
    
    timer.setDelayMS(1000);  // 1 秒间隔
    timer.setRepeatCount(5); // 执行 5 次
    timer.run();
    
    // 等待事件完成
    std::this_thread::sleep_for(std::chrono::seconds(6));
    
    return 0;
}
```

### 8.2 条件触发事件

```cpp
#include "Events/Events.hpp"
#include <iostream>
#include <atomic>

std::atomic<bool> shouldRun{false};

int main() {
    Tiny::Event conditionalEvent(2, "Conditional",
        [] { return shouldRun.load(); },  // 条件：shouldRun 为 true
        [](const std::atomic<bool>& running) {
            std::cout << "Condition met, executing!" << std::endl;
        }
    );
    
    conditionalEvent.setDelayMS(500);
    conditionalEvent.setRepeatCount(0);  // 无限循环
    conditionalEvent.run();
    
    // 3 秒后启用条件
    std::this_thread::sleep_for(std::chrono::seconds(3));
    shouldRun = true;
    
    // 再运行 5 秒
    std::this_thread::sleep_for(std::chrono::seconds(5));
    conditionalEvent.stop();
    
    return 0;
}
```

### 8.3 延迟初始化事件

```cpp
#include "Events/Events.hpp"
#include <iostream>

int main() {
    // 先创建空事件
    Tiny::Event delayedEvent(3, "Delayed");
    
    // 稍后设置回调
    int counter = 0;
    delayedEvent.setCondition([&counter] { return counter < 10; });
    delayedEvent.setEvent([&counter](const std::atomic<bool>& running) {
        if (running) {
            std::cout << "Count: " << ++counter << std::endl;
        }
    });
    
    delayedEvent.setDelayMS(100);
    delayedEvent.run();
    
    // 等待完成
    while (delayedEvent.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return 0;
}
```

### 8.4 多事件管理

```cpp
#include "Events/Events.hpp"
#include <iostream>
#include <vector>

int main() {
    std::vector<Tiny::Event> events;
    
    // 创建多个定时器
    for (int i = 0; i < 3; ++i) {
        events.emplace_back(i, "Timer" + std::to_string(i));
        events[i].setDelayMS(1000 * (i + 1));
        events[i].setRepeatCount(3);
        events[i].setEvent([i](const std::atomic<bool>&) {
            std::cout << "Event " << i << " triggered!" << std::endl;
        });
        events[i].run();
    }
    
    // 等待所有事件完成
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    return 0;
}
```

---

## 9. 注意事项

### 9.1 线程安全

- `Event` 类的成员函数不是线程安全的
- 不要在事件回调中修改事件自身的属性
- 使用原子变量或互斥锁保护共享数据

### 9.2 异常处理

事件回调中的异常会被捕获并输出到 stderr：

```cpp
Event event(1, "Test", [](const std::atomic<bool>&) {
    throw std::runtime_error("Error!");
});
// 输出: "Tiny::Event: An error has occurred: Error!"
```

### 9.3 停止事件

- `stop()` 只是设置停止标志，不会强制终止线程
- 事件会在下一次循环检查时退出
- 如果事件正在执行回调，会等待回调完成

### 9.4 资源管理

- 析构函数会自动调用 `stop()` 并等待线程结束
- 避免在事件运行时销毁事件对象
- 使用智能指针管理事件对象时确保生命周期足够长

### 9.5 延迟设置

- `setDelayMS()` 可以在事件运行时修改
- 修改后会在下一次等待时生效
- 最小延迟受系统调度影响，不宜设置过小（< 10ms）
