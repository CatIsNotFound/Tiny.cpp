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

/// P.S: To only use this single file, you must defined the macro below:
/// #define TINY_CPP_OS_FILE_IMPL

#ifndef TINY_CPP_OS_FILE_H
#define TINY_CPP_OS_FILE_H
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <shlobj.h>
#elif defined(__linux__) || defined(__unix__)
#include <dirent.h>
#elif defined(__APPLE__)
#include <dirent.h>
#endif

namespace Tiny {
    namespace OS {
#if defined(_WIN32) || defined(_WIN64)
        const char* OS_NAME = "win";

        inline std::string convert2Win(const std::string& path) {
            std::string _new_path;
            for (auto c : path) {
                _new_path.push_back(c == '/' ? '\\' : c);
            }
            return _new_path;
        }

        inline std::wstring string2Wide(const std::string& str) {
            auto len = MultiByteToWideChar(CP_ACP, 0, str.data(),
            str.size(), nullptr, 0);
            std::wstring w_str(len, 0);
            MultiByteToWideChar(CP_ACP, 0, str.data(), str.size(),
                &w_str[0], len);
            return w_str;
        }

        inline std::string wide2String(const std::wstring& w_str) {
            auto len = WideCharToMultiByte(CP_ACP, 0, w_str.data(), w_str.size(),
                nullptr, 0, nullptr, nullptr);
            std::string str(len, 0);
            WideCharToMultiByte(CP_ACP, 0, w_str.data(), w_str.size(),
                &str[0], len, nullptr, nullptr);
            return str;
        }
#elif defined(__linux__)
        inline const char* OS_NAME = "linux";
#elif defined(__APPLE__)
        const char* OS_NAME = "apple";
#elif defined(__unix__)
        const char* OS_NAME = "unix";
#else
        const char* OS_NAME = "unknown";
#endif

        enum class FileType : uint8_t {
            Unknown,
            Directory,
            File,
            SymbolLink,
            Device
        };

        inline const char* fileTypeName(FileType type) {
            switch (type) {
                case FileType::Unknown:
                    return "Unknown";
                case FileType::Directory:
                    return "Directory";
                case FileType::File:
                    return "File";
                case FileType::SymbolLink:
                    return "Symbol Link";
                case FileType::Device:
                    return "Device";
            }
            return "";
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
            [[nodiscard]] const std::string& shortFileName() const;

            bool isValid() const;
            bool isDirectory() const;
            bool isFile() const;
            FileType fileType() const;
            size_t fileSize() const;
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
            static bool rmFile(const Path& path);
            static bool rmFile(const std::string& path);
            static bool rmDir(const Path& path, bool recursion = false);
            static bool rmDir(const std::string& path, bool recursion = false);
            static Path currentPath();
            static Path homePath();

            static std::vector<Path> listPath(const Path& path, uint8_t recursion_count = 1);
            static std::vector<Path> listPath(const std::string& path, uint8_t recursion_count = 1);
            static std::vector<Path> listPath(uint8_t recursion_count = 1);
        private:
            static bool rmDirCompletely(const Path& path);
            static std::vector<Path> listAllPath(const Path& path, uint8_t current_recursion, uint8_t recursion_count);
        };
    }
}




#ifdef TINY_CPP_OS_FILE_IMPL
namespace Tiny {
    inline OS::Path::Path(const std::string& path) : _path(path), _type() {
        checkPath();
    }

    inline OS::Path::Path(const Path &path) : _path(path._path),
                                              _type(path._type) {
        checkPath();
    }

    inline OS::Path::Path(Path &&path) noexcept : _path(std::move(path._path)), _type(path._type) {
        checkPath();
    }

    inline OS::Path& OS::Path::operator=(const Path &path) noexcept {
        _path = path._path;
        checkPath();
        return *this;
    }

    inline OS::Path& OS::Path::operator=(Path &&path) noexcept {
        _path = std::move(path._path);
        checkPath();
        return *this;
    }

    inline OS::Path::~Path() = default;

    inline void OS::Path::setPath(const std::string &path) {
        _path = path;
        checkPath();
    }

    inline void OS::Path::setPath(const Path &path) {
        _path = path._path;
        checkPath();
    }

    inline const std::string & OS::Path::path() const {
        return _path;
    }

    inline const std::string &OS::Path::shortFileName() const {
        return _short_file_name;
    }

    inline bool OS::Path::isValid() const {
        return _type != FileType::Unknown;
    }

    inline bool OS::Path::isDirectory() const {
        return _type == FileType::Directory;
    }

    inline bool OS::Path::isFile() const {
        return _type != FileType::Unknown && _type != FileType::Directory;
    }

    inline OS::FileType OS::Path::fileType() const {
        return _type;
    }

