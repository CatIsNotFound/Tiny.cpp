#include "../src/TUI/Terminal.hpp"

using namespace Tiny;

using ter = TUI::Terminal;

int main() {
    ter::print("Input your name: ");
    auto name = ter::readLineW();
    ter::printFormat("Hello, {}!", Win::wide2String(name));
    ter::readLine();
    return 0;
}