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

using namespace Tiny;
using namespace Tiny::Misc;
namespace tui = TUI;
using     ter = TUI::Terminal;
std::mutex mutex;
std::atomic<bool> is_refresh{};

struct Messagebox {
    std::string title{};
    std::string message{};
    enum Type : uint8_t {
        Normal = 1,
        Error = 8,
        Warning = 16,
        Information = 32,
        Question = 64,
        UserCustom = 128
    } type{};
    std::vector<std::string> buttons{};
    int8_t default_option{};
    tui::Color color{};
};

void printHelp(const CommandParser& cmd_parser) {
    ter::printFormat("USAGE: Msgbox <message> --title <title> ...\r\n\r\n");
    ter::printFormat("OPTIONS:\r\n");
    for (auto it = cmd_parser.cbegin(); it != cmd_parser.cend(); ++it) {
        auto& cmd = it->second;
        auto opt = cmd.option_name;
        if (cmd.has_value) opt += "=<VALUE>";
        ter::printFormat("{:4c}--{:<15s}", ' ', opt);
        ter::print(",");
        size_t i = 0;
        for (auto& short_opt : cmd.short_options) {
            if (i > 0) {
                ter::printFormat(", -{}", short_opt);
            } else {
                ter::printFormat(" -{}", short_opt);
            }
            i++;
        }
        ter::printFormat("\t\t{}\r\n", cmd.description);
    }

    ter::printLine("\r\nEXAMPLE:");
    ter::printFormat("{:4c}To display the normal message box, just execute: \r\n"
                     "{:4c}-   Msgbox \"Hello world!\" --title \"Info\"\r\n\r\n"
                     "{:4c}To display the different message box, execute: \r\n"
                     "{:4c}-   Msgbox \"Hello world!\" --title \"Message\" --type=[info,err,warn,quest]\r\n\r\n"
                     "{:4c}To use the custom message box, you have to specified the file! Execute: \r\n"
                     "{:4c}-   Msgbox \"Hello world\" --title=\"Your Message box\" --file=\"/path/to/file\"\r\n\r\n",
                     ' ', ' ', ' ', ' ', ' ', ' ');

    ter::printLine("\r\nNOTE:");
    ter::printFormat("{:4c}The file content needs to strictly follow below format: \r\n\r\n", ' ');
    ter::printFormat("{:8c}{:<14s} = {}\r\n", ' ', "title", "\"info\"");
    ter::printFormat("{:8c}{:<14s} = {}\r\n", ' ', "message", "\"Hello world!\"");
    ter::printFormat("{:8c}{:<14s} = {:<30s} # Available value: info, error, warn, quest, custom, none\r\n",
                        ' ', "type", "info");
    ter::printFormat("{:8c}{:<14s} = {:<30s} # Use ';' as the separator. \r\n",
                        ' ', "buttons", "Save;Discard;Cancel");
    ter::printFormat("{:8c}{:<14s} = {:<30s} # Index of the buttons, and `0` will not set default button.\r\n",
                        ' ', "default_index", "0");
    ter::printFormat("{:8c}{:<14s} = {:<30s} # Available value: red, blue, green, magenta, yellow, cyan, black, white.\r\n",
                        ' ', "color", "blue");
    ter::getKey();
}

