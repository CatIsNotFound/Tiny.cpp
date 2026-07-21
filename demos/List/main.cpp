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
using namespace TUI;

#ifdef TINY_CPP_MY_OS_WINDOWS
#define SPLASH '\\'
#else
#define SPLASH '/'
#endif

std::vector<std::string> split(const std::string& str, size_t per_size) {
    std::vector<std::string> result;
    size_t sz = str.size() / per_size;
    if (str.size() % per_size != 0) sz += 1;
    result.reserve(sz);
    for (size_t i = 0; i < sz; ++i) {
        result.push_back(str.substr(per_size * i, per_size));
    }
    return result;
}

void print_help(CommandParser& cmd_parser) {
    auto scr = Terminal::screenSize();
    Terminal::printFormat("Usage: List [-p|--path] <Path> [OPTIONS]\r\n\r\nOPTIONS:\r\n");
    for (auto& cmd : cmd_parser) {
        Terminal::printFormat("{:>8c}--{:<8s}, ", ' ', cmd.first);
        int idx = 0;
        std::string short_arg;
        for (auto& opt : cmd.second.short_options) {
            short_arg += Terminal::formatString("-{}", opt);
            if (++idx < cmd.second.short_options.size()) {
                short_arg += " | ";
            }
        }

        Terminal::printFormat("{:<16s} ", short_arg.c_str());
        auto pos = Terminal::cursorPosition();
        size_t dis_size = scr.width - pos.column;
        if (dis_size < cmd.second.description.size()) {
            auto texts = split(cmd.second.description, dis_size);
            for (size_t i = 0; i < texts.size(); ++i) {
                Terminal::printLine(texts[i]);
                if (i != texts.size() - 1) {
                    Terminal::moveCursor(pos.row + i, pos.column);
                }
            }
        } else {
            Terminal::printLine(cmd.second.description);
        }
    }
}

void print_version() {
    Terminal::printLine("List Version 1.0");
}

void set_colorful_by_path(const OS::Path& item) {
    if (item.isDirectory()) {
        Terminal::setForegroundColor(Color::Blue, true);
    } else if (item.isSymbolLink()) {
        Terminal::setForegroundColor(Color::Cyan, false);
    } else if (item.userPermission() & OS::P_Execute) {
        Terminal::setForegroundColor(Color::Green, false);
    } else if (!item.isValid()) {
        Terminal::setForegroundColor(Color::Red, false);
        Terminal::setBackgroundColor(Color::Black, true);
    }
}

size_t calcTextSize(const std::string& text) {
    size_t sz = 0;
    auto utf8_list = splitUTF8(text.c_str());
    for (auto& item : utf8_list) {
        int add = 0;
        switch (item.size()) {
        case 1:
            add = 1;
            break;
        default:
            add = 2;
            break;
        }
        sz += add;
    }
    return sz;
}

void getPermission(const OS::Path& path, std::string& result) {
    result.assign(9, '-');
    auto usr = path.userPermission();
    auto grp = path.groupPermission();
    auto oth = path.otherPermission();
    result[0] = usr & OS::P_Read ? 'r' : '-';
    result[1] = usr & OS::P_Write ? 'w' : '-';
    result[2] = usr & OS::P_Execute ? 'x' : '-';
    result[3] = grp & OS::P_Read ? 'r' : '-';
    result[4] = grp & OS::P_Write ? 'w' : '-';
    result[5] = grp & OS::P_Execute ? 'x' : '-';
    result[6] = oth & OS::P_Read ? 'r' : '-';
    result[7] = oth & OS::P_Write ? 'w' : '-';
    result[8] = oth & OS::P_Execute ? 'x' : '-';
}

bool find_item(const OS::Path& path, bool& keep) {
    static size_t cnt = 0;
    static bool is_confirm = false;
    cnt += 1;
    if (!is_confirm && cnt >= 127) {
        Terminal::setForegroundColor(Color::Yellow, true);
        Terminal::printError("List: There are more than 128 files in this directory. Do you still want to list all? (y) ");
        auto user_opt = Terminal::readLine();
        is_confirm = true;
        if (user_opt != "y" && user_opt != "Y") {
            keep = true;
            Terminal::reset();
            return false;
        }
        Terminal::reset();
    }
    if (!path.shortFileName().empty()) {
        return path.shortFileName().front() != '.';
    }
    return true;
}

bool find_all_item(const OS::Path& path, bool& keep) {
    static size_t cnt = 0;
    static bool is_confirm = false;
    cnt += 1;
    if (!is_confirm && cnt >= 127) {
        Terminal::setForegroundColor(Color::Yellow, true);
        Terminal::printError("List: There are more than 128 files in this directory. Do you still want to list all? (y) ");
        auto user_opt = Terminal::readLine();
        is_confirm = true;
        if (user_opt != "y" && user_opt != "Y") {
            keep = true;
            Terminal::reset();
            return false;
        }
        Terminal::reset();
    }
    return true;
}

