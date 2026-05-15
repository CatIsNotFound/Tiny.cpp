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

// 测试 Event 基础构造和属性
TEST(EventTest, BasicConstruction) {
    Event ev(1, "TestEvent");
    EXPECT_EQ(ev.eventID(), 1);
    EXPECT_EQ(ev.eventName(), "TestEvent");
    EXPECT_FALSE(ev.isRunning());
    EXPECT_FALSE(ev.hasEvent());
}

// 测试 Event 完整构造
TEST(EventTest, FullConstruction) {
    auto condition = []() { return true; };
    auto callback = [](const std::atomic<bool>&) {};
    
    Event ev(2, "FullEvent", condition, callback);
    EXPECT_EQ(ev.eventID(), 2);
    EXPECT_EQ(ev.eventName(), "FullEvent");
    EXPECT_TRUE(ev.hasEvent());
}

// 测试 Event 属性设置
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

// 测试 Event 回调设置
TEST(EventTest, CallbackSetters) {
    Event ev(1, "CallbackTest");
    
    EXPECT_FALSE(ev.hasEvent());
    
    ev.setEvent([](const std::atomic<bool>&) {});
    EXPECT_TRUE(ev.hasEvent());
    
    ev.setCondition([]() { return true; });
}

// 测试 Event 拷贝构造
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

// 测试 Event 拷贝赋值
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

// 测试 Event 基本执行
TEST(EventTest, BasicExecution) {
    std::atomic<int> counter{0};
    
    Event ev(1, "Counter", 
        []() { return true; },
        [&counter](const std::atomic<bool>&) { counter++; }
    );
    ev.setRepeatCount(3);
    ev.setDelayMS(10);
    
    ev.run();
    
    // 等待执行完成
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_EQ(counter.load(), 3);
    EXPECT_FALSE(ev.isRunning());
}

// 测试 Event 条件判断
TEST(EventTest, ConditionCheck) {
    std::atomic<int> counter{0};
    std::atomic<bool> shouldRun{false};
    
    Event ev(1, "Conditional",
        [&shouldRun]() { return shouldRun.load(); },
        [&counter](const std::atomic<bool>&) { counter++; }
    );
    ev.setRepeatCount(1);
    ev.setDelayMS(10);
    
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // 条件为 false，不应该执行
    EXPECT_EQ(counter.load(), 0);
    
    shouldRun.store(true);
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // 条件为 true，应该执行
    EXPECT_EQ(counter.load(), 1);
}

// 测试 Event 停止功能
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
    // 应该只执行了少数几次就被停止
    EXPECT_LT(counter.load(), 100);
}

// 测试 Event 延迟设置
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
    
    // 立即检查，应该还没执行
    EXPECT_FALSE(executed.load());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    
    // 延迟后应该执行了
    EXPECT_TRUE(executed.load());
    
    auto elapsed = std::chrono::steady_clock::now() - start;
    EXPECT_GE(elapsed, std::chrono::milliseconds(100));
}

// 测试 Event 重复次数限制
TEST(EventTest, RepeatCount) {
    std::atomic<int> counter{0};
    
    Event ev(1, "Limited",
        []() { return true; },
        [&counter](const std::atomic<bool>&) { counter++; }
    );
    ev.setRepeatCount(5);
    ev.setDelayMS(5);
    
    ev.run();
    
    // 等待足够时间让所有重复执行完成
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_EQ(counter.load(), 5);
    EXPECT_FALSE(ev.isRunning());
}

// 测试 Event 运行状态
TEST(EventTest, RunningState) {
    Event ev(1, "StateTest",
        []() { return true; },
        [](const std::atomic<bool>&) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }
    );
    ev.setRepeatCount(1);
    
    EXPECT_FALSE(ev.isRunning());
    
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(ev.isRunning());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    EXPECT_FALSE(ev.isRunning());
}

// 测试 Event 重新运行
TEST(EventTest, RerunEvent) {
    std::atomic<int> counter{0};
    
    Event ev(1, "Rerunnable",
        []() { return true; },
        [&counter](const std::atomic<bool>&) { ++counter; }
    );
    ev.setRepeatCount(2);
    ev.setDelayMS(5);
    
    // 第一次运行
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(counter.load(), 2);
    
    // 第二次运行
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_EQ(counter.load(), 4);
}

// 测试 Event 无回调情况
TEST(EventTest, NoCallback) {
    Event ev(1, "NoCallback", []() { return true; }, nullptr);
    
    EXPECT_FALSE(ev.hasEvent());
    
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    
    // 没有回调，应该立即结束
    EXPECT_FALSE(ev.isRunning());
}

// 测试 Event 无条件情况
TEST(EventTest, NoCondition) {
    std::atomic<bool> executed{false};
    
    Event ev(1, "NoCondition");
    ev.setEvent([&executed](const std::atomic<bool>&) { executed.store(true); });
    ev.setRepeatCount(1);
    ev.setDelayMS(5);
    
    // 没有设置条件，应该使用默认条件（nullptr检查）
    ev.run();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