Messagebox parseTextToMsgBox(OS::File& file, bool& ok, tui::Position& pos) {
    Messagebox new_box;
    std::vector<std::string> available_props = {
        "title", "message", "type", "buttons",
        "default_index", "color"
    };
    std::unordered_map<std::string, std::string> prop_map;
    size_t line = 0;
    size_t col = 0;
    while (!file.isEOF()) {
        line++;
        col = 0;
        bool read_equal = false;
        uint8_t read_quote = 0;
        std::string prop, value;
        auto buf = file.readLine();
        for (; col < buf.length(); col++) {
            if (buf[col] == '\r' || buf[col] == '\n') continue;
            if (buf[col] == '\"' && buf[col - 1] != '\\') {
                read_quote++;
                continue;
            }
            if (buf[col] == ' ' && read_quote % 2 == 0) continue;
            if (!read_equal) {
                if (buf[col] == '=') {
                    read_equal = true;
                    bool found_prop = false;
                    for (auto& p : available_props) {
                        if (p == prop) {
                            found_prop = true;
                            break;
                        }
                    }
                    if (!found_prop) {
                        pos.row = line;
                        pos.column = col;
                        ok = false;
                        return new_box;
                    }
                    continue;
                }
                prop += buf[col];
            } else {
                if (buf[col] == '=' && buf[col - 1] != '\\') {
                    pos.row = line;
                    pos.column = col;
                    ok = false;
                    return new_box;
                }
                if (col + 1 < buf.length() && buf[col] == '\\') {
                    switch (buf[col + 1]) {
                    case 'n':
                        value += '\n';
                        break;
                    case 't':
                        value += '\t';
                        break;
                    case 'r':
                        value += '\r';
                        break;
                    default:
                        value += '\\';
                        break;
                    }
                    col++;
                    continue;
                }
                if (buf[col] == '\"' && buf[col - 1] != '\\') continue;
                value += buf[col];
            }
        }
        if (col > 3 && !read_equal) {
            pos.row = line;
            pos.column = col;
            ok = false;
            return new_box;
        }
        if (col < 3) {
            line--;
            continue;
        }
        prop_map[prop] = value;
    }

    for (auto& prop : prop_map) {
        if (prop.first == "title") {
            new_box.title = prop.second;
        } else if (prop.first == "message") {
            new_box.message = prop.second;
        } else if (prop.first == "type") {
            if (prop.second == "info") {
                new_box.type = Messagebox::Information;
            } else if (prop.second == "error") {
                new_box.type = Messagebox::Error;
            } else if (prop.second == "warn") {
                new_box.type = Messagebox::Warning;
            } else if (prop.second == "quest") {
                new_box.type = Messagebox::Question;
            } else if (prop.second == "custom") {
                new_box.type = Messagebox::UserCustom;
            } else {
                new_box.type = Messagebox::Normal;
            }
        } else if (prop.first == "buttons") {
            size_t st_pos = 0;
            auto& cmd = prop.second;
            size_t ed_pos = cmd.find_first_of(';');
            while (ed_pos != std::string::npos && ed_pos < cmd.size()) {
                new_box.buttons.push_back(cmd.substr(st_pos, ed_pos - st_pos));
                st_pos = ed_pos + 1;
                ed_pos = cmd.find_first_of(';', st_pos);
            }
            new_box.buttons.push_back(cmd.substr(st_pos));
        } else if (prop.first == "default_index") {
            new_box.default_option = std::stoi(prop.second);
        } else if (prop.first == "color") {
            if (prop.second == "red") {
                new_box.color = tui::Color::Red;
            } else if (prop.second == "blue") {
                new_box.color = tui::Color::Blue;
            } else if (prop.second == "green") {
                new_box.color = tui::Color::Green;
            } else if (prop.second == "magenta") {
                new_box.color = tui::Color::Magenta;
            } else if (prop.second == "yellow") {
                new_box.color = tui::Color::Yellow;
            } else if (prop.second == "cyan") {
                new_box.color = tui::Color::Cyan;
            } else if (prop.second == "black") {
                new_box.color = tui::Color::Black;
            } else if (prop.second == "white") {
                new_box.color = tui::Color::White;
            }
        }
    }

    ok = true;
    return new_box;
}

