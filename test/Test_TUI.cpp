
#include "../src/TUI/TUI.hpp"
#include <iostream>
#include "../src/Events/Events.hpp"
using namespace Tiny::TUI;


int main() {
    int* a = new int(3);
    Tiny::Event ev(1, "Hello World", []{ return true; }, [&a](const std::atomic<bool>& b){
        Terminal::printLine("Hello world!");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Terminal::print("3...");
        if (!b.load()) return;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Terminal::print("2...");
        if (!b.load()) return;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (!b.load()) return;
        Terminal::print("1...");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (!b.load()) return;
        Terminal::print("Finish!\n");
    });
    ev.run();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    ev.stop();
    if (a) {
        delete a;
        a = nullptr;
        Terminal::clearScreen();
        system("pause");
    }
    return 0;
}