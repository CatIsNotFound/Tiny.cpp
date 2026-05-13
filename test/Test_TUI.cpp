
#include "../src/TUI/TUI.hpp"
#include <iostream>
#include "../src/Events/Events.hpp"
using namespace Tiny::TUI;


int main() {
    int* a = new int(3);
    Tiny::Event ev(1, "Hello World", []{ return true; }, [&a] {
        Terminal::printLine("Hello world!");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Terminal::print("3...");
        if (a) *a = 2;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Terminal::print("2...");
        if (a) *a = 1;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (a) *a = 0;
        Terminal::print("1...");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (a) *a = 3;
        Terminal::print("Finish!\n");
    });
    ev.run();
    if (a) {
        delete a;
        a = nullptr;
        Terminal::clearScreen();
        system("pause");
    }
    return 0;
}