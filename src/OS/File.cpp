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
#include <aclapi.h>
#include <sddl.h>
#pragma comment(lib, "advapi32.lib")
#define SPLASH '\\'
#define SPLASH_STR "\\"
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#define SPLASH '/'
#define SPLASH_STR "/"
#endif

#ifdef TINY_CPP_MY_OS_WINDOWS
namespace {
    std::string convertPath(const std::string& path) {
        std::string _new_path;
        for (auto c : path) {
            _new_path.push_back(c == '/' ? '\\' : c);
        }
        return _new_path;
    }

    std::wstring string2Wide(const std::string& str, uint32_t codepage = 65001) {
        auto len = MultiByteToWideChar(codepage, 0, str.data(),
        str.size(), nullptr, 0);
        std::wstring w_str(len, 0);
        MultiByteToWideChar(codepage, 0, str.data(), str.size(),
            &w_str[0], len);
        return w_str;
    }

    std::string wide2String(const std::wstring& w_str, uint32_t codepage = 65001) {
        auto len = WideCharToMultiByte(codepage, 0, w_str.data(), w_str.size(),
            nullptr, 0, nullptr, nullptr);
        std::string str(len, 0);
        WideCharToMultiByte(codepage, 0, w_str.data(), w_str.size(),
            &str[0], len, nullptr, nullptr);
        return str;
    }

    int64_t convertTimeToMS(const FILETIME& f_time) {
        LARGE_INTEGER ull;
        ull.LowPart = f_time.dwLowDateTime;
        ull.HighPart = f_time.dwHighDateTime;
        return (ull.QuadPart / 10000) - 11644473600000LL;
    }
}
#endif

namespace Tiny {
    OS::Path::Path(std::string path) : _path(std::move(path)), _type() {
        checkPath();
    }

    OS::Path::Path(const Path &path) : _path(path._path),
                                       _short_file_name(path._short_file_name),
                                       _type(path._type) {
        checkPath();
    }

    OS::Path::Path(Path &&path) noexcept : _path(std::move(path._path)),
                                           _short_file_name(std::move(path._short_file_name)), 
                                           _type(std::move(path._type)) {
        checkPath();
    }

    OS::Path& OS::Path::operator=(const Path &path) {
        _path = path._path;
        checkPath();
        return *this;
    }

    OS::Path& OS::Path::operator=(Path &&path) noexcept {
        _path = path._path;
        path._type = FileType::Unknown;
        path._path.clear();
        path._short_file_name.clear();
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

    void OS::Path::unset() {
        _path.clear();
        _short_file_name.clear();
        _type = FileType::Unknown;
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
        size_t pos = _path.find_last_of(SPLASH);
        if (pos == 0) return SPLASH_STR;
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
        return _file_size;
    }

    int64_t OS::Path::lastAccessTime() const {
        return _last_access_time;
    }

    int64_t OS::Path::lastWriteTime() const {
        return _last_write_time;
    }

    int64_t OS::Path::lastCreateTime() const {
        return _last_create_time;
    }

    OS::Permission OS::Path::userPermission() const {
        return _permission[0];
    }

    OS::Permission OS::Path::groupPermission() const {
        return _permission[1];
    }

    OS::Permission OS::Path::otherPermission() const {
        return _permission[2];
    }

    OS::Path &OS::Path::operator/(const std::string &path) {
        if (path.front() != SPLASH) {
            setPath(_path + SPLASH_STR + path);
        } else {
            setPath(_path + path);
        }
        return *this;
    }

    OS::Path &OS::Path::join(const std::string &path) {
        if (path.front() != SPLASH) {
            setPath(_path + SPLASH_STR + path);
        } else {
            setPath(_path + path);
        }
        return *this;
    }

    OS::Path &OS::Path::parent() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto coped_path = convertPath(_path);
        setPath(coped_path.substr(0, coped_path.find_last_of(SPLASH) + 1));
#else
        setPath(_path.substr(0, _path.find_last_of(SPLASH) + 1));
#endif
        return *this;
    }