void drawMessageBox(Messagebox& msg_box) {
    if (is_refresh.load()) return;
    auto& renderer = tui::Renderer::self();
    auto color = tui::Color::Black;
    bool intense = false;
    if (msg_box.type == Messagebox::Error) {
        color = tui::Color::Red;
        intense = true;
    } else if (msg_box.type == Messagebox::Warning) {
        color = tui::Color::Yellow;
        intense = true;
    } else if (msg_box.type == Messagebox::Information) {
        color = tui::Color::Blue;
        intense = true;
    } else if (msg_box.type == Messagebox::Question) {
        color = tui::Color::Green;
        intense = true;
    } else {
        color = msg_box.color;
        intense = true;
    }

    auto str_list = split(msg_box.message, '\n');

    tui::Renderer::Style back_style;
    back_style.bg_color = color;
    back_style.intensity = intense;

    tui::Renderer::Style win_style, win_shadow_style;
    win_style.bg_color = tui::Color::White;
    win_style.fg_color = tui::Color::Black;
    win_style.intensity = 3;
    win_shadow_style.bg_color = tui::Color::Black;

    tui::Renderer::Style win_title_style;
    win_title_style.bg_color = back_style.bg_color == TUI::Color::Black ? TUI::Color::White : TUI::Color::Black;
    win_title_style.fg_color = back_style.bg_color == TUI::Color::White ? TUI::Color::Black : TUI::Color::White;
    win_title_style.intensity = 2;

    TUI::Size window_size{};
    size_t btn_min_width = 0;
    for (auto& btn : msg_box.buttons) {
        btn_min_width += btn.size() + 3;
    }
    size_t title_min_width = msg_box.title.length() + 2;
    size_t msg_min_width = 0;
    for (auto& str : str_list) {
        auto size = str.size();
        if (size > msg_min_width) msg_min_width = size;
    }
    msg_min_width += 2;
    window_size.width = max<size_t>(btn_min_width, max<size_t>(title_min_width, msg_min_width));
    window_size.height = max<size_t>(6, 3 + str_list.size());
    window_size.width = max<size_t>(window_size.width, 16);

    TUI::Size scr_size = ter::screenSize();
    TUI::Position win_pos = {};
    win_pos.row = scr_size.height / 2 - window_size.height / 2 - 1;
    win_pos.column = scr_size.width / 2 - window_size.width / 2 - 1;
    TUI::Position shadow_win_pos = win_pos;
    shadow_win_pos.row += 1;
    shadow_win_pos.column += 1;
    TUI::Position end_shadow_win_pos = {shadow_win_pos.row + window_size.height,
        shadow_win_pos.column + window_size.width};
    TUI::Position end_win_pos = { win_pos.row + window_size.height, win_pos.column + window_size.width };

    TUI::Position title_pos{};
    title_pos.row = win_pos.row;
    title_pos.column = win_pos.column + (window_size.width / 2 - title_min_width / 2);

    TUI::Renderer::Style btn_style, def_btn_style;
    btn_style.bg_color = tui::Color::White;
    btn_style.fg_color = tui::Color::Black;
    btn_style.intensity = 3;
    def_btn_style.bg_color = back_style.bg_color;
    def_btn_style.fg_color = back_style.bg_color == tui::Color::White ? tui::Color::Black : tui::Color::White;

    renderer.fillScreen(back_style);
    renderer.fillRect(shadow_win_pos, end_shadow_win_pos, ' ', win_shadow_style);
    renderer.fillRect(win_pos, end_win_pos, ' ', win_style);
    renderer.fillRect({win_pos.row, win_pos.column}, {win_pos.row, win_pos.column + window_size.width}, ' ', win_title_style);
    renderer.setSSF(title_pos, msg_box.title.c_str(), win_title_style);
    for (size_t i = 0; i < str_list.size(); ++i) {
        tui::Position new_text_pos{};
        new_text_pos.row = win_pos.row + 2 + i;
        new_text_pos.column = win_pos.column + 1;
        renderer.setSSF(new_text_pos, str_list[i].c_str(), win_style);
    }
    size_t idx = 0;
    auto btn_width = window_size.width / max<size_t>(msg_box.buttons.size(), 1);
    tui::Position b_pos{};
    for (const auto& btn : msg_box.buttons) {
        b_pos.row = end_win_pos.row;
        b_pos.column = win_pos.column + btn_width * (idx++) + 1;
        int64_t start = static_cast<int64_t>(btn_width) / 2 - btn.size() / 2;
        auto st_pos = clamp<int64_t>(start, 0, window_size.width);
        auto btn_text = makeStringOnCenter(btn, btn_width);
        btn_text.front() = '[';
        btn_text.back() = ']';
        if (idx == msg_box.default_option) {
            renderer.setSSF(b_pos, "{}", def_btn_style, btn_text);
        } else {
            renderer.setSSF(b_pos, "{}", btn_style, btn_text);
        }
    }
    is_refresh.store(true);
}

