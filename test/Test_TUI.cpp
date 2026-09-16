
#include "../src/Tiny.hpp"
using namespace Tiny;
using namespace TUI;


int main(int argc, char *argv[]) {
    OS::exec("cmd.exe /c chcp 65001");
    Application app;
    CurBlock cur_block("cursor");
    Label label("label", {}, {40, 1});
    label.setText("Line Edit Test");
    Button button("ok", {1, 41});
    button.setText("[ OK ]");
    Button echo("echo", {2, 0});
    echo.setText("[ Echo: Normal ]");
    Button alignment("align", {3, 0});
    alignment.setText("[  Alignment: Left  ]");
    LineEdit line_edit("line_edit", {1, 0}, 40);
    line_edit.setPlaceHolderText("Click here to edit...");
    button.setClickedEvent([&line_edit] {
        line_edit.clear();
    });
    echo.setClickedEvent([&line_edit, &echo] {
        static uint8_t c = 1;
        c = (c + 1) % 3;
        line_edit.setEchoMode(static_cast<LineEdit::EchoMode>(c));
        switch (c) {
            case 0:
                echo.setText("[  Echo: None  ]");
                break;
            case 1:
                echo.setText("[ Echo: Normal ]");
                break;
            case 2:
                echo.setText("[Echo: Password]");
                break;
        }
    });
    alignment.setClickedEvent([&line_edit, &alignment] {
        static uint8_t c = 0;
        c = (c + 1) % 3;
        line_edit.setTextAlignment(static_cast<TextAlignment>(c));
        switch (c) {
        case 0:
            alignment.setText("[  Alignment: Left   ]");
            break;
        case 1:
            alignment.setText("[  Alignment: Center ]");
            break;
        case 2:
            alignment.setText("[  Alignment: Right  ]");
            break;
        }
    });
    return app.run();
}


