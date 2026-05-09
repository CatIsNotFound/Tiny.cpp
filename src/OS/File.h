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

#ifndef TINY_CPP_OS_FILE_H
#define TINY_CPP_OS_FILE_H
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace Tiny {
    namespace OS {
#if defined(_WIN32) || defined(_WIN64)
        inline const char* OS_NAME = "win";
        inline std::string convert2Win(const std::string& path);
        inline std::wstring string2Wide(const std::string& str, uint32_t codepage = 65001);
        inline std::string wide2String(const std::wstring& w_str, uint32_t codepage = 65001);
#elif defined(__linux__)
        inline const char* OS_NAME = "linux";
#elif defined(__APPLE__)
        inline const char* OS_NAME = "apple";
#elif defined(__unix__)
        inline const char* OS_NAME = "unix";
#else
        inline const char* OS_NAME = "unknown";
#endif

        enum class FileType : uint8_t {
            Unknown,
            Directory,
            File,
            SymbolLink,
            Device,
            Socket
        };

        inline const char* fileTypeName(FileType type) {
            switch (type) {
                case FileType::Directory:
                    return "Directory";
                case FileType::File:
                    return "File";
                case FileType::SymbolLink:
                    return "Symbol Link";
                case FileType::Device:
                    return "Device";
                case FileType::Socket:
                    return "Socket";
                default:
                    return "Unknown";
            }
        }

        class Path {
        public:
            explicit Path(const std::string& path);
            explicit Path(const Path& path);
            explicit Path(Path&& path) noexcept;
            Path& operator=(const Path& path) noexcept;
            [[nodiscard]] Path& operator=(Path&& path) noexcept;
            ~Path();

            void setPath(const std::string& path);
            void setPath(const Path& path);
            [[nodiscard]] const std::string& path() const;
            [[nodiscard]] std::string extensionName() const;
            [[nodiscard]] std::string fileNameWithoutExtension() const;
            [[nodiscard]] const std::string& shortFileName() const;
            [[nodiscard]] std::string parentDirectory() const;

            [[nodiscard]] bool isValid() const;
            [[nodiscard]] bool isDirectory() const;
            [[nodiscard]] bool isFile() const;
            [[nodiscard]] bool isSymbolLink() const;
            [[nodiscard]] FileType fileType() const;
            [[nodiscard]] size_t fileSize() const;

            Path& operator/(const std::string& path);
        private:
            void checkPath();

            std::string _path;
            std::string _short_file_name{};
            FileType    _type;
        };

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

            static std::vector<Path> listPath(const Path& path, uint8_t recursion_count = 1, const std::function<bool(const Path&)>& filter = {});
            static std::vector<Path> listPath(const std::string& path, uint8_t recursion_count = 1, const std::function<bool(const Path&)>& filter = {});
            static std::vector<Path> listPath(uint8_t recursion_count = 1, const std::function<bool(const Path&)>& filter = {});
        private:
            static bool rmDirCompletely(const Path& path);
            static std::vector<Path> listAllPath(const Path& path, uint8_t current_recursion, uint8_t recursion_count, const std::function<bool(const Path&)>& filter = {});
        };
    }
}

#endif //TINY_CPP_OS_FILE_H

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
