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

#include "../../src/Tiny.hpp"
#include "../Misc/Misc.hpp"
#include <cmath>
#include <deque>
#include <random>

using namespace Tiny;
using namespace TUI;
using term = Terminal;

struct TextEditArea {
    uint32_t start_row{};
    uint32_t end_row{};
    uint32_t last_row{};
    uint32_t cols_count{};
} text_edit_area;

enum EditStatus : uint8_t {
    Default = ' ',
    Unsaved = '*',
    UnEdit  = '>'
} current_edit_status{UnEdit};

std::string current_edit_file{};
Position    current_edit_pos{};
std::unordered_map<uint32_t, std::string> buffers;
bool insert_mode = true;
bool is_shown_title = false;


void readFile(OS::File& file, int64_t pos, size_t count, std::string& destination) {
    file.moveTo(pos);
    destination = file.readText(count);
}

bool saveFile(std::string& error) {
    OS::File write_file(current_edit_file, OS::WriteOnly);
    if (!write_file.isOpen()) {
        OS::lastSystemError(error);
        return false;
    }
    for (size_t r = 0; r < buffers.size(); ++r) {
        if (!write_file.writeLine(buffers[r + 1])) {
            error = term::formatString("Writing failed at row.{}", r);
            return false;
        }
    }
    write_file.close();
    return true;
}

void fatal(const std::string& message) {
    term::setBackgroundColor(Color::Red, false);
    term::setForegroundColor(Color::Yellow, true);
    term::moveCursor(text_edit_area.last_row, 0);
    term::printFormat("{:{}c}\r(E) {}",
        text_edit_area.cols_count, ' ', message.data());
    term::setCursorVisible(true);
    term::getKey();
}

bool question(const std::string& message) {
    term::setBackgroundColor(Color::Blue, false);
    term::setForegroundColor(Color::Yellow, true);
    term::moveCursor(text_edit_area.last_row, 0);
    term::printFormat("{:{}c}\r(?) {} (y/n) [Default: n]",
        text_edit_area.cols_count, ' ', message.data());
    term::setCursorVisible(true);
    auto op = term::getKey();
    term::setCursorVisible(false);
    term::reset();
    if (op == 'y' || op == 'Y') return true;
    return false;
}

template<typename T>
void progress(T value, T max_value) {
    term::reset();
    term::moveCursor(text_edit_area.last_row, 0);
    // term::printFormat("{:{}c}\r",
    //     text_edit_area.cols_count, ' ');
    term::setBackgroundColor(Color::Magenta, false);
    auto div = static_cast<float>(max_value);
    float p = ((div == 0) ? 1.f : static_cast<float>(value) / div);
    const uint32_t V = Misc::max(std::floor(static_cast<float>(text_edit_area.cols_count) * p), 0.f);
    auto text = term::formatString("Loading: {:>3.f}%", p * 100.f);
    if (V <= 13) {
        term::printFormat("{}", text.substr(0, V));
        term::reset();
        if (V < 13) term::printFormat("{}", text.substr(V));
    } else {
        term::printFormat("{}{:{}c}", text, V - 13, ' ');
        term::reset();
    }
}

void drawTitle(bool visible) {
    is_shown_title = visible;
    auto title = "Edit v1.0.0";
    auto help_1 = "Type any key to start edit.";
    auto help_2 = "Type Ctrl+Z to exit editor.";
    auto help_3 = "Type Ctrl+S to save current file.";

    auto w = text_edit_area.cols_count;
    auto h = text_edit_area.last_row / 2 - 3;
    auto lt = strlen(title);
    auto l1 = strlen(help_1);
    auto l2 = strlen(help_2);
    auto l3 = strlen(help_3);

    auto rt = w / 2 - lt / 2;
    auto r1 = w / 2 - l1 / 2;
    auto r2 = w / 2 - l2 / 2;
    auto r3 = w / 2 - l3 / 2;
    term::moveCursor(h, rt);
    term::print(visible ? title : std::string(lt, ' '));
    term::moveCursor(h + 2, r1);
    term::print(visible ? help_1 : std::string(l1, ' '));
    term::moveCursor(h + 3, r2);
    term::print(visible ? help_2 : std::string(l2, ' '));
    term::moveCursor(h + 4, r3);
    term::print(visible ? help_3 : std::string(l3, ' '));
}

