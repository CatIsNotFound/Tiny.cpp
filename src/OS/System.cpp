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

#include "../../../../JetBrains/CLion/bin/mingw/lib/gcc/x86_64-w64-mingw32/13.1.0/include/c++/bits/this_thread_sleep.h"

#ifdef TINY_CPP_MY_OS_WINDOWS
#include <windows.h>
#include <fileapi.h>
#include <shlobj.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/utsname.h>

#endif

#if defined(TINY_CPP_MY_OS_WINDOWS) && !defined(TINY_CPP_DEFINED_WIN)
#define TINY_CPP_DEFINED_WIN
namespace Tiny {
    std::string Win::convert2Win(const std::string& path) {
        std::string _new_path;
        for (auto c : path) {
            _new_path.push_back(c == '/' ? '\\' : c);
        }
        return _new_path;
    }

    std::wstring Win::string2Wide(const std::string& str, uint32_t codepage) {
        auto len = MultiByteToWideChar(codepage, 0, str.data(),
        str.size(), nullptr, 0);
        std::wstring w_str(len, 0);
        MultiByteToWideChar(codepage, 0, str.data(), str.size(),
            &w_str[0], len);
        return w_str;
    }

    std::string Win::wide2String(const std::wstring& w_str, uint32_t codepage) {
        auto len = WideCharToMultiByte(codepage, 0, w_str.data(), w_str.size(),
            nullptr, 0, nullptr, nullptr);
        std::string str(len, 0);
        WideCharToMultiByte(codepage, 0, w_str.data(), w_str.size(),
            &str[0], len, nullptr, nullptr);
        return str;
    }
}
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
            case CPU_Arch::IA32:
                return "ia32";
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
        DWORD major = 0, minor = 0, build = 0;
        DWORD d_size = sizeof(DWORD);
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegQueryValueExA(hKey, "CurrentMajorVersionNumber", nullptr, nullptr, (LPBYTE)&major, &d_size);
            RegQueryValueExA(hKey, "CurrentMinorVersionNumber", nullptr, nullptr, (LPBYTE)&minor, &d_size);
            RegQueryValueExA(hKey, "CurrentBuildNumber", nullptr, nullptr, (LPBYTE)&build, &d_size);
            RegCloseKey(hKey);
            std::ostringstream oss;
            oss << major << "." << minor << "." << build;
            info.version = oss.str();
        } else ret = false;
#elif defined(TINY_CPP_MY_OS_UNIX)

#endif
        return ret;
    }

    bool OS::getCPUInfo(CPU &info) {
        bool ret = true;
#ifdef TINY_CPP_MY_OS_WINDOWS
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
                info.machine = CPU_Arch::IA32;
                break;
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
                for (size_t i = 0; i < info.cores; i++) {
                    wchar_t path[MAX_PATH];
                    swprintf(path, L"\\Processor(%llu)\\%% Processor Time", i);
                    if (PdhAddCounterW(query, path, 0, &counter[i]) != ERROR_SUCCESS) ret = false;
                }
            } else ret = false;
        }
        // Get per core usage
        // info.usages.resize(info.cores);
        PdhCollectQueryData(query);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
        memory.used_ram = memory.total_ram - memory.free_ram;
        memory.total_swap = mem_info_ex.ullTotalPageFile;
        memory.free_swap = mem_info_ex.ullAvailPageFile;
#elif defined(TINY_CPP_MY_OS_UNIX)

#endif
        return ret;
    }

    bool OS::getDiskSpace(DiskSpace &disk_space) {
        bool ret = true;
#ifdef TINY_CPP_MY_OS_WINDOWS
        char buf[1024] = {};
        if (GetLogicalDriveStringsA(1024, buf) == 0) ret = false;
        disk_space.total_disk_space = 0;
        disk_space.free_disk_space = 0;
        disk_space.used_disk_space = 0;
        char* drive = buf;
        for (; *drive ; drive += strlen(drive) + 1) {
            ULARGE_INTEGER total{}, free{};
            auto ok = GetDiskFreeSpaceEx(buf, nullptr, &total, &free);
            if (ok == TRUE) {
                disk_space.total_disk_space += total.QuadPart;
                disk_space.free_disk_space += free.QuadPart;
                disk_space.used_disk_space += total.QuadPart - free.QuadPart;
            }
        }
#elif defined(TINY_CPP_MY_OS_UNIX)

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
        if (!path.isValid() || !path.isFile()) return false;
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
        int dest_fd = open(dest.path().c_str(), O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0777 - umask(0000));
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
        int dest_fd = open(dest.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0777 - umask(0000));
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
                                                      const std::function<bool(const Path&)>& filter) {
        std::vector<Path> paths;
        if (current_recursion > recursion_count) return paths;
#ifdef TINY_CPP_MY_OS_WINDOWS
        WIN32_FIND_DATAW res = {};
        auto str = Tiny::Win::string2Wide(path.path());
        wchar_t find_string[MAX_PATH];
        wsprintfW(find_string, L"%s\\*", str.c_str());
        auto iter = FindFirstFileW(find_string, &res);
        if (iter == INVALID_HANDLE_VALUE) return paths;
        do {
            auto short_file_name = Tiny::Win::wide2String(res.cFileName);
            auto new_found = path.path() + "\\" + short_file_name;
            Path new_path(new_found);
            if (short_file_name == "." || short_file_name == "..") continue;
            if (new_path.isDirectory()) {
                auto found_path = listAllPath(new_path, current_recursion + 1, recursion_count);
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
                auto found_path = listAllPath(new_found, current_recursion + 1, recursion_count);
                paths.insert(paths.end(), found_path.begin(), found_path.end());
            }
            if (filter && !filter(new_found)) continue;
            paths.emplace_back(new_found);
        } while ((iter = readdir(dir)) != nullptr);

        closedir(dir);
#endif
        return paths;
    }
}