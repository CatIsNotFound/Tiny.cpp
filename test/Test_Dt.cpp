#include "../src/Tiny.hpp"

using namespace Tiny;

int main() {
    while (true) {
        DT::DateTime now_1 = DT::DateTime::now(false);
        DT::DateTime now_2 = DT::DateTime::now();
        DT::DateTime parsed(now_2.timestamps(), true);
        DT::DateTime parsed_2(now_2.timestamps(), false);
        TUI::Terminal::printFormat(" T: {}, \r\nt1: {}, \r\nt2: {}\r\n", now_2.timestamps(), parsed.timestamps(), parsed_2.timestamps());
        TUI::Terminal::printFormat("b1: {:s} b2: {:s}\r\n", now_2 == parsed, now_2 == parsed_2);
        TUI::Terminal::printLine(DT::DateTime::formatString("yyyy/MM/dd HH:mm:ss.SSS cc", parsed));
        TUI::Terminal::printLine(DT::DateTime::formatString("yyyy/MM/dd HH:mm:ss.SSS cc", parsed_2));
        TUI::Terminal::printLine(DT::DateTime::formatString("y/M/d H:m:s c", now_1));
        TUI::Terminal::printLine(DT::DateTime::formatString("y/M/d H:m:s c\n", now_2));

        auto input = TUI::Terminal::getInput();
        if (input.type == TUI::InputEvent::Keyboard && input.input.keyboard.key == TUI::KEY_CTRL_C) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}