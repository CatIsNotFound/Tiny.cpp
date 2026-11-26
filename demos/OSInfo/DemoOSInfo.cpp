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
using namespace EV;
using term = TUI::Terminal;

std::mutex mutex{};
std::atomic<bool> need_refresh{};

template <typename T>
T min(T value1, T value2) {
    return value1 < value2 ? value1 : value2;
}

template <typename T>
T max(T value1, T value2) {
    return value1 > value2 ? value1 : value2;
}

void drawInfo(TUI::TermBuffer& renderer) {
    auto win_width = TUI::Terminal::screenSize().width;
    const auto W = min(win_width, 60U);
    const auto IW = min(win_width, 40U);
    const auto II = IW / 2;
    renderer.drawBorder({0, 0}, {2, W}, {}, {});
    const auto III = max(static_cast<int>(II - 8), 0);
    renderer.drawString({1, 6}, "{:>{}c}Host Information", III, ' ');

    TUI::TermStyleList styles;
    TUI::TermStyle style_1, style_2;
    style_1.intensity = 2;
    style_1.colors.color.fg = TUI::Color::Blue;
    style_2.intensity = 2;
    style_2.colors.color.fg = TUI::Color::Yellow;
    styles.push_back(style_1);
    styles.push_back(style_2);

    auto os_info = OS::currentHostInfo();
    renderer.drawBorder({2, 0}, {10, W}, {}, {});
    renderer.drawString({3, 1}, "</>{:>{}s}: <1>{:<{}s}", styles, II, "Hostname", II, os_info.host_name);
    renderer.drawString({4, 1}, "</>{:>{}s}: <1>{:<{}s}", styles, II, "Username", II, os_info.user_name);
    renderer.drawString({5, 1}, "</>{:>{}s}: <1>{:<{}s}", styles, II, "OS", II, os_info.os_name);
    renderer.drawString({6, 1}, "</>{:>{}s}: <1>{:<{}s}", styles, II, "Version", II, os_info.version);
    renderer.drawString({7, 1}, "</>{:>{}s}: <1>{:<{}s}", styles, II, "Arch", II, os_info.machine);
    renderer.drawString({8, 1}, "</>{:>{}s}: <1>{:<{}s}", styles, II, "Memory", II, "Processing...");
    renderer.drawString({9, 1}, "</>{:>{}s}: <1>{:<{}s}", styles, II, "Disk", II, "Processing...");
}

void updateInfo(TUI::TermBuffer& renderer, OS::CPU& cpu, OS::Memory& memory, OS::DiskSpace& disk_space) {
    // std::lock_guard<std::mutex> lock_guard(mutex);
    const auto W = min(TUI::Terminal::screenSize().width, 60U);
    if (need_refresh.load()) {
        TUI::TermStyleList styles;
        TUI::TermStyle style_1, style_2, style_def;
        style_1.intensity = 2;
        style_1.colors.color.fg = TUI::Color::Yellow;
        style_2.intensity = 2;
        style_2.colors.color.fg = TUI::Color::Green;
        styles.push_back(style_def);
        styles.push_back(style_1);
        styles.push_back(style_2);
        renderer.drawString({8, 1}, "{:>20s}: <1>{:.1f}GB / {:.1f}GB ({:.2f} GB free)", styles, "Memory",
            OS::convertDataSize(memory.used_ram, OS::DataUnit::GiB), 
            OS::convertDataSize(memory.total_ram, OS::DataUnit::GiB), 
            OS::convertDataSize(memory.free_ram, OS::DataUnit::GiB)
        );
        renderer.drawString({9, 1}, "{:>20s}: <1>{:.1f}GB / {:.1f}GB ({:.1f}GB free)", styles, "Disk",
            OS::convertDataSize(disk_space.used_bytes, OS::DataUnit::GiB), 
            OS::convertDataSize(disk_space.total_bytes, OS::DataUnit::GiB), 
            OS::convertDataSize(disk_space.free_bytes, OS::DataUnit::GiB)
        );


        renderer.drawBorder({10, 0}, {13, W}, {}, {});
        renderer.drawBorder({13, 0}, {14 + cpu.cores, W}, {}, {});
        const auto FW = W - 13;
        renderer.drawString({11, 2}, "CPU Name : <1>{:<{}s}", styles, FW, cpu.name.substr(0, FW));
        renderer.drawString({12, 2}, "CPU Total: <1>{:5.1f}%  ", styles, cpu.total_usage);
        const auto M = W - 10;
        for (size_t i = 0; i < cpu.cores; i++) {
            auto repeat = static_cast<uint8_t>(static_cast<float>(M) * (cpu.usages[i] * 0.01f));
            std::string s;
            for (size_t j = 0; j < repeat; j++) {
                s += '|';
            }
            renderer.drawString({14 + static_cast<uint32_t>(i), 2}, "<1>CPU{:>2s}: <2>{:<{}s}", styles, i, M, s);
        }
        need_refresh.store(false);
    }
}

void killOne(int) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main() {
    OS::CPU cpu_info{};
    OS::Memory memory{};
    OS::DiskSpace disk_space{};
    need_refresh.store(false);
    Event e1(1, "e1", []{ return true; }, [&cpu_info, &memory, &disk_space](const std::atomic<bool>& ok) {
        std::lock_guard<std::mutex> lock_guard(mutex);
        OS::getCPUInfo(cpu_info, 1000);
        OS::getMemory(memory);
        OS::getDiskSpace(disk_space);
        need_refresh.store(true);
    });

    auto& ren = TUI::TermBuffer::self();
    term::setCursorVisible(false);
    // ren.setResizeEvent(drawInfo);
    TUI::TermBuffer::self().setResizeEvent([](TUI::TermBuffer& r) {
        r.fillScreen();
        drawInfo(r);
    });
    e1.setDelayMS(10);
    e1.setRepeatCount(0);
    e1.run();
    drawInfo(ren);
    auto start_ts = DT::currentTimestamps();
    while (true) {
        auto now_ts = DT::currentTimestamps();
        if (now_ts - start_ts > 1000) {
            updateInfo(ren, cpu_info, memory, disk_space);
            ren.present();
            start_ts = now_ts;
        }
        auto input = term::getInput();
        if (input.type == TUI::InputEvent::Keyboard && tolower(input.input.keyboard.key) == 'q') {
            break;
        }

        if (input.type == TUI::InputEvent::Keyboard && 
            (input.input.keyboard.key == TUI::KEY_CTRL_C ||
            input.input.keyboard.key == TUI::KEY_ESC)) break;
    }
    term::setCursorVisible(true);
    term::clearScreen();
    term::moveCursor(1, 1);
    term::print("Now Exiting...");
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