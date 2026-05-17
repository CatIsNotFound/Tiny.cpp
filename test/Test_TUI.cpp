
#include "../src/TUI/TUI.hpp"
#include <iostream>
#include "../src/Events/Events.hpp"

using namespace Tiny;

int main() {
    Event ev(86, "TryException");
    ev.setCondition([]{ return true; });
    ev.setDelayMS(100);
    ev.setEvent([] (const std::atomic<bool>&) { _sleep(1000); printf("Finished!\n"); });
    ev.run();
    Event r_ev = Event(87, "TrySleep", []{ return true; }, [](const std::atomic<bool>&) { _sleep(1000); printf("Sleep 1s!\n"); });
    r_ev.run();
    r_ev.setRepeatCount(0);
    r_ev.setDelayMS(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    return 0;
}