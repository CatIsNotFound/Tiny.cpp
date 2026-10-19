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

#include "System.hpp"

#include <stdexcept>
#include <algorithm>
#include <atomic>
#include <thread>
#include <mutex>
#include <sstream>
#include <condition_variable>

#ifdef TINY_CPP_MY_OS_WINDOWS
#include <windows.h>
#include <winnt.h>
#include <process.h>
#include <fileapi.h>
#include <shlobj.h>
#include <pdh.h>
#include <sddl.h>
#pragma comment(lib, "pdh.lib")
#elif defined(TINY_CPP_MY_OS_UNIX)
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>
#include <sys/wait.h>
#include <pwd.h>
#include <cstring>
#ifdef __APPLE__
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/host_info.h>
#include <mach/mach_host.h>
#include <mach/processor_info.h>
#elif defined(__linux__)
#include <sys/sysinfo.h>
#elif defined(__FreeBSD__)
#include <sys/sysctl.h>
#endif
#endif

#ifdef TINY_CPP_MY_OS_WINDOWS
namespace {
    std::string convert2Win(const std::string& path) {
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

    size_t G_PID{0};

    BOOL CALLBACK EnumWinEvent(HWND hwnd, LPARAM lParam) {
        DWORD my_pid{};
        GetWindowThreadProcessId(hwnd, &my_pid);
        if (my_pid == G_PID && IsWindowVisible(hwnd)) {
            SendMessage(hwnd, WM_CLOSE, 0, 0L);
        }
        return true;
    }
}
#endif

#ifdef TINY_CPP_MY_OS_UNIX
int pipe_2(int pipefd[2], int flags) {
#ifdef __linux__
    return pipe2(pipefd, flags);
#else
    if (pipe(pipefd) == -1) {
        return -1;
    }

    if (flags & O_CLOEXEC) {
        fcntl(pipefd[0], F_SETFD, FD_CLOEXEC);
        fcntl(pipefd[1], F_SETFD, FD_CLOEXEC);
    }
    if (flags & O_NONBLOCK) {
        fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
        fcntl(pipefd[1], F_SETFL, O_NONBLOCK);
    }
    return 0;
#endif
}

#ifdef __APPLE__
struct TickStat {
    uint64_t total, idle;
};

static std::vector<TickStat> calcTickStat() {
    std::vector<TickStat> out;
    natural_t cpu_count = 0;
    processor_cpu_load_info_t cpu_load = nullptr;
    mach_msg_type_number_t count = 0;

    kern_return_t kr = host_processor_info(
        mach_host_self(),
        PROCESSOR_CPU_LOAD_INFO,
        &cpu_count,
        (processor_info_array_t*)&cpu_load,
        &count
    );

    if (kr != KERN_SUCCESS) return out;

    for (natural_t i = 0; i < cpu_count; ++i) {
        uint64_t total = 0, idle = 0;
        for (int j = 0; j < CPU_STATE_MAX; j++) {
            total += cpu_load[i].cpu_ticks[j];
        }
        idle = cpu_load[i].cpu_ticks[CPU_STATE_IDLE];
        out.push_back({total, idle});
    }

    vm_deallocate(mach_task_self(), (vm_address_t)cpu_load, count * sizeof(natural_t));
    return out;
}
#else
struct CPU_Stat {
    size_t user, nice, system, idle, iowait, irq, softirq;

    size_t total() {
        return user + nice + system + idle + iowait + irq + softirq;
    }

    size_t busy() {
        return total() - idle;
    }
};
#endif
#endif

static void copeWithMaxStrWhenAddStr(std::string& dest, const std::string& addition) {
    if (dest.size() + addition.size() >= dest.max_size()) {
        dest.clear();
    }
    dest += addition;
}

static int64_t getTicks() {
#ifdef TINY_CPP_MY_OS_WINDOWS
    SYSTEMTIME sys{};
    GetSystemTime(&sys);
    FILETIME ft{};
    GetSystemTimeAsFileTime(&ft);

    LARGE_INTEGER ll{};
    ll.LowPart = ft.dwLowDateTime;
    ll.HighPart = ft.dwHighDateTime;
    const int64_t EPOCH_DIFF = 116444736000000000;
    return (ll.QuadPart - EPOCH_DIFF) / 10000;
#elif defined(TINY_CPP_MY_OS_UNIX)
    struct timespec tv{};
    clock_gettime(CLOCK_REALTIME, &tv);
    return tv.tv_sec * 1000LL + tv.tv_nsec / 1000000;
#endif
}

static bool isSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static std::vector<char*> makeArgv(const std::string& cmd) {
    std::vector<char*> argv;
    std::string token;
    bool in_single_quote = false;
    bool in_double_quote = false;
    
    for (char ch : cmd) {
        if (ch == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
        } else if (ch == '"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
        } else if (isSpace(ch) && !in_single_quote && !in_double_quote) {
            if (!token.empty()) {
                argv.push_back(strdup(token.c_str()));
                token.clear();
            }
        } else {
            token.push_back(ch);
        }
    }
    
    if (!token.empty()) {
        argv.push_back(strdup(token.c_str()));
    }
    
    argv.push_back(nullptr);
    return argv;
}

class MySubTask {
public:
    MySubTask(const std::string& command, bool use_shell) : _command(command), _use_shell(use_shell) {}
    ~MySubTask() {
        if (!_detach_mode && _has_started) {
            terminate();
            wait(5000);
            kill();
            clean();
        }
    }

    /// @brief Start the subtask with optional output and error pipes.
    /// @param use_output_pipe Enable output pipe to capture standard output.
    /// @param use_error_pipe  Enable error pipe to capture standard error.
    /// @retval `0`: Successfully started the subtask.
    /// @retval `-1`: Failed to start the subtask. Need to called `OS::lastSystemError()` to get the error code.
    int start(bool use_output_pipe = false, bool use_error_pipe = false) {
        if (!_detach_mode) {
            _use_output_pipe = use_output_pipe;
            _use_error_pipe = use_error_pipe;
        }
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (_sub_process != INVALID_HANDLE_VALUE) return -1;
        auto exec_cmd = string2Wide(_command);
        HANDLE pipes_out[2]{}, pipes_err[2];

        SECURITY_ATTRIBUTES sa{};
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = true;
        sa.lpSecurityDescriptor = nullptr;

        if (_use_output_pipe) {
            if (!CreatePipe(&pipes_out[0], &pipes_out[1], &sa, 4096)) return -1;
            SetHandleInformation(pipes_out[0], HANDLE_FLAG_INHERIT, 0);
        }

        if (_use_error_pipe) {
            if (!CreatePipe(&pipes_err[0], &pipes_err[1], &sa, 4096)) {
                if (_use_output_pipe) {
                    CloseHandle(pipes_out[0]);
                    CloseHandle(pipes_out[1]);
                }
                return -1;
            }
            SetHandleInformation(pipes_err[0], HANDLE_FLAG_INHERIT, 0);
        }

        _jobs = CreateJobObjectW(nullptr, nullptr);
        if (!_jobs) {
            if (_use_output_pipe) {
                CloseHandle(pipes_out[0]);
                CloseHandle(pipes_out[1]);
            }
            if (_use_error_pipe) {
                CloseHandle(pipes_err[0]);
                CloseHandle(pipes_err[1]);
            }
            return -1;
        }
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli{};
        jeli.BasicLimitInformation.LimitFlags = _detach_mode ? 0 : JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(_jobs, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));

        STARTUPINFOW startup{};
        startup.cb = sizeof(startup);
        startup.hStdOutput = _use_output_pipe ? pipes_out[1] : GetStdHandle(STD_OUTPUT_HANDLE);
        startup.hStdError = _use_error_pipe ? pipes_err[1] : GetStdHandle(STD_ERROR_HANDLE);
        startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        startup.dwFlags |= STARTF_USESTDHANDLES;

        PROCESS_INFORMATION proc_info{};
        DWORD create_flag = CREATE_NEW_PROCESS_GROUP | CREATE_SUSPENDED;
        if (_detach_mode) {
            create_flag |= DETACHED_PROCESS;
        }

        auto ok = CreateProcessW(
            nullptr,
            &exec_cmd[0],
            nullptr,
            nullptr,
            true,
            create_flag,
            nullptr,
            nullptr,
            &startup,
            &proc_info
        );

        if (!ok) {
            if (_use_output_pipe) {
                CloseHandle(pipes_out[0]);
                CloseHandle(pipes_out[1]);
            }
            if (_use_error_pipe) {
                CloseHandle(pipes_err[0]);
                CloseHandle(pipes_err[1]);
            }
            return -1;      /// No such Programme file or not the executable file.
        }

        AssignProcessToJobObject(_jobs, proc_info.hProcess);
        ResumeThread(proc_info.hThread);
        CloseHandle(proc_info.hThread);

        _get_pid = true;
        _pid = proc_info.dwProcessId;
        _sub_process = proc_info.hProcess;

