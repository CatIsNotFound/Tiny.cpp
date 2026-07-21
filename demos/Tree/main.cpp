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
#include <array>

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
    Terminal::printFormat("Usage: Tree [-p|--path] <Path> [-R | -r]\r\n\r\nOPTIONS:\r\n");
    for (auto& cmd : cmd_parser) {
        Terminal::printFormat("{:>8c}--{:<10s}, ", ' ', cmd.first);
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
    Terminal::printLine("Tree Version 1.0");
}

void print_my_tree(const std::vector<OS::Path> &paths, size_t level = 0, bool used_color = true) {
    size_t i = 0, sz = paths.size();
    for (auto& p : paths) {
        if (level > 0) Terminal::printFormat("{:{}c}", level * 4, ' ');
        Terminal::print(++i >= sz ? "└─ " : "├─ ");
        if (used_color) {
            if (p.isDirectory()) {
                Terminal::setForegroundColor(Color::Blue, true);
                Terminal::printFormat("{}\r\n", p.shortFileName());
                Terminal::reset();
            } else if (!p.isValid()) {
                Terminal::setForegroundColor(Color::Red, false);
                Terminal::setBackgroundColor(Color::Black, true);
                Terminal::printFormat("{}", p.path().substr(p.path().find_last_of(SPLASH) + 1));
                Terminal::reset();
                Terminal::printLine();
            } else {
                Terminal::printFormat("{}\r\n", p.shortFileName());
            }
        } else {
            Terminal::printFormat("{}\r\n", p.shortFileName());
        }
    }
}

bool list_all(const OS::Path& f_path, bool& stop_all) {
    static size_t found_cnt = 0;
    static bool is_confirm = false;
    found_cnt += 1;
    if (!is_confirm && found_cnt > 127) {
        Terminal::setForegroundColor(Color::Yellow, true);
        Terminal::printError("Tree: There are more than 128 files in this directory. Do you still want to list all? (y) ");
        Terminal::reset();
        auto key = Terminal::readLine();
        is_confirm = true;
        if (key != "y" && key != "Y") {
            stop_all = true;
            return false;
        }
    }
    return true;
}

void print_complex_tree(const std::unordered_map<size_t, std::vector<OS::Path>>& tree,
                        const OS::Path& parent_path, std::array<int, 256>& nodes, size_t cur_level = 1,
                        bool used_color = true) {
    if (tree.find(cur_level) == tree.end()) return;
    // size_t sz = tree.at(cur_level).size();
    auto& sub = tree.at(cur_level);
    std::vector<OS::Path> sub_paths;
    for (auto& p : sub) {
        if (parent_path.path() != p.parentDirectory()) continue;
        sub_paths.push_back(p);
    }
    for (size_t i = 0; i < sub_paths.size(); ++i) {
        nodes[cur_level] = i + 1 < sub_paths.size();
        for (size_t l = 1; l < cur_level; ++l) {
            Terminal::printFormat("{}   ", nodes[l] ? "│" : " ");
        }
        Terminal::printFormat("{}", i + 1 < sub_paths.size() ? "├─ " : "└─ ");
        if (used_color) {
            if (sub_paths[i].isDirectory()) {
                Terminal::setForegroundColor(Color::Blue, true);
                Terminal::printLine(sub_paths[i].shortFileName());

                Terminal::reset();
                print_complex_tree(tree, sub_paths[i], nodes, cur_level + 1, used_color);
            } else if (sub_paths[i].isSymbolLink()) {
                Terminal::setForegroundColor(Color::Green, false);
                Terminal::printLine(sub_paths[i].shortFileName());
                Terminal::reset();
            } else if (!sub_paths[i].isValid()) {
                Terminal::setForegroundColor(Color::Red, false);
                Terminal::setBackgroundColor(Color::Black, true);
                Terminal::printFormat("{}", sub_paths[i].path().substr(sub_paths[i].path().find_last_of(SPLASH) + 1));
                Terminal::reset();
                Terminal::printLine();
            } else {
                Terminal::printLine(sub_paths[i].shortFileName());
            }
        } else {
            Terminal::printLine(sub_paths[i].shortFileName());
            if (sub_paths[i].isDirectory()) {
                print_complex_tree(tree, sub_paths[i], nodes, cur_level + 1, used_color);
            }
        }
    }

}

uint8_t print_recursive_tree(const std::string& path, bool used_color = true) {
    OS::Path root(path);
    if (!root.isValid()) return 1;      // Invalid path!
    if (!root.isDirectory()) return 2;  // Not a directory!
    auto tree = OS::FileSystem::listPathEx(root, 0, &list_all);
    Terminal::printLine(root.path());
    if (tree.empty()) return 0;
    std::array<int, 256> nodes{};
    print_complex_tree(tree, root, nodes, 1, used_color);
    return 0;                           // Successful!
}

uint8_t print_tree(const std::string& path, bool is_recursive, bool used_color = true) {
    if (is_recursive) return print_recursive_tree(path, used_color);
    OS::Path root(path);
    if (!root.isValid()) return 1;      // Invalid path!
    if (!root.isDirectory()) return 2;  // Not a directory!
    Terminal::printLine(root.path());
    auto ls_paths = OS::FileSystem::listPath(root, 1);
    size_t sz = ls_paths.size();
    if (sz >= 128) {
        Terminal::printError("Tree: There are {} files in this directory. Do you still want to continue? (y) ", sz);
        auto ok = Terminal::getKey();
        if (ok != 'y' && ok != 'Y') return 0;
    }
    print_my_tree(ls_paths, 0, used_color);
    return 0; // Successful!
}

int main(int argc, char** argv) {
    CommandParser cmd_parser(argc, argv);
    cmd_parser.addCommand("path", "p", "Specified the path which should be shown by tree view.", true, ".", true, true);
    cmd_parser.addCommand("recursive", "Rr", "Whether to recursively include all possible directories under the specified path.");
    cmd_parser.addCommand("color", "c", "Show different colors based on different items.");
    cmd_parser.addLastCommand("help", "h", "Display the help info.");
    cmd_parser.addLastCommand("version", "v", "Display the version info.");
    int n{};
    std::vector<std::string> missing{};
    auto success = cmd_parser.exec(nullptr, &n, &missing);
    if (success != CommandParser::ParseError::NoError) {
        if (!missing.empty()) {
            Terminal::printError("Tree: Missing argument: {}.\r\nType '-h' or '--help' for help.\r\n", missing.front());
            return 1;
        }
        Terminal::printError("Tree: Unknown option: {}.\r\nType '-h' or '--help' for help.\r\n", argv[n]);
    }
    bool exec_tree = false, exec_recursive = false, exec_colorful = false;
    std::string path;
    auto exec_cmds = cmd_parser.execCommandList();
    for (auto& c : exec_cmds) {
        if (c.option_name == "help") {
            print_help(cmd_parser);
            return 0;
        }
        if (c.option_name == "version") {
            print_version();
            return 0;
        }
        if (c.option_name == "color") {
            exec_colorful = true;
        } else if (c.option_name == "recursive") {
            exec_recursive = true;
        } else if (c.option_name == "path") {
            exec_tree = true;
            path = c.value;
        }
    }

    if (exec_tree) {
        auto err = print_tree(path, exec_recursive, exec_colorful);
        if (err == 1) {
            Terminal::printError("Tree: The specified path \"{}\" is not exist or recognizable!\r\n", path);
            return 1;
        }
        if (err == 2) {
            Terminal::printError("Tree: The specified path \"{}\" is not a directory!\r\n", path);
            return 2;
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