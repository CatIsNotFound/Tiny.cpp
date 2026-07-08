
#include "../src/Tiny.hpp"

using namespace Tiny;

int main() {
    int a{0}, b{0};
    auto event_1 = [&a] (const std::atomic<bool>&) {
        ++a;
    };
    auto event_2 = [&b] (const std::atomic<bool>&) {
        ++b;
    };
    TUI::Terminal::printLine("Waiting for event...");

    Event ev(1, "Limited", []{ return true; }, event_1);
    ev.setDelayMS(100);
    ev.setRepeatCount(0);
    ev.run();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ev.setEvent(event_2);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    TUI::Terminal::printFormat("a = {}, b = {}\r\n", a, b);
    return 0;
}
