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

#include "../src/TUI/Terminal.hpp"
#include "../src/OS/System.hpp"
#include "../src/Parser/CommandParser.hpp"
#include "../src/DateTime/DateTime.hpp"
#include <csignal>

using namespace Tiny;
using namespace TUI;
using namespace OS;

void exitsig(int) {
    Terminal::printLine("stat: Cancelled!");
    exit(127);
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

int printFileStat(const std::string& path, bool recursive_all_path, bool readable) {
    Path my_path(path);
    if (!my_path.isValid()) {
        Terminal::printError("stat: Invalid path: '{}'\r\n", path);
        return 2;
    }
    size_t f_count = 0, d_count = 0, p_size = 0;
    bool is_cancel{}, shown_detail{};
    if (my_path.isDirectory() && recursive_all_path) {
        shown_detail = true;
        Terminal::printLine("Scanning directory, it may be takes a long time...");
        FileSystem::listPathEx(my_path, 0, [&f_count, &d_count, &p_size, &is_cancel](const Path& path, bool& stop) {
            static bool is_confirm{};
            if (!is_confirm && f_count + d_count > 255) {
                Terminal::print() << TStyle::fg(Color::Yellow)
                                  << "stat: The specified directory contains too many items. Do you still want to continue? (Y/y) ";
                auto ok = Terminal::readLine();
                if (ok != "Y" && ok != "y") {
                    stop = true;
                    is_cancel = true;
                    return false;
                }
                is_confirm = true;
            }
            if (path.isDirectory()) {
                d_count++;
            } else {
                f_count++;
            }
            p_size += path.fileSize();
            return true;
        });
        if (is_cancel) shown_detail = false;
    }

    Terminal::reset();
    Terminal::printFormat("Path: {}\r\n", my_path.path());
    Terminal::printFormat("NodeID: {}\r\n", my_path.nodeID());
    Terminal::printFormat("Type: {:<14s}\r\n", OS::fileTypeName(my_path.fileType()));
    Terminal::printFormat("Owner | Group: {} | {}\r\n", my_path.userName(), my_path.groupName());
    std::string result{};
    getPermission(my_path, result);
    Terminal::printFormat("Permission: {}\r\n", result);
    if (readable) {
        DataUnit res_unit{};
        if (shown_detail) {
            auto size = OS::autoConvertDataSize(p_size, res_unit);
            Terminal::printFormat("Size: {:>6.2f}{} ({} file(s), {} dir(s))\r\n", size, OS::dataUnitName(res_unit), f_count, d_count);
        } else {
            auto size = OS::autoConvertDataSize(my_path.fileSize(), res_unit);
            Terminal::printFormat("Size: {:>6.2f}{}\r\n", size, OS::dataUnitName(res_unit));
        }
        DT::DateTime creat_date(my_path.lastCreateTime()), modify_date(my_path.lastWriteTime()), access_date(my_path.lastAccessTime());
        Terminal::printFormat("\r\nCreate time: {}\r\n", creat_date.formatString("yyyy/MM/dd HH:mm:ss"));
        Terminal::printFormat("Update time: {}\r\n", modify_date.formatString("yyyy/MM/dd HH:mm:ss"));
        Terminal::printFormat("Access time: {}\r\n", access_date.formatString("yyyy/MM/dd HH:mm:ss"));
    } else {
        if (shown_detail) {
            Terminal::printFormat("Size: {} ({} file(s), {} dir(s))\r\n", p_size, f_count, d_count);
        } else {
            Terminal::printFormat("Size: {:>6.2f}\r\n", my_path.fileSize());
        }
        Terminal::printFormat("\r\nCreate time: {}\r\n", my_path.lastCreateTime());
        Terminal::printFormat("Update time: {}\r\n", my_path.lastWriteTime());
        Terminal::printFormat("Access time: {}\r\n", my_path.lastAccessTime());
    }
    return 0;
}

int main(int argc, char *argv[]) {
    CommandParser parser(argc, argv);
    parser.addCommand("path", "p", "Specify the path to display.", true, {}, true, true, "PATH");
    parser.addCommand("recursive", "r", "Recursive the path if this path is directory.");
    parser.addCommand("readable", "R", "Show readable data size and datetime.");
    parser.addLastCommand("help", "h?", "Display this help");
    parser.addLastCommand("version", "v", "Display version information");
    int e{};
    std::vector<std::string> m;
    auto err = parser.exec(nullptr, &e, &m);
    if (err != CommandParser::ParseError::NoError) {
        if (!m.empty()) {
            Terminal::printError("stat: Missing option: '{}'\r\n", m.front());
            return 1;
        }
        if (e > 0) {
            Terminal::printError("stat: Invalid argument: '{}'\r\n", argv[e]);
            return 1;
        }
        Terminal::printError("stat: {}!\r\n", CommandParser::getParseErrorName(err));
        return 1;
    }
    auto exec_list = parser.execCommandList();
    std::string check_path{};
    bool is_recv{}, readable{};
    for (auto& command: exec_list) {
        if (command.option_name == "help") {
            Terminal::printLine(parser.generateHelpInfo(Terminal::screenSize().width));
            return 0;
        }
        if (command.option_name == "version") {
            Terminal::printLine("stat version 1.0");
            return 0;
        }
        if (command.option_name == "path") {
            check_path = command.value;
        } else if (command.option_name == "recursive") {
            is_recv = true;
        } else if (command.option_name == "readable") {
            readable = true;
        }
    }

    signal(SIGINT, exitsig);
    signal(SIGTERM, exitsig);
    return printFileStat(check_path, is_recv, readable);
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