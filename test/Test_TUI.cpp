
#include "../src/TUI/TUI.hpp"
#include <iostream>
#include <thread>
using namespace Tiny::TUI;


int main() {
    Terminal::moveCursor(10, 10);
    Terminal::printLine("Hello world! 请按任意键继续！....");
    Terminal::moveCursor(10, 0);
    Terminal::printLine("ASDASDASDASDSADASDASDASDASDASD");
    system("pause");
    return 0;
}