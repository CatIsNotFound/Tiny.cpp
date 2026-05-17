
#include "../src/Tiny.hpp"
#include <iostream>
using namespace Tiny;
using ter = TUI::Terminal;

int main() {
    auto ok = TUI::Terminal::enterRawMode();
    TUI::Terminal::setCursorVisible(false);
    auto scr_size = TUI::Terminal::screenSize();
    ter::setBackgroundColor(0, 32, 192);
    ter::setForegroundColor(192, 32, 64);
    ter::printLine("Screen size: " + std::to_string(scr_size.width) + "x" + std::to_string(scr_size.height));
    ter::setBackgroundColor(TUI::Color::Blue, false);
    ter::setForegroundColor(TUI::Color::White, false);
    ter::printLine("The screen will be restored in 3s...");
    ter::print("Local Data file: ");
    auto pos = ter::cursorPosition();
    ter::printFormat("before pos: (%d, %d)\n", pos.row, pos.column);
    ter::clearInRow(1);
    pos = ter::cursorPosition();
    ter::print("later pos: ");
    ter::printLine(pos.row + "," + pos.column);
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
    TUI::Terminal::setCursorVisible(true);
    return 0;
}