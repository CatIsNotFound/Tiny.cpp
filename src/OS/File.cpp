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

#include "File.hpp"

#ifdef TINY_CPP_MY_OS_WINDOWS
#include <windows.h>
#include <shlobj.h>

#include <utility>
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
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
    OS::Path::Path(std::string path) : _path(std::move(path)), _type() {
        checkPath();
    }

    OS::Path::Path(const Path &path) : _path(path._path),
                                              _type(path._type) {
        checkPath();
    }

    OS::Path::Path(Path &&path) noexcept : _path(std::move(path._path)), _type(path._type) {
        checkPath();
    }

    OS::Path& OS::Path::operator=(const Path &path) {
        _path = path._path;
        checkPath();
        return *this;
    }

    OS::Path& OS::Path::operator=(Path &&path) noexcept {
        _path = std::move(path._path);
        checkPath();
        return *this;
    }

    OS::Path::~Path() = default;

    void OS::Path::setPath(const std::string &path) {
        _path = path;
        checkPath();
    }

    void OS::Path::setPath(const Path &path) {
        _path = path._path;
        checkPath();
    }

    const std::string & OS::Path::path() const {
        return _path;
    }

    std::string OS::Path::extensionName() const {
        if (_type == FileType::Directory) return {};
        auto pos = _short_file_name.find_first_of('.');
        if (pos == std::string::npos) return {};
        return _short_file_name.substr(pos + 1);
    }

    std::string OS::Path::fileNameWithoutExtension() const {
        if (_type == FileType::Directory) return _short_file_name;
        auto pos = _short_file_name.find_last_of('.');
        if (pos == std::string::npos) return _short_file_name;
        return _short_file_name.substr(0, pos);
    }

    const std::string &OS::Path::shortFileName() const {
        return _short_file_name;
    }

    std::string OS::Path::parentDirectory() const {
        size_t pos = _path.find_last_of('/');
        if (pos == 0) return "/";
        return _path.substr(0, pos);
    }

    bool OS::Path::isValid() const {
        return _type != FileType::Unknown;
    }

    bool OS::Path::isDirectory() const {
        return _type == FileType::Directory;
    }

    bool OS::Path::isFile() const {
        return _type != FileType::Unknown && _type != FileType::Directory;
    }

    bool OS::Path::isSymbolLink() const {
        return _type == FileType::SymbolLink;
    }

    OS::FileType OS::Path::fileType() const {
        return _type;
    }

    size_t OS::Path::fileSize() const {
#ifdef TINY_CPP_MY_OS_WINDOWS
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
#elif defined(TINY_CPP_MY_OS_UNIX)
        struct stat file_stat;
        if (stat(_path.c_str(), &file_stat) == -1) return 0;
        return file_stat.st_size;
#else
        return 0;
#endif
    }

    OS::Path &OS::Path::operator/(const std::string &path) {
        setPath(_path + "/" + path);
        return *this;
    }

    OS::Path &OS::Path::join(const std::string &path) {
        setPath(_path + "/" + path);
        return *this;
    }

    OS::Path &OS::Path::parent() {
        setPath(_path.substr(0, _path.find_last_of('/') + 1));
        return *this;
    }

    OS::Path &OS::Path::upper() {
        auto pos = _path.find_last_of('/');
        if (pos >= _path.size() - 1) _path = _path.substr(0, pos);
        setPath(_path.substr(0, pos + 1));
        return *this;
    }

    bool OS::Path::exist(const std::string &path) {
        return checkPath(path) != FileType::Unknown;
    }

    bool OS::Path::isDirectory(const std::string &path) {
        return checkPath(path) == FileType::Directory;
    }

    bool OS::Path::isFile(const std::string &path) {
        return checkPath(path) == FileType::File;
    }

    bool OS::Path::isSymbolLink(const std::string &path) {
        return checkPath(path) == FileType::SymbolLink;
    }

    void OS::Path::checkPath() {
        _type = FileType::Unknown;
        if (_path.empty()) {
            _short_file_name.clear();
            return;
        }
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto my_path = Win::convert2Win(_path);
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
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        char* new_path = realpath(_path.c_str(), nullptr);
        if (new_path) {
            _path = new_path;
            free(new_path);
        } else return;
        struct stat file_stat;
        if (stat(_path.c_str(), &file_stat) == -1) return;
        if (S_ISDIR(file_stat.st_mode)) {
            _type = FileType::Directory;
        } else if (S_ISREG(file_stat.st_mode)) {
            _type = FileType::File;
        } else if (S_ISLNK(file_stat.st_mode)) {
            _type = FileType::SymbolLink;
        } else if (S_ISCHR(file_stat.st_mode) || S_ISBLK(file_stat.st_mode)) {
            _type = FileType::Device;
        } else if (S_ISSOCK(file_stat.st_mode)) {
            _type = FileType::Socket;
        } else {
            _type = FileType::Unknown;
        }
        _short_file_name = _path.substr(_path.find_last_of('/') + 1);
#endif
    }

    OS::FileType OS::Path::checkPath(const std::string &path) {
        FileType type = FileType::Unknown;
        if (path.empty()) {
            return type;
        }
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto my_path = Win::convert2Win(path);
        auto ok = GetFullPathNameA(my_path.c_str(), 0, nullptr, nullptr);
        if (ok == 0) return type;
        std::string new_full_path(ok, 0);
        GetFullPathNameA(my_path.c_str(), ok, &new_full_path[0], nullptr);
        ok = GetFileAttributesA(new_full_path.c_str());
        if (ok == INVALID_FILE_ATTRIBUTES) return type;
        bool is_dir = (ok & FILE_ATTRIBUTE_DIRECTORY);
        bool is_device = (ok & FILE_ATTRIBUTE_DEVICE);
        bool is_link = (ok & FILE_ATTRIBUTE_REPARSE_POINT);
        bool is_file = !(ok & FILE_ATTRIBUTE_DIRECTORY) &&
                       !(ok & FILE_ATTRIBUTE_DEVICE) &&
                       !(ok & FILE_ATTRIBUTE_REPARSE_POINT);

        if (is_dir)         type = FileType::Directory;
        else if (is_device) type = FileType::Device;
        else if (is_link)   type = FileType::SymbolLink;
        else if (is_file)   type = FileType::File;
        else                type = FileType::Unknown;
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        char* new_path = realpath(path.c_str(), nullptr);
        if (new_path) {
            free(new_path);
        } else return false;
        struct stat file_stat;
        if (stat(_path.c_str(), &file_stat) == -1) return;
        if (S_ISDIR(file_stat.st_mode)) {
            type = FileType::Directory;
        } else if (S_ISREG(file_stat.st_mode)) {
            type = FileType::File;
        } else if (S_ISLNK(file_stat.st_mode)) {
            type = FileType::SymbolLink;
        } else if (S_ISCHR(file_stat.st_mode) || S_ISBLK(file_stat.st_mode)) {
            type = FileType::Device;
        } else if (S_ISSOCK(file_stat.st_mode)) {
            type = FileType::Socket;
        } else {
            type = FileType::Unknown;
        }
#endif
        return type;
    }

    OS::File::File(const std::string &path, OpenMode io) : _path(path), _open_mode(io) {
        setup(io);
    }

    OS::File::File(Path path, OpenMode io) : _path(std::move(path)), _open_mode(io) {
        setup(io);
    }

    OS::File::File(File &&file) noexcept : _path(std::move(file._path)),
                                           _open_mode(std::move(file._open_mode)),
                                           _handler(std::move(file._handler)) {
        if (this != &file) file.reset();
    }

    OS::File &OS::File::operator=(File &&file) noexcept {
        if (this == &file) return *this;
        _path = std::move(file._path);
        _open_mode = std::move(file._open_mode);
        _handler = std::move(file._handler);
        file.reset();
        return *this;
    }

    void OS::File::setPath(const std::string &path) {
        if (isOpen()) close();
        _path.setPath(path);
    }

    void OS::File::setPath(const Path &path) {
        if (isOpen()) close();
        _path.setPath(path);
    }

    bool OS::File::isValid() const {
        return _path.isFile();
    }

    bool OS::File::isOpen() const {
#ifdef TINY_CPP_MY_OS_WINDOWS
        return _handler != HFILE_ERROR;
#else
        return _handler != -1;
#endif
    }

    bool OS::File::open(OpenMode io) {
        return setup(io);
    }

    OS::FileData OS::File::read(size_t length) {
        if (!isValid() || !isOpen()) return {};
        FileData out(length + 1);
#ifdef TINY_CPP_MY_OS_WINDOWS
        DWORD bytes_read;
        SetFilePointer(reinterpret_cast<HANDLE>(_handler), _position, nullptr, FILE_BEGIN);
        auto ok = ReadFile(reinterpret_cast<HANDLE>(_handler), out.data(), length, &bytes_read, nullptr);
        if (ok) _position += bytes_read;
#elif defined(TINY_CPP_MY_OS_UNIX)
        lseek64(_handler, _position, SEEK_SET);
        auto read_length = ::read(_handler, out.data(), length * sizeof(uint8_t));
        _position += read_length;
#endif
        return out;
    }

    OS::FileData OS::File::readAll() {
        if (!isValid() || !isOpen()) return {};
        auto file_size = _path.fileSize();
        FileData out(file_size + 1);
        _position = 0;
#ifdef TINY_CPP_MY_OS_WINDOWS
        SetFilePointer(reinterpret_cast<HANDLE>(_handler), 0, nullptr, FILE_BEGIN);
        ReadFile(reinterpret_cast<HANDLE>(_handler), out.data(), file_size, nullptr, nullptr);
#elif defined(TINY_CPP_MY_OS_UNIX)
        lseek64(_handler, 0, SEEK_SET);
        ::read(_handler, out.data(), file_size * sizeof(uint8_t));
#endif
        return out;
    }

    std::string OS::File::readText(size_t length) {
        if (!isValid() || !isOpen()) return {};
        std::string out(length, 0);
#ifdef TINY_CPP_MY_OS_WINDOWS
        DWORD bytes_read;
        SetFilePointer(reinterpret_cast<HANDLE>(_handler), _position, nullptr, FILE_BEGIN);
        auto ok = ReadFile(reinterpret_cast<HANDLE>(_handler), &out[0], length, &bytes_read, nullptr);
        if (ok) _position += bytes_read;
#elif defined(TINY_CPP_MY_OS_UNIX)
        lseek64(_handler, _position, SEEK_SET);
        auto read_length = ::read(_handler, &out[0], length * sizeof(uint8_t));
        _position += read_length;
#endif
        return out;
    }

    std::string OS::File::readAllText() {
        if (!isValid() || !isOpen()) return {};
        auto file_size = _path.fileSize();
        std::string out(file_size, 0);
        _position = 0;
#ifdef TINY_CPP_MY_OS_WINDOWS
        SetFilePointer(reinterpret_cast<HANDLE>(_handler), 0, nullptr, FILE_BEGIN);
        ReadFile(reinterpret_cast<HANDLE>(_handler), &out[0], file_size, nullptr, nullptr);
#elif defined(TINY_CPP_MY_OS_UNIX)
        lseek64(_handler, 0, SEEK_SET);
        ::read(_handler, &out[0], file_size * sizeof(uint8_t));
#endif
        return out;
    }

    bool OS::File::write(const FileData &data, size_t length) {
        if (!isValid() || !isOpen()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (_open_mode & Append) {
            auto ok = SetFilePointer(reinterpret_cast<HANDLE>(_handler), 0, nullptr, FILE_END);
            if (ok == INVALID_SET_FILE_POINTER) return false;
        }
        auto ok = WriteFile(reinterpret_cast<HANDLE>(_handler), data.data(), length, nullptr, nullptr);
        return ok == TRUE;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = ::write(_handler, data.data(), length * sizeof(uint8_t));
        return ok != -1;
#endif
        return false;
    }

    bool OS::File::write(const char *data, size_t length) {
        if (!isValid() || !isOpen()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (_open_mode & Append) {
            auto ok = SetFilePointer(reinterpret_cast<HANDLE>(_handler), 0, nullptr, FILE_END);
            if (ok == INVALID_SET_FILE_POINTER) return false;
        }
        auto ok = WriteFile(reinterpret_cast<HANDLE>(_handler), data, length, nullptr, nullptr);
        return ok == TRUE;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = ::write(_handler, data, length * sizeof(char));
        return ok != -1;
#endif
        return false;
    }

    bool OS::File::write(const FileData &data) {
        if (!isValid() || !isOpen()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (_open_mode & Append) {
            auto ok = SetFilePointer(reinterpret_cast<HANDLE>(_handler), 0, nullptr, FILE_END);
            if (ok == INVALID_SET_FILE_POINTER) return false;
        }
        auto ok = WriteFile(reinterpret_cast<HANDLE>(_handler), data.data(), data.size(), nullptr, nullptr);
        return ok == TRUE;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = ::write(_handler, data.data(), data.size() * sizeof(uint8_t));
        return ok != -1;
#endif
        return false;
    }

    void OS::File::close() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        CloseHandle(reinterpret_cast<HANDLE>(_handler));
#elif defined(TINY_CPP_MY_OS_UNIX)
        ::close(_handler);
#endif
        reset();
    }

    void OS::File::reset() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        _handler = HFILE_ERROR;
#elif defined(TINY_CPP_MY_OS_UNIX)
        _handler = -1;
#endif
        _position = 0;
        _open_mode = Unknown;
        _path.setPath("");
    }

    bool OS::File::setup(const OpenMode IO) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (IO == Unknown || !_path.isFile()) {
            _handler = HFILE_ERROR;
            return false;
        }
        DWORD open_mode = 0;
        if (IO & ReadOnly)  open_mode |= OF_READ;
        if (IO & WriteOnly) open_mode |= OF_WRITE | OF_CREATE;
        if (IO & ReadWrite) open_mode |= OF_READWRITE | OF_CREATE;
        OFSTRUCT ofs;
        auto handler = OpenFile(_path.path().c_str(), &ofs, open_mode);
        if (handler == HFILE_ERROR) return false;
        _handler = handler;
        _position = 0;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (IO == Unknown || !_path.isFile()) {
            _handler = -1;
            return false;
        }
        int flags = 0;
        if (IO & ReadOnly)  flags |= O_RDONLY;
        if (IO & WriteOnly) flags |= O_WRONLY | O_CREAT;
        if (IO & ReadWrite) flags |= O_RDWR | O_CREAT;
        if (IO & Append)    flags |= O_APPEND;
        auto handler = ::open(_path.path().c_str(), flags, 0777 - umask(0000));
        if (handler == -1) return false;
        _handler = handler;
        _position = 0;
#endif
        return true;
    }
}

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
