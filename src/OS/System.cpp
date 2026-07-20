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

#include "System.hpp"

#include <thread>

#ifdef TINY_CPP_MY_OS_WINDOWS
#include <windows.h>
#include <fileapi.h>
#include <shlobj.h>
#include <pdh.h>
#include <sddl.h>
#pragma comment(lib, "pdh.lib")
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>
#include <pwd.h>
#include <cstring>
#ifdef __APPLE__
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/host_info.h>
#include <mach/mach_host.h>
#include <mach/processor_info.h>
#else
#include <sys/sysinfo.h>
#endif
#endif

#ifdef TINY_CPP_MY_OS_WINDOWS
namespace {
    std::string convert2Win(const std::string& path) {
        std::string _new_path;
        for (auto c : path) {
            _new_path.push_back(c == '/' ? '\\' : c);
        }
        return _new_path;
    }

    std::wstring string2Wide(const std::string& str, uint32_t codepage = 65001) {
        auto len = MultiByteToWideChar(codepage, 0, str.data(),
        str.size(), nullptr, 0);
        std::wstring w_str(len, 0);
        MultiByteToWideChar(codepage, 0, str.data(), str.size(),
            &w_str[0], len);
        return w_str;
    }

    std::string wide2String(const std::wstring& w_str, uint32_t codepage = 65001) {
        auto len = WideCharToMultiByte(codepage, 0, w_str.data(), w_str.size(),
            nullptr, 0, nullptr, nullptr);
        std::string str(len, 0);
        WideCharToMultiByte(codepage, 0, w_str.data(), w_str.size(),
            &str[0], len, nullptr, nullptr);
        return str;
    }
}
#endif

#ifdef TINY_CPP_MY_OS_UNIX
#ifdef __APPLE__
struct TickStat {
    uint64_t total, idle;
};

static std::vector<TickStat> calcTickStat() {
    std::vector<TickStat> out;
    natural_t cpu_count = 0;
    processor_cpu_load_info_t cpu_load = nullptr;
    mach_msg_type_number_t count = 0;

    kern_return_t kr = host_processor_info(
        mach_host_self(),
        PROCESSOR_CPU_LOAD_INFO,
        &cpu_count,
        (processor_info_array_t*)&cpu_load,
        &count
    );

    if (kr != KERN_SUCCESS) return out;

    for (natural_t i = 0; i < cpu_count; ++i) {
        uint64_t total = 0, idle = 0;
        for (int j = 0; j < CPU_STATE_MAX; j++) {
            total += cpu_load[i].cpu_ticks[j];
        }
        idle = cpu_load[i].cpu_ticks[CPU_STATE_IDLE];
        out.push_back({total, idle});
    }

    vm_deallocate(mach_task_self(), (vm_address_t)cpu_load, count * sizeof(natural_t));
    return out;
}
#else
struct CPU_Stat {
    size_t user, nice, system, idle, iowait, irq, softirq;

    size_t total() {
        return user + nice + system + idle + iowait + irq + softirq;
    }

    size_t busy() {
        return total() - idle;
    }
};
#endif
#endif

namespace Tiny {
    const char *OS::getCPUArchName(CPU_Arch cpu_arch) {
        switch (cpu_arch) {
            case CPU_Arch::X86:
                return "x86";
            case CPU_Arch::X86_64:
                return "x86_64";
            case CPU_Arch::ARM32:
                return "arm32";
            case CPU_Arch::ARM64:
                return "arm64";
            case CPU_Arch::LoongArch:
                return "loongArch";
            case CPU_Arch::MIPS:
                return "mips";
            case CPU_Arch::RISCV:
                return "risc-v";
            case CPU_Arch::IA64:
                return "ia64";
            default:
                return "Unknown";
        }
    }

    OS::HostInfo OS::currentHostInfo() {
        HostInfo host_info;
        getHostInfo(host_info);
        return host_info;
    }

    OS::CPU OS::currentCPUInfo() {
        CPU cpu_info;
        getCPUInfo(cpu_info);
        return cpu_info;
    }

    OS::Memory OS::currentMemory() {
        Memory memory;
        getMemory(memory);
        return memory;
    }

    OS::DiskSpace OS::currentDiskSpace() {
        DiskSpace disk_space;
        getDiskSpace(disk_space);
        return disk_space;
    }