        if (_use_output_pipe) {
            CloseHandle(pipes_out[1]);
            _pipe_output = pipes_out[0];
        }
        if (_use_error_pipe) {
            CloseHandle(pipes_err[1]);
            _pipe_error = pipes_err[0];
        }
        _has_started = true;
        if (_detach_mode) {
            CloseHandle(_pipe_output);
            CloseHandle(_pipe_error);
            CloseHandle(_sub_process);
            CloseHandle(_jobs);
        }
#else
        int pipes_out[2]{-1, -1};
        int pipes_err[2]{-1, -1};
        if (_use_output_pipe && pipe_2(pipes_out, O_CLOEXEC) == -1) return -1;
        if (_use_error_pipe && pipe_2(pipes_err, O_CLOEXEC) == -1) {
            if (_use_output_pipe) {
                close(pipes_out[0]);
                close(pipes_out[1]);
            }
            return -1;
        }
        auto _real_pid = fork();
        if (_real_pid < 0) {
            if (_use_output_pipe) {
                close(pipes_out[0]);
                close(pipes_out[1]);
            }
            if (_use_error_pipe) {
                close(pipes_err[0]);
                close(pipes_err[1]);
            }
            return -1;
        } else if (_real_pid == 0) {
            if (_use_output_pipe) {
                dup2(pipes_out[1], STDOUT_FILENO);
                close(pipes_out[0]);
                close(pipes_out[1]);
            }
            if (_use_error_pipe) {
                dup2(pipes_err[1], STDERR_FILENO);
                close(pipes_err[0]);
                close(pipes_err[1]);
            }
            setpgid(0, 0);
            int ret_code{};
            if (_use_shell) {
                ret_code = execlp("sh", "sh", "-c", _command.c_str(), nullptr);
            } else {
                size_t pos = _command.find_first_of(' ');
                if (pos == std::string::npos) {
                    ret_code = execl(_command.c_str(), nullptr);
                } else {
                    std::string cmd = _command.substr(0, pos);
                    auto args = makeArgv(_command);
                    ret_code = execv(cmd.c_str(), args.data());
                    for (auto a : args) {
                        if (a) free(a);
                    }
                }
            }
            setpgid(0, 0);
            _exit(127);
            return ret_code != -1;
        } else {
            _get_pid = true;
            _pid = _real_pid;
            _has_started = true;
            if (_detach_mode) {
                close(pipes_out[0]);
                close(pipes_out[1]);
                close(pipes_err[0]);
                close(pipes_err[1]);
                return 0;
            }
            if (_use_output_pipe) {
                close(pipes_out[1]);
                _pipe_output = pipes_out[0];
            }
            if (_use_error_pipe) {
                close(pipes_err[1]);
                _pipe_error = pipes_err[0];
            }
        }
#endif
        return 0;
    }

    void setDetachModeEnabled(bool enable) {
        if (_has_started) return;
        _detach_mode = enable;
    }

    bool detachModeEnabled() const {
        return _detach_mode;
    }

    bool readOutputPipe(std::string& dest) {
        if (!_use_output_pipe || !_has_started || _detach_mode) return false;
        dest.clear();
        bool is_read{}, has_output{};
        while (true) {
            is_read = drain(_pipe_output, dest);
            if (!has_output && is_read) has_output = true;
            if (!is_read) break;
        }
        return has_output;
    }

    bool readErrorPipe(std::string& dest) {
        if (!_use_error_pipe || !_has_started || _detach_mode) return false;
        dest.clear();
        bool is_read{}, has_output{};
        while (true) {
            is_read = drain(_pipe_error, dest);
            if (!has_output && is_read) has_output = true;
            if (!is_read) break;
        }
        return has_output;
    }

    bool isRunning() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (_sub_process == INVALID_HANDLE_VALUE) return false;
        DWORD ret_code{};
        GetExitCodeProcess(_sub_process, &ret_code);
        return ret_code == STILL_ACTIVE;
#else
        if (!_has_started || _detach_mode || _is_exited) return false;
        pid_t ret_pid{-1};
        int rt_code{};
        do {
            ret_pid = waitpid(_pid, &rt_code, WNOHANG);
        } while (ret_pid == -1 && errno == EINTR);
        if (ret_pid == 0) {
            return true;
        } else if (ret_pid == _pid) {
            updateReturnCode(rt_code);
            return false;
        }
        _is_exited = true;
        return false;
#endif
    }

    /// @brief Wait for the subtask to finish execution or until the specified timeout.
    /// @param timeout_ms Specify the timeout in milliseconds. If set to `0`, it will wait indefinitely.
    /// @retval `-1`: The subtask has not started or is in detach mode.
    /// @retval `0`: The subtask is still running after the timeout.
    /// @retval `1`: The subtask has finished execution within the timeout.
    int8_t wait(int64_t timeout_ms) {
        if (_detach_mode || !_has_started) return -1;
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (_sub_process == INVALID_HANDLE_VALUE) return -1;
        DWORD ret = WaitForSingleObject(_sub_process, timeout_ms > 0 ? timeout_ms : INFINITE);
        if (ret == WAIT_OBJECT_0) {
            return 1;
        }
        if (ret == WAIT_TIMEOUT) {
            return 0;
        }
        return -1;
#else
        if (_is_exited) return 1;
        int status{-1};
        if (timeout_ms == 0) {
            while (true) {
                auto ret_pid = waitpid(_pid, &status, 0);
                if (ret_pid == _pid) {
                    updateReturnCode(status);
                    return 1;
                } else if (ret_pid == -1 && errno == EINTR) {
                    continue;
                }
                _is_exited = true;
                return 1;
            }
        }
        pid_t exit_pid{-1};
        int64_t st = getTicks(), ed = 0;
        bool is_timedout{};
        while (true) {
            exit_pid = waitpid(_pid, &status, WNOHANG);
            ed = getTicks();
            is_timedout = (timeout_ms > 0 && ed - st >= timeout_ms);
            if (exit_pid == _pid) {
                updateReturnCode(status);
                return 1;
            } else if (is_timedout) {
                _ret_code = 259;
                return 0;
            }
            if (exit_pid == -1 && errno != EINTR) {
                _is_exited = true;
                return 1;
            }
            usleep(1000);
        }
#endif
    }

    void terminate() {
        if (!_has_started || _detach_mode) return;
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (_sub_process == INVALID_HANDLE_VALUE || !isRunning()) return;
        GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, _pid);
        closeAllWindows(_pid);
#else
        if (_pid > 0 && !_is_exited) {
            killpg(_pid, SIGTERM);
            _ret_code = 127;
        }
#endif
    }

    void kill() {
        if (!_has_started || _detach_mode) return;
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (_jobs != INVALID_HANDLE_VALUE) {
            TerminateJobObject(_jobs, 255);
            return;
        }
        if (_sub_process != INVALID_HANDLE_VALUE) {
            TerminateProcess(_sub_process, 255);
        }
#else
        if (_pid > 0 && !_is_exited) {
            killpg(_pid, SIGKILL);
            _ret_code = 255;
        }
#endif
    }

    /// @brief  Get the current exit code.
    /// @retval `-1` : The task is not started.
    /// @retval `0`  : The task is exited normally.
    /// @retval `127`: The task is terminated by signal or not found.
    /// @retval `255`: The task is killed.
    /// @retval `259`: The task is still running.
    int exitCode(int64_t timeout_ms = 1000) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (_sub_process == INVALID_HANDLE_VALUE) return -1;
        DWORD exit_code{};
        GetExitCodeProcess(_sub_process, &exit_code);
        return static_cast<int>(exit_code);
#else
        if (!_has_started) return -1;
        if (timeout_ms == 0) {
            if (!_is_exited && isRunning()) {
                return 259;
            }
        } else {
            wait(timeout_ms);
        }
        return _ret_code;
#endif
    }

    size_t pid() {
        if (_get_pid) {
            _get_pid = false;
            return _pid;
        }
        return 0;
    }

    void clean() {
        _pid = 0;
        _get_pid = false;
        _has_started = false;
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (_pipe_error != INVALID_HANDLE_VALUE) CloseHandle(_pipe_error);
        if (_pipe_output != INVALID_HANDLE_VALUE) CloseHandle(_pipe_output);
        if (_sub_process != INVALID_HANDLE_VALUE) CloseHandle(_sub_process);
        if (_jobs != INVALID_HANDLE_VALUE) CloseHandle(_jobs);
#else
        if (_pipe_output != -1) close(_pipe_output);
        if (_pipe_error != -1)  close(_pipe_error);
        _is_exited = false;
        _ret_code = -1;
#endif
    }

private:
    std::string _command{};
    size_t _pid{};
    bool _use_shell{}, _use_output_pipe{}, _use_error_pipe{}, _get_pid{}, _detach_mode{}, _has_started{};
#ifdef TINY_CPP_MY_OS_WINDOWS
    HANDLE _pipe_output{INVALID_HANDLE_VALUE}, _pipe_error{INVALID_HANDLE_VALUE};
    HANDLE _jobs{INVALID_HANDLE_VALUE}, _sub_process{INVALID_HANDLE_VALUE};

    static bool drain(HANDLE h, std::string& dest) {
        bool ret{};
        DWORD avail = 0;
        char buf[1024];
        while (PeekNamedPipe(h, nullptr, 0, nullptr, &avail, nullptr)) {
            if (avail == 0) break;
            DWORD rb = 0;
            if (!ReadFile(h, buf, sizeof(buf), &rb, nullptr) || rb == 0) break;
            if (!ret) ret = true;
            dest.append(buf, rb);
        }
        return ret;
    }

    static bool closeAllWindows(size_t pid) {
        G_PID = pid;
        return EnumWindows(EnumWinEvent, 0);
    }
