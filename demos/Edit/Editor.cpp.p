
#include "Editor.hpp"

bool Editor::load(const std::string &text, size_t length) {
    size_t load_len = length == 0 ? text.length() : length;
    size_t read_rows{}, read_len{};
    _buffers.clear();
    _buffers.emplace_back("");
    for (char c: text) {
        _buffers[read_rows] += c;
        read_len++;
        if (c == '\r' || c == '\n') {
            _buffers.emplace_back("");
            read_rows++;
        }
        if (read_len >= load_len) break;
    }
    textAddedEvent();
    return true;
}

void Editor::moveCursor(const TUI::Position &pos) {
}

void Editor::moveCursor(uint32_t row, uint32_t col) {
}

void Editor::moveRows(int32_t row) {
}

void Editor::moveCols(int32_t col) {
}

void Editor::addText(const std::string &text) {
    if (_cur_pos.row > _buffers.size()) return;
    if (_cur_pos.column > _buffers[_cur_pos.row].length()) return;
    if (_cur_pos.column < _buffers[_cur_pos.row].length()) {
        _buffers[_cur_pos.row].insert(_cur_pos.column, text);
    } else {
        _buffers[_cur_pos.row] += text;
    }
    _need_render = true;
    textAddedEvent();
}

void Editor::insertLine(uint32_t row, const std::string &text) {
    if (row > _buffers.size()) return;
    _buffers.insert(_buffers.begin() + row, text);
    _need_render = true;
    textAddedEvent();
}

bool Editor::popChar() {

    return false;
}

void Editor::removeCurrentLine() {
}

bool Editor::removeLines(uint32_t offset, uint32_t count) {

    return false;
}

const TUI::Position & Editor::cursorPosition() const {
    return _cur_pos;
}

const std::string & Editor::lineOfText(uint32_t row) const {
    return {};
}

void Editor::renderEvent(TUI::Renderer &renderer) {
    if (!_need_render) return;
    TUI::Position temp_pos = position();
    TUI::Size temp_size = size();
    uint32_t i = 0;
    if (_auto_line_break) {
        for (const auto& buf: _buffers) {
            renderer.setSSF({temp_pos.row + (i++), 0}, "{}", style(S_Normal), buf.substr(_s_col, buf.length()));
        }
    } else {
        for (const auto& buf: _buffers) {
            renderer.setSSF({temp_pos.row + (i++), 0}, "{}", style(S_Normal), buf.substr(_s_col, temp_size.width));
        }
    }
}

void Editor::resizeEvent(uint32_t, uint32_t) {
}

void Editor::moveEvent(uint32_t, uint32_t) {
}

void Editor::keyEvent(TUI::KeyEvent keyboard) {

}

void Editor::mouseEvent(TUI::MouseEvent mouse) {
}

void Editor::focusEvent(bool focus) {
    TUI::Terminal::setCursorVisible(focus);
}

void Editor::enableEvent(bool enable) {
}

void Editor::clickedEvent() {
}

void Editor::textAddedEvent() {
}

void Editor::textRemovedEvent() {
}