void print_list(const OS::Path& path, bool humanity, bool color, bool shown_all) {
    auto output_list = OS::FileSystem::listPathEx(path, 1, shown_all ? &find_all_item : &find_item);
    Terminal::printFormat("{:<10s}  {:>20s}  {:>12s}  {}\r\n", "Mode", "Update Time", "File Size", "Name");
    for (auto& item : output_list[1]) {
        std::string perm;
        getPermission(path, perm);
        char type = '-';
        if (item.isDirectory()) type = 'd';
        if (item.isSymbolLink()) type = 'l';
        if (!item.isValid()) type = '?';
        if (item.fileType() == OS::FileType::Device) type = 'D';
        if (item.fileType() == OS::FileType::Socket) type = 'S';
        if (humanity) {
            auto dt_str = DT::DateTime::formatString("yyyy/MM/dd HH:mm:ss", DT::DateTime(item.lastWriteTime()));
            OS::DataUnit unit{};
            auto size = OS::autoConvertDataSize(item.fileSize(), unit);
            Terminal::printFormat("{}{}  {:>20s}  {:>11.1f}{}  ", type, perm, dt_str, size, OS::dataUnitName(unit)[0]);
        } else {
            Terminal::printFormat("{}{}  {:>20s}  {:>12s}  ", type, perm, std::to_string(item.lastWriteTime()), std::to_string(item.fileSize()));
        }
        if (color) {
            set_colorful_by_path(item);
        }
        std::string temp = item.shortFileName();
        if (item.shortFileName().empty()) {
            temp = item.path().substr(item.path().find_last_of(SPLASH) + 1);
        }
        Terminal::printLine(temp);
        if (color) {
            Terminal::reset();
        }
    }
}

void print_filename_only(const OS::Path& path, bool one, bool color, bool shown_hide) {
    auto output_list = OS::FileSystem::listPathEx(path, 1, shown_hide ? &find_all_item : &find_item);
    if (!one) {
        size_t max_cols = 0, max_size = 0;
        Size scr_size = Terminal::screenSize();
        std::vector<std::string> lists(output_list[1].size(), {""});
        size_t idx = 0;
        for (auto& item : output_list[1]) {
            auto temp = item.shortFileName();
            auto temp_size = item.shortFileName().size();
            if (temp_size == 0) {
                size_t pos = item.path().find_last_of(SPLASH);
                temp_size = item.path().size() - pos + 1;
                temp = item.path().substr(pos + 1);
            }
            if (max_size < temp_size) {
                max_size = temp_size;
            }
            lists[idx++] = temp;
        }
        max_size += 2;
        max_cols = scr_size.width / (max_size);
        size_t cur_col = 0;
        idx = 0;
        for (auto& item : output_list[1]) {
            if (color) {
                set_colorful_by_path(item);
            }

            Terminal::printFormat("{}", lists[idx]);
            int64_t dis = static_cast<int64_t>(max_size) - calcTextSize(lists[idx++]);
            if (++cur_col >= max_cols) {
                cur_col = 0;
                Terminal::print("\r\n");
            } else if (dis > 0) {
                Terminal::moveRightCursor(dis);
            }
            if (color) Terminal::reset();
        }
    } else {
        for (auto& item : output_list[1]) {
            if (color) {
                set_colorful_by_path(item);
            }
            Terminal::printFormat("{}\r\n", item.shortFileName());
            if (color) Terminal::reset();
        }
    }
}

int exec_list_item(const std::string& path, uint8_t opt) {
    OS::Path my_path(path);
    if (!my_path.isValid()) {
        Terminal::printError("List: No such path to list!");
        return 3;
    }

    if (!my_path.isDirectory()) {
        Terminal::printError("List: Not a directory!");
        return 4;
    }

    if ((opt & 8) == 0) {
        print_filename_only(my_path, opt & 1, opt & 2, opt & 16);
    } else {
        print_list(my_path, opt & 4, opt & 2, opt & 16);
    }
    Terminal::printLine();
    return 0;
}

int main(int argc, char** argv) {
    CommandParser cmd_parser(argc, argv);
    cmd_parser.addLastCommand("version", "v", "Display the version info.");
    cmd_parser.addLastCommand("help", "?", "Display the help info.");
    cmd_parser.addCommand("color", "c", "Show different colors based on different items.", false);
    cmd_parser.addCommand("list", "l", "Display the items in more detail, one by one, in the form of a list.");
    cmd_parser.addCommand("humanity", "h", "Readability Enhancement.");
    cmd_parser.addCommand("inline", "1", "Only one item is displayed per row.");
    cmd_parser.addCommand("all", "a", "Shown all of the items. (Including hidden items.)");
    cmd_parser.addCommand("path", "p", "List the specified <Path>", true, {"."}, true, true);

    std::vector<std::string> missing;
    auto ok = cmd_parser.exec(nullptr, nullptr, &missing);
    if (ok != CommandParser::ParseError::NoError) {
        if (ok == CommandParser::ParseError::MissingArgument || ok == CommandParser::ParseError::MissingDefaultCommand) {
            Terminal::printError("List: {}! Missing option: {}, please type '-?' or '--help' for help!\r\n", CommandParser::getParseErrorName(ok), missing.front());
            return 1;
        }
        Terminal::printError("List: {}! Type '-?' or '--help' for help!\r\n", CommandParser::getParseErrorName(ok));
        return 1;
    }
    auto exec_list = cmd_parser.execCommandList();
    std::string passed;
    uint8_t opt{};
    for (auto& cmd : exec_list) {
        if (cmd.option_name == "help") {
            print_help(cmd_parser);
            return 0;
        }
        if (cmd.option_name == "version") {
            print_version();
            return 0;
        }
        if (cmd.option_name == "path") {
            passed = cmd.value;
        } else if (cmd.option_name == "inline") {
            opt |= 1;
        } else if (cmd.option_name == "color") {
            opt |= 2;
        } else if (cmd.option_name == "humanity") {
            opt |= 4;
        } else if (cmd.option_name == "list") {
            opt |= 8;
        } else if (cmd.option_name == "all") {
            opt |= 16;
        }
    }
    if (passed.empty()) {
        Terminal::printError("List: No specified path!\r\n");
        return 2;
    }
    return exec_list_item(passed, opt);
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