    OS::Path &OS::Path::upper() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        _path = convertPath(_path);
#endif
        auto pos = _path.find_last_of(SPLASH);
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
        auto my_path = convertPath(_path);
        WIN32_FILE_ATTRIBUTE_DATA file_info{};
        auto w_path = string2Wide(my_path);
        auto suc = GetFileAttributesExW(w_path.c_str(), GetFileExInfoStandard, reinterpret_cast<LPVOID>(&file_info));
        if (!suc) return;
        bool is_dir = (file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        bool is_device = (file_info.dwFileAttributes & FILE_ATTRIBUTE_DEVICE);
        bool is_link = (file_info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
        bool is_file = !(file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                       !(file_info.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) &&
                       !(file_info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
        if (is_dir)         _type = FileType::Directory;
        else if (is_device) _type = FileType::Device;
        else if (is_link)   _type = FileType::SymbolLink;
        else if (is_file)   _type = FileType::File;
        else                _type = FileType::Unknown;

        _last_create_time = convertTimeToMS(file_info.ftCreationTime);
        _last_access_time = convertTimeToMS(file_info.ftLastAccessTime);
        _last_write_time = convertTimeToMS(file_info.ftLastWriteTime);
        ULARGE_INTEGER ull;
        ull.LowPart = file_info.nFileSizeLow;
        ull.HighPart = file_info.nFileSizeHigh;
        _file_size = ull.QuadPart;
        PSID owner_sid{}, group_sid{};
        PACL dacl{};
        PSECURITY_DESCRIPTOR security_descriptor{};
        auto is_ok = GetNamedSecurityInfoW(w_path.c_str(), SE_FILE_OBJECT,
                          OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
                                    &owner_sid, &group_sid, &dacl, nullptr, &security_descriptor);
        if (is_ok == ERROR_SUCCESS && dacl != nullptr) {
            auto getEffectivePerm = [&](PSID pSid, Permission& perm) {
                TRUSTEE_W trustee = {};
                trustee.TrusteeForm = TRUSTEE_IS_SID;
                trustee.TrusteeType = TRUSTEE_IS_USER;
                trustee.ptstrName = (LPWSTR)pSid;

                ACCESS_MASK rights = 0;
                if (GetEffectiveRightsFromAclW(dacl, &trustee, &rights) == ERROR_SUCCESS) {
                    perm = P_None;
                    if (rights & FILE_GENERIC_READ)    perm = static_cast<Permission>(perm | P_Read);
                    if (rights & FILE_GENERIC_WRITE)   perm = static_cast<Permission>(perm | P_Write);
                    if (rights & FILE_GENERIC_EXECUTE) perm = static_cast<Permission>(perm | P_Execute);
                }
            };

            getEffectivePerm(owner_sid, _permission[0]);
            getEffectivePerm(group_sid, _permission[1]);

            PSID other_sid{};
            SID_IDENTIFIER_AUTHORITY worldAuth = SECURITY_WORLD_SID_AUTHORITY;
            if (AllocateAndInitializeSid(&worldAuth, 1, SECURITY_WORLD_RID,
                                          0,0,0,0,0,0,0, &other_sid)) {
                getEffectivePerm(other_sid, _permission[2]);
                FreeSid(other_sid);
            }

            LocalFree(security_descriptor);
        }

        auto ok = GetFullPathNameW(w_path.c_str(), 0, nullptr, nullptr);
        if (ok == 0) return;
        std::wstring new_full_path(ok, 0);
        GetFullPathNameW(w_path.c_str(), ok, &new_full_path[0], nullptr);
        _path = wide2String(new_full_path);
        while (_path.back() == '\0') _path.pop_back();
        if (_path.back() == '\\') _path.pop_back();
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
#ifdef __linux__
        _last_access_time = file_stat.st_atim.tv_sec * 1000 + (file_stat.st_atim.tv_nsec / 1000);
        _last_write_time = file_stat.st_mtim.tv_sec * 1000 + (file_stat.st_mtim.tv_nsec / 1000);
        _last_create_time = file_stat.st_ctim.tv_sec * 1000 + (file_stat.st_ctim.tv_nsec / 1000);
#elif defined(__APPLE__)
        _last_access_time = file_stat.st_atimespec.tv_sec * 1000 + (file_stat.st_atimespec.tv_nsec / 1000);
        _last_write_time = file_stat.st_mtimespec.tv_sec * 1000 + (file_stat.st_mtimespec.tv_nsec / 1000);
        _last_create_time = file_stat.st_ctimespec.tv_sec * 1000 + (file_stat.st_ctimespec.tv_nsec / 1000);
#else
        _last_access_time = file_stat.st_atime * 1000;
        _last_write_time = file_stat.st_mtime * 1000;
        _last_create_time = file_stat.st_ctime * 1000;
#endif
        _file_size = file_stat.st_size;
        uint8_t perm = 0;
        perm |= S_IRUSR(file_stat.st_mode) ? P_Read : P_None;
        perm |= S_IWUSR(file_stat.st_mode) ? P_Write : P_None;
        perm |= S_IXUSR(file_stat.st_mode) ? P_Execute : P_None;
        _permission[0] = static_cast<Permission>(perm);
        perm = 0;
        perm |= S_IRGRP(file_stat.st_mode) ? P_Read : P_None;
        perm |= S_IWGRP(file_stat.st_mode) ? P_Write : P_None;
        perm |= S_IXGRP(file_stat.st_mode) ? P_Execute : P_None;
        _permission[1] = static_cast<Permission>(perm);
        perm = 0;
        perm |= S_IROTH(file_stat.st_mode) ? P_Read : P_None;
        perm |= S_IWOTH(file_stat.st_mode) ? P_Write : P_None;
        perm |= S_IXOTH(file_stat.st_mode) ? P_Execute : P_None;
        _permission[2] = static_cast<Permission>(perm);

        if (_path.back() == '/') _path.pop_back();
        _short_file_name = _path.substr(_path.find_last_of(SPLASH) + 1);
#endif
    }

    OS::FileType OS::Path::checkPath(const std::string &path) {
        FileType type = FileType::Unknown;
        if (path.empty()) {
            return type;
        }
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto my_path = convertPath(path);
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
        } else return type;
        struct stat file_stat;
        if (stat(path.c_str(), &file_stat) == -1) return type;
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

    OS::File::File(const std::string &path, uint8_t open_mode) : _path(path), _open_mode(open_mode) {
        setup(open_mode);
    }

    OS::File::File(Path path, uint8_t open_mode) : _path(std::move(path)), _open_mode(open_mode) {
        setup(open_mode);
    }

    OS::File::File(File &&file) noexcept : _path(std::move(file._path)),
                                           _open_mode(std::move(file._open_mode)),
                                           _handler(std::move(file._handler)) {
        if (this != &file) {
            file.reset();
            file._file_size = 0;
            file._path.unset();
        }
    }

    OS::File &OS::File::operator=(File &&file) noexcept {
        if (this == &file) return *this;
        
        if (isOpen()) close();
        
        _path = file._path;
        _open_mode = std::move(file._open_mode);
        _handler = std::move(file._handler);
        file.reset();
        file._file_size = 0;
        file._path.unset();
        return *this;
    }

    OS::File::~File() {
        if (isOpen()) close();
    }

    void OS::File::setPath(const std::string &path) {
        if (isOpen()) close();
        _path.setPath(path);
    }

    void OS::File::setPath(const Path &path) {
        if (isOpen()) close();
        _path.setPath(path);
    }

    bool OS::File::isFile() const {
        return _path.isValid() && _path.isFile();
    }

    bool OS::File::isNull() const {
        return !_path.isValid();
    }

    bool OS::File::isOpen() const {
#ifdef TINY_CPP_MY_OS_WINDOWS
        return _handler != INVALID_HANDLE_VALUE && _handler != nullptr;
#else
        return _handler != -1;
#endif
    }

    bool OS::File::open(uint8_t open_mode) {
        return setup(open_mode);
    }

    OS::FileData OS::File::read(size_t length) {
        if (!isFile() || !isOpen()) return {};
        FileData out(length + 1);
#ifdef TINY_CPP_MY_OS_WINDOWS
        DWORD bytes_read;
        SetFilePointer(_handler, _position, nullptr, FILE_BEGIN);
        auto ok = ReadFile(_handler, out.data(), length, &bytes_read, nullptr);
        if (ok) _position += bytes_read;
#elif defined(TINY_CPP_MY_OS_UNIX)
        lseek(_handler, _position, SEEK_SET);
        auto read_length = ::read(_handler, out.data(), length * sizeof(uint8_t));
        _position += read_length;
#endif
        return out;
    }

    OS::FileData OS::File::readAll() {
        if (!isFile() || !isOpen()) return {};
        FileData out(_file_size + 1);
        _position = 0;
#ifdef TINY_CPP_MY_OS_WINDOWS
        SetFilePointer(_handler, 0, nullptr, FILE_BEGIN);
        DWORD read_bytes = 0;
        ReadFile(_handler, out.data(), out.size(), &read_bytes, nullptr);
        _position += read_bytes;
#elif defined(TINY_CPP_MY_OS_UNIX)
        lseek(_handler, 0, SEEK_SET);
        auto read_bytes = ::read(_handler, out.data(), out.size() * sizeof(uint8_t));
        _position += read_bytes;
#endif
        return out;
    }

    std::string OS::File::readText(size_t length) {
        if (!isFile() || !isOpen()) return {};
        std::string out(length, 0);
#ifdef TINY_CPP_MY_OS_WINDOWS
        DWORD bytes_read;
        SetFilePointer(_handler, _position, nullptr, FILE_BEGIN);
        auto ok = ReadFile(_handler, &out[0], length, &bytes_read, nullptr);
        if (ok) _position += bytes_read;
#elif defined(TINY_CPP_MY_OS_UNIX)
        lseek(_handler, _position, SEEK_SET);
        auto read_length = ::read(_handler, &out[0], length * sizeof(uint8_t));
        _position += read_length;
#endif
        return out;
    }

    std::string OS::File::readLine() {
        if (!isFile() || !isOpen()) return {};
        std::string out;
        char ch = '\0';
#ifdef TINY_CPP_MY_OS_WINDOWS
        DWORD bytes_read;
        SetFilePointer(_handler, _position, nullptr, FILE_BEGIN);
        do {
            auto ok = ReadFile(_handler, &ch, 1, &bytes_read, nullptr);
            if (ok && bytes_read > 0) {
                _position += bytes_read;
                out += ch;
            } else break;
        } while (ch != '\n' && ch != '\r' && ch != '\0');
#elif defined(TINY_CPP_MY_OS_UNIX)
        lseek(_handler, _position, SEEK_SET);
        ssize_t read_length = 0;
        do {
            read_length = ::read(_handler, &ch, sizeof(uint8_t));
            if (read_length > 0) {
                _position += read_length;
                out += ch;
            } else break;
        } while (ch != '\n' && ch != '\r' && ch != '\0');
#endif
        if (out.back() == '\n' || out.back() == '\r') out.pop_back();
        return out;
    }

    std::string OS::File::readAllText() {
        if (!isFile() || !isOpen()) return {};
        auto file_size = _path.fileSize();
        std::string out(file_size, 0);
        _position = 0;
#ifdef TINY_CPP_MY_OS_WINDOWS
        SetFilePointer(_handler, 0, nullptr, FILE_BEGIN);
        DWORD read_bytes = 0;
        ReadFile(_handler, &out[0], file_size, &read_bytes, nullptr);
        _position += read_bytes;
#elif defined(TINY_CPP_MY_OS_UNIX)
        lseek(_handler, 0, SEEK_SET);
        auto read_bytes = ::read(_handler, &out[0], file_size * sizeof(uint8_t));
        _position += read_bytes;
#endif
        return out;
    }

    bool OS::File::write(const FileData &data, size_t length) {
        if (!isFile() || !isOpen()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        DWORD written_bytes = 0;
        auto ok = WriteFile(_handler, data.data(), length, &written_bytes, nullptr);
        _position += written_bytes;
        return ok == TRUE;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = ::write(_handler, data.data(), length * sizeof(uint8_t));
        if (ok >= 0) {
            _position += ok;
            return true;
        } else {
            return false;
        }
#endif
    }

    bool OS::File::write(const char *data, size_t length) {
        if (!isFile() || !isOpen()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        DWORD written_bytes = 0;
        auto ok = WriteFile(_handler, data, length, &written_bytes, nullptr);
        _position += written_bytes;
        return ok == TRUE;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = ::write(_handler, data, length * sizeof(char));
        if (ok >= 0) {
            _position += ok;
            return true;
        } else {
            return false;
        }
#endif
    }

    bool OS::File::write(const std::string &string) {
        if (!isFile() || !isOpen()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        DWORD written_bytes = 0;
        auto ok = WriteFile(_handler, string.data(), string.size(), &written_bytes, nullptr);
        _position += written_bytes;
        return ok == TRUE;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = ::write(_handler, string.data(), string.size() * sizeof(char));
        if (ok >= 0) {
            _position += ok;
            return true;
        } else {
            return false;
        }
#endif
    }

    bool OS::File::write(const FileData &data) {
        if (!isFile() || !isOpen()) return false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = WriteFile(_handler, data.data(), data.size(), nullptr, nullptr);
        return ok == TRUE;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = ::write(_handler, data.data(), data.size() * sizeof(uint8_t));
        if (ok >= 0) {
            _position += ok;
            return true;
        } else {
            return false;
        }
#endif
    }

    bool OS::File::writeLine(const std::string &string) {
        if (!isFile() || !isOpen()) return false;
        std::string out = string + "\r\n";
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto ok = WriteFile(_handler, out.data(), out.size(), nullptr, nullptr);
        return ok == TRUE;
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto ok = ::write(_handler, out.data(), out.size() * sizeof(char));
        if (ok >= 0) {
            _position += ok;
            return true;
        } else {
            return false;
        }
#endif
    }

    void OS::File::close() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        CloseHandle(_handler);
#elif defined(TINY_CPP_MY_OS_UNIX)
        ::close(_handler);
#endif
        reset();
    }

    bool OS::File::isEOF() const {
        return _position >= _file_size;
    }

    void OS::File::moveToStart() {
        _position = 0;
    }

    void OS::File::moveToEnd() {
        _position = _file_size;
    }

    void OS::File::moveTo(int64_t pos) {
        if (pos < 0) {
            auto new_pos = _file_size + pos;
            _position = new_pos >= _file_size ? 0 : new_pos;
        } else {
            _position = pos >= _file_size ? _file_size : pos;
        }
    }

    size_t OS::File::fileSize() const {
        return _file_size;
    }

    std::string OS::File::path() const {
        return _path.path();
    }

    std::string OS::File::fileName() const {
        return _path.shortFileName();
    }

    void OS::File::reset() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        _handler = INVALID_HANDLE_VALUE;
#elif defined(TINY_CPP_MY_OS_UNIX)
        _handler = -1;
#endif
        _position = 0;
        _open_mode = Unknown;
    }

    bool OS::File::setup(const uint8_t IO) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (IO == Unknown || IO == Append || _path.isDirectory()) {
            _handler = INVALID_HANDLE_VALUE;
            return false;
        }
        DWORD open_mode = 0;
        DWORD need_create = 0;
        if (IO & ReadOnly) {
            open_mode |= GENERIC_READ;
            need_create = OPEN_EXISTING;
        }
        if (IO & WriteOnly) {
            open_mode |= GENERIC_WRITE;
            need_create = OPEN_ALWAYS;
        }
        if (IO & ReadWrite) {
            open_mode |= GENERIC_READ | GENERIC_WRITE;
            need_create = OPEN_ALWAYS;
        }
        if (IO & Append) {
            open_mode |= FILE_APPEND_DATA;
            if (open_mode & GENERIC_WRITE) open_mode &= ~GENERIC_WRITE;
        }

        auto str = string2Wide(_path.path());
        auto handler = CreateFileW(str.c_str(), open_mode, FILE_SHARE_READ, nullptr, need_create, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handler == INVALID_HANDLE_VALUE) return false;
        _handler = handler;
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (IO == Unknown || IO == Append || _path.isDirectory()) {
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
#endif
        _file_size = _path.fileSize();
        _position = (IO & Append) ? _file_size : 0;
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
