

#ifndef TINY_DEMOS_DIALOG_MYUI_HPP
#define TINY_DEMOS_DIALOG_MYUI_HPP

#include "../src/Tiny.hpp"
#include <bitset>
#include <algorithm>

using namespace Tiny;

namespace MyUI {
    static std::string mergeString(const std::vector<std::string>& str_list, size_t length) {
        std::stringstream ss;
        for (size_t i = 0; i < length; ++i) {
            if (i >= length || i >= str_list.size()) break;
            ss << str_list[i];
        }
        return ss.str();
    }

    class Button : public TUI::AbstractWidget {
    public:
        Button(const std::string& name, const TUI::Position& position, const TUI::Size& size)
                : AbstractWidget(name, position, size) {
            _style_sheet.resize(4);
        }

        ~Button() override = default;

        void setCorner(const TUI::Renderer::Corner& corner) {
            _corner = corner;
            _status.set(3);
        }

        void setText(const char* string) {
            _text = string;
            _status.set(3);
        }

        void setText(const std::string& string) {
            _text = string;
            _status.set(3);
        }

        void setStyle(uint8_t id, const TUI::Renderer::Style& style) {
            if (id >= _style_sheet.size()) return;
            _style_sheet[id] = style;
            _status.set(3);
        }

        const TUI::Renderer::Corner& corner() const {
            return _corner;
        }

        const TUI::Renderer::Style& style(uint8_t id) const {
            return _style_sheet[id];
        }

        uint8_t styleCount() const {
            return _style_sheet.size();
        }

        const std::string& text() const {
            return _text;
        }
    protected:
        void renderEvent() override {
            if (!_status.test(3)) return;
            auto& renderer = TUI::Renderer::self();
            TUI::Position end_pos = {
                position().row + size().height - 1,
                position().column + size().width - 1,
            };
            TUI::Renderer::Style& active_style = _style_sheet[_status.test(0)];
            renderer.drawBorder(position(), end_pos, _corner, active_style);
            TUI::Position text_pos = {};
            calcText(text_pos);
            renderer.setSSF({text_pos.row, position().column + 1}, "{:{}c}", active_style, size().width - 2, ' ');
            renderer.setSSF(text_pos, _dis_text.c_str(), active_style);
        }

        void resizeEvent(uint32_t width, uint32_t height) override {
            if (width < 9 || height < 3) {
                resize(9, 3);
                return;
            }
            _status.set(3);
        }

        void moveEvent(uint32_t x, uint32_t y) override {
            _status.set(3);
        }

    private:
        void calcText(TUI::Position& new_pos) {
            auto text_utf8 = TUI::splitUTF8(_text.c_str());
            auto text_len = text_utf8.size();
            auto& size = AbstractWidget::size();
            auto& pos = AbstractWidget::position();
            new_pos.column = pos.column + (size.width / 2 - text_len / 2);
            new_pos.row = pos.row + size.height / 2;
            if (size.width < text_len) {
                auto min = std::min<size_t>(size.width, text_len);
                _dis_text = mergeString(text_utf8, min);
                return;
            }
            _dis_text = _text;
        }

        TUI::Renderer::Corner _corner{};
        TUI::Renderer::StyleList _style_sheet{};
        std::string _text{"Text"}, _dis_text{"Text"};
        std::bitset<4> _status{};   // 1 = is_hovered, 2 = is_enabled, 3 = is_changed_pos_or_size
    };

}

#endif //TINY_DEMOS_DIALOG_MYUI_HPP