    bool OS::getHostInfo(HostInfo &info) {
        bool ret = true;
#ifdef TINY_CPP_MY_OS_WINDOWS
        /// Host name
        info.host_name.resize(MAX_COMPUTERNAME_LENGTH + 1);
        DWORD size = info.host_name.size();
        if (!GetComputerNameA(&info.host_name[0], &size)) ret = false;
        /// User name
        info.user_name.resize(256);
        if (!GetUserNameA(&info.user_name[0], &size)) ret = false;
        info.user_name.resize(size);
        /// System Version
        HKEY hKey;
        char dis_product[128] = {};
        char dis_version[32] = {};
        char dis_cur_ver[32] = {};
        char dis_cur_build[32] = {};
        DWORD dis_product_len = 128;
        DWORD dis_ver_len = 32;
        DWORD dis_cur_ver_len = 32;
        DWORD dis_cur_build_len = 32;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegQueryValueExA(hKey, "ProductName", nullptr, nullptr, (LPBYTE)&dis_product, &dis_product_len);
            if (RegQueryValueExA(hKey, "WinREVersion", nullptr, nullptr, (LPBYTE)&dis_version, &dis_ver_len) != ERROR_SUCCESS) {
                RegQueryValueExA(hKey, "CurrentVersion", nullptr, nullptr, (LPBYTE)&dis_cur_ver, &dis_cur_ver_len);
                RegQueryValueExA(hKey, "CurrentBuild", nullptr, nullptr, (LPBYTE)&dis_cur_build, &dis_cur_build_len);
                sprintf(dis_version, "%s.%s", dis_cur_ver, dis_cur_build);
            }
            RegCloseKey(hKey);
            info.version = dis_version;
            info.os_name = dis_product;
        } else ret = false;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char machine_arch[32] = {};
            DWORD machine_arch_len = 32;
            RegQueryValueExA(hKey, "PROCESSOR_ARCHITECTURE", nullptr, nullptr, (LPBYTE)&machine_arch, &machine_arch_len);
            RegCloseKey(hKey);
            info.machine = machine_arch;
        } else ret = false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        struct utsname uts_name;
        uname(&uts_name);
        info.host_name = uts_name.nodename;
        info.user_name = getpwuid(getuid())->pw_name;
        info.machine = uts_name.machine;
        info.version = uts_name.release;
        // Get OS Name
#ifdef __APPLE__
        info.os_name = "macOS";
        char new_version[128] = {};
        size_t len = sizeof(new_version);
        sysctlbyname("kern.osproductversion", new_version, &len, nullptr, 0);
        info.version = new_version;
#else
        File file("/etc/os-release", OpenMode::ReadOnly);
        if (file.isOpen()) {
            while (true) {
                auto buf = file.readLine();
                if (buf.find("PRETTY_NAME=") != std::string::npos) {
                    info.os_name = buf.substr(13);
                    info.os_name.pop_back();
                    info.os_name.pop_back();
                    break;
                }
                if (buf.empty()) break;
            }
            file.close();
        } else {
            ret = false;
        }
