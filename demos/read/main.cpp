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
#include <csignal>
#include <climits>
using namespace Tiny;
using namespace TUI;

bool is_cancelled{};
static std::unique_ptr<OS::File> my_file{};

void exit_sig(int signal) {
    is_cancelled = true;
    if (my_file) {
        my_file->close();
    }
    Terminal::printError("read: Cancelled by signal {}!\r\n", signal);
    exit(signal);
}

uint64_t str2UInt(const std::string& str, bool* is_valid) {
    uint64_t ret = 0;
    *is_valid = true;
    for (auto& c : str) {
        if (isdigit(c)) {
            if (ret > (UINT64_MAX - (c - '0')) / 10) {
                *is_valid = false;
                return 0;
            }
            ret = ret * 10 + (c - '0');
        } else {
            *is_valid = false;
            break;
        }
    }
    return *is_valid ? ret : 0;
}

int readFile(const std::string& file_name, uint64_t offset, uint64_t count, uint64_t offset_bytes,
             uint64_t limit_bytes, bool shown_line_no, bool no_empty_line) {
    my_file.reset(new OS::File(file_name, OS::ReadOnly));
    if (!my_file->isOpen()) {
        Terminal::printError("read: File \"{}\" is not the valid file.\r\n", file_name);
        return 8;
    }
    if (offset_bytes > 0) my_file->moveTo(static_cast<int64_t>(offset_bytes));
    size_t line_no{};
    auto printLine = [&shown_line_no, &line_no, &no_empty_line] (const std::string& buf) {
        if (no_empty_line && buf.empty()) return;
        if (shown_line_no) {
            Terminal::setForegroundColor(Color::Blue, true);
            Terminal::printFormat("{:>6s} | ", line_no + 1);
            Terminal::reset();
        }
        Terminal::printLine(buf);
    };
    bool print_finished{};
    /// Read all text
    if (offset == 0 && count == 0 && limit_bytes == 0) {
        while (!my_file->isEOF() && !is_cancelled) {
            printLine(my_file->readLine());
            line_no++;
        }
    } else {
        bool print_flag{};
        size_t cnt_lines{}, cnt_bytes{};
        while (!my_file->isEOF() && !is_cancelled) {
            std::string buf;
            if (print_flag) {
                buf = my_file->readLine(limit_bytes == 0 ? 0 : limit_bytes - cnt_bytes);
            } else {
                buf = my_file->readLine();
            }
            if (no_empty_line && buf.empty()) {
                line_no++;
                continue;
            }
            if (!print_finished && line_no >= offset) {
                if (!cnt_lines) print_flag = true;
                if ((count > 0 && cnt_lines >= count) || (limit_bytes > 0 && cnt_bytes >= limit_bytes)) {
                    print_flag = false;
                    print_finished = true;
                    break;
                }
                cnt_lines++;
                cnt_bytes += buf.size();
            }
            if (print_flag) printLine(buf);
            line_no++;
        }
    }
    my_file->close();
    return 0;
}

int main(int argc, char *argv[]) {
    CommandParser parser(argc, argv);
    parser.addCommand("file", "f", "Specified the file path to read.", true, {}, true, true, "PATH");
    parser.addCommand("line", "n", "Show line number while reading file.");
    parser.addCommand("offset", "so", "Output content after skipping `n` lines.", true, "0", false, false, "n");
    parser.addCommand("limit", "lc", "The total number of lines to read.", true, "0", false, false, "n");
    parser.addCommand("offset-bytes", "SO", "Output content after skipping `n` byte(s).", true, "0", false, false, "n");
    parser.addCommand("limit-bytes", "LC", "The total number of bytes to read.", true, "0", false, false, "n");
    parser.addCommand("no-empty-line", "N", "No Output empty line when output content.");
    parser.addLastCommand("help", "h?", "Display this help information.");
    parser.addLastCommand("version", "v", "Display version information.");

    int n{};
    std::vector<std::string> missing_list;
    auto err = parser.exec(nullptr, &n, &missing_list);
    if (err != CommandParser::ParseError::NoError) {
        if (!missing_list.empty()) {
            Terminal::printError("read: Missing argument: \"{}\".", missing_list.front());
            return 1;
        }
        if (n > 0) {
            Terminal::printError("read: {}: '{}'.\r\n", CommandParser::getParseErrorName(err), argv[n]);
            return 1;
        }
        Terminal::printError("read: {}! Try to type \"--help\" for more information.\r\n",
                    CommandParser::getParseErrorName(err));
        return 2;
    }

    auto exec = parser.execCommandList();
    uint64_t offset{}, count{}, offset_bytes{}, count_bytes{};
    bool show_line_no{}, no_empty_line{};
    std::string file_name{};
    for (const auto & cmd: exec) {
        if (cmd.option_name == "help") {
            Terminal::printLine(parser.generateHelpInfo(Terminal::screenSize().width, false));
            Terminal::printFormat("RETURN VALUES:\r\n"
                                  "    0 - No error found\r\n"
                                  "    1 - Argument error\r\n"
                                  "    2 - Generic argument error\r\n"
                                  "    4 - Specified file is not found\r\n"
                                  "    6 - The numeric is invalid\r\n"
                                  "    8 - Can't open the specified file\r\n");
            return 0;
        }
        if (cmd.option_name == "version") {
            Terminal::printLine("read version 1.0\r\n");
            return 0;
        }
        if (cmd.option_name == "file") {
            if (!OS::Path::exist(cmd.value)) {
                Terminal::printError("read: File \"{}\" is not exist!\r\n", cmd.value);
                return 4;
            }
            file_name = cmd.value;
        } else if (cmd.option_name == "offset") {
            bool is_ok{};
            offset = str2UInt(cmd.value, &is_ok);
            if (!is_ok) {
                Terminal::printError("read: Invalid number: \"{}\".\r\n", cmd.value);
                return 6;
            }
        } else if (cmd.option_name == "limit") {
            bool is_ok{};
            count = str2UInt(cmd.value, &is_ok);
            if (!is_ok) {
                Terminal::printError("read: Invalid number: \"{}\".\r\n", cmd.value);
                return 6;
            }
        } else if (cmd.option_name == "offset-bytes") {
            bool is_ok{};
            offset_bytes = str2UInt(cmd.value, &is_ok);
            if (!is_ok) {
                Terminal::printError("read: Invalid number: \"{}\".\r\n", cmd.value);
                return 6;
            }
        } else if (cmd.option_name == "limit-bytes") {
            bool is_ok{};
            count_bytes = str2UInt(cmd.value, &is_ok);
            if (!is_ok) {
                Terminal::printError("read: Invalid number: \"{}\".\r\n", cmd.value);
                return 6;
            }
        } else if (cmd.option_name == "line") {
            show_line_no = true;
        } else if (cmd.option_name == "no-empty-line") {
            no_empty_line = true;
        }
    }
    signal(SIGINT, exit_sig);
    signal(SIGTERM, exit_sig);
    return readFile(file_name, offset, count, offset_bytes, count_bytes, show_line_no, no_empty_line);
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
