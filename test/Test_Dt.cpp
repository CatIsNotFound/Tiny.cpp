#include "../src/Tiny.hpp"
using namespace Tiny;

int main() {
    while (true) {
        auto pos = TUI::Terminal::cursorPosition();
        TUI::Terminal::printFormat("Now: {}\r\n", DT::DateTime::formatString("yyyy/MM/dd HH:mm:ss", DT::DateTime::now()));
        TUI::Terminal::printFormat("UTC: {}\r\n", DT::DateTime::formatString("yyyy/MM/dd HH:mm:ss", DT::DateTime::now(false)));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        TUI::Terminal::moveCursor(pos);
    }
    return 0;
}