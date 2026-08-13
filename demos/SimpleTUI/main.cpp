
#include <utility>

#include "../src/Tiny.hpp"

using namespace Tiny;
using namespace TUI;
using namespace DT;
using namespace OS;

std::vector<std::string> main_menu = {
    "_File", "_Edit", "_Help"
};

std::vector<std::string> sub_file_menu = {
      "_New", "_Open", "_Save", "Save _as", "/", "E_xit"
};

std::vector<std::string> sub_edit_menu = {
    "_Cut", "Cop_y", "_Paste", "_Delete", "/", "Select _all", "Select _none"
};

std::vector<std::string> sub_help_menu = {
    "_Display help", "_About this"
};

Position calcEndOfPosition(const Position& position, const Size& size) {
    Position new_pos = { position.row + size.height - 1, position.column + size.width - 1 };
    return new_pos;
}

// class MInputEvent : public EV::AbstractEvent {
// public:
//     MInputEvent(InputEvent&& input_event)
//         : AbstractEvent(typeid(MInputEvent)), _input_event(std::move(input_event)) {}
//     const InputEvent& event() const { return _input_event; }
//     void setEvent(const InputEvent& input_event) { _input_event = input_event; }
// private:
//     InputEvent _input_event{};
// };
//
// class Widget : public AbstractWidget, public AbstractEventHandler {
// public:
//     Widget(const std::string& name, const Position& position, const Size& size, std::type_index index)
//         : AbstractWidget(name, position, size), AbstractEventHandler(index) {}
//     virtual ~Widget() = default;
// protected:
//     virtual void renderEvent() = 0;
//     virtual void resizeEvent(uint32_t width, uint32_t height) = 0;
//     virtual void moveEvent(uint32_t x, uint32_t y) = 0;
//     virtual void mouseEvent(MouseEvent mouse) = 0;
//     virtual void keyEvent(KeyEvent keyboard) = 0;
//
//     void onEvent(const AbstractEvent &event) override {
//         if (event.hashCode() == typeid(MInputEvent).hash_code()) {
//             auto minput = dynamic_cast<const MInputEvent&>(event);
//
//             auto& input_event = minput.event();
//             if (input_event.type == InputEvent::Keyboard) {
//                 keyEvent(input_event.input.keyboard);
//             } else if (input_event.type == InputEvent::Mouse) {
//                 mouseEvent(input_event.input.mouse);
//             }
//         }
//     }
// };
//
// class Button : public Widget {
// public:
//     Button(const std::string& text, const Position& position, uint32_t width, std::type_index index)
//             : Widget(text, position, {width, 3}, index), _m_name(text) {
//         parseText();
//         _style.resize(4);
//         _style[1].property |= Renderer::Style::Bolder | Renderer::Style::Underline;
//         _style[2].property |= Renderer::Style::Reverse;
//         _style[3].property |= Renderer::Style::Bolder | Renderer::Style::Underline | Renderer::Style::Reverse;
//     }
//
//     Button(const std::string& text, const Position& position, uint32_t width)
//             : Widget(text, position, {width, 3}, typeid(Button)), _m_name(text) {
//         parseText();
//         _style.resize(4);
//         _style[1].property |= Renderer::Style::Bolder | Renderer::Style::Underline;
//         _style[2].property |= Renderer::Style::Reverse;
//         _style[3].property |= Renderer::Style::Bolder | Renderer::Style::Underline | Renderer::Style::Reverse;
//     }
//
//     void setCorner(Renderer::Corner&& corner) {
//         _corner = std::move(corner);
//     }
//
//     void setCorner(const Renderer::Corner& corner) {
//         _corner = corner;
//     }
//
//     void setText(const std::string& text) {
//         _m_name = text;
//         parseText();
//     }
//
// protected:
//     void renderEvent() override {
//         if (!_is_changed) return;
//         auto end_pos = calcEndOfPosition(position(), size());
//         renderer().drawBorder(position(), end_pos, _corner, _style[0]);
//         auto dis_text_pos = position();
//         dis_text_pos.column += 1;
//         dis_text_pos.row += size().height / 2;
//         auto dis_start_of_pos = dis_text_pos;
//         auto dis_end_of_pos = dis_text_pos;
//         dis_end_of_pos.row = end_pos.row - 1;
//         dis_end_of_pos.column = end_pos.column - 1;
//         auto dis_text_length = static_cast<int64_t>(size().width) - 5;
//         if (dis_text_length < 0) {
//             for (size_t i = 0; i < 3; ++i) {
//                 renderer().set(dis_text_pos, '.', _active ? _style[2] : _style[0]);
//                 dis_text_pos.column += 1;
//             }
//         } else {
//             int64_t dis = (size().width - 2) / 2 - _dis_text_length / 2;
//             dis_text_pos.column += dis > 0 ? dis : 0;
//             size_t c = 0;
//             for (size_t i = 0; i < dis_text_length; ) {
//                 renderer().set(dis_text_pos, _utf8_chars[c], i == _bolder_pos ? (_active ? _style[3] : _style[1]) :
//                                                                                 (_active ? _style[2] : _style[0]));
//                 dis_text_pos.column += renderer().charAt(dis_text_pos).length();
//                 i += _utf8_chars[c++].size();
//                 if (i >= _dis_text_length) break;
//             }
//         }
//         _is_changed = false;
//     }
//     void resizeEvent(uint32_t width, uint32_t height) override {}
//     void moveEvent(uint32_t x, uint32_t y) override {}
//     void keyEvent(KeyEvent keyboard) override {
//         if (keyboard.key != _key_event.key || keyboard.sp_key != _key_event.sp_key) {
//             _key_event = keyboard;
//         }
//         if (keyboard.is_pressed) {
//             _active = !_active;
//             _is_changed = true;
//         }
//     }
//
//     void mouseEvent(MouseEvent mouse) override {}
// private:
//     void parseText() {
//         auto sz = _m_name.find_first_of('_');
//         if (sz == std::string::npos) {
//             _bolder_pos = std::string::npos;
//             _utf8_chars = splitUTF8(_m_name.c_str(), &_dis_text_length);
//             return;
//         }
//         _bolder_pos = sz;
//         _m_name.erase(_m_name.begin() + sz);
//         _utf8_chars = splitUTF8(_m_name.c_str(), &_dis_text_length);
//     }
//     std::vector<std::string> _utf8_chars{};
//     std::string _m_name;
//     size_t _dis_text_length{};
//     size_t _bolder_pos{};
//     Renderer::Corner _corner{};
//     std::vector<Renderer::Style> _style{};
//     KeyEvent _key_event{};
//     bool _active{};
//     MouseEvent _mouse_event{};
//     bool _is_changed{true};
// };
//
// void drawUI(Renderer& renderer) {
//     auto scr = Terminal::screenSize();
//     renderer.drawBorder({0, 0}, {2, scr.width - 1}, {});
//     renderer.drawBorder({2, 0}, {32, scr.width - 1}, {});
//
// }
//
// int main(int argc, char *argv[]) {
//     auto& renderer = Renderer::self();
//     Terminal::setMouseEnabled(true);
//     Button btn("你好世界", {2, 2}, 40, typeid(MInputEvent));
//     // Renderer::Corner m_corner;
//     // m_corner.left = "│";
//     // m_corner.right = "│";
//     // m_corner.top = "─";
//     // m_corner.bottom = "─";
//     // m_corner.left_bottom = "└";
//     // m_corner.left_top = "┌";
//     // m_corner.right_top = "┐";
//     // m_corner.right_bottom = "┘";
//     // btn.setCorner(m_corner);
//
//     MInputEvent input_event({});
//     EventListener listener;
//     listener.add(&btn);
//     auto bus = EV::EventBus::global();
//     bus->install(&listener);
//     while (true) {
//         auto in = Terminal::getInput();
//         if (in.type == InputEvent::Keyboard && in.input.keyboard.key == KEY_ESC) break;
//         input_event.setEvent(in);
//         bus->emit(&input_event);
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//         renderer.present();
//     }
//     return 0;
// }

int main(int argc, char *argv[]) {
    TUI::Application app(argc, argv);
    Renderer::Style style;
    style.bg_color = Color::Blue;
    auto& ev_bus = EventBus::self();
    /// Get Screen size
    ev_bus.subscribe<Renderer>([&style](const AbstractEvent& ev) {
        if (ev.hash() != typeid(ResizeTermEvent).hash_code()) return;
        auto& re = dynamic_cast<const ResizeTermEvent &>(ev);
        auto& new_size = re.newSize();
        Renderer::self().setStrF({1, 1}, "{}x{}", new_size.width, new_size.height);
    });
    ev_bus.subscribe<Application>([] (const AbstractEvent& ev) {
        auto& e = dynamic_cast<const UserInputEvent&>(ev);
        auto input = e.inputEvent();
        if (input.type == InputEvent::Keyboard) {
            Renderer::self().setStrF({2, 1}, "Keyboard Toggled!");
        } else {
            Renderer::self().setStrF({2, 1}, "Mouse Toggled!");
        }
    });
    // Renderer::self().fillScreen(style);
    return app.run();
}