int main(int argc, char** argv) {
    CommandParser cmd_parser(argc, argv);
    cmd_parser.addCommand("message", "m", "Set the message of message box", true, {}, true, true);
    cmd_parser.addCommand("title", "t", "Set the title of message box", true, {}, true);
    cmd_parser.addCommand("type", "p", "Set the type of message box", true, "info", false);
    cmd_parser.addCommand("index", "i", "Default index of the buttons", true, "-1");
    cmd_parser.addCommand("buttons", "b", "Set buttons of message box", true);
    cmd_parser.addLastCommand("file", "f", "Import the file to display message box", true);
    cmd_parser.addLastCommand("help", "h", "Print this help message");
    cmd_parser.addLastCommand("version", "v", "Print the version message");

    std::vector<std::string> err_arg;
    int err_pos = 0;
    auto err = cmd_parser.exec(nullptr, &err_pos, &err_arg);

    if (err != CommandParser::ParseError::NoError) {
        if (err_pos > 0) {
            ter::setForegroundColor(tui::Color::Red, true);
            ter::printError("Error: {} at pos \"{}\".\r\n", CommandParser::getParseErrorName(err), argv[err_pos]);
            ter::printError("p.s: Type '-h' or '--help' to display the help info.\r\n");
            ter::reset();
            return 2;
        }
        if (!err_arg.empty()) {
            ter::setForegroundColor(tui::Color::Red, true);
            ter::printError("Error: {} (Missing option: {})\r\n", CommandParser::getParseErrorName(err), err_arg.front());
            ter::printError("p.s: Type '-h' or '--help' to display the help info.\r\n");
            ter::reset();
            return 3;
        }
        ter::setForegroundColor(tui::Color::Red, true);
        ter::printError("Error: {}\r\n", CommandParser::getParseErrorName(err));
        ter::printError("p.s: Type '-h' or '--help' to display the help info.\r\n");
        ter::reset();
        return 1;
    }

    auto cmds = cmd_parser.execCommandList();
    bool file_mode = false;
    Messagebox msg_box;
    std::vector<std::string> def_buttons;
    std::string file_name;
    for (auto& cmd : cmds) {
        if (cmd.option_name == "help") {
            printHelp(cmd_parser);
            return 0;
        }
        if (cmd.option_name == "version") {
            ter::printLine("Version 1.0.0");
            return 0;
        }
        if (cmd.option_name == "file") {
            file_mode = true;
            file_name = cmd.value;
            break;
        }
        if (cmd.option_name == "message") {
            replaceAll(cmd.value, R"(\n)", "\n");
            replaceAll(cmd.value, R"(\t)", "\t");
            replaceAll(cmd.value, R"(\r)", "\r");
            msg_box.message = cmd.value;
        }
        else if (cmd.option_name == "title") {
            msg_box.title = cmd.value;
        }
        else if (cmd.option_name == "type") {
            if (cmd.value == "info") {
                msg_box.type = Messagebox::Information;
                def_buttons.emplace_back("Ok");
                msg_box.default_option = 1;
            } else if (cmd.value == "error") {
                msg_box.type = Messagebox::Error;
                def_buttons.emplace_back("Ok");
                msg_box.default_option = 1;
            } else if (cmd.value == "warn") {
                msg_box.type = Messagebox::Warning;
                def_buttons.emplace_back("Ok");
                def_buttons.emplace_back("Ignore");
                msg_box.default_option = 1;
            } else if (cmd.value == "quest") {
                msg_box.type = Messagebox::Question;
                def_buttons.emplace_back("Yes");
                def_buttons.emplace_back("No");
                def_buttons.emplace_back("Cancel");
                msg_box.default_option = 2;
            } else if (cmd.value == "none" || cmd.value.empty()) {
                msg_box.type = Messagebox::Normal;
                def_buttons.emplace_back("Ok");
                msg_box.default_option = 1;
            } else if (cmd.value == "custom") {
                msg_box.type = Messagebox::UserCustom;
                def_buttons.emplace_back("Ok");
                msg_box.default_option = 1;
            } else {
                ter::setForegroundColor(tui::Color::Red, true);
                ter::printError("Error: Unknown type name: \"{}\"\r\n", cmd.value);
                ter::printError("Available type names are: info, error, warn, none, custom\r\n");
                ter::printError("p.s: Type '-h' or '--help' to display the help info.\r\n");
                ter::reset();
                return 8;
            }
        }
        else if (cmd.option_name == "index") {
            msg_box.default_option = std::stoi(cmd.value);
        }
        else if (cmd.option_name == "buttons") {
            size_t st_pos = 0;
            size_t pos = cmd.value.find_first_of(';');
            while (pos != std::string::npos && pos < cmd.value.size()) {
                msg_box.buttons.push_back(cmd.value.substr(st_pos, pos - st_pos));
                st_pos = pos + 1;
                pos = cmd.value.find_first_of(';', st_pos);
            }
            msg_box.buttons.push_back(cmd.value.substr(st_pos));
        }
    }
    if (def_buttons.empty()) {
        msg_box.type = Messagebox::Normal;
        def_buttons.emplace_back("Ok");
        msg_box.default_option = 1;
    }
    if (msg_box.buttons.empty()) msg_box.buttons = def_buttons;

    if (file_mode) {
        OS::File file(file_name, OS::ReadOnly);
        if (!file.isOpen()) {
            ter::setForegroundColor(tui::Color::Red, true);
            ter::printFormat("Error: Failed to open file \"{}\"\r\n", file.path());
            ter::printLine("p.s: Type '-h' or '--help' to display the help info.");
            ter::reset();
            return 16;
        }
        bool ok = false;
        tui::Position pos{};
        msg_box = parseTextToMsgBox(file, ok, pos);
        if (!ok) {
            ter::setForegroundColor(tui::Color::Red, true);
            ter::printError("Error: Parsing failed at {}:{}:{}.\r\n", file.path(), pos.row, pos.column);
            ter::printError("p.s: Type '-h' or '--help' to display the help info.\r\n");
            ter::reset();
            return 32;
        }
    }
    Event refresh_event(1001, "refresh");
    refresh_event.setCondition([&] { return is_refresh.load(); });
    refresh_event.setDelayMS(10);
    refresh_event.setEvent([](const std::atomic<bool>&) {
        TUI::Renderer::self().present();
        TUI::Terminal::setCursorVisible(false);
        is_refresh.store(false);
    });
    refresh_event.setRepeatCount(65535);
    refresh_event.run();
    while (true) {
        tui::Renderer::self().setResizeEvent([&msg_box](TUI::Renderer&) {
            drawMessageBox(msg_box);
        });
        drawMessageBox(msg_box);
        tui::SP_Keys sp_keys;
        auto ch = ter::getKey(&sp_keys);
        if (ch == tui::KEY_SPECIAL) {
            if (sp_keys == tui::SP_KEY_LEFT) {
                msg_box.default_option = clamp<int8_t>(msg_box.default_option - 1, 1, msg_box.buttons.size());
            } else if (sp_keys == tui::SP_KEY_RIGHT) {
                msg_box.default_option = clamp<int8_t>(msg_box.default_option + 1, 1, msg_box.buttons.size());
            }
        } else if (ch == tui::KEY_ESC) {
            refresh_event.stop();
            TUI::Terminal::setCursorVisible(true);
            return 0;
        } else if (ch == tui::KEY_CR) {
            refresh_event.stop();
            TUI::Terminal::setCursorVisible(true);
            return msg_box.default_option;
        }
    }

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