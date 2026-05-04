

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
            Device,
            Special
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
                case FileType::Special:
                    return "Special";
            }
            return "";
        }

        class Path {
        public:
            explicit Path(const std::string& path);
            explicit Path(const Path& path);
            explicit Path(Path&& path) noexcept;
            [[nodiscard]] Path& operator=(const Path& path);
            [[nodiscard]] Path& operator=(Path&& path) noexcept;
            ~Path();

            void setPath(const std::string& path);
            [[nodiscard]] const std::string& path() const;

            bool isExist() const;
            bool isDirectory() const;
            bool isFile() const;
            FileType fileType() const;
            size_t fileSize() const;
        private:
            void checkPath();

            std::string _path;
            FileType    _type;
        };

        class FileSystem {
        public:
            static FileSystem& global();
            bool changePath(const Path& path);
            bool makeDirectory(const std::string& path);
            bool makeFile(const std::string& path, const std::vector<uint8_t>& data = {});
            bool makeFile(const std::string& path, const std::string& data);
            bool rmFile(const std::string& path);
            bool rmDirectory(const std::string& path, bool recursion = false);
            Path currentPath();
        protected:
            explicit FileSystem();
        private:
            static std::string _current_path;
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

    inline OS::Path & OS::Path::operator=(const Path &path) {
        _path = path._path;
        _type = path._type;
        return *this;
    }

    inline OS::Path & OS::Path::operator=(Path &&path) noexcept {
        _path = std::move(path._path);
        _type = path._type;
        return *this;
    }

    inline OS::Path::~Path() = default;

    inline void OS::Path::setPath(const std::string &path) {
        _path = path;
        checkPath();
    }

    inline const std::string & OS::Path::path() const {
        return _path;
    }

    inline bool OS::Path::isExist() const {
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

#endif
    }

    inline void OS::Path::checkPath() {
#if defined(_WIN32) || defined(_WIN64)
        auto my_path = convert2Win(_path);
        auto ok = GetFileAttributesA(my_path.c_str());
        if (ok == INVALID_FILE_ATTRIBUTES) return;
        bool is_dir = !!(ok & FILE_ATTRIBUTE_DIRECTORY);
        bool is_device = !!(ok & FILE_ATTRIBUTE_DEVICE);
        bool is_link = !!(ok & FILE_ATTRIBUTE_REPARSE_POINT);
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
        _path.assign(new_full_path);
#elif defined(__linux__) || defined(__unix__)
        // TODO:

#elif defined(__APPLE__)
        // TODO:

#endif
    }

}

#endif //TINY_CPP_OS_FILE_IMPL
#endif //TINY_CPP_OS_FILE_H


