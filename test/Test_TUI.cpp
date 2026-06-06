
#include "../src/Tiny.hpp"
#include <iostream>
using namespace Tiny;
using SS = TUI::Renderer::Style;
using ter = TUI::Terminal;

auto renderer = TUI::Renderer::self();

void refresh(int) {
    renderer.present();
}

int main() {

    SS style1, style2;
    style1.bg_color = TUI::Color::White;
    style1.fg_color = TUI::Color::Black;
    style1.intensity = 1;
    style2.bg_color = TUI::Color::Blue;
    style2.fg_color = TUI::Color::Yellow;
    style2.intensity = 2;

    renderer.fillRows(1, 5, 'R', style1);
    renderer.fillCols(3, 6, 'C', style2);
    renderer.setSSF({1, 30}, "请按任意键以继续...", style1);

    renderer.present();


    while (TUI::Terminal::getKey() != TUI::KEY_CTRL_B) {}
    return 0;
}