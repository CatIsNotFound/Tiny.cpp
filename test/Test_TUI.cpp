
#include "../src/Tiny.hpp"
using namespace Tiny;
using namespace TUI;

class MSlider : public Slider {
public:
    MSlider(const std::string& name, const Position& position, uint8_t width, Object* parent = nullptr) :
        Slider(name, position, width, parent) {}
    void setLabel(Label* label) {
        _label = label;
    }
protected:
    void valueChangedEvent() override {
        Slider::valueChangedEvent();
        if (!_label) return;
        _label->setText(Terminal::formatString("{}: {:3s}", objectName(), value()));
    }

private:
    Label* _label{};
};

int main(int argc, char *argv[]) {
    OS::exec("cmd.exe /c chcp 65001");
    Position p1(8, 1), p2(16, 1);
    Size sz(1, 8);
    bool ok = isPointInRect({12, 1}, p1, sz);

    Application app;
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
    app.setZOrder(&line_edit, &button);
    button.setEvent([&line_edit](Button& button) {
        line_edit.clear();
    });
    echo.setEvent([&line_edit](Button& button) {
        static uint8_t c = 1;
        c = (c + 1) % 3;
        line_edit.setEchoMode(static_cast<LineEdit::EchoMode>(c));
        switch (c) {
            case 0:
                button.setText("[  Echo: None  ]");
                break;
            case 1:
                button.setText("[ Echo: Normal ]");
                break;
            case 2:
                button.setText("[Echo: Password]");
                break;
        }
    });
    alignment.setEvent([&line_edit](Button& button) {
        static uint8_t c = 0;
        c = (c + 1) % 3;
        line_edit.setTextAlignment(static_cast<TextAlignment>(c));
        switch (c) {
        case 0:
            button.setText("[  Alignment: Left   ]");
            break;
        case 1:
            button.setText("[  Alignment: Center ]");
            break;
        case 2:
            button.setText("[  Alignment: Right  ]");
            break;
        }
    });
    MSlider slider_v("slider_v", {8, 16}, 10);
    MSlider slider_h("slider_h", {8, 1}, 10);
    slider_v.setOrientation(Orientation::V);
    slider_v.setValue(50);
    slider_h.setValue(50);
    slider_h.setOrientation(Orientation::V);
    slider_h.setOrientation(Orientation::H);
    slider_v.setMouseTracingEnabled(true);
    slider_h.setMouseTracingEnabled(true);
    slider_v.setInvertedEnabled(true);
    Label label_h("label_h: 100", {9, 1});
    Label label_v("label_v: 100", {10, 1});
    slider_h.setLabel(&label_h);
    slider_v.setLabel(&label_v);
    ProgressBar prg_bar("prg_bar", {19, 1}, 10);
    prg_bar.setValue(0);
    prg_bar.setInvertedEnabled(true);

    Label label_p("progress:   0%", {20, 1});
    EV::Event ev(18, "prg", [&prg_bar, &label_p](const std::atomic<bool>&) {
        static uint8_t D = 1;
        prg_bar.setValue(prg_bar.value() + 1 * D);
        if (prg_bar.value() == 0) D = 1;
        if (prg_bar.value() == 100) D = -1;
        label_p.setText(Terminal::formatString("progress: {:>3.2f}%  ", prg_bar.value()));
    });
    ev.setCondition([]{ return true; });
    ev.setRepeatCount(0);
    ev.setDelayMS(500);
    CurBlock cur("cur");
    ev.run();
    return app.run();
}