void refreshEditor(OS::File& file) {
    term::setCursorVisible(false);
    auto file_name = file.isNull() ? term::formatString("{} [New file]", file.path()) : file.fileName();

    auto scr = term::screenSize();
    text_edit_area.last_row = scr.height - 1;
    text_edit_area.end_row = text_edit_area.last_row - 1;
    text_edit_area.cols_count = scr.width;

    auto space_count = static_cast<int32_t>(scr.width) - 17 - file_name.size();
    if (space_count < 0) space_count = 0;
    term::clearScreen();

    term::setBackgroundColor(Color::White, false);
    term::setForegroundColor(Color::Black, false);
    term::moveCursor(scr.height - 1, 0);
    term::printFormat("{:{}c}\r> {}", scr.width, ' ', file_name.data());
    term::reset();

    // TODO: Display edit area

}

bool initEditor(OS::File& file) {
    term::setCursorVisible(false);
    auto file_name = file.isNull() ? term::formatString("{} [New file]", file.path()) : file.fileName();

    auto scr = term::screenSize();
    text_edit_area.last_row = scr.height - 1;
    text_edit_area.end_row = text_edit_area.last_row - 1;
    text_edit_area.cols_count = scr.width;

    auto space_count = static_cast<int32_t>(scr.width) - 17 - file_name.size();
    if (space_count < 0) space_count = 0;
    term::clearScreen();

    if (file.isNull()) {
        drawTitle(true);
        term::setBackgroundColor(Color::White, false);
        term::setForegroundColor(Color::Black, false);
        term::moveCursor(scr.height - 1, 0);
        term::printFormat("{:{}c}\r> {}", scr.width, ' ', file_name.data());
        term::reset();
    } else {
        if (!file.open(OS::ReadOnly)) {
            fatal(term::formatString("Failed to open file \"{}\"!", file.path()));
            return false;
        }

        std::atomic<uint64_t> read_count{};
        std::deque<char> ch_deque{'\0'};
        bool is_full{false};
        Event load_file_event(1, "load file");
        load_file_event.setEvent([&read_count, &file, &ch_deque, &is_full]
                                            (const std::atomic<bool>& run) {
            size_t rows = 1;
            buffers.emplace(1, "");
            while (!file.isEOF() && run) {
                auto data = file.read(1);
                if (!is_full) ch_deque.push_back(static_cast<char>(data.front()));
                read_count.store(read_count.load() + 1);
                if (data.front() == '\r' || data.front() == '\n') {
                    rows++;
                    if (buffers.find(rows) == buffers.end()) buffers.emplace(rows, "");
                    continue;
                }
                if (data.front() >= ' ' && data.front() <= '~') buffers[rows] += data.front();
            }
            file.close();
        });
        load_file_event.setCondition([]{return true;});
        load_file_event.setRepeatCount(1);
        load_file_event.setDelayMS(10);
        if (file.fileSize() >= 1024 * 1024 &&
            !question("This file size is large than 1MB, open it anyway?")) {
            return false;
        }
        term::moveCursor(text_edit_area.last_row, 0);
        term::printFormat("{:{}c}\r", text_edit_area.cols_count, ' ');
        load_file_event.run();
        Position pos = {1, 1};
        while (load_file_event.isRunning() || !ch_deque.empty()) {
            progress(read_count.load(), file.fileSize());
            if (!is_full && pos.row > text_edit_area.end_row) {
                is_full = true;
            }
            if (!is_full && !ch_deque.empty()) {
                char ch = ch_deque.front();
                ch_deque.pop_front();
                if (ch == '\n') {
                    pos.row += 1;
                    pos.column = 1;
                    continue;
                }
                if (ch < ' ' || ch > '~') continue;

                if (pos.column >= text_edit_area.cols_count) continue;
                term::moveCursor(pos.row - 1, pos.column - 1);
                term::print(ch);
                pos.column += 1;
            }
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (is_full) {
                ch_deque.clear();
            }
        }
        term::setBackgroundColor(Color::White, false);
        term::setForegroundColor(Color::Black, false);
        term::moveCursor(scr.height - 1, 0);
        term::printFormat("{:{}c}\r> {} ({} rows)", scr.width, ' ', file_name.data(), buffers.size());
        term::reset();
    }
    return true;
}

