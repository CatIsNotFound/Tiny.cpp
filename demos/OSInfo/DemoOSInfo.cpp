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
using term = TUI::Terminal;

std::mutex mutex{};
std::atomic<bool> need_refresh{};

void drawInfo(TUI::Renderer& renderer) {
    renderer.drawBorder({0, 0}, {2, 60}, {}, {});
    renderer.setStrF({1, 4}, "{:>18c}Host Information", ' ');

    TUI::Renderer::StyleList styles;
    TUI::Renderer::Style style_1, style_2;
    style_1.intensity = 2;
    style_1.fg_color = TUI::Color::Blue;
    style_2.intensity = 2;
    style_2.fg_color = TUI::Color::Yellow;
    styles.push_back(style_1);
    styles.push_back(style_2);

    auto os_info = OS::currentHostInfo();
    renderer.drawBorder({2, 0}, {10, 60}, {}, {});
    renderer.setSSFX({3, 1}, "<1>{:>20s}: <2>{:<20s}", styles, "Hostname", os_info.host_name);
    renderer.setSSFX({4, 1}, "<1>{:>20s}: <2>{:<20s}", styles, "Username", os_info.user_name);
    renderer.setSSFX({5, 1}, "<1>{:>20s}: <2>{:<20s}", styles, "OS", os_info.os_name);
    renderer.setSSFX({6, 1}, "<1>{:>20s}: <2>{:<20s}", styles, "Version", os_info.version);
    renderer.setSSFX({7, 1}, "<1>{:>20s}: <2>{:<20s}", styles, "Arch", os_info.machine);
    renderer.setSSFX({8, 1}, "<1>{:>20s}: <2>{:<32s}", styles, "Memory", "Processing...");
    renderer.setSSFX({9, 1}, "<1>{:>20s}: <2>{:<32s}", styles, "Disk", "Processing...");
}

void updateInfo(TUI::Renderer& renderer, OS::CPU& cpu, OS::Memory& memory, OS::DiskSpace& disk_space) {
    // std::lock_guard<std::mutex> lock_guard(mutex);
    if (need_refresh.load()) {
        TUI::Renderer::StyleList styles;
        TUI::Renderer::Style style_1, style_2, style_def;
        style_1.intensity = 2;
        style_1.fg_color = TUI::Color::Yellow;
        style_2.intensity = 2;
        style_2.fg_color = TUI::Color::Green;
        styles.push_back(style_def);
        styles.push_back(style_1);
        styles.push_back(style_2);
        renderer.setSSFX({8, 1}, "{:>20s}: <2>{:.1f}GB / {:.1f}GB ({:.2f} GB free)", styles, "Memory",
            OS::convertDataSize(memory.used_ram, OS::DataUnit::GiB), 
            OS::convertDataSize(memory.total_ram, OS::DataUnit::GiB), 
            OS::convertDataSize(memory.free_ram, OS::DataUnit::GiB)
        );
        renderer.setSSFX({9, 1}, "{:>20s}: <2>{:.1f}GB / {:.1f}GB ({:.1f}GB free)", styles, "Disk",
            OS::convertDataSize(disk_space.used_bytes, OS::DataUnit::GiB), 
            OS::convertDataSize(disk_space.total_bytes, OS::DataUnit::GiB), 
            OS::convertDataSize(disk_space.free_bytes, OS::DataUnit::GiB)
        );


        renderer.drawBorder({10, 0}, {12, 60}, {}, {});
        renderer.drawBorder({12, 0}, {13 + cpu.cores, 60}, {}, {});
        renderer.setStrF({11, 2}, "CPU Total: {:.1f}%  ", cpu.total_usage);
        for (size_t i = 0; i < cpu.cores; i++) {
            auto repeat = static_cast<uint8_t>(50.f * (cpu.usages[i] * 0.01f));
            std::string s;
            for (size_t j = 0; j < repeat; j++) {
                s += "|";
            }
            renderer.setSSFX({13 + static_cast<uint32_t>(i), 2}, "<2>CPU{:>2s}: <3>{:<50s}", styles, i, s);
        }
        need_refresh.store(false);
    }
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

    auto& ren = TUI::Renderer::self();
    term::setCursorVisible(false);
    ren.setResizeEvent(drawInfo);
    e1.setDelayMS(10);
    e1.setRepeatCount(0);
    e1.run();
    auto start_ts = DT::currentTimestamps();
    while (true) {
        updateInfo(ren, cpu_info, memory, disk_space);
        auto now_ts = DT::currentTimestamps();
        if (now_ts - start_ts > 1000) {
            ren.present();
            start_ts = now_ts;
        }
        auto input = term::getInput();
        if (input.type == TUI::InputEvent::Keyboard && tolower(input.input.keyboard.key) == 'q') {
            break;
        }
    }
    term::setCursorVisible(true);
    ren.clear();
    term::clearScreen();
    ren.setStrF({1, 1}, "Now Exiting...");
    ren.present();
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