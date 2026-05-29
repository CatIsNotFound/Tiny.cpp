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
        ter::getKey(ch, sp);
        if (ch == Tiny::TUI::KEY_CTRL_C || ch == Tiny::TUI::KEY_CTRL_Z) {
            break;
        }
        auto old_pos = ter::cursorPosition();
        auto key_name = TUI::getKeyName(ch, sp);
        if (ch == Tiny::TUI::KEY_SPECIAL) {
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

void match_word() {
    ter::enterRawMode();
    size_t page_index = 1;
    auto width = ter::screenSize().width;
    std::string split(width, '-');

    ter::moveCursor(ter::screenSize().height - 1, 0);
    ter::reverseColor(true);
    ter::print("Ctrl+Z: Quit, Home: 1st page, End: Last page, PgUp: Page up, PgDn: Page down");
    ter::reverseColor(false);

    OS::File file("./assets/TUI/book.txt", OS::ReadOnly);
    if (!file.isOpen()) {
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
        ter::getKey(ch, sp_keys);
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
}

int main() {
    ter::print("Please press any key to continue...");
    ter::getKey();
    while (true) {
        ter::printLine("Which one would you like to test? ");
        ter::printLine("1) Keyboard test");
        ter::printLine("2) Input text test");
        ter::printLine("3) Match words test");
        ter::printLine("0) Exit");
        ter::print("Please select: ");
        auto opt = ter::readLine();
        if (opt == "1") {
            test_input();
        } else if (opt == "2") {
            input_text();
        } else if (opt == "3") {
            match_word();
        } else if (opt == "0") {
            return 0;
        }
    }
    return 0;
}