    inline size_t OS::Path::fileSize() const {
#if defined(_WIN32) || defined(_WIN64)
        size_t size = 0;
        auto h_file = CreateFileA(_path.c_str(), GENERIC_READ,
            FILE_SHARE_READ, nullptr, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h_file == INVALID_HANDLE_VALUE) return 0;
        LARGE_INTEGER large_int;
        auto ok = GetFileSizeEx(h_file, &large_int);
        CloseHandle(h_file);
        if (!ok) return 0;
        return large_int.QuadPart;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:

#else
        return 0;
#endif
    }

    inline void OS::Path::checkPath() {
#if defined(_WIN32) || defined(_WIN64)
        auto my_path = convert2Win(_path);
        auto ok = GetFileAttributesA(my_path.c_str());
        if (ok == INVALID_FILE_ATTRIBUTES) return;
        bool is_dir = (ok & FILE_ATTRIBUTE_DIRECTORY);
        bool is_device = (ok & FILE_ATTRIBUTE_DEVICE);
        bool is_link = (ok & FILE_ATTRIBUTE_REPARSE_POINT);
        bool is_file = !(ok & FILE_ATTRIBUTE_DIRECTORY) &&
                       !(ok & FILE_ATTRIBUTE_DEVICE) &&
                       !(ok & FILE_ATTRIBUTE_REPARSE_POINT);

        if (is_dir)         _type = FileType::Directory;
        else if (is_device) _type = FileType::Device;
        else if (is_link)   _type = FileType::SymbolLink;
        else if (is_file)   _type = FileType::File;
        else                _type = FileType::Unknown;
        ok = GetFullPathNameA(my_path.c_str(), 0, nullptr, nullptr);
        if (ok == 0) return;
        std::string new_full_path(ok, 0);
        GetFullPathNameA(my_path.c_str(), ok, &new_full_path[0], nullptr);
        _path.assign(new_full_path.begin(), new_full_path.end() - 1);
        _short_file_name = _path.substr(_path.find_last_of('\\') + 1);
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:

#endif
    }

    inline bool OS::FileSystem::chDir(const Path &path) {
#if defined(_WIN32) || defined(_WIN64)
        if (!path.isValid()) return false;
        auto ok = SetCurrentDirectoryA(path.path().data());
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:
#else
        return false;
#endif
        return true;
    }

    inline bool OS::FileSystem::chDir(const std::string &path) {
#if defined(_WIN32) || defined(_WIN64)
        auto ok = SetCurrentDirectoryA(path.data());
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:
#else
        return false;
#endif
        return true;
    }

    inline bool OS::FileSystem::rmFile(const Path &path) {
        if (!path.isValid() || !path.isFile()) return false;
#if defined(_WIN32) || defined(_WIN64)
        auto ok = DeleteFileA(path.path().data());
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:
#else
        return false;
#endif
        return true;
    }

    inline bool OS::FileSystem::rmDir(const Path &path, bool recursion) {
        if (!path.isValid() || !path.isDirectory()) return false;
#if defined(_WIN32) || defined(_WIN64)
        if (recursion) {
            if (!rmDirCompletely(path)) return false;
        }
        auto ok = RemoveDirectoryA(path.path().data());
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:
#else
        return false;
#endif
        return true;
    }

    inline bool OS::FileSystem::mkDir(const Path &path) {
#if defined(_WIN32) || defined(_WIN64)
        auto ok = CreateDirectoryA(path.path().data(), nullptr);
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:
#else
        return false;
#endif
        return true;
    }

    inline bool OS::FileSystem::mkDir(const std::string &path) {
        Path my_path(path);
        if (my_path.isValid()) {
            return my_path.isDirectory();
        }
#if defined(_WIN32) || defined(_WIN64)
        auto ok = CreateDirectoryA(path.c_str(), nullptr);
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:
#else
        return false;
#endif
        return true;
    }

    inline bool OS::FileSystem::mkFile(const Path &path, const std::vector<uint8_t> &data) {
#if defined(_WIN32) || defined(_WIN64)
        auto handler = CreateFileA(path.path().data(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handler == INVALID_HANDLE_VALUE) return false;
        auto ok = WriteFile(handler, data.data(), data.size(), nullptr, nullptr);
        CloseHandle(handler);
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:
#else
        return false;
#endif
        return true;
    }

    inline bool OS::FileSystem::mkFile(const std::string &path, const std::vector<uint8_t> &data) {
#if defined(_WIN32) || defined(_WIN64)
        auto handler = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handler == INVALID_HANDLE_VALUE) return false;
        auto ok = WriteFile(handler, data.data(), data.size(), nullptr, nullptr);
        CloseHandle(handler);
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:
#else
        return false;
#endif
        return true;
    }