#endif
#endif
        return ret;
    }

    bool OS::getCPUInfo(CPU &info, size_t internal) {
        bool ret = true;
#ifdef TINY_CPP_MY_OS_WINDOWS
#ifndef PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64
#define PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64 13
#endif
#ifndef PROCESSOR_ARCHITECTURE_ARM64
#define PROCESSOR_ARCHITECTURE_ARM64 12
#endif
#ifndef PROCESSOR_ARCHITECTURE_IA32_ON_ARM64
#define PROCESSOR_ARCHITECTURE_IA32_ON_ARM64 14
#endif
        SYSTEM_INFO sys_info;
        GetNativeSystemInfo(&sys_info);
        switch (sys_info.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_INTEL:
                info.machine = CPU_Arch::X86;
                break;
            case PROCESSOR_ARCHITECTURE_AMD64:
                info.machine = CPU_Arch::X86_64;
                break;
            case PROCESSOR_ARCHITECTURE_ARM:
            case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64:
                info.machine = CPU_Arch::ARM32;
                break;
            case PROCESSOR_ARCHITECTURE_ARM64:
                info.machine = CPU_Arch::ARM64;
                break;
            case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64:
            case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
            case PROCESSOR_ARCHITECTURE_IA64:
                info.machine = CPU_Arch::IA64;
                break;
            default:
                info.machine = CPU_Arch::Unknown;
        }
        /// CPU Cores
        info.cores = sys_info.dwNumberOfProcessors;
        info.page_size = sys_info.dwPageSize;

        /// CPU Usage
        static PDH_HQUERY query{};
        static std::vector<PDH_HCOUNTER> counter(info.cores, nullptr);

        // Init counter
        if (!query) {
            if (PdhOpenQueryA(nullptr, 0, &query) == ERROR_SUCCESS) {
                info.usages.resize(info.cores);
                for (uint32_t i = 0; i < info.cores; i++) {
                    wchar_t path[MAX_PATH];
                    swprintf(path, L"\\Processor Information(0,%u)\\%% Processor Time", i);
                    if (PdhAddCounterW(query, path, 0, &counter[i]) != ERROR_SUCCESS) ret = false;
                }
            } else ret = false;
        }
        // Get per core usage
        info.usages.resize(info.cores);
        PdhCollectQueryData(query);
        std::this_thread::sleep_for(std::chrono::milliseconds(internal));
        PdhCollectQueryData(query);
        for (size_t i = 0; i < info.cores; i++) {
            PDH_FMT_COUNTERVALUE value;
            auto err_code = PdhGetFormattedCounterValue(counter[i], PDH_FMT_DOUBLE,
                                           nullptr, &value);
            if (err_code == ERROR_SUCCESS) {
                info.usages[i] = static_cast<float>(value.doubleValue);
                info.total_usage += info.usages[i];
            } else {
                ret = false;
            }
        }
        if (info.cores > 0) info.total_usage /= info.cores;

#elif defined(TINY_CPP_MY_OS_UNIX)
        // CPU machine
        struct utsname uts_name;
        uname(&uts_name);
        std::string machine = uts_name.machine;
        if (machine == "x86_64") info.machine = CPU_Arch::X86_64;
        else if (machine == "arm64" || machine == "aarch64") info.machine = CPU_Arch::ARM64;
        else if (machine.find_first_of("armv") != std::string::npos) info.machine = CPU_Arch::ARM32;
        else if (machine == "i386" || machine == "i686") info.machine = CPU_Arch::X86;
        else if (machine.find_first_of("loongarch") != std::string::npos) info.machine = CPU_Arch::LoongArch;
        else if (machine.find_first_of("mips") != std::string::npos) info.machine = CPU_Arch::MIPS;
        else if (machine.find_first_of("riscv") != std::string::npos) info.machine = CPU_Arch::RISCV;
        else if (machine == "ia64") info.machine = CPU_Arch::IA64;

        // CPU info
#ifdef __APPLE__
        size_t length = sizeof(info.cores);
        if (sysctlbyname("hw.ncpu", &info.cores, &length, nullptr, 0) == -1) info.cores = 1;
        info.usages.resize(info.cores);

        auto prev = calcTickStat();
        std::this_thread::sleep_for(std::chrono::milliseconds(internal));
        auto next = calcTickStat();

        // Calc the CPU usage
        float total_usage = 0;
        for (size_t i = 0; i < info.cores; ++i) {
            auto d_total = (next[i].total - prev[i].total);
            auto d_idle = (next[i].idle - prev[i].idle);
            if (d_total != 0) {
                info.usages[i] = 100.f - (100.f * static_cast<double>(d_idle) / static_cast<double>(d_total));
            }
            total_usage += info.usages[i];
        }
        info.total_usage = total_usage / static_cast<float>(info.cores);
#elif defined(__linux__)
        info.cores = get_nprocs();
        info.usages.resize(info.cores);
        std::function<std::vector<CPU_Stat>()> getCPUStat = [&info, &ret] {
            std::vector<CPU_Stat> output_list;
            File cpu_stat("/proc/stat");
            if (cpu_stat.open(OpenMode::ReadOnly)) {
                std::string line;
                size_t i = 0;
                do {
                    line = cpu_stat.readLine();
                    auto pos = line.find_first_of("cpu"); 
                    if (pos != std::string::npos) {
                        std::istringstream iss(line);
                        std::string cpu_name;
                        CPU_Stat stat;
                        iss >> cpu_name >> stat.user >> stat.nice >> stat.system 
                            >> stat.idle >> stat.iowait >> stat.irq >> stat.softirq;
                        if (i <= info.cores) {
                            output_list.push_back(stat);    
                        } else {
                            break;
                        }
                        i += 1;
                    } else {
                        break;
                    }
                } while (true);
                cpu_stat.close();
            } else {
                ret = false;
            }
            return output_list;
        };
        auto prev = getCPUStat();
        std::this_thread::sleep_for(std::chrono::milliseconds(internal));
        auto next = getCPUStat();
        for (size_t i = 0; i < info.cores + 1; ++i) {
            auto d_total = next[i].total() - prev[i].total();
            auto d_busy = next[i].busy() - prev[i].busy();
            if (i == 0) {
                info.total_usage = static_cast<double>(d_busy) / static_cast<double>(d_total) * 100.f; 
            } else {
                info.usages[i - 1] = static_cast<double>(d_busy) / static_cast<double>(d_total) * 100.f; 
            }
        }
#endif
        info.page_size = sysconf(_SC_PAGE_SIZE);
#endif
        return ret;
    }

    bool OS::getMemory(Memory &memory) {
        bool ret = true;
#ifdef TINY_CPP_MY_OS_WINDOWS
        /// Memory RAM
        MEMORYSTATUSEX mem_info_ex{};
        mem_info_ex.dwLength = sizeof(mem_info_ex);
        if (!GlobalMemoryStatusEx(&mem_info_ex)) ret = false;
        memory.total_ram = mem_info_ex.ullTotalPhys;
        memory.free_ram = mem_info_ex.ullAvailPhys;
        memory.available_ram = memory.free_ram;
        memory.used_ram = memory.total_ram - memory.free_ram;
        memory.total_swap = mem_info_ex.ullTotalPageFile;
        memory.free_swap = mem_info_ex.ullAvailPageFile;
#elif defined(TINY_CPP_MY_OS_UNIX)
#ifdef __APPLE__
        // TODO: 
        vm_size_t page_size = 0;
        host_page_size(mach_host_self(), &page_size);
        memory.total_ram = page_size * sysconf(_SC_PHYS_PAGES);
        vm_statistics64 vm_stat;
        mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
        kern_return_t kr = host_statistics64(
            mach_host_self(),
            HOST_VM_INFO64,
            (host_info64_t)&vm_stat,
            &count
        );
        if (kr == KERN_SUCCESS) {
            uint64_t free_count = vm_stat.free_count * page_size;
            uint64_t active_count = vm_stat.active_count * page_size;
            uint64_t inactive_count = vm_stat.inactive_count * page_size;
            uint64_t wired_count = vm_stat.wire_count * page_size;
            uint64_t compress_count = vm_stat.compressor_page_count * page_size;
            uint64_t speculative_count = vm_stat.speculative_count * page_size;
            uint64_t used_count = active_count + wired_count + compress_count;
            uint64_t avail_count = inactive_count + free_count + speculative_count;
            // Only for Apple
            memory.app_free_mem = free_count;
            memory.app_active_mem = active_count;
            memory.app_inactive_mem = inactive_count;
            memory.app_wired_mem = wired_count;
            memory.app_compress_mem = compress_count;
            memory.app_speculative_mem = speculative_count;
            memory.free_ram = free_count;
            memory.used_ram = used_count;
            memory.available_ram = avail_count;
        } else {
            ret = false;

        }

        struct xsw_usage xsw{};
        size_t len = sizeof(xsw);
        sysctlbyname("vm.swapusage", &xsw, &len, nullptr, 0);
        memory.total_swap = xsw.xsu_total; 
        memory.free_swap = xsw.xsu_avail;  

#else
        File mem_info("/proc/meminfo", OpenMode::ReadOnly);
        if (mem_info.isOpen()) {
            size_t cnt = 0;
            while (true) {
                auto line = mem_info.readLine();
                bool is_valid = false;
                is_valid = (line.find("MemTotal") != std::string::npos) || 
                            (line.find("MemFree") != std::string::npos) ||
                            (line.find("MemAvailable") != std::string::npos) ||
                            (line.find("SwapTotal") != std::string::npos) ||
                            (line.find("SwapFree") != std::string::npos);
                if (!is_valid) continue;
                cnt += 1;
                std::istringstream iss(line);
                std::string temp;
                size_t kb = 0;
                iss >> temp >> kb;
                if (temp.find("MemTotal") != std::string::npos) {
                    memory.total_ram = kb * 1024;
                } else if (temp.find("MemFree") != std::string::npos) {
                    memory.free_ram = kb * 1024;
                } else if (temp.find("MemAvailable") != std::string::npos) {
                    memory.available_ram = kb * 1024;
                } else if (temp.find("SwapTotal") != std::string::npos) {
                    memory.total_swap = kb * 1024;
                } else if (temp.find("SwapFree") != std::string::npos) {
                    memory.free_swap = kb * 1024;
                }
                if (cnt >= 5 || line.empty()) break;
            }
            memory.used_ram = memory.total_ram - memory.available_ram;
            mem_info.close();
        } else {
            ret = false;
        }
#endif
#endif
        return ret;
    }

    bool OS::getDiskSpace(DiskSpace &disk_space) {
        bool ret = true;
#ifdef TINY_CPP_MY_OS_WINDOWS
        char buf[1024] = {};
        if (GetLogicalDriveStringsA(1024, buf) == 0) ret = false;
        disk_space.total_bytes = 0;
        disk_space.free_bytes = 0;
        disk_space.used_bytes = 0;
        char* drive = buf;
        for (; *drive ; drive += strlen(drive) + 1) {
            ULARGE_INTEGER total{}, free{};
            auto ok = GetDiskFreeSpaceEx(buf, nullptr, &total, &free);
            if (ok == TRUE) {
                disk_space.total_bytes += total.QuadPart;
                disk_space.free_bytes += free.QuadPart;
                disk_space.used_bytes += total.QuadPart - free.QuadPart;
            }
        }
        disk_space.available_bytes = disk_space.free_bytes;
#elif defined(TINY_CPP_MY_OS_UNIX)
#ifdef __android__
        const char* root = "/data";
#else
        const char* root = "/";
#endif
        struct statvfs st{};
        if (statvfs(root, &st) == -1) {
            ret = false;
        } else {
            auto block_size = st.f_frsize;
            disk_space.total_bytes = st.f_blocks * block_size;
            disk_space.free_bytes = st.f_bfree * block_size;
            disk_space.available_bytes = st.f_bavail * block_size;
            disk_space.used_bytes = disk_space.total_bytes - disk_space.free_bytes;
        }
#endif
        return ret;
    }

    OS::CPU_Arch OS::getCurrentCPUArch() {
#if defined(__x86_64__) || defined(_M_X64)
        return CPU_Arch::X86_64;
#elif defined(__i386__) || defined(_M_IX86)
        return CPU_Arch::X86;
#elif defined(__aarch64__) || defined(_M_ARM64)
        return CPU_Arch::ARM64;
#elif defined(__arm__) || defined(_M_ARM)
        return CPU_Arch::ARM;
#elif defined(__loongarch__)
        return CPU_Arch::LOONGARCH;
#elif defined(__mips__)
        return CPU_Arch::MIPS;
#elif defined(__riscv)
        return CPU_Arch::RISCV;
#elif defined(__ia64__)
        return CPU_Arch::IA64;
#else
        return CPU_Arch::Unknown;
#endif
    }

    void OS::lastSystemError(std::string& info, int* err_code) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto err = GetLastError();
        if (err_code) *err_code = err;
        if (err == 0) {
            info.clear();
            return;
        }
        SetLastError(0);
        void* msg_buf = nullptr;
        auto out_size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            err,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
            (LPSTR)&msg_buf,
            0,
            nullptr);

        if (out_size > 0) {
            info = static_cast<char*>(msg_buf);
            info.pop_back();
            info.pop_back();
            if (msg_buf) LocalFree(msg_buf);
            return;
        }
        if (msg_buf) LocalFree(msg_buf);
        char out[32]{};
        snprintf(out, 32, "Unknown error! code: 0x%x", err);
        out[31] = '\0';
        info = out;
