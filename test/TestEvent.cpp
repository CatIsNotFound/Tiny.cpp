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

using namespace Tiny;

TEST(EventTest, BasicConstruction) {
    Event ev(1, "TestEvent");
    EXPECT_EQ(ev.eventID(), 1);
    EXPECT_EQ(ev.eventName(), "TestEvent");
    EXPECT_FALSE(ev.isRunning());
    EXPECT_FALSE(ev.hasEvent());
}

TEST(EventTest, FullConstruction) {
    auto condition = []() { return true; };
    auto callback = [](const std::atomic<bool>&) {};
    
    Event ev(2, "FullEvent", condition, callback);
    EXPECT_EQ(ev.eventID(), 2);
    EXPECT_EQ(ev.eventName(), "FullEvent");
    EXPECT_TRUE(ev.hasEvent());
}

TEST(EventTest, EventOnlyConstruction) {
    auto callback = [](const std::atomic<bool>&) {};

    Event ev(3, "EventOnly", callback);
    EXPECT_EQ(ev.eventID(), 3);
    EXPECT_EQ(ev.eventName(), "EventOnly");
    EXPECT_TRUE(ev.hasEvent());
    EXPECT_FALSE(ev.isRunning());
}

TEST(EventTest, PropertySetters) {
    Event ev(1, "Original");
    
    ev.setID(10);
    EXPECT_EQ(ev.eventID(), 10);
    
    ev.setName("Modified");
    EXPECT_EQ(ev.eventName(), "Modified");
    
    ev.setDelayMS(500);
    EXPECT_EQ(ev.eventDelayMS(), 500);
    
    ev.setRepeatCount(5);
    EXPECT_EQ(ev.eventRepeatCount(), 5);
}

TEST(EventTest, CallbackSetters) {
    Event ev(1, "CallbackTest");
    
    EXPECT_FALSE(ev.hasEvent());
    
    ev.setEvent([](const std::atomic<bool>&) {});
    EXPECT_TRUE(ev.hasEvent());
    
    ev.setCondition([]() { return true; });
}

TEST(EventTest, CopyConstruction) {
    auto condition = []() { return true; };
    auto callback = [](const std::atomic<bool>&) {};
    
    Event ev1(1, "Original", condition, callback);
    ev1.setDelayMS(100);
    ev1.setRepeatCount(3);
    
    Event ev2(ev1);
    
    EXPECT_EQ(ev2.eventID(), 1);
    EXPECT_EQ(ev2.eventName(), "Original");
    EXPECT_EQ(ev2.eventDelayMS(), 100);
    EXPECT_EQ(ev2.eventRepeatCount(), 3);
    EXPECT_TRUE(ev2.hasEvent());
}

TEST(EventTest, CopyAssignment) {
    auto condition = []() { return true; };
    auto callback = [](const std::atomic<bool>&) {};
    
    Event ev1(1, "Source", condition, callback);
    ev1.setDelayMS(200);
    
    Event ev2(2, "Target");
    ev2 = ev1;
    
    EXPECT_EQ(ev2.eventID(), 1);
    EXPECT_EQ(ev2.eventName(), "Source");
    EXPECT_EQ(ev2.eventDelayMS(), 200);
    EXPECT_FALSE(ev2.isRunning());
}

