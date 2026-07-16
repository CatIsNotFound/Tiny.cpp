

#include "MyUI.hpp"
using namespace Tiny::DT;

int main() {
    auto& renderer = TUI::Renderer::self();

    MyUI::Button btn("btn", {4, 2}, {20, 3});
    btn.setText("Hello World");
    btn.draw();
    auto start_ts = currentTimestamps();
    while (true) {
        auto end_ts = currentTimestamps();
        if (end_ts - start_ts > 1_s) {
            renderer.present();
            renderer.clear();
            btn.move(rand() % 40 + 8, rand() % 10 + 1);

            btn.draw();
            start_ts = end_ts;
        }
        auto input = TUI::Terminal::getInput();
        if (input.type == TUI::InputEvent::Keyboard) {
            if (input.input.keyboard.key == 'q') {
                break;
            }
        }
    }

    return 0;
}