#else
    int _pipe_output{-1}, _pipe_error{-1};
    mutable int _ret_code{-1};
    mutable bool _is_exited{};

    static bool drain(int fd, std::string& dest) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);
        timeval timeout{};
        auto ret = select(fd + 1, &read_fds, nullptr, nullptr, &timeout);
        if (ret <= 0) return false;
        ssize_t read_bytes{};
        char bytes[1024];
        bool has_data{}; 
        do {
            read_bytes = read(fd, bytes, sizeof(bytes));
            if (read_bytes > 0) {
                dest.append(bytes, read_bytes);
                has_data = true;
            }
            if (read_bytes == 0) break;
            ret = select(fd + 1, &read_fds, nullptr, nullptr, &timeout);
        } while (ret > 0);
        return has_data;
    }

    void updateReturnCode(int status) {
        _is_exited = true;
        if (WIFEXITED(status)) {
            _ret_code = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            _ret_code = 255;
        }
    }
#endif
};

namespace CRC32Utils {
    static std::array<uint32_t, 256> crc32_table{
        0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4,
        0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
        0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
        0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
        0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a,
        0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
        0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f,
        0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
        0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
        0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
        0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5,
        0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
        0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
        0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
        0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1, 0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
        0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
        0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda, 0xaf0a1b4c,
        0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
        0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31,
        0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
        0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
        0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
        0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
        0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
        0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8,
        0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
    };

    static uint32_t update(uint32_t crc, const uint8_t* data, size_t len) {
        for (size_t i = 0; i < len; i++) {
            auto c = data[i];
            crc = crc32_table[ (crc ^ c) & 0xFF ] ^ (crc >> 8);
        }
        return crc;
    }

    static uint32_t calc(const uint8_t* data, size_t len) {
        auto crc32 = update(0xffffffff, data, len);
        return crc32 ^ 0xffffffff;
    }
};

namespace Tiny {
    const char *OS::getCPUArchName(CPU_Arch cpu_arch) {
        switch (cpu_arch) {
            case CPU_Arch::X86:
                return "x86";
            case CPU_Arch::X86_64:
                return "x86_64";
            case CPU_Arch::ARM32:
                return "arm32";
            case CPU_Arch::ARM64:
                return "arm64";
            case CPU_Arch::LoongArch:
                return "loongArch";
            case CPU_Arch::MIPS:
                return "mips";
            case CPU_Arch::RISCV:
                return "risc-v";
            case CPU_Arch::IA64:
                return "ia64";
            default:
                return "Unknown";
        }
    }

    OS::HostInfo OS::currentHostInfo() {
        HostInfo host_info;
        getHostInfo(host_info);
        return host_info;
    }

    OS::CPU OS::currentCPUInfo() {
        CPU cpu_info;
        getCPUInfo(cpu_info);
        return cpu_info;
    }

    OS::Memory OS::currentMemory() {
        Memory memory;
        getMemory(memory);
        return memory;
    }

    OS::DiskSpace OS::currentDiskSpace() {
        DiskSpace disk_space;
        getDiskSpace(disk_space);
        return disk_space;
    }

    bool OS::getHostInfo(HostInfo &info) {
        bool ret = true;
#ifdef TINY_CPP_MY_OS_WINDOWS
        /// Host name
        info.host_name.resize(MAX_COMPUTERNAME_LENGTH + 1);
        DWORD size = info.host_name.size();
        if (!GetComputerNameA(&info.host_name[0], &size)) ret = false;
        /// User name
        info.user_name.resize(256);
        if (!GetUserNameA(&info.user_name[0], &size)) ret = false;
        info.user_name.resize(size);
        /// System Version
        HKEY hKey;
        char dis_product[128] = {};
        char dis_version[32] = {};
        char dis_cur_ver[32] = {};
        char dis_cur_build[32] = {};
        DWORD dis_product_len = 128;
        DWORD dis_ver_len = 32;
        DWORD dis_cur_ver_len = 32;
        DWORD dis_cur_build_len = 32;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            RegQueryValueExA(hKey, "ProductName", nullptr, nullptr, (LPBYTE)&dis_product, &dis_product_len);
            if (RegQueryValueExA(hKey, "WinREVersion", nullptr, nullptr, (LPBYTE)&dis_version, &dis_ver_len) != ERROR_SUCCESS) {
                RegQueryValueExA(hKey, "CurrentVersion", nullptr, nullptr, (LPBYTE)&dis_cur_ver, &dis_cur_ver_len);
                RegQueryValueExA(hKey, "CurrentBuild", nullptr, nullptr, (LPBYTE)&dis_cur_build, &dis_cur_build_len);
                sprintf(dis_version, "%s.%s", dis_cur_ver, dis_cur_build);
            }
            RegCloseKey(hKey);
            info.version = dis_version;
            info.os_name = dis_product;
        } else ret = false;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char machine_arch[32] = {};
            DWORD machine_arch_len = 32;
            RegQueryValueExA(hKey, "PROCESSOR_ARCHITECTURE", nullptr, nullptr, (LPBYTE)&machine_arch, &machine_arch_len);
            RegCloseKey(hKey);
            info.machine = machine_arch;
        } else ret = false;
#elif defined(TINY_CPP_MY_OS_UNIX)
        struct utsname uts_name;
        uname(&uts_name);
        info.host_name = uts_name.nodename;
        info.user_name = getpwuid(getuid())->pw_name;
        info.os_name = uts_name.sysname;
        info.machine = uts_name.machine;
        info.version = uts_name.release;
        // Get OS Name
#ifdef __APPLE__
        char new_version[128] = {};
        size_t len = sizeof(new_version);
        sysctlbyname("kern.osproductversion", new_version, &len, nullptr, 0);
        info.version = new_version;
#elif defined(__linux__)
        File file("/etc/os-release", OpenMode::ReadOnly);
        if (file.isOpen()) {
            while (true) {
                auto buf = file.readLine();
                if (buf.find("PRETTY_NAME=") != std::string::npos) {
                    info.os_name = buf.substr(13);
                    info.os_name.pop_back();
                    break;
                }
                if (buf.empty()) break;
            }
            file.close();
        } else {
            ret = false;
        }
