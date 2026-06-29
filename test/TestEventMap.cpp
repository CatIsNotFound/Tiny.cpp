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

#include <gtest/gtest.h>
#include "../src/Events/Events.hpp"
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <algorithm>

using namespace Tiny;

TEST(EventMapTest, BasicConstruction) {
    EventsMap map;
    EXPECT_EQ(map.size(), 0);
    EXPECT_FALSE(map.exist(1));
}


TEST(EventMapTest, AddEventAndExist) {
    EventsMap map;
    Event ev(1, "TestEvent",
        []() { return true; },
        [](const std::atomic<bool>&) {}
    );

    EXPECT_TRUE(map.addEvent(ev));
    EXPECT_EQ(map.size(), 1);
    EXPECT_TRUE(map.exist(1));
    EXPECT_FALSE(map.exist(2));
}


TEST(EventMapTest, DuplicateAddEvent) {
    EventsMap map;
    Event ev(1, "TestEvent",
        []() { return true; },
        [](const std::atomic<bool>&) {}
    );

    EXPECT_TRUE(map.addEvent(ev));
    EXPECT_FALSE(map.addEvent(ev));
    EXPECT_EQ(map.size(), 1);
}

TEST(EventMapTest, ExecEventByObject) {
    EventsMap map;
    std::atomic<int> counter(0);

    Event ev(1, "ExecEvent",
        []() { return true; },
        [&counter](const std::atomic<bool>&) { counter++; }
    );
    ev.setRepeatCount(3);
    ev.setDelayMS(5);

    EXPECT_TRUE(map.execEvent(ev));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_EQ(counter.load(), 3);
    EXPECT_TRUE(map.exist(1));
    EXPECT_EQ(map.size(), 1);
}


TEST(EventMapTest, DuplicateExecEventByObject) {
    EventsMap map;
    Event ev(1, "ExecEvent",
        []() { return true; },
        [](const std::atomic<bool>&) {}
    );

    EXPECT_TRUE(map.execEvent(ev));
    EXPECT_FALSE(map.execEvent(ev));
}


TEST(EventMapTest, ExecEventByID) {
    EventsMap map;
    std::atomic<int> counter(0);

    Event ev(1, "ExecByID",
        []() { return true; },
        [&counter](const std::atomic<bool>&) { counter++; }
    );
    ev.setRepeatCount(2);
    ev.setDelayMS(5);

    EXPECT_TRUE(map.addEvent(ev));
    EXPECT_TRUE(map.execEvent(1));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_EQ(counter.load(), 2);
}


TEST(EventMapTest, ExecEventByIDNotFound) {
    EventsMap map;
    EXPECT_FALSE(map.execEvent(999));
}


TEST(EventMapTest, RemoveEvent) {
    EventsMap map;
    Event ev(1, "RemoveEvent",
        []() { return true; },
        [](const std::atomic<bool>&) {
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }
    );

    EXPECT_TRUE(map.addEvent(ev));
    EXPECT_EQ(map.size(), 1);
    map.execEvent(1);
    EXPECT_TRUE(map.removeEvent(1));
    EXPECT_EQ(map.size(), 0);
    EXPECT_FALSE(map.exist(1));
}


TEST(EventMapTest, RemoveEventNotFound) {
    EventsMap map;
    EXPECT_FALSE(map.removeEvent(999));
}


