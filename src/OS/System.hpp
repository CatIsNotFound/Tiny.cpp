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

#ifndef TINY_CPP_OS_SYSTEM_HPP
#define TINY_CPP_OS_SYSTEM_HPP
#include <sstream>
#include <deque>
#include <vector>
#include <functional>
#include <unordered_map>

#include "File.hpp"

namespace Tiny {
    namespace OS {
#if defined(_WIN32) || defined(_WIN64)
        constexpr const char* Name("windows");
#elif defined(__linux__)
        constexpr const char* Name("linux");
#elif defined(__APPLE__)
        constexpr const char* Name("apple");
#elif defined(__unix__)
        constexpr const char* Name("unix");
#elif defined(__android__)
        constexpr const char* Name("android");
#elif __OHOS__
        return "openHarmony";
#else
        constexpr const char* Name("unknown");
#endif
        struct HostInfo {
            std::string host_name{};
            std::string user_name{};
            std::string os_name{};
            std::string machine{};
            std::string version{};
        };

        enum class CPU_Arch {
            Unknown     = 0,
            X86         = 1,
            X86_64      = 2,
            ARM32       = 3,
            ARM64       = 4,
            LoongArch   = 5,  /* Loong Arch, including LA32R, LA32S, LA64 */
            MIPS        = 6,  /* MIPS, including MIPS32, MIPS64 */
            RISCV       = 7,  /* RISC-V, including riscv32, riscv64 */
            IA64        = 8
        };

        struct CPU {
            CPU_Arch machine;
            uint32_t cores{};
            uint32_t page_size{};
            float total_usage{};
            std::vector<float> usages{};
        };

        struct Memory {
            size_t total_ram{};
            size_t free_ram{};
            size_t available_ram{};
            size_t used_ram{};
            size_t total_swap{};
            size_t free_swap{};
#ifdef __APPLE__
            size_t app_free_mem{};
            size_t app_active_mem{};
            size_t app_inactive_mem{};
            size_t app_wired_mem{};
            size_t app_compress_mem{};
            size_t app_speculative_mem{};
#endif
        };

        struct DiskSpace {
            size_t total_bytes{};
            size_t free_bytes{};
            size_t used_bytes{};
            size_t available_bytes{};
        };

        const char* getCPUArchName(CPU_Arch cpu_arch);
        HostInfo currentHostInfo();
        CPU currentCPUInfo();
        Memory currentMemory();
        DiskSpace currentDiskSpace();
        bool getHostInfo(HostInfo& info);
        bool getCPUInfo(CPU& info, size_t internal = 50);
        bool getMemory(Memory& memory);
        bool getDiskSpace(DiskSpace& disk_space);
        CPU_Arch getCurrentCPUArch();

        void lastSystemError(std::string& info, int* err_code = nullptr);
        bool isAdmin();


        class FileSystem {
        public:
            static bool chDir(const Path& path);
            static bool chDir(const std::string& path);
            static bool mkDir(const Path& path);
            static bool mkDir(const std::string& path);
            static bool mkFile(const Path& path, const std::vector<uint8_t>& data = {});
            static bool mkFile(const std::string& path, const std::vector<uint8_t>& data = {});
            static bool mkFile(const Path& path, const std::string& data);
            static bool mkFile(const std::string& path, const std::string& data);
            static bool mkLink(const std::string& path, const std::string& link_dest);
            static bool mkLink(const std::string& path, const Path& link_dest);
            static bool cpFile(const Path& src, const Path& dest);
            static bool cpFile(const Path& src, const std::string& dest);
            static bool cpDir(const Path& src, const Path& dest);
            static bool cpDir(const Path& src, const std::string& dest);
            static bool mvFile(const Path& src, const Path& dest);
            static bool mvFile(const Path& src, const std::string& dest);
            static bool mvDir(const Path& src, const Path& dest);
            static bool mvDir(const Path& src, const std::string& dest);
            static bool rmFile(const Path& path);
            static bool rmFile(const std::string& path);
            static bool rmDir(const Path& path, bool recursion = false);
            static bool rmDir(const std::string& path, bool recursion = false);
            static Path currentPath();
            static Path homePath();
            static Path cachePath();
            static Path localDataPath();

            static std::vector<Path> listPath(const Path& path, uint8_t recursion_count = 1, const std::function<bool(const Path&)>& filter = {});
            static std::vector<Path> listPath(const std::string& path, uint8_t recursion_count = 1, const std::function<bool(const Path&)>& filter = {});
            static std::vector<Path> listPath(uint8_t recursion_count = 1, const std::function<bool(const Path&)>& filter = {});
            static std::unordered_map<size_t, std::vector<Path>> listPathEx(const Path& path, uint8_t recursion_count = 1,
                                                const std::function<bool(const Path&, bool&)>& found_event = {});
            static std::unordered_map<size_t, std::vector<Path>> listPathEx(const std::string& path, uint8_t recursion_count = 1,
                                                const std::function<bool(const Path&, bool&)>& found_event = {});
        private:
            static bool rmDirCompletely(const Path& path);
            static std::deque<std::string> mkDirCompletely(const Path& path);
            static std::vector<Path> listAllPath(const Path &path, uint8_t current_recursion, uint8_t recursion_count,
                                                 const std::function<bool(const Path &)> &filter = {});
            static std::unordered_map<size_t, std::vector<Path>> listAllPaths(const Path &path, uint8_t current_recursion, uint8_t recursion_count,
                                                                              bool &stop_all, const std::function<bool(const Path&, bool&)>& found_event = {});
        };
    }
}

#endif //TINY_CPP_OS_SYSTEM_HPP

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