#endif
#endif
        return ret;
    }

    bool OS::getCPUInfo(CPU &info, size_t internal) {
        bool ret = true;
#ifdef TINY_CPP_MY_OS_WINDOWS
#ifndef PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64
#define PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64 13
#endif
#ifndef PROCESSOR_ARCHITECTURE_ARM64
#define PROCESSOR_ARCHITECTURE_ARM64 12
#endif
#ifndef PROCESSOR_ARCHITECTURE_IA32_ON_ARM64
#define PROCESSOR_ARCHITECTURE_IA32_ON_ARM64 14
#endif
        SYSTEM_INFO sys_info;
        GetNativeSystemInfo(&sys_info);
        switch (sys_info.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_INTEL:
                info.machine = CPU_Arch::X86;
                break;
            case PROCESSOR_ARCHITECTURE_AMD64:
                info.machine = CPU_Arch::X86_64;
                break;
            case PROCESSOR_ARCHITECTURE_ARM:
            case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64:
                info.machine = CPU_Arch::ARM32;
                break;
            case PROCESSOR_ARCHITECTURE_ARM64:
                info.machine = CPU_Arch::ARM64;
                break;
            case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64:
            case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
            case PROCESSOR_ARCHITECTURE_IA64:
                info.machine = CPU_Arch::IA64;
                break;
            default:
                info.machine = CPU_Arch::Unknown;
        }
        /// CPU Cores
        info.cores = sys_info.dwNumberOfProcessors;
        info.page_size = sys_info.dwPageSize;

        /// CPU Usage
        static PDH_HQUERY query{};
        static std::vector<PDH_HCOUNTER> counter(info.cores, nullptr);

        // Init counter
        if (!query) {
            if (PdhOpenQueryA(nullptr, 0, &query) == ERROR_SUCCESS) {
                info.usages.resize(info.cores);
                for (uint32_t i = 0; i < info.cores; i++) {
                    wchar_t path[MAX_PATH];
                    swprintf(path, L"\\Processor Information(0,%u)\\%% Processor Time", i);
                    if (PdhAddCounterW(query, path, 0, &counter[i]) != ERROR_SUCCESS) ret = false;
                }
            } else ret = false;
        }
        // Get per core usage
        info.usages.resize(info.cores);
        PdhCollectQueryData(query);
        std::this_thread::sleep_for(std::chrono::milliseconds(internal));
        PdhCollectQueryData(query);
        for (size_t i = 0; i < info.cores; i++) {
            PDH_FMT_COUNTERVALUE value;
            auto err_code = PdhGetFormattedCounterValue(counter[i], PDH_FMT_DOUBLE,
                                           nullptr, &value);
            if (err_code == ERROR_SUCCESS) {
                info.usages[i] = static_cast<float>(value.doubleValue);
                info.total_usage += info.usages[i];
            } else {
                ret = false;
            }
        }
        if (info.cores > 0) info.total_usage /= info.cores;

#elif defined(TINY_CPP_MY_OS_UNIX)
        // CPU machine
        struct utsname uts_name;
        uname(&uts_name);
        std::string machine = uts_name.machine;
        if (machine == "x86_64") info.machine = CPU_Arch::X86_64;
        else if (machine == "arm64" || machine == "aarch64") info.machine = CPU_Arch::ARM64;
        else if (machine.find_first_of("armv") != std::string::npos) info.machine = CPU_Arch::ARM32;
        else if (machine == "i386" || machine == "i686") info.machine = CPU_Arch::X86;
        else if (machine.find_first_of("loongarch") != std::string::npos) info.machine = CPU_Arch::LoongArch;
        else if (machine.find_first_of("mips") != std::string::npos) info.machine = CPU_Arch::MIPS;
        else if (machine.find_first_of("riscv") != std::string::npos) info.machine = CPU_Arch::RISCV;
        else if (machine == "ia64") info.machine = CPU_Arch::IA64;

        // CPU info
#ifdef __APPLE__
        size_t length = sizeof(info.cores);
        if (sysctlbyname("hw.ncpu", &info.cores, &length, nullptr, 0) == -1) info.cores = 1;
        info.usages.resize(info.cores);

        auto prev = calcTickStat();
        std::this_thread::sleep_for(std::chrono::milliseconds(internal));
        auto next = calcTickStat();

        // Calc the CPU usage
        float total_usage = 0;
        for (size_t i = 0; i < info.cores; ++i) {
            auto d_total = (next[i].total - prev[i].total);
            auto d_idle = (next[i].idle - prev[i].idle);
            if (d_total != 0) {
                info.usages[i] = 100.f - (100.f * static_cast<double>(d_idle) / static_cast<double>(d_total));
            }
            total_usage += info.usages[i];
        }
        info.total_usage = total_usage / static_cast<float>(info.cores);
#elif defined(__linux__)
        info.cores = get_nprocs();
        info.usages.resize(info.cores);
        static std::function<std::vector<CPU_Stat>()> getCPUStat = [&info, &ret] {
            std::vector<CPU_Stat> output_list;
            File cpu_stat("/proc/stat");
            if (cpu_stat.open(OpenMode::ReadOnly)) {
                std::string line;
                size_t i = 0;
                do {
                    line = cpu_stat.readLine();
                    auto pos = line.find_first_of("cpu");
                    if (pos != std::string::npos) {
                        std::istringstream iss(line);
                        std::string cpu_name;
                        CPU_Stat stat;
                        iss >> cpu_name >> stat.user >> stat.nice >> stat.system
                            >> stat.idle >> stat.iowait >> stat.irq >> stat.softirq;
                        output_list.push_back(stat);
                        i += 1;
                    } else {
                        break;
                    }
                } while (true);
                cpu_stat.close();
            } else {
                ret = false;
            }
            return output_list;
        };
        auto prev = getCPUStat();
        std::this_thread::sleep_for(std::chrono::milliseconds(internal));
        auto next = getCPUStat();
        for (size_t i = 0; i < info.cores + 1; ++i) {
            auto d_total = next[i].total() - prev[i].total();
            auto d_busy = next[i].busy() - prev[i].busy();
            if (i == 0) {
                info.total_usage = static_cast<double>(d_busy) / static_cast<double>(d_total) * 100.f;
            } else {
                info.usages[i - 1] = static_cast<double>(d_busy) / static_cast<double>(d_total) * 100.f;
            }
        }
#elif defined(__FreeBSD__)
        size_t u32_sz = sizeof(uint32_t);
        sysctlbyname("hw.ncpu", &info.cores, &u32_sz, nullptr, 0);
        sysctlbyname("hw.pagesize", &info.page_size, &u32_sz, nullptr, 0);
        std::vector<size_t> value, sum_value;
        size_t val_sz = 0, sum_sz = 0;
        if (sysctlbyname("kern.cp_times", nullptr, &val_sz, nullptr, 0) == -1 ||
            sysctlbyname("kern.cp_time", nullptr, &sum_sz, nullptr, 0) == -1) {
            return false;
        }
        value.resize(val_sz / sizeof(size_t));
        sum_value.resize(sum_sz / sizeof(size_t));
        info.usages.resize(info.cores);

        static std::function<std::vector<CPU_Stat>(const std::vector<size_t>, uint32_t)> calcTicks =
                [&](const std::vector<size_t> value, uint32_t ncpu) {
            std::vector<CPU_Stat> result;
            result.resize(ncpu);
            for (size_t i = 0; i < ncpu; ++i) {
                result[i].user = value[i * 5];
                result[i].nice = value[i * 5 + 1];
                result[i].system = value[i * 5 + 2];
                result[i].irq = value[i * 5 + 3];
                result[i].idle = value[i * 5 + 4];
            }
            return result;
        };

        sysctlbyname("kern.cp_times", &value[0], &val_sz, nullptr, 0);
        sysctlbyname("kern.cp_time", &sum_value[0], &sum_sz, nullptr, 0);
        auto prev = calcTicks(value, info.cores);
        auto sum_prev = calcTicks(sum_value, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(internal));
        sysctlbyname("kern.cp_times", &value[0], &val_sz, nullptr, 0);
        sysctlbyname("kern.cp_time", &sum_value[0], &sum_sz, nullptr, 0);
        auto next = calcTicks(value, info.cores);
        auto sum_next = calcTicks(sum_value, 1);
        for (size_t i = 0; i < info.cores; ++i) {
            info.usages[i] = static_cast<double>(next[i].busy() - prev[i].busy()) /
                             static_cast<double>(next[i].total() - prev[i].total()) * 100.f;
        }
        info.total_usage = static_cast<double>(sum_next.front().busy() - sum_prev.front().busy()) /
                           static_cast<double>(sum_next.front().total() - sum_prev.front().total()) * 100.f;
#endif
        info.page_size = sysconf(_SC_PAGE_SIZE);
#endif
        return ret;
    }

    bool OS::getMemory(Memory &memory) {
        bool ret = true;
#ifdef TINY_CPP_MY_OS_WINDOWS
        /// Memory RAM
        MEMORYSTATUSEX mem_info_ex{};
        mem_info_ex.dwLength = sizeof(mem_info_ex);
        if (!GlobalMemoryStatusEx(&mem_info_ex)) ret = false;
        memory.total_ram = mem_info_ex.ullTotalPhys;
        memory.free_ram = mem_info_ex.ullAvailPhys;
        memory.available_ram = memory.free_ram;
        memory.used_ram = memory.total_ram - memory.free_ram;
        memory.total_swap = mem_info_ex.ullTotalPageFile;
        memory.free_swap = mem_info_ex.ullAvailPageFile;
#elif defined(TINY_CPP_MY_OS_UNIX)
#ifdef __APPLE__
        vm_size_t page_size = 0;
        host_page_size(mach_host_self(), &page_size);
        memory.total_ram = page_size * sysconf(_SC_PHYS_PAGES);
        vm_statistics64 vm_stat;
        mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
        kern_return_t kr = host_statistics64(
            mach_host_self(),
            HOST_VM_INFO64,
            (host_info64_t)&vm_stat,
            &count
        );
        if (kr == KERN_SUCCESS) {
            uint64_t free_count = vm_stat.free_count * page_size;
            uint64_t active_count = vm_stat.active_count * page_size;
            uint64_t inactive_count = vm_stat.inactive_count * page_size;
            uint64_t wired_count = vm_stat.wire_count * page_size;
            uint64_t compress_count = vm_stat.compressor_page_count * page_size;
            uint64_t speculative_count = vm_stat.speculative_count * page_size;
            uint64_t used_count = active_count + wired_count + compress_count;
            uint64_t avail_count = inactive_count + free_count + speculative_count;
            // Only for Apple
            memory.app_free_mem = free_count;
            memory.app_active_mem = active_count;
            memory.app_inactive_mem = inactive_count;
            memory.app_wired_mem = wired_count;
            memory.app_compress_mem = compress_count;
            memory.app_speculative_mem = speculative_count;
            memory.free_ram = free_count;
            memory.used_ram = used_count;
            memory.available_ram = avail_count;
        } else {
            ret = false;

        }

        struct xsw_usage xsw{};
        size_t len = sizeof(xsw);
        sysctlbyname("vm.swapusage", &xsw, &len, nullptr, 0);
        memory.total_swap = xsw.xsu_total;
        memory.free_swap = xsw.xsu_avail;

#elif defined(__linux__)
        File mem_info("/proc/meminfo", OpenMode::ReadOnly);
        if (mem_info.isOpen()) {
            size_t cnt = 0;
            while (true) {
                auto line = mem_info.readLine();
                bool is_valid = false;
                is_valid = (line.find("MemTotal") != std::string::npos) ||
                            (line.find("MemFree") != std::string::npos) ||
                            (line.find("MemAvailable") != std::string::npos) ||
                            (line.find("SwapTotal") != std::string::npos) ||
                            (line.find("SwapFree") != std::string::npos);
                if (!is_valid) continue;
                cnt += 1;
                std::istringstream iss(line);
                std::string temp;
                size_t kb = 0;
                iss >> temp >> kb;
                if (temp.find("MemTotal") != std::string::npos) {
                    memory.total_ram = kb * 1024;
                } else if (temp.find("MemFree") != std::string::npos) {
                    memory.free_ram = kb * 1024;
                } else if (temp.find("MemAvailable") != std::string::npos) {
                    memory.available_ram = kb * 1024;
                } else if (temp.find("SwapTotal") != std::string::npos) {
                    memory.total_swap = kb * 1024;
                } else if (temp.find("SwapFree") != std::string::npos) {
                    memory.free_swap = kb * 1024;
                }
                if (cnt >= 5 || line.empty()) break;
            }
            memory.used_ram = memory.total_ram - memory.available_ram;
            mem_info.close();
        } else {
            ret = false;
        }
#elif defined(__FreeBSD__)
        size_t sz = sizeof(size_t);
        size_t page_size{};
        size_t total_pages{}, free_pages{}, inactive_pages{}, cache_pages{}, wired_pages{};
        sysctlbyname("vm.stats.vm.v_page_size", &page_size, &sz, nullptr, 0);
        sysctlbyname("vm.stats.vm.v_page_count", &total_pages, &sz, nullptr, 0);
        sysctlbyname("vm.stats.vm.v_free_count", &free_pages, &sz, nullptr, 0);
        sysctlbyname("vm.stats.vm.v_inactive_count", &inactive_pages, &sz, nullptr, 0);
        sysctlbyname("vm.stats.vm.v_cache_count", &cache_pages, &sz, nullptr, 0);
        sysctlbyname("vm.stats.vm.v_wired_count", &wired_pages, &sz, nullptr, 0);
        sysctlbyname("vm.swap_total", &memory.total_swap, &sz, nullptr, 0);
        memory.total_ram = page_size * total_pages;
        memory.free_ram = (free_pages + inactive_pages + cache_pages) * page_size;
        memory.used_ram = memory.total_ram - memory.free_ram;
#endif
#endif
        return ret;
    }

    bool OS::getDiskSpace(DiskSpace &disk_space) {
        bool ret = true;
#ifdef TINY_CPP_MY_OS_WINDOWS
        char buf[1024] = {};
        if (GetLogicalDriveStringsA(1024, buf) == 0) ret = false;
        disk_space.total_bytes = 0;
        disk_space.free_bytes = 0;
        disk_space.used_bytes = 0;
        char* drive = buf;
        for (; *drive ; drive += strlen(drive) + 1) {
            ULARGE_INTEGER total{}, free{};
            auto ok = GetDiskFreeSpaceEx(buf, nullptr, &total, &free);
            if (ok == TRUE) {
                disk_space.total_bytes += total.QuadPart;
                disk_space.free_bytes += free.QuadPart;
                disk_space.used_bytes += total.QuadPart - free.QuadPart;
            }
        }
        disk_space.available_bytes = disk_space.free_bytes;
#elif defined(TINY_CPP_MY_OS_UNIX)
#ifdef __android__
        const char* root = "/data";
#else
        const char* root = "/";
#endif
        struct statvfs st{};
        if (statvfs(root, &st) == -1) {
            ret = false;
        } else {
            auto block_size = st.f_frsize;
            disk_space.total_bytes = st.f_blocks * block_size;
            disk_space.free_bytes = st.f_bfree * block_size;
            disk_space.available_bytes = st.f_bavail * block_size;
            disk_space.used_bytes = disk_space.total_bytes - disk_space.free_bytes;
        }
#endif
        return ret;
    }

    OS::CPU_Arch OS::getCurrentCPUArch() {
#if defined(__x86_64__) || defined(_M_X64)
        return CPU_Arch::X86_64;
#elif defined(__i386__) || defined(_M_IX86)
        return CPU_Arch::X86;
#elif defined(__aarch64__) || defined(_M_ARM64)
        return CPU_Arch::ARM64;
#elif defined(__arm__) || defined(_M_ARM)
        return CPU_Arch::ARM;
#elif defined(__loongarch__)
        return CPU_Arch::LOONGARCH;
#elif defined(__mips__)
        return CPU_Arch::MIPS;
#elif defined(__riscv)
        return CPU_Arch::RISCV;
#elif defined(__ia64__)
        return CPU_Arch::IA64;
#else
        return CPU_Arch::Unknown;
#endif
    }

    void OS::lastSystemError(std::string& info, int* err_code) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        auto err = GetLastError();
        if (err_code) *err_code = err;
        if (err == 0) {
            info.clear();
            return;
        }
        SetLastError(0);
        void* msg_buf = nullptr;
        auto out_size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            err,
            MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
            (LPSTR)&msg_buf,
            0,
            nullptr);

        if (out_size > 0) {
            info = static_cast<char*>(msg_buf);
            info.pop_back();
            info.pop_back();
            if (msg_buf) LocalFree(msg_buf);
            return;
        }
        if (msg_buf) LocalFree(msg_buf);
        char out[32]{};
        snprintf(out, 32, "Unknown error! code: 0x%x", err);
        out[31] = '\0';
        info = out;
