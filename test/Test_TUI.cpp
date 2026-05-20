
#include "../src/Tiny.hpp"
#include <iostream>
using namespace Tiny;
using ter = TUI::Terminal;
char ch = '\0';
static void updateInfo(const std::atomic<bool>&) {
    OS::HostInfo host_info = OS::currentHostInfo();
    static OS::Memory memory{};
    OS::getMemory(memory);
    static OS::DiskSpace disk_space{};
    OS::getDiskSpace(disk_space);
    static OS::CPU cpu_info{};
    OS::getCPUInfo(cpu_info);
    ter::moveCursor(1, 1);
    ter::printFormat("{:12s} {}", "Host:", host_info.host_name);
    ter::moveCursor(2, 1);
    ter::printFormat("{:12s} {}", "User:", host_info.user_name);
    ter::moveCursor(3, 1);
    ter::printFormat("{:12s} {}", "OS:", OS::Name);
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
    ter::printFormat("{:12s} {:.2f}GB / {:.2f}GB ({:>6.2f}%)", "Memory:",
        used, total, (used / total) * 100.f);
    ter::moveCursor(10, 1);
    ter::printFormat("{:12s} {:.1f}GB / {:.1f}GB", "Swap:",
        (memory.total_swap - memory.free_swap) / 1024.f / 1024.f / 1024.f,
        memory.total_swap / 1024.f / 1024.f / 1024.f);
    ter::moveCursor(11, 1);
    ter::printFormat("{:12s} {:<4.2f}GB / {:<4.2f}GB ({:4.2f}GB Free)", "Disk:",
        (disk_space.used_disk_space) / 1024.f / 1024.f / 1024.f,
        disk_space.total_disk_space / 1024.f / 1024.f / 1024.f,
        disk_space.free_disk_space / 1024.f / 1024.f / 1024.f);
    ter::moveCursor(14, 0);
    ter::printLine("+---- CPU Usage Info ----+");
    for (size_t i = 0; i < cpu_info.cores; i++) {
        ter::printFormat("|    CPU {:>3s}: {:>6.2f}%    |\n", i, cpu_info.usages[i]);
    }
    ter::printFormat("+{:24c}+\n", '-');
    ter::printLine("Please press any key to exit...");
}

int main() {
    ter::enterRawMode();
    ter::setCursorVisible(false);
    ter::printFormat("+{:48c}+\n", '-');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("|{:48c}|\n", ' ');
    ter::printFormat("+{:48c}+\n", '-');
    Event ev(1, "Update", &updateInfo);
    ev.setDelayMS(1000);
    ev.setRepeatCount(0);
    ev.run();
    while (getchar() != '\n') {}
    ev.stop();
    ter::setCursorVisible(true);
    ter::leaveRawMode();
    getchar();
    return 0;
}