TEST(EventTest, CopyWhenThreadRunning) {
    auto cond = []() { return true; };
    auto ev = [](const std::atomic<bool>& f) {
        while (f.load()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };

    Event e1(1, "event 1", cond, ev);
    e1.setDelayMS(10);
    e1.run();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Event e2(2, "event 2", cond, ev);
    e2.setDelayMS(1);
    e1 = e2;
    e1.run();
    
    EXPECT_EQ(e1.eventID(), e2.eventID());
    EXPECT_EQ(e1.eventName(), e2.eventName());
    EXPECT_EQ(e1.isRunning(), true);
    EXPECT_EQ(e1.eventDelayMS(), 1);
}


TEST(EventTest, BasicExecution) {
    std::atomic<int> counter{0};
    
    Event ev(1, "Counter", 
        []() { return true; },
        [&counter](const std::atomic<bool>&) { counter++; }
    );
    ev.setRepeatCount(3);
    ev.setDelayMS(10);
    
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(counter.load(), 3);
    EXPECT_FALSE(ev.isRunning());
}

TEST(EventTest, ConditionCheck) {
    std::atomic<int> counter{0};
    std::atomic<bool> shouldRun{false};
    
    Event ev(1, "Conditional",
        [&shouldRun]() { return shouldRun.load(); },
        [&counter](const std::atomic<bool>&) { counter.fetch_add(1); }
    );
    ev.setRepeatCount(1);
    ev.setDelayMS(10);
    
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    EXPECT_EQ(counter.load(), 0);
    
    shouldRun.store(true);
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    EXPECT_EQ(counter.load(), 1);
}

TEST(EventTest, StopExecution) {
    std::atomic<int> counter{0};
    
    Event ev(1, "Stoppable",
        []() { return true; },
        [&counter](const std::atomic<bool>& running) { 
            counter++;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    );
    ev.setRepeatCount(100);
    ev.setDelayMS(10);
    
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    
    ev.stop();
    
    EXPECT_FALSE(ev.isRunning());
    EXPECT_LT(counter.load(), 100);
}

TEST(EventTest, DelayExecution) {
    auto start = std::chrono::steady_clock::now();
    std::atomic<bool> executed{false};
    
    Event ev(1, "Delayed",
        []() { return true; },
        [&executed](const std::atomic<bool>&) { executed.store(true); }
    );
    ev.setRepeatCount(1);
    ev.setDelayMS(100);
    
    ev.run();
    
    EXPECT_FALSE(executed.load());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    EXPECT_TRUE(executed.load());
    
    auto elapsed = std::chrono::steady_clock::now() - start;
    EXPECT_GE(elapsed, std::chrono::milliseconds(100));
}

TEST(EventTest, RepeatCount) {
    std::atomic<int> counter{0};
    
    Event ev(1, "Limited",
        []() { return true; },
        [&counter](const std::atomic<bool>&) { counter++; }
    );
    ev.setRepeatCount(5);
    ev.setDelayMS(5);
    
    ev.run();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_EQ(counter.load(), 5);
    EXPECT_FALSE(ev.isRunning());
}

TEST(EventTest, RunningState) {
    Event ev(1, "StateTest",
        []() { return true; },
        [](const std::atomic<bool>&) { std::this_thread::sleep_for(std::chrono::milliseconds(1000)); }
    );
    ev.setRepeatCount(1);
    
    EXPECT_FALSE(ev.isRunning());
    
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(ev.isRunning());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    
    EXPECT_FALSE(ev.isRunning());
}

TEST(EventTest, RerunEvent) {
    std::atomic<int> counter{0};
    
    Event ev(1, "Rerunnable",
        []() { return true; },
        [&counter](const std::atomic<bool>&) { ++counter; }
    );
    ev.setRepeatCount(2);
    ev.setDelayMS(5);
    
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(counter.load(), 2);
    
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(counter.load(), 4);
}

TEST(EventTest, NoCallback) {
    Event ev(1, "NoCallback", []() { return true; }, nullptr);
    
    EXPECT_FALSE(ev.hasEvent());
    
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    
    EXPECT_FALSE(ev.isRunning());
}

TEST(EventTest, NoCondition) {
    std::atomic<bool> executed{false};
    
    Event ev(1, "NoCondition");
    ev.setEvent([&executed](const std::atomic<bool>&) { executed.store(true); });
    ev.setRepeatCount(1);
    ev.setDelayMS(5);
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
}

TEST(EventTest, ExecutionCount) {
    std::atomic<int> counter{0};

    Event ev(1, "ExecCount",
        []() { return true; },
        [&counter](const std::atomic<bool>&) { counter++; }
    );
    ev.setRepeatCount(3);
    ev.setDelayMS(5);

    EXPECT_EQ(ev.executionCount(), 0);

    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_EQ(ev.executionCount(), 3u);
    EXPECT_EQ(counter.load(), 3);
}

TEST(EventTest, AllowedFailedDefault) {
    Event ev(1, "Test");
    EXPECT_TRUE(ev.allowedFailedEnabled());
}

TEST(EventTest, SetAllowedFailedEnabled) {
    Event ev(1, "Test");

    ev.setAllowedFailedEnabled(false);
    EXPECT_FALSE(ev.allowedFailedEnabled());

    ev.setAllowedFailedEnabled(true);
    EXPECT_TRUE(ev.allowedFailedEnabled());
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