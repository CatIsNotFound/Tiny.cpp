
#include "../src/Tiny.hpp"
#include <iostream>
using namespace Tiny;
using ter = TUI::Terminal;

int main() {
    auto ok = TUI::Terminal::enterRawMode();
    auto scr_size = TUI::Terminal::screenSize();
    ter::setBackgroundColor(TUI::Color::Blue);
    ter::setForegroundColor(TUI::Color::Red, false);
    ter::printLine("Screen size: " + std::to_string(scr_size.width) + "x" + std::to_string(scr_size.height));
    ter::setBackgroundColor(0, 128, 48);
    ter::setForegroundColor(124, 255, 64);
    ter::print("The screen will be restored in 3s...\r");
    ter::print("Local Data file: ");
    ter::printLine(OS::FileSystem::localDataPath().path());
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    ter::reset();
    bool ok2 = ter::leaveRawMode();
    if (ok) {
        ter::printLine("OK 1!\t123123\r\n");
    }
    if (ok2) {
        ter::printLine("OK 2!\t123\r\n");
    }
    return 0;
}