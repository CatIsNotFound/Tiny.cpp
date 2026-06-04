# Events Module - Event Class

Namespace: `Tiny`

---

## Table of Contents

1. [Class Overview](#1-class-overview)
2. [Header File](#2-header-file)
3. [Constructors](#3-constructors)
4. [Destructor](#4-destructor)
5. [Assignment Operator](#5-assignment-operator)
6. [Member Functions](#6-member-functions)
7. [Usage Examples](#7-usage-examples)
8. [Notes](#8-notes)

---

## 1. Class Overview

The `Event` class provides asynchronous event management functionality, supporting conditional triggering, timed delays, and repeat execution count control. It uses an independent thread internally to execute the event loop, suitable for scheduled tasks, background processing, and other scenarios.

### Core Features

- **Conditional Triggering**: Supports custom condition functions that execute only when conditions are met
- **Timed Execution**: Configurable execution interval (milliseconds)
- **Repeat Control**: Supports setting execution count, 0 means infinite loop
- **Thread Safety**: Uses atomic variables to control running state
- **Exception Handling**: Automatically catches and reports exceptions in callbacks

---

## 2. Header File

```cpp
#include "Events/Events.hpp"
```

---

## 3. Constructors

### 3.1 Constructor with Condition

```cpp
Event(uint32_t id, std::string name,
      const std::function<bool()>& condition,
      const std::function<void(const std::atomic<bool>&)>& event);
```

| Parameter | Type | Description | Nullable |
|-----------|------|-------------|----------|
| `id` | `uint32_t` | Unique event identifier | No |
| `name` | `std::string` | Event name | No (can be empty string) |
| `condition` | `std::function<bool()>` | Execution condition callback | Yes |
| `event` | `std::function<void(const std::atomic<bool>&)>` | Event execution callback | Yes |

**Description**: The `event` callback is executed when `condition` returns `true`.

### 3.2 Constructor without Condition

```cpp
Event(uint32_t id, std::string name,
      const std::function<void(const std::atomic<bool>&)>& event);
```

| Parameter | Type | Description | Nullable |
|-----------|------|-------------|----------|
| `id` | `uint32_t` | Unique event identifier | No |
| `name` | `std::string` | Event name | No |
| `event` | `std::function<void(const std::atomic<bool>&)>` | Event execution callback | Yes |

**Description**: Condition defaults to `[] { return true; }`, meaning always execute.

### 3.3 Empty Event Constructor

```cpp
Event(uint32_t id, std::string name);
```

| Parameter | Type | Description | Nullable |
|-----------|------|-------------|----------|
| `id` | `uint32_t` | Unique event identifier | No |
| `name` | `std::string` | Event name | No |

**Description**: Only sets ID and name, `setCondition()` and `setEvent()` must be called later to set callbacks.

### 3.4 Copy Constructor

```cpp
Event(const Event& other);
```

**Description**: Copies all properties of the source object, but resets the execution counter, does not copy thread state.

---

## 4. Destructor

```cpp
virtual ~Event();
```

**Behavior**:
- Automatically calls `stop()` to stop the event
- Waits for the event thread to finish (join)
- Releases thread resources

---

## 5. Assignment Operator

```cpp
Event& operator=(const Event& other);
```

**Behavior**:
1. First stops the current event (if running)
2. Copies all properties of the source object
3. Resets the execution counter
4. Does not copy thread state

**Return Value**: Reference to self

---

## 6. Member Functions

### 6.1 Setter Functions

#### setID

```cpp
void setID(uint32_t id);
```
- **Function**: Set event ID
- **Parameter**: `id` - New identifier
- **Return Value**: None

#### setName

```cpp
void setName(const std::string& name);
```
- **Function**: Set event name
- **Parameter**: `name` - New name
- **Return Value**: None

#### setDelayMS

```cpp
void setDelayMS(uint32_t delay);
```
- **Function**: Set event execution interval (milliseconds)
- **Parameter**: `delay` - Delay time in milliseconds, default 1000ms
- **Return Value**: None
- **Notes**: Modifications take effect immediately while event is running

#### setRepeatCount

```cpp
void setRepeatCount(uint32_t count);
```
- **Function**: Set repeat execution count
- **Parameter**: `count` - Execution count, 0 means infinite loop
- **Return Value**: None
- **Default Value**: 1 (execute once)

#### setCondition

```cpp
void setCondition(const std::function<bool()>& condition);
```
- **Function**: Set execution condition
- **Parameter**: `condition` - Callback function returning bool
- **Return Value**: None
- **Default Condition**: `[] { return true; }`

#### setEvent

```cpp
void setEvent(const std::function<void(const std::atomic<bool>&)>& callback);
```
- **Function**: Set event callback function
- **Parameter**: `callback` - Event execution body, receives running state reference
- **Return Value**: None

### 6.2 Query Functions

#### eventID

```cpp
[[nodiscard]] uint32_t eventID() const;
```
- **Function**: Get event ID
- **Return Value**: Event identifier

#### eventName

```cpp
[[nodiscard]] const std::string& eventName() const;
```
- **Function**: Get event name
- **Return Value**: Constant reference to event name

#### eventDelayMS

```cpp
[[nodiscard]] uint32_t eventDelayMS() const;
```
- **Function**: Get execution delay
- **Return Value**: Delay in milliseconds

#### eventRepeatCount

```cpp
[[nodiscard]] uint32_t eventRepeatCount() const;
```
- **Function**: Get set repeat count
- **Return Value**: Repeat count (0 means infinite)

#### isRunning

```cpp
[[nodiscard]] bool isRunning() const;
```
- **Function**: Check if event is running
- **Return Value**: `true` means running

#### hasEvent

```cpp
[[nodiscard]] bool hasEvent() const;
```
- **Function**: Check if event callback is set
- **Return Value**: `true` means callback is set

### 6.3 Control Functions

#### run

```cpp
void run();
```
- **Function**: Start event execution
- **Return Value**: None
- **Constraints**:
  - Does nothing if event is already running
  - Does nothing if event callback is not set
  - Creates background thread on first call
  - Repeated calls wake waiting threads

**Execution Flow**:
1. Check if already running, return if true
2. Check if event callback exists, return if not
3. Create background thread
4. Loop execution in thread:
   - Check stop flag
   - Check execution condition
   - Execute event callback
   - Wait for delay time
   - Check repeat count

#### stop

```cpp
void stop();
```
- **Function**: Stop event execution
- **Return Value**: None
- **Notes**: Does not immediately terminate thread, only sets stop flag, event exits on next loop check

---

## 7. Usage Examples

### 7.1 Basic Timer

```cpp
#include "Events/Events.hpp"
#include <iostream>

int main() {
    // Create timer event (execute once per second, 5 times total)
    Tiny::Event timer(1, "Timer",
        [] { return true; },  // Condition: always execute
        [](const std::atomic<bool>& running) {
            if (running) {
                std::cout << "Timer tick!" << std::endl;
            }
        }
    );
    
    timer.setDelayMS(1000);  // 1 second interval
    timer.setRepeatCount(5); // Execute 5 times
    timer.run();
    
    // Wait for event completion
    std::this_thread::sleep_for(std::chrono::seconds(6));
    
    return 0;
}
```

### 7.2 Conditional Event

```cpp
#include "Events/Events.hpp"
#include <iostream>
#include <atomic>

std::atomic<bool> shouldRun{false};

int main() {
    Tiny::Event conditionalEvent(2, "Conditional",
        [] { return shouldRun.load(); },  // Condition: shouldRun is true
        [](const std::atomic<bool>& running) {
            std::cout << "Condition met, executing!" << std::endl;
        }
    );
    
    conditionalEvent.setDelayMS(500);
    conditionalEvent.setRepeatCount(0);  // Infinite loop
    conditionalEvent.run();
    
    // Enable condition after 3 seconds
    std::this_thread::sleep_for(std::chrono::seconds(3));
    shouldRun = true;
    
    // Run for another 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));
    conditionalEvent.stop();
    
    return 0;
}
```

### 7.3 Delayed Initialization Event

```cpp
#include "Events/Events.hpp"
#include <iostream>

int main() {
    // Create empty event first
    Tiny::Event delayedEvent(3, "Delayed");
    
    // Set callback later
    int counter = 0;
    delayedEvent.setCondition([&counter] { return counter < 10; });
    delayedEvent.setEvent([&counter](const std::atomic<bool>& running) {
        if (running) {
            std::cout << "Count: " << ++counter << std::endl;
        }
    });
    
    delayedEvent.setDelayMS(100);
    delayedEvent.run();
    
    // Wait for completion
    while (delayedEvent.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return 0;
}
```

### 7.4 Multiple Event Management

```cpp
#include "Events/Events.hpp"
#include <iostream>
#include <vector>

int main() {
    std::vector<Tiny::Event> events;
    
    // Create multiple timers
    for (int i = 0; i < 3; ++i) {
        events.emplace_back(i, "Timer" + std::to_string(i));
        events[i].setDelayMS(1000 * (i + 1));
        events[i].setRepeatCount(3);
        events[i].setEvent([i](const std::atomic<bool>&) {
            std::cout << "Event " << i << " triggered!" << std::endl;
        });
        events[i].run();
    }
    
    // Wait for all events to complete
    std::this_thread::sleep_for(std::chrono::seconds(10));
    
    return 0;
}
```

---

## 8. Notes

### 8.1 Thread Safety

- `Event` class member functions are not thread-safe
- Do not modify event properties in the event callback
- Use atomic variables or mutexes to protect shared data

### 8.2 Exception Handling

Exceptions in event callbacks are caught and output to stderr:

```cpp
Event event(1, "Test", [](const std::atomic<bool>&) {
    throw std::runtime_error("Error!");
});
// Output: "Tiny::Event: An error has occurred: Error!"
```

### 8.3 Stopping Events

- `stop()` only sets the stop flag, does not forcefully terminate the thread
- Event exits on next loop check
- If event is executing callback, waits for callback completion

### 8.4 Resource Management

- Destructor automatically calls `stop()` and waits for thread completion
- Avoid destroying event objects while event is running
- When using smart pointers to manage event objects, ensure sufficient lifetime

### 8.5 Delay Settings

- `setDelayMS()` can be modified while event is running
- Changes take effect on next wait
- Minimum delay is affected by system scheduling, should not be set too small (< 10ms)
