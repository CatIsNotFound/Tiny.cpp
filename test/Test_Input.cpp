/*************************************************************************************
 * MIT License                                                                       *
 *                                                                                   *
 * Copyright (c) 2026 CatIsNotFound                                                  *
 *                                                                                   *
 * Permission is hereby granted, free of charge, to any person obtaining a copy      *
 * of this software and associated documentation files (the "Software"), to deal     *
 * in the Software without restriction, including without limitation the rights      *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         *
 * copies of the Software, and to permit persons to whom the Software is             *
 * furnished to do so, subject to the following conditions:                          *
 *                                                                                   *
 * The above copyright notice and this permission notice shall be included in all    *
 * copies or substantial portions of the Software.                                   *
 *                                                                                   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     *
 * SOFTWARE.                                                                         *
 *                                                                                   *
 *************************************************************************************/

#include "../src/Tiny.hpp"

using namespace Tiny;
using ter = TUI::Terminal;

template<typename T>
T clamp(T value, T min, T max) {
    return (value < min) ? min : (value > max) ? max : value;
}

std::vector<std::string> splitViewText(const std::string& text) {
    auto scr_width = ter::screenSize().width;
    size_t len = 0;
    std::vector<std::string> tokens;
    std::string token;
    for (auto& t : text) {
        if (len >= scr_width || t == '\n') {
            tokens.push_back(token);
            token.clear();
            len = 0;
            continue;
        }
        token.push_back(t);
        len++;
    }
    if (len < scr_width && !token.empty()) { tokens.push_back(token); }
    return tokens;
}

void test_input() {
    ter::enterRawMode();
    uint8_t ch = 0;
    ter::moveCursor(ter::screenSize().height - 1, 0);
    ter::reverseColor(true);
    ter::print("Press Ctrl+C or Ctrl+Z to exit...");
    ter::reverseColor(false);
    ter::moveCursor(0, 0);
    while (true) {
        TUI::SP_Keys sp{};
        ch = ter::getKey(&sp);
        if (ch == TUI::KEY_CTRL_C || ch == TUI::KEY_CTRL_Z) {
            break;
        }
        auto old_pos = ter::cursorPosition();
        auto key_name = TUI::getKeyName(ch, sp);
        if (ch == TUI::KEY_SPECIAL) {
            ter::printFormat("{:#d} (SP_{}){:60c}\r\n", ch, key_name, ' ');
        } else {
            ter::printFormat("{:#d} ({}){:50c}\r\n", ch, key_name, ' ');
        }
        ter::moveCursor(ter::screenSize().height - 1, 0);
        ter::reverseColor(true);
        ter::print("Press Ctrl+C or Ctrl+Z to exit...");
        ter::reverseColor(false);
        old_pos.row += 1;
        old_pos.column = 0;
        ter::moveCursor(old_pos);
    }
    ter::leaveRawMode();
}

void input_text() {
    ter::enterRawMode();
    ter::moveCursor(ter::screenSize().height - 1, 0);
    ter::reverseColor(true);
    ter::print("Press Enter without any text to exit...");
    ter::reverseColor(false);
    ter::moveCursor(0, 0);
    while (true) {
        auto text = ter::readLineW();
        if (text.empty()) break;
    }
    ter::leaveRawMode();
}

void mouse_test() {
    ter::enterRawMode();
    ter::setMouseEnabled(true);
    ter::moveCursor(ter::screenSize().height - 1, 0);
    ter::reverseColor(true);
    ter::print("Press any mouse button, and click this line to quit.");
    ter::reverseColor(false);
    TUI::Position mouse_pos{};
    bool is_pressed{};
    while (true) {
        auto mouse_btn = static_cast<TUI::SP_Mouse>(ter::getMouseButton(&mouse_pos, &is_pressed));
        auto scr_height = ter::screenSize().height - 1;
        ter::moveCursor(scr_height, 0);
        ter::setBackgroundColor(TUI::Color::Yellow, true);
        ter::setForegroundColor(TUI::Color::Blue);
        ter::reverseColor(true);
        ter::clearInRow(scr_height);
        ter::printFormat("Pos: (R {}, C {}), M: {}, Is pressed: {}, Click this line to quit.", mouse_pos.row, mouse_pos.column, 
                                                                                               TUI::getMouseName(mouse_btn), is_pressed);
        ter::reverseColor(false);
        ter::setBackgroundColor(TUI::Color::Default, false);
        ter::moveCursor(mouse_pos);
        if (mouse_btn != TUI::SP_MOUSE_MOVED && mouse_btn != TUI::SP_MOUSE_UNKNOWN && mouse_pos.row >= scr_height) break;
    }
    ter::setMouseEnabled(false);
    ter::leaveRawMode();
}