#else
        int err = errno;
        if (err_code) *err_code = err;
        if (err == 0) {
            info.clear();
            return;
        }
        auto err_text = strerror(err);
        if (err_text == nullptr) {
            std::ostringstream oss;
            oss << "Unknown error! code: 0x" << std::hex << err;
            info = oss.str();
            return;
        }
        info = err_text;
#endif
    }

    bool OS::isAdmin() {
#ifdef TINY_CPP_MY_OS_WINDOWS
        BOOL isElevated{};
        PSID adminSid = nullptr;

        if (!ConvertStringSidToSidW(L"S-1-5-32-544", &adminSid))
            return false;

        CheckTokenMembership(nullptr, adminSid, &isElevated);
        LocalFree(adminSid);

        return isElevated;
#else
        return geteuid() == 0;
#endif
    }

    namespace OS {
        class Process::Child::__SUBPROCESS_PIMPL__ {
        public:
            explicit __SUBPROCESS_PIMPL__() = default;
            ~__SUBPROCESS_PIMPL__() {
                if (_running.load()) {
                    sendSignal(9);
                }
                _use_thread.store(false);
                _run_var.notify_all();
                if (_sub_thread.joinable()) _sub_thread.join();
            }

            bool setListenStreamEnabled(bool enable) {
                if (_running.load()) return false;
                _enable_listen_stream.store(enable);
                return true;
            }

            bool setOutputPipeEnabled(bool enable) {
                if (_running.load()) return false;
                _use_output_pipe = enable;
                return true;
            }

            bool setErrorPipeEnabled(bool enable) {
                if (_running.load()) return false;
                _use_error_pipe = enable;
                return true;
            }

            void setTerminateTimeout(uint32_t timeout) {
                std::lock_guard<std::mutex> lock(_rw_mutex);
                _timer_enable.store(true);
                _start_ts.store(getTicks());
                _timeout_ms.store(timeout);
            }

            bool run(const std::string &command, bool use_shell = false) {
                if (_running.load()) return false;
                _use_shell = use_shell;
                if (_use_shell) {
#ifdef TINY_CPP_MY_OS_WINDOWS
                    _command = "cmd /c " + command;
#else
                    _command = "sh -c \"" + command + "\"";
#endif
                } else {
                    _command = command;
                }
                _running.store(true);
                if (!_use_thread.load()) {
                    _use_thread.store(true);
                    _sub_thread = std::thread(&__SUBPROCESS_PIMPL__::loop, this);
                } else {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _run_var.notify_all();
                }
                std::unique_lock<std::mutex> lock(_rw_mutex);
                _started_var.wait(lock, [this] { return _notify_pid.load() || !_running.load(); });
                _notify_pid.store(false);
                return true;
            }

            int wait() {
                std::unique_lock<std::mutex> lock(_wait_done_mutex);
                _wait_done_var.wait(lock, [&] { return !_running.load(); });
                return _ret_code.load();
            }

            void stop() {
                sendSignal(15);
            }

            bool sendSignal(int signum) {
                if (!_running.load() || _sent_signal.load()) return false;
                std::lock_guard<std::mutex> lock(_rw_mutex);
                _signal.store(signum);
                _sent_signal.store(true);
                return true;
            }

            size_t pid() {
                if (!_running.load()) return 0;
                return _pid.load();
            }

            int returnCode() const {
                return _ret_code.load();
            }

            bool running() const {
                return _running.load();
            }

            bool isSentSignal() const {
                return _sent_signal.load();
            }

            bool isListenStreamEnabled() const {
                return _enable_listen_stream.load();
            }

            bool listenOutput(int64_t timeout_ms = 0, std::string* dest = nullptr) {
                if (!_enable_listen_stream.load()) return false;
                auto st = getTicks();
                int64_t ed{};
                do {
                    ed = getTicks();
                    if (_notify_output.load()) {
                        std::lock_guard<std::mutex> lock(_rw_mutex);
                        _notify_output.store(false);
                        if (dest) *dest = _cur_output;
                        return true;
                    }
                } while (timeout_ms > 0 && ed - st < timeout_ms);
                return false;
            }

            bool listenError(int64_t timeout_ms = 0, std::string* dest = nullptr) {
                if (!_enable_listen_stream.load()) return false;
                auto st = getTicks();
                int64_t ed{};
                do {
                    ed = getTicks();
                    if (_notify_error.load()) {
                        std::lock_guard<std::mutex> lock(_rw_mutex);
                        _notify_error.store(false);
                        if (dest) *dest = _cur_error;
                        return true;
                    }
                } while (timeout_ms > 0 && ed - st < timeout_ms);
                return false;
            }

            bool listenAll(int64_t timeout_ms, std::string* output, std::string* error) {
                if (!_enable_listen_stream.load() || !output || !error) return false;
                auto st = getTicks();
                int64_t ed{};
                bool is_get{};
                do {
                    ed = getTicks();
                    if (_notify_output.load()) {
                        std::lock_guard<std::mutex> lock(_rw_mutex);
                        _notify_output.store(false);
                        if (output) {
                            *output = _cur_output;
                            is_get = true;
                        }
                    }
                    if (_notify_error.load()) {
                        std::lock_guard<std::mutex> lock(_rw_mutex);
                        _notify_error.store(false);
                        if (error) {
                            *error = _cur_error;
                            is_get = true;
                        }
                    }
                    if (is_get) return true;
                } while (timeout_ms > 0 && ed - st < timeout_ms);
                return false;
            }

            bool currentOutput(std::string* dest) const {
                if (_enable_listen_stream && _use_output_pipe && dest) {
                    *dest = _cur_output;
                    return true;
                }
                return false;
            }

            bool currentError(std::string* dest) const {
                if (_enable_listen_stream && _use_error_pipe && dest) {
                    *dest = _cur_error;
                    return true;
                }
                return false;
            }

            bool output(std::string* dest) const {
                if (_use_output_pipe && dest) {
                    *dest = _output;
                    return true;
                }
                return false;
            }

            bool error(std::string* dest) const {
                if (_use_error_pipe && dest) {
                    *dest = _error;
                    return true;
                }
                return false;
            }

            __SUBPROCESS_PIMPL__(const __SUBPROCESS_PIMPL__&) = delete;
            __SUBPROCESS_PIMPL__(__SUBPROCESS_PIMPL__&&) = delete;
            __SUBPROCESS_PIMPL__& operator=(const __SUBPROCESS_PIMPL__&) = delete;
            __SUBPROCESS_PIMPL__& operator=(__SUBPROCESS_PIMPL__&&) = delete;
        private:
            void loop() {
                while (_use_thread.load()) {
                    {
                        std::unique_lock<std::mutex> lock(_mutex);
                        _run_var.wait(lock, [this] {
                            return _running.load() || !_use_thread.load();
                        });
                        if (!_use_thread.load()) {
                            break;
                        }
                    }
                    {
                        std::lock_guard<std::mutex> lock(_mutex);
                        _ret_code.store(runCommand());
                        _running.store(false);
                        _pid.store(0);
                    }
                    {
                        std::lock_guard<std::mutex> lock(_wait_done_mutex);
                        _wait_done_var.notify_all();
                    }
                }
            }

            int runCommand() {
                MySubTask my_sub_task(_command, _use_shell);
                int ret = my_sub_task.start(_use_output_pipe, _use_error_pipe);
                if (ret != 0) return ret;
                {
                    std::unique_lock<std::mutex> lock(_rw_mutex);
                    _notify_pid.store(true);
                    _pid.store(my_sub_task.pid());
                    _started_var.notify_all();
                }
                bool kill_signal{}, terminate_signal{}, timed_out{};
                while (my_sub_task.isRunning()) {
                    if (_use_output_pipe || _use_error_pipe) {
                        std::unique_lock<std::mutex> lock(_rw_mutex);
                        std::string dest;
                        if (my_sub_task.readOutputPipe(dest)) {
                            if (_enable_listen_stream) {
                                if (_notify_output.load()) {
                                    copeWithMaxStrWhenAddStr(_cur_output, dest);
                                } else {
                                    _cur_output = dest;
                                    _notify_output.store(true);
                                }
                                copeWithMaxStrWhenAddStr(_output, _cur_output);
                            } else {
                                copeWithMaxStrWhenAddStr(_output, dest);
                            }
                        }
                        if (my_sub_task.readErrorPipe(dest)) {
                            if (_enable_listen_stream) {
                                if (_notify_error.load()) {
                                    copeWithMaxStrWhenAddStr(_cur_error, dest);
                                } else {
                                    _cur_error = dest;
                                    _notify_error.store(true);
                                }
                                copeWithMaxStrWhenAddStr(_error, _cur_error);
                            } else {
                                copeWithMaxStrWhenAddStr(_error, dest);
                            }
                        }
                    }
                    if (_sent_signal.load()) {
                        auto sig = _signal.load();
                        if (sig == 9) {
                            kill_signal = true;
                        } else if (sig == 15) {
                            terminate_signal = true;
                        }
                        std::lock_guard<std::mutex> lock(_rw_mutex);
                        _sent_signal.store(false);
                        _signal.store(0);
                    }
                    if (_timer_enable.load()) {
                        auto now = getTicks();
                        auto timeout_ms = _timeout_ms.load();
                        if (timeout_ms > 0 && now - _start_ts.load() >= timeout_ms) {
                            timed_out = true;
                        }
                    }
                    if (terminate_signal || timed_out) {
                        my_sub_task.terminate();
                        kill_signal = my_sub_task.wait(1000) != 1;
                        if (!kill_signal) break;
                    }

                    if (kill_signal) {
                        my_sub_task.kill();
                        break;
                    }
                }

                if (_use_output_pipe || _use_error_pipe) {
                    std::lock_guard<std::mutex> lock(_rw_mutex);
                    std::string dest;
                    while (my_sub_task.readOutputPipe(dest)) {
                        if (_enable_listen_stream) {
                            if (_notify_output.load()) {
                                copeWithMaxStrWhenAddStr(_cur_output, dest);
                            } else {
                                _cur_output = dest;
                                _notify_output.store(true);
                            }
                            copeWithMaxStrWhenAddStr(_output, _cur_output);
                        } else {
                            copeWithMaxStrWhenAddStr(_output, dest);
                        }
                    }
                    while (my_sub_task.readErrorPipe(dest)) {
                        if (_enable_listen_stream) {
                            if (_notify_error.load()) {
                                copeWithMaxStrWhenAddStr(_cur_error, dest);
                            } else {
                                _cur_error = dest;
                                _notify_error.store(true);
                            }
                            copeWithMaxStrWhenAddStr(_error, _cur_error);
                        } else {
                            copeWithMaxStrWhenAddStr(_error, dest);
                        }
                    }
                }

                int ret_code = my_sub_task.exitCode();
                my_sub_task.clean();
                return ret_code;
            }

            std::string _cur_output{}, _cur_error{}, _command{}, _output{}, _error{};
            std::thread _sub_thread{};
            std::atomic<size_t> _pid{};
            std::atomic<int64_t> _start_ts{};
            std::atomic<uint32_t> _timeout_ms{};
            bool _use_output_pipe{}, _use_error_pipe{};
            std::atomic<bool> _running{}, _sent_signal{}, _timer_enable{}, _use_shell{}, _use_thread{};
            std::atomic<int> _ret_code{}, _signal{};
            std::atomic<bool> _notify_pid{}, _notify_output{},_notify_error{}, _enable_listen_stream{};
            std::mutex _mutex{}, _rw_mutex{}, _wait_done_mutex{};
            std::condition_variable _run_var{}, _wait_done_var{}, _started_var{};
        };
    }

    OS::Process::Child::Child(bool catch_mode, bool use_pipe2) {
        _pimpl.reset(new __SUBPROCESS_PIMPL__());
        _pimpl->setListenStreamEnabled(catch_mode);
        _pimpl->setOutputPipeEnabled(use_pipe2);
        _pimpl->setErrorPipeEnabled(use_pipe2);
    }

    OS::Process::Child::Child(const std::string &execute_path, bool catch_mode, bool use_pipe2) {
        _pimpl.reset(new __SUBPROCESS_PIMPL__());
        _pimpl->setListenStreamEnabled(catch_mode);
        _pimpl->setOutputPipeEnabled(use_pipe2);
        _pimpl->setErrorPipeEnabled(use_pipe2);
        _pimpl->run(execute_path, false);
    }

    OS::Process::Child::~Child() {
        stop();
    }

    bool OS::Process::Child::run(const std::string &programme) {
        auto new_cmd = programme;
        return _pimpl->run(new_cmd);
    }

    bool OS::Process::Child::exec(const std::string &command) {
        return _pimpl->run(command, true);
    }

    int OS::Process::Child::wait() {
        return _pimpl->wait();
    }

    int OS::Process::Child::stop(size_t timeout_ms) {
        _pimpl->setTerminateTimeout(timeout_ms);
        return _pimpl->wait();
    }

    void OS::Process::Child::terminate() {
        _pimpl->stop();
    }

    void OS::Process::Child::kill() {
        _pimpl->sendSignal(9);
    }

    size_t OS::Process::Child::pid() const {
        return _pimpl->pid();
    }

    bool OS::Process::Child::running() const {
        return _pimpl->running();
    }

    int OS::Process::Child::exitCode() const {
        return _pimpl->returnCode();
    }

    std::string OS::Process::Child::catchOutput(int64_t timeout_ms, bool* ok) const {
        if (!_pimpl->isListenStreamEnabled()) return {};
        std::string dest;
        bool ret = _pimpl->listenOutput(timeout_ms, &dest);
        if (ok) *ok = ret;
        return dest;
    }

    std::string OS::Process::Child::catchError(int64_t timeout_ms, bool* ok) const {
        if (!_pimpl->isListenStreamEnabled()) return {};
        std::string dest;
        bool ret = _pimpl->listenError(timeout_ms, &dest);
        if (ok) *ok = ret;
        return dest;
    }

    bool OS::Process::Child::catchAll(std::string &output, std::string &error, int64_t timeout_ms) const {
        return _pimpl->listenAll(timeout_ms, &output, &error);
    }

    std::string OS::Process::Child::lastOutput(bool *ok) const {
        if (!_pimpl->isListenStreamEnabled()) return {};
        std::string dest;
        if (ok) *ok = _pimpl->currentOutput(&dest);
        else _pimpl->currentOutput(&dest);
        return dest;
    }

    std::string OS::Process::Child::lastError(bool *ok) const {
        if (!_pimpl->isListenStreamEnabled()) return {};
        std::string dest;
        if (ok) *ok = _pimpl->currentError(&dest);
        else _pimpl->currentError(&dest);
        return dest;
    }

    std::string OS::Process::Child::output(bool *ok) const {
        std::string dest;
        if (ok) *ok = _pimpl->output(&dest);
        else _pimpl->output(&dest);
        return dest;
    }

    std::string OS::Process::Child::errorOutput(bool *ok) const {
        std::string dest;
        if (ok) *ok = _pimpl->error(&dest);
        else _pimpl->error(&dest);
        return dest;
    }

    int OS::Process::exec(const std::string &command, size_t timeout_ms, std::string* output, std::string* error) {
        if (output) output->clear();
        if (error) error->clear();
        if (command.empty()) return -1;
        auto st = getTicks();
        int64_t ed{};
        MySubTask my_sub_task(command, true);
        int ret = my_sub_task.start(output != nullptr, error != nullptr);
        if (ret != 0) return ret;
        std::string dest;
        do {
            ed = getTicks();
            if (output && my_sub_task.readOutputPipe(dest)) {
                copeWithMaxStrWhenAddStr(*output, dest);
            }
            if (error && my_sub_task.readErrorPipe(dest)) {
                copeWithMaxStrWhenAddStr(*error, dest);
            }
        } while ((timeout_ms == 0 || ed - st < timeout_ms) && my_sub_task.isRunning());
        if (timeout_ms > 0) {
            my_sub_task.kill();
        }
        if (output) {
            while (my_sub_task.readOutputPipe(dest)) {
                copeWithMaxStrWhenAddStr(*output, dest);
            }
        }
        if (error) {
            while (my_sub_task.readErrorPipe(dest)) {
                copeWithMaxStrWhenAddStr(*error, dest);
            }
        }
        int ret_code = my_sub_task.exitCode();
        my_sub_task.clean();
        return ret_code;
    }

    bool OS::Process::detach(const std::string &programme, size_t* pid) {
        MySubTask my_sub_task(programme, false);
        my_sub_task.setDetachModeEnabled(true);
        int ret = my_sub_task.start() == 0;
        if (pid) *pid = my_sub_task.pid();
        return ret;
    }

    bool OS::Process::kill(size_t pid, bool force) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (!force) {
            if (!GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pid)) {
                return false;
            }
            G_PID = pid;
            return EnumWindows(EnumWinEvent, 0);
        }
        auto hnd = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
        if (hnd == INVALID_HANDLE_VALUE) return false;
        auto ret = TerminateProcess(hnd, 255);
        CloseHandle(hnd);
        return ret;
