
#include "../src/TUI/TUI.hpp"
#include <iostream>
#include "../src/Events/Events.hpp"

using namespace Tiny;

int main() {
    Tiny::TUI::Terminal terminal;
    auto scr_size = terminal.screenSize();
    terminal.printLine("Screen size: " + std::to_string(scr_size.width) + "x" + std::to_string(scr_size.height));
    return 0;
}