void updateStatus() {
    term::moveCursor(text_edit_area.last_row, 0);
    term::setBackgroundColor(Color::White, false);
    term::setForegroundColor(Color::Black, false);
    term::print(current_edit_status);
    auto space_count = static_cast<int32_t>(text_edit_area.cols_count) - 25;
    term::moveCursor(text_edit_area.last_row, space_count);
    term::printFormat("{:15c}{:15c}{:>7s}:{:<7s}", ' ', '\b', current_edit_pos.row, current_edit_pos.column);
    if (insert_mode) {
        term::print("[INS]");
    } else {
        term::setBackgroundColor(Color::Black, true);
        term::setForegroundColor(Color::White, true);
        term::print("[COV]");
    }
    term::reset();

    auto row = Misc::min(current_edit_pos.row - 1, text_edit_area.end_row);
    term::moveCursor(row, current_edit_pos.column - 1);
}

void inputHandle() {
    while (true) {
        auto input = term::getInput();
        if (input.type == InputEvent::Keyboard && input.input.keyboard.is_pressed) {
            auto& key = input.input.keyboard;
            if (key.key == KEY_CTRL_Z) {
                if (current_edit_status == Unsaved &&
                    question(term::formatString("Do you want to save file\"{}\"?", current_edit_file))) {
                    // TODO: Save file
                    std::string err_info;
                    if (saveFile(err_info)) {
                        current_edit_status = Default;
                        updateStatus();
                    } else {
                        fatal("Failed to save file! E: "  + err_info);
                        updateStatus();
                    }
                }
                break;
            }
            if (key.key == KEY_CTRL_S) {
                // TODO: Save file
                std::string err_info;
                if (saveFile(err_info)) {
                    current_edit_status = Default;
                    updateStatus();
                } else {
                    fatal("Failed to save file! E: "  + err_info);
                    updateStatus();
                }
            }
            else if (current_edit_status == UnEdit) {
                if (is_shown_title) drawTitle(false);
                current_edit_status = Default;
                current_edit_pos = {1, 1};
                updateStatus();
                term::moveCursor(0, 0);
                term::setCursorVisible(true);
                if (buffers.empty()) {
                    buffers.emplace(1, "");
                }
            }
            else if (key.key == KEY_SPECIAL) {
                if (key.sp_key == SP_KEY_LEFT) {
                    current_edit_pos.column = Misc::max(current_edit_pos.column - 1, 1u);
                } else if (key.sp_key == SP_KEY_RIGHT) {
                    current_edit_pos.column = Misc::min(static_cast<uint64_t>(current_edit_pos.column + 1),
                                                    buffers[current_edit_pos.row].size() + 1);
                } else if (key.sp_key == SP_KEY_UP) {
                    current_edit_pos.row = Misc::max(current_edit_pos.row - 1, 1u);
                    current_edit_pos.column = Misc::min(static_cast<uint64_t>(current_edit_pos.column),
                                                        buffers[current_edit_pos.row].size() + 1);
                } else if (key.sp_key == SP_KEY_DOWN) {
                    current_edit_pos.row = Misc::min(static_cast<uint64_t>(current_edit_pos.row + 1), buffers.size());
                    current_edit_pos.column = Misc::min(static_cast<uint64_t>(current_edit_pos.column),
                                                        buffers[current_edit_pos.row].size() + 1);
                } else if (key.sp_key == SP_KEY_INSERT) {
                    insert_mode = !insert_mode;
                }
                updateStatus();
            }
            else if (key.key == KEY_CR) {
                if (current_edit_status != Unsaved) {
                    current_edit_status = Unsaved;
                }
                if (buffers.find(current_edit_pos.row) != buffers.end()) {
                    auto& buf = buffers[current_edit_pos.row];
                    std::string temp{};
                    if (current_edit_pos.column < buf.size()) {
                        temp = buf.substr(current_edit_pos.column - 1);
                    }
                    buffers[current_edit_pos.row + 1] = temp;
                }
                current_edit_pos.row += 1;
                current_edit_pos.column = 1;
                updateStatus();
            }
            else if (key.key == KEY_BK) {
                if (current_edit_status != Unsaved) {
                    current_edit_status = Unsaved;
                }
                if (buffers.find(current_edit_pos.row) != buffers.end()) {
                    auto& buf = buffers[current_edit_pos.row];
                    auto rm = static_cast<int64_t>(current_edit_pos.column) - 2;
                    if (rm < 0) continue;
                    if (!buf.empty()) buf.erase(buf.begin() + rm);
                    if (rm < buf.size()) {
                        term::printFormat("\b{} ", buf.substr(rm,
                            text_edit_area.cols_count - rm));
                    } else {
                        term::print("\b \b");
                    }
                }

                if (current_edit_pos.column == 1) {
                    if (current_edit_pos.row == 1) continue;
                    current_edit_pos.row -= 1;
                    if (buffers.find(current_edit_pos.row) != buffers.end()) {
                        current_edit_pos.column = buffers[current_edit_pos.row].size() + 1;
                    } else {
                        // Q: Maybe have problem...
                        current_edit_pos.column = 1;
                    }
                    updateStatus();
                    continue;
                }
                current_edit_pos.column -= 1;
                updateStatus();
            }
            else if (key.key >= ' ' && key.key <= '~') {
                if (current_edit_status != Unsaved) {
                    current_edit_status = Unsaved;
                }
                if (buffers.find(current_edit_pos.row) != buffers.end()) {
                    auto& buf = buffers[current_edit_pos.row];
                    if (insert_mode) {
                        buf.insert(buf.begin() + current_edit_pos.column - 1, key.key);
                        if (current_edit_pos.column - 1 >= buf.size()) {
                            term::print(key.key);
                        } else {
                            term::printFormat("{}", buf.substr(current_edit_pos.column - 1,
                                text_edit_area.cols_count - current_edit_pos.column - 1));
                        }
                    } else {
                        buf[current_edit_pos.column - 1] = key.key;
                        term::print(key.key);
                    }
                }

                current_edit_pos.column += 1;
                updateStatus();
            }

        } else if (input.type == InputEvent::Mouse) {
            // auto& mouse = input.input.mouse;
        }
    }
}