#else
        int ret{};
        if (!force) {
            ret = ::kill(pid, 15);
        } else {
            ret = ::kill(pid, 9);
        }
        return ret == 0;
#endif
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
        if (!Path::isFile(path.path())) return false;
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
        if (path.isValid()) {
            return path.isDirectory();
        }
        auto create_dir = mkDirCompletely(path);
#ifdef TINY_CPP_MY_OS_WINDOWS
        while (!create_dir.empty()) {
            auto ok = CreateDirectoryA(create_dir.front().c_str(), nullptr);
            if (ok == 0) return false;
            create_dir.pop_front();
        }
#elif defined(TINY_CPP_MY_OS_UNIX)
        while (!create_dir.empty()) {
            if (mkdir(create_dir.front().c_str(), 0777 - umask(0000)) == -1) return false;
            create_dir.pop_front();
        }
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
        auto create_dir = mkDirCompletely(my_path);
#ifdef TINY_CPP_MY_OS_WINDOWS
        while (!create_dir.empty()) {
            auto ok = CreateDirectoryA(create_dir.front().c_str(), nullptr);
            if (ok == 0) return false;
            create_dir.pop_front();
        }
#elif defined(TINY_CPP_MY_OS_UNIX)
        while (!create_dir.empty()) {
            if (mkdir(create_dir.front().c_str(), 0777 - umask(0000)) == -1) return false;
            create_dir.pop_front();
        }
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
        auto ok = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777 - umask(0000));
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
        if (!access(link_dest.c_str(), F_OK)) {
            unlink(link_dest.c_str());
        }
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
        if (!access(link_dest.path().c_str(), F_OK)) {
            unlink(link_dest.path().c_str());
        }
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
        int dest_fd = open(dest.path().c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777 - umask(0000));
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
        int dest_fd = open(dest.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777 - umask(0000));
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
        if (FAILED(SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, user))) {
            return Path("");
        }
        return Path(user);
