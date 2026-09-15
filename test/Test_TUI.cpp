
#include "../src/Tiny.hpp"
using namespace Tiny;
using namespace TUI;


int main(int argc, char *argv[]) {
    OS::exec("cmd.exe /c chcp 65001");
    Application app;
    std::vector<std::unique_ptr<Button>> btn;
    btn.reserve(10);

    for (int i = 0; i < 9; ++i) {
        btn.emplace_back(new Button("Btn " + std::to_string(i + 1),
            {static_cast<uint32_t>(5 * (i / 3) + 2), static_cast<uint32_t>(11 * (i % 3) + 2) },
            {9, 3}));
        btn.back()->setAlignment(static_cast<Alignment>(i));
        btn.back()->setEnabled(i % 2);
    }

    Label label("label", {}, {40, 1});
    Label mouse_label("LM", {1, 0});
    mouse_label.setText("TR");
    label.setText("Please click one of the buttons!");
    CurBlock cur_block("cursor");
    auto start = DT::currentTimestamps();
    auto clicked = [&label, &btn, &start] {
        label.setText(DT::formatTime("Clicke\\d at HH:mm:ss.SSS", DT::currentTimestamps() - start));
        for (auto& bt : btn) {
            bt->setEnabled(!bt->enabled());
        }
    };
    for (auto& bt : btn) {
        bt->setClickedEvent(clicked);
    }
    return app.run();
}