int main(int argc, char** argv) {
    CommandParser cmd_parser(argc, argv);
    cmd_parser.addCommand("file", "f", "Open a specified file. ",
        true, {}, true, true);
    cmd_parser.addLastCommand("version", "v", "Display the version of application.");
    cmd_parser.addLastCommand("help", "h", "Display the help information");

    int pos = -1;
    std::vector<std::string> missing;
    auto is_ok = cmd_parser.exec(nullptr, &pos, &missing);
    if (is_ok != CommandParser::ParseError::NoError) {
        if (pos > 0) {
            term::printFormat("Error: {}: \"{}\"\r\n", CommandParser::getParseErrorName(is_ok), argv[pos]);
            return 1;
        }

        if (!missing.empty() && missing.front() == "file") {
            term::enterRawMode();
            auto temp = OS::File("untitled.txt");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (initEditor(temp)) {
                inputHandle();
            }
            term::setCursorVisible(true);
            term::leaveRawMode();
            return 0;
        }

        term::printFormat("Error: {}!\r\n", CommandParser::getParseErrorName(is_ok));
        return 2;
    }

    auto cmd_list = cmd_parser.execCommandList();
    for (const auto & command: cmd_list) {
        if (command.option_name == "help") {
            term::printFormat("Usage: {} [FILE PATH]\r\n\r\n", argv[0]);
            term::printFormat("    OPTIONS:\r\n");
            for (auto & commands: cmd_parser) {
                auto& cmd = commands.second;
                term::printFormat("{:8c}--{:<9s}, -{}{:8c}{}\r\n", ' ',
                    cmd.option_name, cmd.short_options, ' ', cmd.description);
            }
            term::printFormat("\r\n{:4c}EXAMPLE:\r\n", ' ');
            term::printFormat("{:8c}{:<64s}{}\r\n", ' ', argv[0], "Open text editor");
            term::printFormat("{:8c}{:<64s}{}\r\n", ' ', term::formatString("{} <FILE PATH>", argv[0]),
                                                     "Open specified file by text editor");
            return 0;
        }
        if (command.option_name == "version") {
            term::printLine("Edit (Simple Text Editor) ver. 1.0.0");
            return 0;
        }
        if (command.option_name == "file") {
            auto file_path = OS::File(command.value);
            if (!file_path.isNull() && !file_path.isFile()) {
                term::printFormat("Error: Path \"{}\" is not the regular file!\r\n", file_path.path());
                return 4;
            }
            current_edit_file = file_path.path();
            term::enterRawMode();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (initEditor(file_path)) {
                inputHandle();
            }
            term::setCursorVisible(true);
            term::leaveRawMode();
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