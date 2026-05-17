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
#include <deque>
#include "File.hpp"

namespace Tiny {
    namespace OS {
#if defined(_WIN32) || defined(_WIN64)
        constexpr const char* Name("win");
#elif defined(__linux__)
        constexpr const char* Name("linux");
#elif defined(__APPLE__)
        constexpr const char* Name("apple");
#elif defined(__unix__)
        constexpr const char* Name("unix");
#else
        constexpr const char* Name("unknown");
#endif
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

        private:
            static bool rmDirCompletely(const Path& path);
            static std::deque<std::string> mkDirCompletely(const Path& path);
            static std::vector<Path> listAllPath(const Path& path, uint8_t current_recursion, uint8_t recursion_count, const std::function<bool(const Path&)>& filter = {});
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
