
#include "../src/Tiny.hpp"
#include <iostream>
using namespace Tiny;
using ter = TUI::Terminal;
char ch = '\0';
static void updateInfo(const std::atomic<bool>& running) {
    OS::HostInfo host_info = OS::currentHostInfo();
    static OS::Memory memory{};
    OS::getMemory(memory);
    static OS::DiskSpace disk_space{};
    OS::getDiskSpace(disk_space);
    static OS::CPU cpu_info{};
    OS::getCPUInfo(cpu_info, 1000);
    if (!running) return;
    ter::moveCursor(1, 1);
    ter::printFormat("{:12s} {}", "Host:", host_info.host_name);
    ter::moveCursor(2, 1);
    ter::printFormat("{:12s} {}", "User:", host_info.user_name);
    ter::moveCursor(3, 1);
    ter::printFormat("{:12s} {}", "OS:", host_info.os_name);
    ter::moveCursor(4, 1);
    ter::printFormat("{:12s} {}", "Machine:", OS::getCPUArchName(cpu_info.machine));
    ter::moveCursor(5, 1);
    ter::printFormat("{:12s} {}", "Version:", host_info.version);
    ter::moveCursor(6, 1);
    ter::printFormat("{:12s} {}", "CPU Core(s):", cpu_info.cores);
    ter::moveCursor(7, 1);
    ter::printFormat("{:12s} {}", "Page size:", cpu_info.page_size);
    ter::moveCursor(8, 1);
    ter::printFormat("{:12s} {:.2f}%{:4c}", "CPU Usage:", cpu_info.total_usage, ' ');
    ter::moveCursor(9, 1);
    double used = memory.used_ram / 1024.f / 1024.f / 1024.f;
    double total = memory.total_ram / 1024.f / 1024.f / 1024.f;
    double avail = memory.available_ram / 1024.f / 1024.f / 1024.f;
    ter::printFormat("{:12s} {:.2f}GB / {:.2f}GB ({:.2f}GB Available)", "Memory:",
        used, total, avail);
    ter::moveCursor(10, 1);
    ter::printFormat("{:12s} {:.2f}GB / {:.2f}GB", "Swap:",
        (memory.total_swap - memory.free_swap) / 1024.f / 1024.f / 1024.f,
        memory.total_swap / 1024.f / 1024.f / 1024.f);
    ter::moveCursor(11, 1);
    ter::printFormat("{:12s} {:<4.2f}GB / {:<4.2f}GB ({:4.2f}GB Available)", "Disk:",
        (disk_space.used_bytes) / 1024.f / 1024.f / 1024.f,
        disk_space.total_bytes / 1024.f / 1024.f / 1024.f,
        disk_space.available_bytes / 1024.f / 1024.f / 1024.f);
    ter::moveCursor(14, 0);
    ter::printLine("+---- CPU Usage Info ----+");
    for (size_t i = 0; i < cpu_info.cores; i++) {
        ter::printFormat("|    CPU {:>3s}: {:>6.2f}%    |\r\n", i, cpu_info.usages[i]);
    }
    if (cpu_info.cores == 0) ter::printFormat("|{:24c}|\r\n", ' ');
    ter::printFormat("+{:24c}+\r\n", '-');
#ifdef __APPLE__
    ter::printFormat("[Only Apple can view memory!!]\r\n");
    ter::printFormat("{:>16s}: {:.2f}GB\r\n", "active", memory.app_active_mem  / 1024.f / 1024.f / 1024.f);
    ter::printFormat("{:>16s}: {:.2f}GB\r\n", "inactive", memory.app_inactive_mem  / 1024.f / 1024.f / 1024.f);
    ter::printFormat("{:>16s}: {:.2f}GB\r\n", "free", memory.app_free_mem  / 1024.f / 1024.f / 1024.f);
    ter::printFormat("{:>16s}: {:.2f}GB\r\n", "wired", memory.app_wired_mem  / 1024.f / 1024.f / 1024.f);
    ter::printFormat("{:>16s}: {:.2f}GB\r\n", "compress", memory.app_compress_mem  / 1024.f / 1024.f / 1024.f);
    ter::printFormat("{:>16s}: {:.2f}GB\r\n", "speculative", memory.app_speculative_mem  / 1024.f / 1024.f / 1024.f);
#endif
    ter::printLine("Please press 'Enter' key to exit...");
}

int main() {
    ter::enterRawMode();
    // ter::clearScreen();
    ter::setCursorVisible(false);
    ter::printFormat("+{:54c}+\r\n", '-');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("|{:54c}|\r\n", ' ');
    ter::printFormat("+{:54c}+\r\n", '-');
    auto start = std::chrono::system_clock::now().time_since_epoch().count();
    Event ev(1, "Update", &updateInfo);
    ev.setDelayMS(0);
    ev.setRepeatCount(0);
    ev.run();
    char c;
    while (ev.isRunning()) {
        c = getchar();
        if (c == '\n' || c == '\r') {
            ev.stop();
        }
    }
    ter::setCursorVisible(true);
    ter::leaveRawMode();
    auto end = std::chrono::system_clock::now().time_since_epoch().count();
    ter::printFormat("Running at {} secs.", end - start);
    auto cur_pos = ter::cursorPosition();
    ter::printFormat("\r\nP({}, {})\r\n", cur_pos.row, cur_pos.column);
    getchar();
    return 0;
}