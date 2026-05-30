
#include "../src/Tiny.hpp"
#include <iostream>
using namespace Tiny;
using ter = TUI::Terminal;

auto renderer = TUI::Renderer::self();

int main() {
    TUI::Char ch = "a";
    TUI::Renderer::Corner corner;
    renderer.drawBorder({2, 0}, {4, 10}, {});
    renderer.drawBorder({4, 0}, {10, 10}, {});
    renderer.fillRect({6, 2}, {9, 8}, '*');
    renderer.set(1, 1, "哈");
    renderer.set(2, 1, "哈");
    renderer.set(3, 1, "哈");
    renderer.set(4, 1, "~");
    renderer.set(5, 1, "~");
    renderer.present();
    TUI::Terminal::getKey();
    renderer.set(1, 11, 'E');
    renderer.present();
    TUI::Terminal::getKey();
    return 0;
}