#elif defined(TINY_CPP_MY_OS_UNIX)
        auto home_path = std::getenv("HOME");
        return Path(home_path);
#endif
    }

    OS::Path OS::FileSystem::cachePath() {
        auto cache_path = homePath() / ".cache";
        if (!cache_path.isValid()) {
            mkDir(cache_path);
            cache_path.reset();
        }
        return cache_path;
    }

    OS::Path OS::FileSystem::localDataPath() {
        std::string local_data(256, 0);
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (FAILED(SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, &local_data[0]))) {
            return Path("");
        }
#elif defined(TINY_CPP_MY_OS_UNIX)
        if (isAdmin()) {
            local_data = "/root/.local/share";
            mkDir(local_data);
        } else {
            local_data = std::getenv("HOME");
            local_data += "/.local/share";
            mkDir(local_data);
        }
#endif
        return Path(local_data);
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

    OS::FileSystem::LayerMap OS::FileSystem::listPathEx(const Path &path,
                uint8_t recursion_count,
                const std::function<bool(const Path &, bool &)> &found_event) {
        if (!path.isValid()) return {};
        if (recursion_count == 0) recursion_count = 255;
        bool st_all{};
        return listAllPaths(path, 1, recursion_count, st_all, found_event);
    }

    OS::FileSystem::LayerMap OS::FileSystem::listPathEx(const std::string &path,
                uint8_t recursion_count,
                const std::function<bool(const Path &, bool &)> &found_event) {
        auto current_path = Path(".");
        if (!current_path.isValid()) return {};
        if (recursion_count == 0) recursion_count = 255;
        bool st_all{};
        return listAllPaths(current_path, 1, recursion_count, st_all, found_event);
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

    std::deque<std::string> OS::FileSystem::mkDirCompletely(const Path &path) {
        std::deque<std::string> paths;
        Path current_path(path);
        while (!current_path.isValid()) {
            paths.push_front(current_path.path());
            current_path.upper();
        }
        return paths;
    }

    std::vector<OS::Path> OS::FileSystem::listAllPath(const Path &path, uint8_t current_recursion,
                                                      uint8_t recursion_count,
                                                      const std::function<bool(const Path &)> &filter) {
        std::vector<Path> paths;
        if (current_recursion > recursion_count) return paths;
#ifdef TINY_CPP_MY_OS_WINDOWS
        WIN32_FIND_DATAW res = {};
        auto str = string2Wide(path.path());
        wchar_t find_string[MAX_PATH];
        wsprintfW(find_string, L"%s\\*", str.c_str());
        auto iter = FindFirstFileW(find_string, &res);
        if (iter == INVALID_HANDLE_VALUE) return paths;
        do {
            auto short_file_name = wide2String(res.cFileName);
            auto new_found = path.path() + "\\" + short_file_name;
            if (short_file_name == "." || short_file_name == "..") continue;
            Path new_path(new_found);
            if (new_path.isDirectory()) {
                auto found_path = listAllPath(new_path, current_recursion + 1, recursion_count, filter);
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
                auto found_path = listAllPath(new_found, current_recursion + 1, recursion_count, filter);
                paths.insert(paths.end(), found_path.begin(), found_path.end());
            }
            if (filter && !filter(new_found)) continue;
            paths.emplace_back(new_found);
        } while ((iter = readdir(dir)) != nullptr);

        closedir(dir);
#endif
        return paths;
    }

    OS::FileSystem::LayerMap OS::FileSystem::listAllPaths(const Path &path,
                                  uint8_t current_recursion, uint8_t recursion_count, bool &stop_all,
                                  const std::function<bool(const Path &, bool &)> &found_event) {
        LayerMap paths;
        if (current_recursion > recursion_count) return paths;
        paths.insert({current_recursion, {}});
        auto& cur_paths = paths[current_recursion];
#ifdef TINY_CPP_MY_OS_WINDOWS
        WIN32_FIND_DATAW res = {};
        auto str = string2Wide(path.path());
        wchar_t find_string[MAX_PATH];
        wsprintfW(find_string, L"%s\\*", str.c_str());
        auto iter = FindFirstFileW(find_string, &res);
        if (iter == INVALID_HANDLE_VALUE) return paths;
        bool stop_now = false;
        do {
            auto short_file_name = wide2String(res.cFileName);
            auto new_found = path.path() + "\\" + short_file_name;
            if (short_file_name == "." || short_file_name == "..") continue;
            Path new_path(new_found);
            if (new_path.isDirectory()) {
                auto found_path = listAllPaths(new_path, current_recursion + 1, recursion_count, stop_all, found_event);
                if (stop_all) break;
                for (auto& ff_path : found_path) {
                    auto& LV = ff_path.first;
                    if (paths.find(LV) != paths.end()) {
                        auto& temp_group = paths.at(LV);
                        temp_group.insert(temp_group.end(), ff_path.second.begin(), ff_path.second.end());
                    } else {
                        paths.emplace(LV, ff_path.second);
                    }
                }
            }
            if (found_event) {
                if (found_event(new_path, stop_now)) {
                    cur_paths.emplace_back(new_path);
                }
            } else {
                cur_paths.emplace_back(new_path);
            }
            if (stop_now) {
                stop_all = true;
                break;
            }
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
        bool stop_now = false;
        do {
            std::string short_file_name = iter->d_name;
            if (short_file_name == "." || short_file_name == "..") continue;
            auto full_path = path.path() + "/" + short_file_name;
            Path new_path(full_path);
            if (new_path.isDirectory()) {
                auto found_path = listAllPaths(new_path, current_recursion + 1, recursion_count, stop_all, found_event);
                if (stop_all) break;
                for (auto& ff_path : found_path) {
                    auto& LV = ff_path.first;
                    if (paths.find(LV) != paths.end()) {
                        auto& temp_group = paths.at(LV);
                        temp_group.insert(temp_group.end(), ff_path.second.begin(), ff_path.second.end());
                    } else {
                        paths.emplace(LV, ff_path.second);
                    }
                }
            }
            if (found_event) {
                if (found_event(new_path, stop_now)) {
                    cur_paths.emplace_back(new_path);
                }
            } else {
                cur_paths.emplace_back(new_path);
            }
            if (stop_now) {
                stop_all = true;
                break;
            }
        } while ((iter = readdir(dir)) != nullptr);

        closedir(dir);
#endif
        return paths;
    }

    void OS::FileSnapShot::addPath(Path *path) {
        bool is_found = false;
        if (!_last_paths_snapshot.empty()) {
            is_found = std::find(_last_paths_snapshot.begin(), _last_paths_snapshot.end(), path) != _last_paths_snapshot.end();
        }
        if (!is_found) {
            _last_paths_snapshot.emplace_back(path);
        }
    }

    void OS::FileSnapShot::addPath(Path &path) {
        addPath(&path);
    }

    void OS::FileSnapShot::removePath(const std::string &path) {
        if (_last_paths_snapshot.empty()) return;
        auto iter = std::find_if(_last_paths_snapshot.begin(), _last_paths_snapshot.end(),
            [&path] (const Path* p) {
                Path my_path(path);
                return p->path() == my_path.path();
            });
        if (iter != _last_paths_snapshot.end()) {
            _last_paths_snapshot.erase(iter);
        }
    }

    void OS::FileSnapShot::removePath(Path *path) {
        if (_last_paths_snapshot.empty()) return;
        auto iter = std::find(_last_paths_snapshot.begin(), _last_paths_snapshot.end(), path);
        if (iter != _last_paths_snapshot.end()) {
            _last_paths_snapshot.erase(iter);
        }
    }

    void OS::FileSnapShot::removePath(Path &path) {
        removePath(&path);
    }

    void OS::FileSnapShot::clearAll() {
        _last_paths_snapshot.clear();
    }

    void OS::FileSnapShot::setTrigger(Trigger trigger) {
        _trigger = trigger;
    }

    bool OS::FileSnapShot::check(uint32_t check_count, uint32_t check_delay_ms, std::vector<uint64_t> *result) {
        if (_result_map.empty()) init();
        if (result && !result->empty()) result->clear();
        size_t found = 0;
        auto func = [this, &result, &found]() {
            compare();
            bool ok{};
            for (auto& pair: _result_map) {
                if (pair.second.trigger) {
                    if (result && std::find(result->begin(), result->end(), pair.first) == result->end()) {
                        result->emplace_back(pair.first);
                    }
                    found++;
                    if (!ok) ok = true;
                }
            }
            endCapture();
            return ok;
        };

        int64_t st = getTicks(), ed = 0;
        while (true) {
            if (!check_count) break;
            ed = getTicks();
            if (ed - st > check_delay_ms) {
                if (func()) break;
                check_count--;
                st = getTicks();
            }
        }

        return found > 0;
    }

    uint8_t OS::FileSnapShot::trigger() const {
        return _trigger;
    }

    void OS::FileSnapShot::init() {
        for (auto path: _last_paths_snapshot) {
            Result result;
            result.before = makeSnapShot(*path);
            _result_map[path->uniqueID()] = result;
        }
    }

    void OS::FileSnapShot::compare() {
        for (auto & path: _last_paths_snapshot) {
            path->reset();
            _result_map[path->uniqueID()].after = makeSnapShot(*path);
            auto& res = _result_map[path->uniqueID()];
            if ((_trigger & Filename) && (res.before.meta_data._path != res.after.meta_data._path)) {
                res.trigger |= Filename;
            }
            if ((_trigger & Filesize) && (res.before.meta_data._file_size != res.after.meta_data._file_size)) {
                res.trigger |= Filesize;
            }
            if ((_trigger & Permission) && (res.before.meta_data._permission != res.after.meta_data._permission)) {
                res.trigger |= Permission;
            }
            if (_trigger & Timestamp) {
                bool b1 = res.before.meta_data._last_write_time != res.after.meta_data._last_write_time;
                bool b2 = res.before.meta_data._last_create_time != res.after.meta_data._last_create_time;
                if (b1 || b2) {
                    res.trigger |= Timestamp;
                }
            }
            if (_trigger & Owner) {
                bool b1 = res.before.meta_data.owner != res.after.meta_data.owner;
                bool b2 = res.before.meta_data.group != res.after.meta_data.group;
                if (b1 || b2) {
                    res.trigger |= Owner;
                }
            }
            if (_trigger & Context) {
                bool ok1{}, ok2{};
                if (res.before.detect_file) {
                    if (res.before.files_crc32 == res.after.files_crc32) {
                        _trigger |= Context;
                    }
                }
            }
        }
    }

    void OS::FileSnapShot::endCapture() {
        for (auto & path: _last_paths_snapshot) {
            _result_map[path->uniqueID()].before = _result_map[path->uniqueID()].after;
            _result_map[path->uniqueID()].after = {};
            _result_map[path->uniqueID()].trigger = 0;
        }
    }

    OS::FileSnapShot::Snapshot OS::FileSnapShot::makeSnapShot(const Path &path) {
        Snapshot result;
        auto& meta = result.meta_data;
        meta._path = path.path();
        meta._type = path.fileType();
        meta._permission[0] = path.userPermission();
        meta._permission[1] = path.groupPermission();
        meta._permission[2] = path.otherPermission();
        meta._last_access_time = path.lastAccessTime();
        meta._last_write_time = path.lastWriteTime();
        meta._last_create_time = path.lastCreateTime();
        meta._node_id = path.nodeID();
        meta._file_size = path.fileSize();
        meta.owner = path.ownerName();
        meta.group = path.groupName();
        if (path.isDirectory()) {
            result.detect_file = false;
        } else if (path.isValid()) {
            result.detect_file = true;
            if (_trigger & Context) {
                File file(path, ReadOnly);
                if (file.isOpen()) {
                    if (path.fileSize() <= 2048) {
                        auto datas = file.read(2048);
                        result.files_crc32.before.cur_crc32 = CRC32Utils::calc(datas.data(), datas.size());
                    } else {
                        auto bg = file.read(2048);
                        file.moveTo(-2048, File::Where::End);
                    }
                    file.close();
                }
            }
        }
        return result;
    }
}
