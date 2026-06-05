
#include "../src/Tiny.hpp"
#include <iostream>
using namespace Tiny;
using SS = TUI::Renderer::Style;
using ter = TUI::Terminal;

auto renderer = TUI::Renderer::self();


int main() {
    TUI::Char ch = "a";
    TUI::Renderer::Corner corner;
    renderer.drawBorder({2, 0}, {4, 10}, {});
    renderer.drawBorder({4, 0}, {10, 10}, {});
    renderer.fillRect({6, 2}, {9, 8}, '*');
    renderer.setStrF({9, 14}, "Nobody nobody but you");
    SS style;
    style.bg_color = TUI::Color::Blue;
    style.fg_color = TUI::Color::Yellow;
    style.property = SS::Bolder | SS::Italic;
    style.intensity = 3;
    // renderer.setSSF(TUI::Position{1, 0}, "~Beautiful~", style);
    renderer.set(2, 3, 'A', style);
    renderer.present();
    TUI::Terminal::getKey();
    renderer.set(1, 11, 'E', style);
    renderer.present();
    TUI::Terminal::getKey();
    return 0;
}