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

#ifndef TINY_CPP_OS_FILE_HPP
#define TINY_CPP_OS_FILE_HPP
#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

/*******************************************************************************************
 * P.S: - To enabled using Windows32 lib, please use the macro below:                      *
 *      #define TINY_CPP_MY_OS_WINDOWS                                                     *
 *      - To enabled using Unix lib (for Linux, MacOS, etc.), please use the macro below:  *
 *      #define TINY_CPP_MY_OS_UNIX                                                        *
 *******************************************************************************************/

#if !defined(TINY_CPP_MY_OS_WINDOWS) && !defined(TINY_CPP_MY_OS_UNIX)
    #if defined(_WIN32) || defined(_WIN64)
        #define TINY_CPP_MY_OS_WINDOWS
    #elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
        #define TINY_CPP_MY_OS_UNIX
    #endif
#endif

#if defined(TINY_CPP_MY_OS_WINDOWS) && !defined(TINY_CPP_DEFINED_WIN)
#define TINY_CPP_DEFINED_WIN
namespace Tiny {
    namespace Win {
        std::string convert2Win(const std::string& path);
        std::wstring string2Wide(const std::string& str, uint32_t codepage = 65001);
        std::string wide2String(const std::wstring& w_str, uint32_t codepage = 65001);
    }
}
#endif

namespace Tiny {
    namespace OS {
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
            Path(std::string  path);
            Path(const Path& path);
            [[nodiscard]] Path(Path&& path) noexcept;
            Path& operator=(const Path& path);
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
            Path& join(const std::string& path);
            Path& parent();
            Path& upper();

            static bool exist(const std::string& path);
            static bool isDirectory(const std::string& path);
            static bool isFile(const std::string& path);
            static bool isSymbolLink(const std::string& path);
        private:
            void checkPath();
            static FileType checkPath(const std::string &path);

            std::string _path;
            std::string _short_file_name{};
            FileType    _type;
        };

        using FileData = std::vector<uint8_t>;
        enum OpenMode : uint8_t {
            Unknown   = 0,
            Text      = 1,
            Binary    = 2,
            ReadOnly  = 4,
            WriteOnly = 8,
            ReadWrite = 16,
            Append    = 32
        };
        class File {
        public:
            File(const std::string& path, OpenMode io = Unknown);
            File(Path  path, OpenMode io = Unknown);
            File(File&& file) noexcept;
            [[nodiscard]] File& operator=(File&& file) noexcept;

            void setPath(const std::string& path);
            void setPath(const Path& path);
            bool isValid() const;
            bool isOpen() const;
            bool open(OpenMode io);
            FileData read(size_t length);
            FileData readAll();
            std::string readText(size_t length);
            std::string readAllText();
            bool write(const FileData& data, size_t length);
            bool write(const char* data, size_t length);
            bool write(const FileData& data);
            void close();

            File(const File&) = delete;
            File& operator=(const File&) = delete;
        private:
            void reset();
            bool setup(OpenMode IO);
            Path _path;
            int _handler{};
            OpenMode _open_mode{};
            size_t _position{};
        };
    }
}

#endif //TINY_CPP_OS_FILE_HPP

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