    inline bool OS::FileSystem::mkFile(const Path &path, const std::string &data) {
        #if defined(_WIN32) || defined(_WIN64)
        auto handler = CreateFileA(path.path().data(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handler == INVALID_HANDLE_VALUE) return false;
        auto ok = WriteFile(handler, data.data(), data.size(), nullptr, nullptr);
        CloseHandle(handler);
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:
#else
        return false;
#endif
        return true;
    }

    inline bool OS::FileSystem::mkFile(const std::string &path, const std::string &data) {
#if defined(_WIN32) || defined(_WIN64)
        auto handler = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handler == INVALID_HANDLE_VALUE) return false;
        auto ok = WriteFile(handler, data.data(), data.size(), nullptr, nullptr);
        CloseHandle(handler);
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:
#else
        return false;
#endif
        return true;
    }

    inline bool OS::FileSystem::rmFile(const std::string &path) {
#if defined(_WIN32) || defined(_WIN64)
        auto ok = DeleteFileA(path.c_str());
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:

#endif
        return true;
    }

    inline bool OS::FileSystem::rmDir(const std::string &path, bool recursion) {
        auto cur_dir = Path(path);
        if (!cur_dir.isValid() || !cur_dir.isDirectory()) return false;
#if defined(_WIN32) || defined(_WIN64)
        if (recursion) {
            if (!rmDirCompletely(cur_dir)) return false;
        }
        BOOL ok = RemoveDirectoryA(cur_dir.path().data());
        if (ok == 0) return false;
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:

#endif
        return true;
    }

    inline OS::Path OS::FileSystem::currentPath() {
        return Path(".");
    }

    inline OS::Path OS::FileSystem::homePath() {
        char user[256] = {};
#if defined(_WIN32) || defined(_WIN64)
        if (FAILED(SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, user)))
            return Path("");
#endif
        return Path(user);
    }

    inline std::vector<OS::Path> OS::FileSystem::listPath(const std::string &path, uint8_t recursion_count) {
        Path current_path(path);
        if (!current_path.isValid()) return {};
        if (recursion_count == 0) recursion_count = 255;
        return listAllPath(current_path, 1, recursion_count);
    }

    inline std::vector<OS::Path> OS::FileSystem::listPath(const Path &path, uint8_t recursion_count) {
        if (!path.isValid()) return {};
        if (recursion_count == 0) recursion_count = 255;
        return listAllPath(path, 1, recursion_count);
    }

    inline std::vector<OS::Path> OS::FileSystem::listPath(uint8_t recursion_count) {
        auto current_path = Path(".");
        if (!current_path.isValid()) return {};
        if (recursion_count == 0) recursion_count = 255;
        return listAllPath(current_path, 1, recursion_count);
    }

    inline bool OS::FileSystem::rmDirCompletely(const Path &path) {
        auto paths = listPath(path, 0);
#if defined(_WIN32) || defined(_WIN64)
        BOOL ok = 1;
        for (auto& path : paths) {
            BOOL is_ok = 0;
            if (path.isDirectory()) {
                is_ok = RemoveDirectoryA(path.path().c_str());
            } else {
                is_ok = DeleteFileA(path.path().c_str());
            }
            if (!is_ok) ok = false;
        }
        return ok;
#elif defined(__linux__) || defined(__unix__)

#elif defined(__APPLE__)

#endif
        return true;
    }

    inline std::vector<OS::Path> OS::FileSystem::listAllPath(const Path &path, uint8_t current_recursion,
                                                             uint8_t recursion_count) {
        std::vector<Path> paths;
        if (current_recursion > recursion_count) return paths;
#if defined(_WIN32) || defined(_WIN64)
        WIN32_FIND_DATAW res = {};
        auto str = string2Wide(path.path());
        wchar_t find_string[MAX_PATH];
        wsprintfW(find_string, L"%s\\*", str.c_str());
        auto iter = FindFirstFileW(find_string, &res);
        if (iter == INVALID_HANDLE_VALUE) return paths;
        do {
            auto short_file_name = wide2String(res.cFileName);
            auto new_found = path.path() + "\\" + short_file_name;
            Path new_path(new_found);
            if (short_file_name == "." || short_file_name == "..") continue;
            if (new_path.isDirectory()) {
                auto found_path = listAllPath(new_path, current_recursion + 1, recursion_count);
                paths.insert(paths.end(), found_path.begin(), found_path.end());
            }

            paths.emplace_back(new_path);
        } while (FindNextFileW(iter, &res));
        FindClose(iter);
#elif defined(__linux__) || defined(__unix__)

#elif defined(__APPLE__)

#endif
        return paths;
    }
}

#endif //TINY_CPP_OS_FILE_IMPL
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
