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
#include <string>
#include <vector>
#include <array>
#include <cmath>

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

        inline size_t operator""_B(unsigned long long n) noexcept {
            return n;
        }

        inline size_t operator""_KB(unsigned long long n) noexcept {
            return n * 1000;
        }

        inline size_t operator""_KiB(unsigned long long n) noexcept {
            return n * 1024;
        }

        inline size_t operator""_MB(unsigned long long n) noexcept {
            return n * 1000000;
        }

        inline size_t operator""_MiB(unsigned long long n) noexcept {
            return n * 1048576;
        }

        inline size_t operator""_GB(unsigned long long n) noexcept {
            return n * 1000000000;
        }

        inline size_t operator""_GiB(unsigned long long n) noexcept {
            return n * 1073741824;
        }

        inline size_t operator""_TB(unsigned long long n) noexcept {
            return n * 1000000000000;
        }

        inline size_t operator""_TiB(unsigned long long n) noexcept {
            return n * 1099511627776;
        }

        enum class DataUnit : int8_t {
            B,
            KiB,
            MiB,
            GiB,
            TiB
        };

        inline double convertDataSize(size_t size, DataUnit dst_unit, DataUnit src_unit = DataUnit::B) {
            int8_t t = static_cast<int8_t>(dst_unit) - static_cast<int8_t>(src_unit);
            if (t > 0) {
                return size / pow(1024.0, abs(t));
            }
            return size * pow(1024.0, abs(t));
        }

        inline long double autoConvertDataSize(size_t size, DataUnit& result_unit) {
            auto nums = static_cast<long double>(size);
            int8_t add{};
            while (nums > 1024.0) {
                nums /= 1024.0;
                add++;
            }
            if (add >= static_cast<int8_t>(DataUnit::TiB)) result_unit = DataUnit::TiB;
            else result_unit = static_cast<DataUnit>(add);
            return nums;
        }

        inline const char* dataUnitName(const DataUnit& data_unit) {
            switch (data_unit) {
                case DataUnit::B:
                    return "B";
                case DataUnit::KiB:
                    return "KiB";
                case DataUnit::MiB:
                    return "MiB";
                case DataUnit::GiB:
                    return "GiB";
                case DataUnit::TiB:
                    return "TiB";
                default:
                    return "NaN";
            }
        }

        enum Permission : uint8_t {
            P_None,
            P_Execute = 1,
            P_Write = 2,
            P_WriteExec = 3,
            P_Read = 4,
            P_ReadExec = 5,
            P_ReadWrite = 6,
            P_All = 7
        };

        class Path {
            friend class File;
        public:
            Path(std::string  path);
            Path(const Path& path);
            Path(Path&& path) noexcept;
            Path& operator=(const Path& path);
            Path& operator=(Path&& path) noexcept;
            ~Path();

            void setPath(const std::string& path);
            void setPath(const Path& path);
            void unset();
            const std::string& path() const;
            std::string extensionName() const;
            std::string fileNameWithoutExtension() const;
            const std::string& shortFileName() const;
            std::string parentDirectory() const;

            bool isValid() const;
            bool isDirectory() const;
            bool isFile() const;
            bool isSymbolLink() const;
            FileType fileType() const;
            size_t fileSize() const;

            int64_t lastAccessTime() const;
            int64_t lastWriteTime() const;
            int64_t lastCreateTime() const;

            Permission userPermission() const;
            Permission groupPermission() const;
            Permission otherPermission() const;

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
            std::array<Permission, 3> _permission{};
            int64_t _last_access_time{};
            int64_t _last_write_time{};
            int64_t _last_create_time{};
            size_t _file_size{};
        };

        using FileData = std::vector<uint8_t>;
        enum OpenMode : uint8_t {
            Unknown   = 0,
            ReadOnly  = 1,
            WriteOnly = 2,
            ReadWrite = 4,
            Append    = 8
        };
        class File {
        public:
            File(const std::string& path, uint8_t open_mode = Unknown);
            File(Path  path, uint8_t open_mode = Unknown);
            File(File&& file) noexcept;
            File& operator=(File&& file) noexcept;
            ~File();

            void setPath(const std::string& path);
            void setPath(const Path& path);
            bool isFile() const;
            bool isNull() const;
            bool isOpen() const;
            bool open(uint8_t open_mode);
            FileData read(size_t length);
            FileData readAll();
            std::string readText(size_t length);
            std::string readLine();
            std::string readAllText();
            bool write(const FileData& data, size_t length);
            bool write(const char* data, size_t length);
            bool write(const std::string& string);
            bool write(const FileData& data);
            bool writeLine(const std::string& string);
            void close();
            bool isEOF() const;
            void moveToStart();
            void moveToEnd();
            void moveTo(int64_t pos);

            size_t fileSize() const;
            std::string path() const;
            std::string fileName() const;

            File(const File&) = delete;
            File& operator=(const File&) = delete;
        private:
            void reset();
            bool setup(uint8_t IO);
            Path _path;
            size_t _position{}, _file_size{};
            uint8_t _open_mode{};
#ifdef TINY_CPP_MY_OS_WINDOWS
            void* _handler{};
#else
            int _handler{-1};
#endif
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