TEST(EventMapTest, RemoveAllFreeEvents) {
    EventsMap map;
    Event ev1(1, "RunningEvent",
        []() { return true; },
        [](const std::atomic<bool>& running) { std::this_thread::sleep_for(std::chrono::milliseconds(200)); }
    );
    Event ev2(2, "FreeEvent",
        []() { return true; },
        [](const std::atomic<bool>&) {}
    );

    map.addEvent(ev1);
    map.addEvent(ev2);
    map.execEvent(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    EXPECT_TRUE(map.event(1).isRunning());
    EXPECT_FALSE(map.event(2).isRunning());

    EXPECT_TRUE(map.removeAllFreeEvents());
    EXPECT_EQ(map.size(), 1);
    EXPECT_TRUE(map.exist(1));
    EXPECT_FALSE(map.exist(2));

    map.stopEvent(1);
    map.waitEvent(1);
}


TEST(EventMapTest, StopEvent) {
    EventsMap map;
    std::atomic<int> counter(0);

    Event ev(1, "Stoppable",
        []() { return true; },
        [&counter](const std::atomic<bool>&) {
            counter++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    );
    ev.setRepeatCount(100);
    ev.setDelayMS(10);

    map.addEvent(ev);
    map.execEvent(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    map.stopEvent(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_LT(counter.load(), 100);
}

TEST(EventMapTest, StopEventNotFound) {
    EventsMap map;
    map.stopEvent(999);
    SUCCEED();
}


TEST(EventMapTest, WaitEvent) {
    EventsMap map;
    int counter(0);

    Event ev(1, "Waitable",
        []() { return true; },
        [&counter](const std::atomic<bool>&) {
            counter++;
        }
    );
    ev.setRepeatCount(1);
    ev.setDelayMS(10);

    map.addEvent(ev);
    map.execEvent(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    map.waitEvent(1);

    EXPECT_EQ(counter, 1);
    EXPECT_FALSE(map.event(1).isRunning());
}

TEST(EventMapTest, WaitEventNotFound) {
    EventsMap map;
    map.waitEvent(999);
    SUCCEED();
}


TEST(EventMapTest, StopAllAndWaitAllEvents) {
    EventsMap map;
    std::atomic<int> counter1(0);
    std::atomic<int> counter2(0);

    Event ev1(1, "Event1",
        []() { return true; },
        [&counter1](const std::atomic<bool>&) {
            counter1++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    );
    ev1.setRepeatCount(100);
    ev1.setDelayMS(10);

    Event ev2(2, "Event2",
        []() { return true; },
        [&counter2](const std::atomic<bool>&) {
            counter2++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    );
    ev2.setRepeatCount(100);
    ev2.setDelayMS(10);

    map.addEvent(ev1);
    map.addEvent(ev2);
    map.execEvent(1);
    map.execEvent(2);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    map.stopAllEvents();
    map.waitAllEvents();

    EXPECT_LT(counter1.load(), 100);
    EXPECT_LT(counter2.load(), 100);
}


TEST(EventMapTest, SetConditionByID) {
    EventsMap map;
    std::atomic<int> counter(0);
    std::atomic<bool> shouldRun(false);

    Event ev(1, "Conditional",
        []() { return true; },
        [&counter](const std::atomic<bool>&) { counter++; }
    );
    ev.setRepeatCount(1);
    ev.setDelayMS(5);

    map.addEvent(ev);
    map.setConditionByID(1, [&shouldRun]() { return shouldRun.load(); });
    map.execEvent(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_EQ(counter.load(), 0);

    shouldRun.store(true);
    map.execEvent(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_EQ(counter.load(), 1);
}


TEST(EventMapTest, SetConditionByIDNotFound) {
    EventsMap map;
    EXPECT_FALSE(map.setConditionByID(999, []() { return true; }));
}


TEST(EventMapTest, SetEventByID) {
    EventsMap map;
    std::atomic<int> counter(0);

    Event ev(1, "SetCallback", []() { return true; }, [](const std::atomic<bool>&) {});

    map.addEvent(ev);
    map.setEventByID(1, [&counter](const std::atomic<bool>&) { counter++; });
    map.setRepeatByID(1, 1);
    map.setDelayByID(1, 5);
    map.execEvent(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_EQ(counter.load(), 1);
}


TEST(EventMapTest, SetEventByIDNotFound) {
    EventsMap map;
    EXPECT_FALSE(map.setEventByID(999, [](const std::atomic<bool>&) {}));
}


TEST(EventMapTest, SetDelayByID) {
    EventsMap map;
    Event ev(1, "DelayTest", []() { return true; }, [](const std::atomic<bool>&) {});

    map.addEvent(ev);
    EXPECT_TRUE(map.setDelayByID(1, 200));
    EXPECT_EQ(map.event(1).eventDelayMS(), 200);
}


TEST(EventMapTest, SetDelayByIDNotFound) {
    EventsMap map;
    EXPECT_FALSE(map.setDelayByID(999, 100));
}


TEST(EventMapTest, SetRepeatByID) {
    EventsMap map;
    Event ev(1, "RepeatTest", []() { return true; }, [](const std::atomic<bool>&) {});

    map.addEvent(ev);
    EXPECT_TRUE(map.setRepeatByID(1, 5));
    EXPECT_EQ(map.event(1).eventRepeatCount(), 5);
}


TEST(EventMapTest, SetRepeatByIDNotFound) {
    EventsMap map;
    EXPECT_FALSE(map.setRepeatByID(999, 5));
}


TEST(EventMapTest, GetEventByID) {
    EventsMap map;
    Event ev(1, "GetEvent", []() { return true; }, [](const std::atomic<bool>&) {});
    ev.setDelayMS(123);
    ev.setRepeatCount(4);

    map.addEvent(ev);
    const Event& ref = map.event(1);
    EXPECT_EQ(ref.eventID(), 1);
    EXPECT_EQ(ref.eventName(), "GetEvent");
    EXPECT_EQ(ref.eventDelayMS(), 123);
    EXPECT_EQ(ref.eventRepeatCount(), 4);
}


TEST(EventMapTest, GetEventByIDNotFound) {
    EventsMap map;
    EXPECT_THROW(map.event(999), std::out_of_range);
}


TEST(EventMapTest, GetEventByObject) {
    EventsMap map;
    Event ev(1, "FindEvent", []() { return true; }, [](const std::atomic<bool>&) {});
    ev.setDelayMS(50);
    ev.setRepeatCount(2);

    map.addEvent(ev);
    EXPECT_TRUE(map.event(ev));

    Event notMatch(1, "Different", []() { return true; }, [](const std::atomic<bool>&) {});
    notMatch.setDelayMS(50);
    notMatch.setRepeatCount(2);
    EXPECT_FALSE(map.event(notMatch));

    Event notExist(999, "NotExist", []() { return true; }, [](const std::atomic<bool>&) {});
    EXPECT_FALSE(map.event(notExist));
}


TEST(EventMapTest, EventIDList) {
    EventsMap map;
    map.addEvent(Event(3, "Event3", []() { return true; }, [](const std::atomic<bool>&) {}));
    map.addEvent(Event(1, "Event1", []() { return true; }, [](const std::atomic<bool>&) {}));
    map.addEvent(Event(2, "Event2", []() { return true; }, [](const std::atomic<bool>&) {}));

    std::vector<size_t> ids = map.eventIDList();
    EXPECT_EQ(ids.size(), 3);
    EXPECT_TRUE(std::find(ids.begin(), ids.end(), 1) != ids.end());
    EXPECT_TRUE(std::find(ids.begin(), ids.end(), 2) != ids.end());
    EXPECT_TRUE(std::find(ids.begin(), ids.end(), 3) != ids.end());
}

TEST(EventMapTest, Iterators) {
    EventsMap map;
    map.addEvent(Event(1, "IterEvent", []() { return true; }, [](const std::atomic<bool>&) {}));

    size_t count = 0;
    for (auto it = map.cbegin(); it != map.cend(); ++it) {
        count++;
        EXPECT_EQ(it->first, 1);
    }
    EXPECT_EQ(count, 1);
}

TEST(EventMapTest, DestructorWaitsEvents) {
    std::atomic<int> counter(0);
    {
        EventsMap map;
        Event ev(1, "DestructEvent",
            []() { return true; },
            [&counter](const std::atomic<bool>&) { counter++; }
        );
        ev.setRepeatCount(1);
        ev.setDelayMS(5);

        if (map.addEvent(ev)) {
            map.execEvent(1);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    EXPECT_EQ(counter.load(), 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
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