void view_file(const char* exec_path) {
    ter::enterRawMode();
    size_t page_index = 1;
    auto width = ter::screenSize().width;
    std::string split(width, '-');

    ter::moveCursor(ter::screenSize().height - 1, 0);
    ter::reverseColor(true);
    ter::clearInRow(ter::screenSize().height - 1);
    ter::print("Ctrl+Z: Quit, Home: 1st page, End: Last page, PgUp: Page up, PgDn: Page down");
    ter::reverseColor(false);

    auto e_path = OS::Path(exec_path).parent().join("assets/TUI/book.txt");
    OS::File file(e_path, OS::ReadOnly);
    if (!file.isOpen()) {
        ter::clearScreen();
        ter::setForegroundColor(TUI::Color::Red, true);
        ter::printLine("Error: Path \"" + e_path.path() + "\" is not loaded!\r\n");
        ter::readLine();
        ter::reset();
        ter::leaveRawMode();
        return;
    }
    auto buf = file.readAllText();
    file.close();
    const size_t ROW_START = 2, ROW_END = ter::screenSize().height - 2;
    auto tokens = splitViewText(buf);
    size_t tokens_size = tokens.size();
    size_t page_cnt = tokens.size() / (ROW_END - ROW_START) + 1;

    ter::moveCursor(0, 0);
    ter::setBolder(true);
    ter::print("::>> Test Scroll Page <<::");
    ter::setBolder(false);

    auto drawPage = [&]() {
        for (size_t i = ROW_START; i <= ROW_END; ++i) {
            ter::moveCursor(i, 0);
            ter::clearInRow(i);
            size_t idx = (page_index - 1) * (ROW_END - ROW_START) + (i - ROW_START);
            if (idx < tokens.size()) {
                ter::print(tokens[idx]);
            }
        }
    };

    drawPage();
    ter::moveCursor(ROW_START, 0);

    while (true) {
        uint8_t ch = 0;
        TUI::SP_Keys sp_keys{};
        ch = ter::getKey(&sp_keys);
        auto cur = ter::cursorPosition();

        if (ch == TUI::KEY_CTRL_Z) break;

        if (ch == TUI::KEY_SPECIAL && sp_keys == TUI::SP_KEY_LEFT) {
            int col = clamp((int)cur.column - 1, 0, (int)width);
            ter::moveCursor(cur.row, col);
        } else if (ch == TUI::KEY_SPECIAL && sp_keys == TUI::SP_KEY_RIGHT) {
            int col = clamp((int)cur.column + 1, 0, (int)width);
            ter::moveCursor(cur.row, col);
        } else if (ch == TUI::KEY_SPECIAL && sp_keys == TUI::SP_KEY_UP) {
            auto row = clamp((size_t)(cur.row - 1), ROW_START, ROW_END);
            ter::moveCursor(row, cur.column);
        } else if (ch == TUI::KEY_SPECIAL && sp_keys == TUI::SP_KEY_DOWN) {
            auto row = clamp((size_t)(cur.row + 1), ROW_START, ROW_END);
            ter::moveCursor(row, cur.column);
        } else if (ch == TUI::KEY_SPECIAL && sp_keys == TUI::SP_KEY_HOME) {
            page_index = 1;
            drawPage();
            ter::moveCursor(ROW_START, 0);
        } else if (ch == TUI::KEY_SPECIAL && sp_keys == TUI::SP_KEY_END) {
            page_index = page_cnt;
            drawPage();
        } else if (ch == TUI::KEY_SPECIAL && sp_keys == TUI::SP_KEY_PAGE_UP) {
            page_index = clamp(page_index - 1, (size_t)1, page_cnt);
            drawPage();
            ter::moveCursor(ROW_START, 0);
        } else if (ch == TUI::KEY_SPECIAL && sp_keys == TUI::SP_KEY_PAGE_DOWN) {
            page_index = clamp(page_index + 1, (size_t)1, page_cnt);
            drawPage();
        }
    }
    ter::leaveRawMode();
    ter::clearScreen();
}