#else
        int err = errno;
        if (err_code) *err_code = err;
        if (err == 0) {
            info.clear();
            return;
        }
        auto err_text = strerror(err);
        if (err_text == nullptr) {
            std::ostringstream oss;
            oss << "Unknown error! code: 0x" << std::hex << err;
            info = oss.str();
            return;
        }
        info = err_text;
#endif
    }

    bool OS::isAdmin() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        BOOL isElevated = FALSE;
        PSID adminSid = nullptr;

        if (!ConvertStringSidToSidW(L"S-1-5-32-544", &adminSid))
            return false;

        CheckTokenMembership(NULL, adminSid, &isElevated);
        LocalFree(adminSid);

        return isElevated != FALSE;
#else
        return geteuid() == 0;
#endif
    }

    bool OS::FileSystem::chDir(const Path &path) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (!path.isValid()) return false;
        auto ok = SetCurrentDirectoryA(path.path().data());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (chdir(path.path().c_str()) == -1) return false;
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::chDir(const std::string &path) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = SetCurrentDirectoryA(path.data());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (chdir(path.c_str()) == -1) return false;
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::rmFile(const Path &path) {
        if (!Path::isFile(path.path())) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = DeleteFileA(path.path().data());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (unlink(path.path().c_str()) == -1) return false;
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::rmDir(const Path &path, bool recursion) {
        if (!path.isValid() || !path.isDirectory()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (recursion) {
            if (!rmDirCompletely(path)) return false;
        }
        auto ok = RemoveDirectoryA(path.path().data());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (recursion) {
            if (!rmDirCompletely(path)) return false;
        }
        if (rmdir(path.path().c_str()) == -1) return false;
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mkDir(const Path &path) {
        if (path.isValid()) {
            return path.isDirectory();
        }
        auto create_dir = mkDirCompletely(path);
#ifdef TINY_CPP_MY_OS_WINDOWS
        while (!create_dir.empty()) {
            auto ok = CreateDirectoryA(create_dir.front().c_str(), nullptr);
            if (ok == 0) return false;
            create_dir.pop_front();
        }
#elif defined(TINY_CPP_MY_OS_UNIX)
        while (!create_dir.empty()) {
            if (mkdir(create_dir.front().c_str(), 0777 - umask(0000)) == -1) return false;
            create_dir.pop_front();
        }
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mkDir(const std::string &path) {
        Path my_path(path);
        if (my_path.isValid()) {
            return my_path.isDirectory();
        }
        auto create_dir = mkDirCompletely(my_path);
#ifdef TINY_CPP_MY_OS_WINDOWS
        while (!create_dir.empty()) {
            auto ok = CreateDirectoryA(create_dir.front().c_str(), nullptr);
            if (ok == 0) return false;
            create_dir.pop_front();
        }
#elif defined(TINY_CPP_MY_OS_UNIX)
        while (!create_dir.empty()) {
            if (mkdir(create_dir.front().c_str(), 0777 - umask(0000)) == -1) return false;
            create_dir.pop_front();
        }
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mkFile(const Path &path, const std::vector<uint8_t> &data) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto handler = CreateFileA(path.path().data(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handler == INVALID_HANDLE_VALUE) return false;
        auto ok = WriteFile(handler, data.data(), data.size(), nullptr, nullptr);
        CloseHandle(handler);
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = open(path.path().c_str(), O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0777 - umask(0000));
        if (ok == -1) return false;
        auto size = write(ok, data.data(), data.size() * sizeof(uint8_t));
        if (size == -1) return false;
        close(ok);
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mkFile(const std::string &path, const std::vector<uint8_t> &data) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto handler = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handler == INVALID_HANDLE_VALUE) return false;
        auto ok = WriteFile(handler, data.data(), data.size(), nullptr, nullptr);
        CloseHandle(handler);
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0777 - umask(0000));
        if (ok == -1) return false;
        auto size = write(ok, data.data(), data.size() * sizeof(uint8_t));
        if (size == -1) return false;
        close(ok);
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mkFile(const Path &path, const std::string &data) {
        #ifdef TINY_CPP_MY_OS_WINDOWS
        auto handler = CreateFileA(path.path().data(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handler == INVALID_HANDLE_VALUE) return false;
        auto ok = WriteFile(handler, data.data(), data.size(), nullptr, nullptr);
        CloseHandle(handler);
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = open(path.path().c_str(), O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0777 - umask(0000));
        if (ok == -1) return false;
        auto size = write(ok, data.data(), data.size() * sizeof(uint8_t));
        if (size == -1) return false;
        close(ok);
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mkFile(const std::string &path, const std::string &data) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto handler = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handler == INVALID_HANDLE_VALUE) return false;
        auto ok = WriteFile(handler, data.data(), data.size(), nullptr, nullptr);
        CloseHandle(handler);
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0777 - umask(0000));
        if (ok == -1) return false;
        auto size = write(ok, data.data(), data.size() * sizeof(char));
        if (size == -1) return false;
        close(ok);
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mkLink(const std::string &path, const std::string &link_dest) {
        Path dest(link_dest);
        if (!dest.isValid()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = CreateSymbolicLinkA(path.c_str(),
            link_dest.c_str(), dest.isDirectory());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (!access(link_dest.c_str(), F_OK)) {
            unlink(link_dest.c_str());
        }
        auto ok = symlink(link_dest.c_str(), path.c_str());
        if (ok == -1) return false;
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mkLink(const std::string &path, const Path &link_dest) {
        if (!link_dest.isValid()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = CreateSymbolicLinkA(path.c_str(),
            link_dest.path().c_str(), link_dest.isDirectory());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (!access(link_dest.path().c_str(), F_OK)) {
            unlink(link_dest.path().c_str());
        }
        auto ok = symlink(link_dest.path().c_str(), path.c_str());
        if (ok == -1) return false;
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::cpFile(const Path &src, const Path &dest) {
        if (!src.isValid() || src.isDirectory()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = CopyFileA(src.path().c_str(), dest.path().c_str(), false);
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        int src_fd = open(src.path().c_str(), O_RDONLY);
        if (src_fd == -1) return false;
        int dest_fd = open(dest.path().c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777 - umask(0000));
        if (dest_fd == -1) {
            close(src_fd);
            return false;
        }
        char buffer[4096];
        ssize_t bytes_read;
        while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
            ssize_t bytes_written = write(dest_fd, buffer, bytes_read);
            if (bytes_written == -1) {
                close(src_fd);
                close(dest_fd);
                return false;
            }
        }
        close(src_fd);
        close(dest_fd);
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::cpFile(const Path &src, const std::string &dest) {
        if (!src.isValid() || src.isDirectory()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = CopyFileA(src.path().c_str(), dest.c_str(), false);
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        int src_fd = open(src.path().c_str(), O_RDONLY);
        if (src_fd == -1) return false;
        int dest_fd = open(dest.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777 - umask(0000));
        if (dest_fd == -1) {
            close(src_fd);
            return false;
        }
        char buffer[4096];
        ssize_t bytes_read;
        while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
            ssize_t bytes_written = write(dest_fd, buffer, bytes_read);
            if (bytes_written == -1) {
                close(src_fd);
                close(dest_fd);
                return false;
            }
        }
        close(src_fd);
        close(dest_fd);
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::cpDir(const Path &src, const Path &dest) {
        if (!src.isValid() || !src.isDirectory()) return false;
        auto file_list = listAllPath(src, 1, 0);
        auto parent_dir = src.parentDirectory();
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = CreateDirectoryA(dest.path().c_str(), nullptr);
        if (ok == 0) return false;
        for (auto& file : file_list) {
            if (file.isDirectory()) {
                bool is_ok = cpDir(file, dest.path() + "/" + file.shortFileName());
                if (!is_ok) return false;
            } else {
                auto new_file_path = dest.path() + "/" + file.shortFileName();
                ok = CopyFileA(file.path().c_str(), new_file_path.c_str(), false);
                if (!ok) return false;
            }
        }
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = mkdir(dest.path().c_str(), 0777 - umask(0000));
        if (ok == -1) return false;
        for (auto& file : file_list) {
            if (file.isDirectory()) {
                bool is_ok = cpDir(file, dest.path() + "/" + file.shortFileName());
                if (!is_ok) return false;
            } else {
                auto new_file_path = dest.path() + "/" + file.shortFileName();
                ok = cpFile(file, new_file_path);
                if (!ok) return false;
            }
        }
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::cpDir(const Path &src, const std::string &dest) {
        if (!src.isValid() || !src.isDirectory()) return false;
        auto file_list = listAllPath(src, 1, 0);
        auto parent_dir = src.parentDirectory();
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = CreateDirectoryA(dest.c_str(), nullptr);
        if (ok == 0) return false;
        for (auto& file : file_list) {
            if (file.isDirectory()) {
                bool is_ok = cpDir(file, dest + "/" + file.shortFileName());
                if (!is_ok) return false;
            } else {
                auto new_file_path = dest + "/" + file.shortFileName();
                ok = CopyFileA(file.path().c_str(), new_file_path.c_str(), false);
                if (!ok) return false;
            }
        }
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = mkdir(dest.c_str(), 0777 - umask(0000));
        if (ok == -1) return false;
        for (auto& file : file_list) {
            if (file.isDirectory()) {
                bool is_ok = cpDir(file, dest + "/" + file.shortFileName());
                if (!is_ok) return false;
            } else {
                auto new_file_path = dest + "/" + file.shortFileName();
                ok = cpFile(file, new_file_path);
                if (!ok) return false;
            }
        }
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mvFile(const Path &src, const Path &dest) {
        if (!src.isValid()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = MoveFileA(src.path().c_str(), dest.path().c_str());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = rename(src.path().c_str(), dest.path().c_str());
        if (ok == -1) return false;
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mvFile(const Path &src, const std::string &dest) {
        if (!src.isValid()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = MoveFileA(src.path().c_str(), dest.c_str());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = rename(src.path().c_str(), dest.c_str());
        if (ok == -1) return false;
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mvDir(const Path &src, const Path &dest) {
        if (!src.isValid()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = MoveFileA(src.path().c_str(), dest.path().c_str());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = rename(src.path().c_str(), dest.path().c_str());
        if (ok == -1) return false;
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::mvDir(const Path &src, const std::string &dest) {
        if (!src.isValid()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = MoveFileA(src.path().c_str(), dest.c_str());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = rename(src.path().c_str(), dest.c_str());
        if (ok == -1) return false;
#else
        return false;
#endif
        return true;
    }

    bool OS::FileSystem::rmFile(const std::string &path) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = DeleteFileA(path.c_str());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (unlink(path.c_str()) == -1) return false;
#endif
        return true;
    }

    bool OS::FileSystem::rmDir(const std::string &path, bool recursion) {
        auto cur_dir = Path(path);
        if (!cur_dir.isValid() || !cur_dir.isDirectory()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (recursion) {
            if (!rmDirCompletely(cur_dir)) return false;
        }
        BOOL ok = RemoveDirectoryA(cur_dir.path().data());
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (recursion) {
            if (!rmDirCompletely(cur_dir)) return false;
        }
        if (rmdir(path.c_str()) == -1) return false;
#endif
        return true;
    }

    OS::Path OS::FileSystem::currentPath() {
        return Path(".");
    }

    OS::Path OS::FileSystem::homePath() {
        char user[256] = {};
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (FAILED(SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, user)))
            return Path("");
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto home_path = std::getenv("HOME");
        strcpy(user, home_path);
#endif
        return Path(user);
    }

    OS::Path OS::FileSystem::cachePath() {
        return homePath() / ".cache";
    }

    OS::Path OS::FileSystem::localDataPath() {
        std::string local_data(256, 0);
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (FAILED(SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, &local_data[0])))
            return Path("");
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto home_path = std::getenv("HOME");
        local_data = home_path;
        local_data += "/.local/share";
#endif
        return Path(local_data);
    }

    std::vector<OS::Path> OS::FileSystem::listPath(const std::string &path, uint8_t recursion_count,
                                                   const std::function<bool(const Path&)>& filter) {
        Path current_path(path);
        if (!current_path.isValid()) return {};
        if (recursion_count == 0) recursion_count = 255;
        return listAllPath(current_path, 1, recursion_count, filter);
    }

    std::vector<OS::Path> OS::FileSystem::listPath(const Path &path, uint8_t recursion_count,
                                                          const std::function<bool(const Path&)>& filter) {
        if (!path.isValid()) return {};
        if (recursion_count == 0) recursion_count = 255;
        return listAllPath(path, 1, recursion_count, filter);
    }

    std::vector<OS::Path> OS::FileSystem::listPath(uint8_t recursion_count,
                                                          const std::function<bool(const Path&)>& filter) {
        auto current_path = Path(".");
        if (!current_path.isValid()) return {};
        if (recursion_count == 0) recursion_count = 255;
        return listAllPath(current_path, 1, recursion_count, filter);
    }

    OS::FileSystem::LayerMap OS::FileSystem::listPathEx(const Path &path,
                uint8_t recursion_count,
                const std::function<bool(const Path &, bool &)> &found_event) {
        if (!path.isValid()) return {};
        if (recursion_count == 0) recursion_count = 255;
        bool st_all{};
        return listAllPaths(path, 1, recursion_count, st_all, found_event);
    }

    OS::FileSystem::LayerMap OS::FileSystem::listPathEx(const std::string &path,
                uint8_t recursion_count,
                const std::function<bool(const Path &, bool &)> &found_event) {
        auto current_path = Path(".");
        if (!current_path.isValid()) return {};
        if (recursion_count == 0) recursion_count = 255;
        bool st_all{};
        return listAllPaths(current_path, 1, recursion_count, st_all, found_event);
    }

    bool OS::FileSystem::rmDirCompletely(const Path &path) {
        auto paths = listPath(path, 0);
#ifdef TINY_CPP_MY_OS_WINDOWS
        BOOL ok = 1;
        for (auto& t_path : paths) {
            BOOL is_ok = 0;
            if (t_path.isDirectory()) {
                is_ok = RemoveDirectoryA(t_path.path().c_str());
            } else {
                is_ok = DeleteFileA(t_path.path().c_str());
            }
            if (!is_ok) ok = false;
        }
        return ok;
#elif defined(TINY_CPP_MY_OS_UNIX)
        bool is_ok = true;
        for (auto& path : paths) {
            if (path.isDirectory()) {
                if (rmdir(path.path().data()) == -1) is_ok = false;
            } else {
                if (unlink(path.path().data()) == -1) is_ok = false;
            }
        }
        return is_ok;
#endif
        return true;
    }

    std::deque<std::string> OS::FileSystem::mkDirCompletely(const Path &path) {
        std::deque<std::string> paths;
        Path current_path(path);
        while (!current_path.isValid()) {
            paths.push_front(current_path.path());
            current_path.upper();
        }
        return paths;
    }

    std::vector<OS::Path> OS::FileSystem::listAllPath(const Path &path, uint8_t current_recursion,
                                                      uint8_t recursion_count,
                                                      const std::function<bool(const Path &)> &filter) {
        std::vector<Path> paths;
        if (current_recursion > recursion_count) return paths;
#ifdef TINY_CPP_MY_OS_WINDOWS
        WIN32_FIND_DATAW res = {};
        auto str = string2Wide(path.path());
        wchar_t find_string[MAX_PATH];
        wsprintfW(find_string, L"%s\\*", str.c_str());
        auto iter = FindFirstFileW(find_string, &res);
        if (iter == INVALID_HANDLE_VALUE) return paths;
        do {
            auto short_file_name = wide2String(res.cFileName);
            auto new_found = path.path() + "\\" + short_file_name;
            if (short_file_name == "." || short_file_name == "..") continue;
            Path new_path(new_found);
            if (new_path.isDirectory()) {
                auto found_path = listAllPath(new_path, current_recursion + 1, recursion_count, filter);
                paths.insert(paths.end(), found_path.begin(), found_path.end());
            }
            if (filter && !filter(new_path)) continue;
            paths.emplace_back(new_path);
        } while (FindNextFileW(iter, &res));
        FindClose(iter);
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto dir = opendir(path.path().data());
        if (!dir) return paths;
        auto iter = readdir(dir);
        if (!iter) {
            closedir(dir);
            return paths;
        }
        do {
            std::string short_file_name = iter->d_name;
            if (short_file_name == "." || short_file_name == "..") continue;
            auto full_path = path.path() + "/" + short_file_name;
            Path new_found(full_path);
            if (new_found.isDirectory()) {
                auto found_path = listAllPath(new_found, current_recursion + 1, recursion_count, filter);
                paths.insert(paths.end(), found_path.begin(), found_path.end());
            }
            if (filter && !filter(new_found)) continue;
            paths.emplace_back(new_found);
        } while ((iter = readdir(dir)) != nullptr);

        closedir(dir);
#endif
        return paths;
    }

    std::unordered_map<size_t, std::vector<OS::Path>> OS::FileSystem::listAllPaths(const Path &path,
                                                      uint8_t current_recursion, uint8_t recursion_count, bool &stop_all,
                                                      const std::function<bool(const Path &, bool &)> &found_event) {
        std::unordered_map<size_t, std::vector<Path>> paths;
        if (current_recursion > recursion_count) return paths;
        paths.insert({current_recursion, {}});
        auto& cur_paths = paths[current_recursion];
#ifdef TINY_CPP_MY_OS_WINDOWS
        WIN32_FIND_DATAW res = {};
        auto str = string2Wide(path.path());
        wchar_t find_string[MAX_PATH];
        wsprintfW(find_string, L"%s\\*", str.c_str());
        auto iter = FindFirstFileW(find_string, &res);
        if (iter == INVALID_HANDLE_VALUE) return paths;
        bool stop_now = false;
        do {
            auto short_file_name = wide2String(res.cFileName);
            auto new_found = path.path() + "\\" + short_file_name;
            if (short_file_name == "." || short_file_name == "..") continue;
            Path new_path(new_found);
            if (new_path.isDirectory()) {
                auto found_path = listAllPaths(new_path, current_recursion + 1, recursion_count, stop_all, found_event);
                if (stop_all) break;
                for (auto& ff_path : found_path) {
                    auto& LV = ff_path.first;
                    if (paths.find(LV) != paths.end()) {
                        auto& temp_group = paths.at(LV);
                        temp_group.insert(temp_group.end(), ff_path.second.begin(), ff_path.second.end());
                    } else {
                        paths.emplace(LV, ff_path.second);
                    }
                }
            }
            if (found_event) {
                if (found_event(new_path, stop_now)) {
                    cur_paths.emplace_back(new_path);
                }
            } else {
                cur_paths.emplace_back(new_path);
            }
            if (stop_now) {
                stop_all = true;
                break;
            }
        } while (FindNextFileW(iter, &res));
        FindClose(iter);
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto dir = opendir(path.path().data());
        if (!dir) return paths;
        auto iter = readdir(dir);
        if (!iter) {
            closedir(dir);
            return paths;
        }
        bool stop_now = false;
        do {
            std::string short_file_name = iter->d_name;
            if (short_file_name == "." || short_file_name == "..") continue;
            auto full_path = path.path() + "/" + short_file_name;
            Path new_path(full_path);
            if (new_path.isDirectory()) {
                auto found_path = listAllPaths(new_path, current_recursion + 1, recursion_count, stop_all, found_event);
                if (stop_all) break;
                for (auto& ff_path : found_path) {
                    auto& LV = ff_path.first;
                    if (paths.find(LV) != paths.end()) {
                        auto& temp_group = paths.at(LV);
                        temp_group.insert(temp_group.end(), ff_path.second.begin(), ff_path.second.end());
                    } else {
                        paths.emplace(LV, ff_path.second);
                    }
                }
            }
            if (found_event) {
                if (found_event(new_path, stop_now)) {
                    cur_paths.emplace_back(new_path);
                }
            } else {
                cur_paths.emplace_back(new_path);
            }
            if (stop_now) {
                stop_all = true;
                break;
            }
        } while ((iter = readdir(dir)) != nullptr);

        closedir(dir);
#endif
        return paths;
    }
}
