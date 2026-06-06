
#include "../src/Tiny.hpp"
#include <iostream>
using namespace Tiny;
using SS = TUI::Renderer::Style;
using ter = TUI::Terminal;

auto& renderer = TUI::Renderer::self();

SS style1, style2, win_shadow, btn_shadow, btn_fill;

void refresh(int) {
    renderer.present();
}

void drawBox() {
    renderer.fillScreen(style2);
    renderer.fillRect({0, 0}, {20, 60}, ' ', style2);
    renderer.fillRect({2, 2}, {16, 41}, ' ', win_shadow);
    renderer.drawBorder({1, 1}, {15, 40}, {}, style1);
    renderer.setSSF({1, 14}, " 无奖竞猜 ", style1);
    renderer.fillRect({2, 2}, {14, 39}, ' ', style1);
    renderer.setSSF({3, 3}, "你觉得顽石老师不错吗？", style1);
    // renderer.fillRect({12, 8}, {14, 17}, ' ', btn_shadow);
    renderer.drawBorder({11, 7}, {13, 16}, {}, style2);
    renderer.fillRect({12, 8}, {12, 15}, ' ', style2);
    renderer.setSSF({12, 10}, "不错", style2);
    renderer.drawBorder({11, 22}, {13, 31}, {}, style2);
    renderer.fillRect({12, 23}, {12, 30}, ' ', btn_fill);
    renderer.setSSF({12, 23}, "非常不错", btn_fill);
}

int main() {
    auto scr_size = TUI::Terminal::screenSize();

    style1.bg_color = TUI::Color::White;
    style1.fg_color = TUI::Color::Black;
    style1.intensity = 1;
    style2.bg_color = TUI::Color::Red;
    style2.fg_color = TUI::Color::White;
    style2.intensity = 2;

    win_shadow.bg_color = TUI::Color::White;
    btn_shadow.bg_color = TUI::Color::Black;
    btn_fill.bg_color = TUI::Color::Red;
    btn_fill.fg_color = TUI::Color::Yellow;
    btn_fill.intensity = 2;

    drawBox();
    renderer.present();
    while (TUI::Terminal::getKey() != TUI::KEY_CTRL_D) {
        scr_size = TUI::Terminal::screenSize();
        drawBox();
        renderer.setSSF({0, 0}, "Size: {:>3}x{:>3}", style2, scr_size.width, scr_size.height);
        renderer.present();
    }
    return 0;
}