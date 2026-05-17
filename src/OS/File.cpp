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

    OS::Path& OS::Path::operator=(const Path &path) noexcept {
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

    void OS::Path::checkPath() {
        _type = FileType::Unknown;
        if (_path.empty()) {
            _short_file_name.clear();
            return;
        }
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto my_path = Tiny::Win::convert2Win(_path);
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
        // TODO:
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
        FileData out;
        if (!isValid() || !isOpen()) return out;
        out.resize(length + 1);
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

    bool OS::File::write(const FileData &data, size_t length) {
        if (!isValid() || !isOpen()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
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
        auto handler = ::open(_path.path().c_str(), flags, 0777 - umask(0000));
        if (handler == -1) return false;
        _handler = handler;
        _position = 0;
#endif
        return true;
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
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = CreateDirectoryA(path.path().data(), nullptr);
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (path.isValid() && path.isDirectory()) {
            return (chmod(path.path().c_str(), 0777 - umask(0000)) != -1);
        }
        if (mkdir(path.path().c_str(), 0777 - umask(0000)) == -1) return false;
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
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = CreateDirectoryA(path.c_str(), nullptr);
        if (ok == 0) return false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (mkdir(path.c_str(), 0777 - umask(0000)) == -1) return false;
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
