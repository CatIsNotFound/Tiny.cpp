
#include "../src/Tiny.hpp"

using namespace Tiny;
using namespace EV;

struct N {
    int a;
    bool b;
};

class NEvent : public AbstractEvent {
public:
    NEvent(N n) : AbstractEvent(typeid(N)), _n(n) {}
    const N& getN() const { return _n; }
private:
    N _n;
};

class TestEvent1 : public AbstractEventHandler {
public:
    TestEvent1() : AbstractEventHandler(typeid(N)) {}
    ~TestEvent1() override = default;

protected:
    void onEvent(const AbstractEvent& event) override {
        auto data = dynamic_cast<const NEvent&>(event).getN();
        TUI::Terminal::print() << "TestEvent1: " << data.a << " " << data.b << "\r\n";
    }
};



int main() {
    N n1 = {12, true};
    N n2 = {24, false};
    N n3 = {1, true};
    NEvent t1(n1), t2(n2), t3(n3);
    TestEvent1 tev1, tev2;
    EventListener listener;
    listener.add(&tev1);
    listener.add(&tev2);
    listener.remove(&tev1);
    auto bus = EventBus::global();
    auto inst = bus->install(&listener);
    bus->emit(&t3);
    bus->emit(&t2);
    bus->emit(&t1);
    bus->uninstall(inst);
    TUI::Terminal::print() << "listener: " << listener.size() << "\r\n";

    return 0;
}

