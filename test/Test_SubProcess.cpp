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

int main(int argc, char *argv[]) {
    auto st = DT::currentTimestamps();
    TUI::Terminal::printLine("Running process...");
    OS::SubProcess sub_process(false, false);
#ifdef TINY_CPP_MY_OS_WINDOWS
    sub_process.exec(".\\assets\\keep_cmd.bat");
#else
    sub_process.exec("./assets/keep_cmd.bat");
#endif
    TUI::Terminal::printLine("Sended command.");
    TUI::Terminal::printFormat("Now using on pid {}...\r\n", sub_process.pid());
    sub_process.stop();
    auto ed = DT::currentTimestamps();
    TUI::Terminal::printFormat("Usage times: {}ms\r\n", ed - st);
    st = DT::currentTimestamps();
#ifdef TINY_CPP_MY_OS_WINDOWS
    OS::exec("powershell -C .\\assets\\keep_cmd.bat", 5000);
#else
    OS::exec("sh -c ./assets/keep_cmd", 5000);
#endif
    TUI::Terminal::printFormat("Usage times: {}ms.\r\n", DT::currentTimestamps() - st);
    return 0;
}