void test_input_event() {
    ter::enterRawMode();
    bool is_on = ter::setMouseEnabled(true);
    ter::setBackgroundColor(TUI::Color::Blue);
    ter::setForegroundColor(TUI::Color::Yellow, true);
    ter::clearInRow(0);
    ter::print("Press Ctrl+Z to quit.\r\n");
#ifdef TINY_CPP_USE_GPM
    if (!strcmp(getenv("TERM"), "linux")) {
        if (is_on) {
            ter::setBackgroundColor(TUI::Color::Yellow, true);
            ter::setForegroundColor(TUI::Color::Blue);
            ter::clearInRow(1);
            ter::printLine("You are using GPM Lib!");
            ter::reset();
        }
    }
#endif
    ter::reset();
    ter::setScrollRegion(2, ter::screenSize().height - 1);
    ter::moveCursor(2, 0);
    while (true) {
        auto ev = ter::getInput();
        if (ev.type == TUI::InputEvent::Keyboard && ev.input.keyboard.key == TUI::KEY_CTRL_Z) break;
        auto it = ev.type;
        if (it == 0) continue;
        ter::printFormat("Input type: {}, ", (it == 1 ? "Keyboard" : "Mouse"));
        if (it == TUI::InputEvent::Keyboard) {
            ter::printFormat("Key: {}, Is pressed: {:s}\r\n",
                TUI::getKeyName(ev.input.keyboard.key, ev.input.keyboard.sp_key), ev.input.keyboard.is_pressed);
        } else if (it == TUI::InputEvent::Mouse) {
            ter::printFormat("Button: {}, Pos: ({}, {}), Is pressed: {:s}\r\n", TUI::getMouseName(ev.input.mouse.button),
                ev.input.mouse.position.column, ev.input.mouse.position.row, ev.input.mouse.is_pressed);
        } else {
            ter::printLine();
        }
    }
    ter::resetScrollRegion();
    ter::setMouseEnabled(false);
    ter::leaveRawMode();
}

int main(int argc, char* argv[]) {
    ter::print("Please press any key to continue...");
    ter::getKey();
    while (true) {
        ter::printLine("Which one would you like to test? ");
        ter::printLine("1) Keyboard test");
        ter::printLine("2) Input text test");
        ter::printLine("3) Mouse Test");
        ter::printLine("4) View File");
        ter::printLine("5) Input Event Test");
        ter::printLine("0) Exit");
        ter::print("Input the number: ");
        auto opt = ter::readLine();
        if (opt == "1") {
            test_input();
        } else if (opt == "2") {
            input_text();
        } else if (opt == "3") {
            mouse_test();
        } else if (opt == "4") {
            view_file(argv[0]);
        } else if (opt == "5") {
            test_input_event();
        } else if (opt == "0") {
            return 0;
        } else if (!opt.empty()){
            ter::setForegroundColor(TUI::Color::Red, true);
            ter::printFormat("Unsupported option: \"{}\"\r\n", opt);
            ter::reset();
        } else {
            ter::setForegroundColor(TUI::Color::Red, true);
            ter::printLine("You have not select the number!");
            ter::reset();
        }
    }
    return 0;
}

/*************************************************************************************
 * MIT License                                                                       *
 *                                                                                   *
 * Copyright (c) 2026 CatIsNotFound                                                  *
 *                                                                                   *
 * Permission is hereby granted, free of charge, to any person obtaining a copy      *
 * of this software and associated documentation files (the "Software"), to deal     *
 * in the Software without restriction, including without limitation the rights      *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         *
 * copies of the Software, and to permit persons to whom the Software is             *
 * furnished to do so, subject to the following conditions:                          *
 *                                                                                   *
 * The above copyright notice and this permission notice shall be included in all    *
 * copies or substantial portions of the Software.                                   *
 *                                                                                   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     *
 * SOFTWARE.                                                                         *
 *                                                                                   *
 *************************************************************************************/