
#include "../src/Tiny.hpp"
#include <iostream>
using namespace Tiny;
using ter = TUI::Terminal;

static void updateInfo(const std::atomic<bool>&) {
    OS::HostInfo host_info = OS::hostInfo();
    ter::moveCursor(1, 1);
    ter::printFormat("{:12s} {}", "Host:", host_info.host_name);
    ter::moveCursor(2, 1);
    ter::printFormat("{:12s} {}", "User:", host_info.user_name);
    ter::moveCursor(3, 1);
    ter::printFormat("{:12s} {}", "OS:", OS::Name);
    ter::moveCursor(4, 1);
    ter::printFormat("{:12s} {}", "Machine:", host_info.machine);
    ter::moveCursor(5, 1);
    ter::printFormat("{:12s} {}", "Version:", host_info.version);
    ter::moveCursor(6, 1);
    ter::printFormat("{:12s} {}", "CPU Core(s):", host_info.cpu_cores);
    ter::moveCursor(7, 1);
    ter::printFormat("{:12s} {}", "Page size:", host_info.page_size);
    ter::moveCursor(8, 1);
    ter::printFormat("{:12s} {:.2f}GB / {:.2f}GB", "Memory:",
        host_info.used_ram / 1024.f / 1024.f / 1024.f,
        host_info.total_ram / 1024.f / 1024.f / 1024.f);
    ter::moveCursor(9, 1);
    ter::printFormat("{:12s} {:.1f}GB / {:.1f}GB", "Swap:",
        (host_info.total_swap - host_info.free_swap) / 1024.f / 1024.f / 1024.f,
        host_info.total_swap / 1024.f / 1024.f / 1024.f);
    ter::moveCursor(10, 1);
    ter::printFormat("{:12s} {:<4.2f}GB / {:<4.2f}GB ({:4.2f}GB Free)", "Disk:",
        (host_info.total_disk_space - host_info.free_disk_space) / 1024.f / 1024.f / 1024.f,
        host_info.total_disk_space / 1024.f / 1024.f / 1024.f,
        host_info.free_disk_space / 1024.f / 1024.f / 1024.f);
    ter::moveCursor(13, 0);
}

int main() {
    ter::clearScreen();
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
    Event ev(1, "Update", []{ return true; }, &updateInfo);
    ev.setDelayMS(1000);
    ev.setRepeatCount(10);
    ev.run();
    while (ev.isRunning()) {}
    ter::setCursorVisible(true);
    return 0;
}