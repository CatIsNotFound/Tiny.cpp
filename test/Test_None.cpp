#include "../src/Tiny.hpp"
#include <atomic>
using namespace Tiny;

struct Position {
    float x, y;
};

template <>
struct Tiny::TUI::Format::Formatter<Position> {
    static void format(std::string& dest, const ::Position& value, const FmtSpec& fmt) {
        Sink sink{dest};
        if (fmt.data_type == 'p') {
            auto p = reinterpret_cast<size_t>(&value);
            formatTo(sink, digitToString(p, 16, false), fmt);
            return;
        }
        if (fmt.data_type == 's' || fmt.data_type == '\0') {
            std::string s("(");
            s += std::to_string(value.x);
            s += ", ";
            s += std::to_string(value.y);
            s += ')';
            formatTo(sink, s, fmt);
        }
    }
};

std::string status(OS::Process::Status status) {
    switch (status) {
    case OS::Process::Status::Unknown:
        // return "😵";
        return "?";
    case OS::Process::Status::Running:
        // return "∑";
        return "R";
    case OS::Process::Status::Sleeping:
        // return "💤";
        return "S";
    case OS::Process::Status::UninterruptibleSleep:
        // return "💦";
        return "U";
    case OS::Process::Status::Stopped:
        // return "🚫";
        return "T";
    case OS::Process::Status::Zombie:
        // return "💀";
        return "Z";
    case OS::Process::Status::Terminated:
        // return "❌";
        return "X";
    default:
        // return "😵";
        return "?";
    }
}

using namespace TUI;
using namespace OS;

static StyleList generate() {
    StyleList style_list;
    Style s_normal, s_bold, s_err, s_selected, s_sorted, s_header, s_low, s_warn, s_high;
    s_normal.intensity = 2;
    s_normal.fg_color = Color::Default;
    s_normal.bg_color = Color::Default;
    s_bold.intensity = 2;
    s_bold.fg_color = Color::Blue;
    s_bold.bg_color = Color::Default;
    s_err.intensity = 2;
    s_err.fg_color = Color::Red;
    s_err.bg_color = Color::Black;
    s_selected.intensity = 2;
    s_selected.property |= Style::Reverse;
    s_sorted.intensity = 1;
    s_sorted.fg_color = Color::Black;
    s_sorted.bg_color = Color::Green;
    s_low.intensity = 2;
    s_low.fg_color = Color::Green;
    s_low.bg_color = Color::Default;
    s_warn.intensity = 2;
    s_warn.fg_color = Color::Yellow;
    s_warn.bg_color = Color::Default;
    s_high.intensity = 2;
    s_high.fg_color = Color::Red;
    s_high.bg_color = Color::Default;
    style_list.push_back(s_normal);
    style_list.push_back(s_bold);
    style_list.push_back(s_err);
    style_list.push_back(s_selected);
    style_list.push_back(s_sorted);
    style_list.push_back(s_header);
    style_list.push_back(s_low);
    style_list.push_back(s_warn);
    style_list.push_back(s_high);
    return style_list;
}

static StyleList styles;
enum Header : uint8_t {
    H_PID, H_PPID, H_STATES, H_CPU, H_MEM, H_USER, H_PRG_NAME
};

#define HEADER_COUNT 7
static std::atomic<uint8_t> sorted_header_no{};
static std::atomic<bool> reversed_sort{};
static std::vector<Process::Process> proc_list;
static std::vector<Process::Process*> filter_list;
static CPU cpu;
static Memory memory;
static uint64_t delay_ms{1000};
static uint64_t skip_index{};
static std::atomic<bool> update_info{}, update_proc{}, from_user{}, running_main{}, filter_mode{};
struct CpuFilter {
    float val{};
    int8_t opt{};
};

struct MemFilter {
    size_t val{};
    int8_t opt{};
};
static std::tuple<size_t, size_t, Process::Status, CpuFilter, MemFilter, std::string, std::string> filter_tuple{};

static const char* headerName(Header header) {
    switch (header) {
        case H_PID:
            return "PID";
        case H_PPID:
            return "PPID";
        case H_STATES:
            return "State";
        case H_CPU:
            return "%CPU";
        case H_MEM:
            return "%Mem";
        case H_USER:
            return "User";
        case H_PRG_NAME:
            return "Name";
    }
    return "";
}

static uint64_t str2Ull(const std::string& s, bool& ok) {
    uint64_t ret{};
    for (auto& ch: s) {
        if (!std::isdigit(ch)) {
            ok = false;
            return 0;
        }
        ret = ret * 10 + (ch - '0');
    }
    ok = true;
    return ret;
}

static float str2float(const std::string& s, bool& ok, uint8_t limit_precision = 2) {
    float ret{};
    bool pre_mode{};
    float pre{1.f};
    for (auto& ch: s) {
        if (ch == '.') {
            if (!pre_mode) {
                pre_mode = true;
                continue;
            }
            ok = false;
            return 0;
        }
        if (!std::isdigit(ch)) {
            ok = false;
            return 0;
        }
        if (pre_mode) {
            pre *= 0.1f;
            if (pre < powf(10, 0.f - static_cast<float>(limit_precision))) {
                break;
            }
            ret += pre * static_cast<float>(ch - '0');
        } else {
            ret = ret * 10 + static_cast<float>(ch - '0');
        }
    }
    ok = true;
    return ret;
}

static std::string strip(const std::string &s) {
    if (s.empty()) return {};
    size_t st_pos{}, ed_pos{s.size()};
    bool found_st{}, found_ed{};
    for (size_t i = 0; i < s.size(); ++i) {
        if (st_pos > ed_pos) break;
        if (!found_st && !isspace(s[i]))              { st_pos = i; found_st = true; }
        if (!found_ed && !isspace(s[ed_pos - i - 1])) { ed_pos = ed_pos - i - 1; found_ed = true; }
        if (found_st && found_ed) break;
    }
    if (!found_st && !found_ed) return {};
    return s.substr(st_pos, ed_pos - st_pos + 1);
}

void draw() {
    auto scr_size = Terminal::screenSize();
    auto& renderer = Renderer::self();

    if (update_info.load()) {
        auto now = DT::currentTimestamps(true);
        auto mem_usage = static_cast<double>(memory.used_ram) / static_cast<double>(memory.total_ram) * 100.f;
        auto mem_avail = static_cast<double>(memory.available_ram) / static_cast<double>(memory.total_ram) * 100.f;
        renderer.setStrF({0, 0},
            "Uptime: {:12s}, CPU: {} core(s), {:5.1f}% total, Memory: {:5.1f}% used, {:5.1f}% available",
            DT::formatTime("d:hh:mm:ss", now),  cpu.cores, cpu.total_usage, mem_usage, mem_avail);
        auto cpu_width = scr_size.width / 2 - 1;
        auto cpu_bar_width = cpu_width - 4;
        /// 01[||||||||]  02[|||||||||]
        uint32_t shown_cpu = std::min(cpu.cores, 8U);
        for (uint32_t i = 0; i < shown_cpu; ++i) {
            auto c = static_cast<uint32_t>((static_cast<float>(cpu_bar_width) * (cpu.usages[i] * 0.01f)));
            auto prg = Terminal::formatString("{:{}c}", c, '|').substr(0, cpu_bar_width);
            renderer.setSSFX({i / 2 + 1, (i + 1) % 2 ? 0 : cpu_width}, "<1>{:*0>2#d}:<{}>{:<{}s}", styles,
                i + 1, cpu.usages[i] >= 85.f ? 9 : (cpu.usages[i] >= 60.f ? 8 : 7), cpu_bar_width, prg);
        }
        renderer.fillRows(5, 5);
        renderer.setStrF({5, 0}, "Process: {:>5s}/{:<5s}",
            skip_index, proc_list.size());
        update_info.store(false);
    }
    if (from_user.load()) {
        uint32_t col = 0;
        const uint32_t PW = scr_size.width - (7 * 5 + 17 + 11);
        for (uint32_t i = 0; i < HEADER_COUNT; ++i) {
            uint32_t col_width = 6;
            if (i == H_MEM) col_width = 10;
            if (i == H_USER) col_width = 16;
            if (i == H_PRG_NAME) col_width = PW;
            if (sorted_header_no == i) {
                renderer.setSSFX({6, col}, "<5>{}{:<{}s}", styles,
                    reversed_sort ? "▼" : "▲", col_width, headerName(static_cast<Header>(i)));
            } else {
                renderer.setSSFX({6, col}, "<4> {:<{}s}", styles,
                    col_width, headerName(static_cast<Header>(i)));
            }
            col += col_width;
        }
        from_user.store(false);
    }

    if (update_proc.load()) {
        const uint32_t PW = scr_size.width - (7 * 5 + 17 + 11);
        const uint32_t PH = scr_size.height - 8;

        uint32_t idx = 0, i = 0;
        for (auto& process: proc_list) {
            if (i++ < skip_index) continue;
            DataUnit data_unit;
            auto sz = autoConvertDataSize(process.memory, data_unit);
            renderer.setSSFX({7 + idx, 0}, "{:>6s}{:>6s}{:>6s}{:>6.1f} {:>6.1f}{:<3s}{:>16s} {:<{}s}", styles,
                process.pid, process.ppid, status(process.status), process.cpu, sz, dataUnitName(data_unit), process.user_name,
                PW, process.exe_path.substr(0, PW));
            ++idx;
            if (idx >= PH) break;
        }
        if (idx < PH) {
            do {
                renderer.fillRows(7 + idx, 1);
            } while (++idx >= PH);
        }
        update_proc.store(false);
    }
    renderer.present();
}

bool look_for(Header header) {
    auto& renderer = Renderer::self();
    // auto scr_size = Terminal::screenSize();
    std::array<const char*, HEADER_COUNT> range_tips = {
        "",
        "",
        "[R = running, S = sleeping, T = stopped, Z = zombie, ...]",
        "[Format: [>|=|<]<usage%> e.g: 30, >30, <30]",
        "[Format: [>|=|<]<size>[k|m|g]   e.g: 30k, >40m, <50g]",
        "",
        ""
    };
    renderer.fillRows(5, 5, ' ', styles[3]);
    renderer.setSSFX({5, 0}, "<4>Search {}: {} ", styles, headerName(header), range_tips[header]);
    renderer.present();
    Terminal::setCursorVisible(true);
#ifdef TINY_CPP_MY_OS_WINDOWS
    auto value = Terminal::readLine();
#else
    auto value = Terminal::readLine();
#endif
    Terminal::setCursorVisible(false);
    size_t v1{}, v2{};
    Process::Status v3{};
    CpuFilter v4{};
    MemFilter v5{};
    std::string v6, v7;
    uint8_t off{1};
    bool is_ok{};
    switch (header) {
        case H_PID:
            v1 = str2Ull(value, is_ok);
            if (is_ok) {
                std::get<0>(filter_tuple) = v1;
            }
            break;
        case H_PPID:
            v2 = str2Ull(value, is_ok);
            if (is_ok) {
                std::get<1>(filter_tuple) = v2;
            }
            break;
        case H_STATES:
            if (!value.empty()) {
                for (char c: value) {
                    if (c == 'R' || c == 'r') {
                        v3 = Process::Status::Running;
                        break;
                    } else if (c == 'S' || c == 's') {
                        v3 = Process::Status::Sleeping;
                        break;
                    } else if (c == 'U' || c == 'u') {
                        v3 = Process::Status::UninterruptibleSleep;
                        break;
                    } else if (c == 'Z' || c == 'z') {
                        v3 = Process::Status::Zombie;
                        break;
                    } else if (c == 'X' || c == 'x') {
                        v3 = Process::Status::Terminated;
                        break;
                    } else if (c == '?') {
                        v3 = Process::Status::Unknown;
                        break;
                    }
                }
                if (is_ok) {
                    std::get<2>(filter_tuple) = v3;
                }
            }
            break;
        case H_CPU:
            if (value.empty()) break;
            if (isdigit(value[0]) || value[0] == '=') {
                v4.opt = 0;
            } else if (value[0] == '>') {
                v4.opt = -1;
            } else if (value[0] == '<') {
                v4.opt = 1;
            }
            if (value.size() == 1) {
                if (isdigit(value[0])) {
                    v4.val = static_cast<float>(value[0] - '0');
                    std::get<3>(filter_tuple) = v4;
                    is_ok = true;
                }
                break;
            }
            off = 1;
            if (value[1] == '=') {
                v4.opt = v4.opt + v4.opt;
                off += 1;
            }
            v4.val = str2float(value.substr(off), is_ok);
            if (is_ok) {
                std::get<3>(filter_tuple) = v4;
            }
            break;
        case H_MEM:
            if (value.empty()) break;
            if (isdigit(value[0]) || value[0] == '=') {
                v5.opt = 0;
            } else if (value[0] == '>') {
                v5.opt = -1;
            } else if (value[0] == '<') {
                v5.opt = 1;
            }
            if (value.size() == 1) {
                if (isdigit(value[0])) {
                    v5.val = static_cast<size_t>(value[0] - '0');
                    std::get<4>(filter_tuple) = v5;
                    is_ok = true;
                }
                break;
            }
            off = 1;
            if (value[1] == '=') {
                v5.opt = v5.opt + v5.opt;
                off += 1;
            }
            v5.val = str2Ull(value.substr(off), is_ok);
            if (is_ok) {
                std::get<4>(filter_tuple) = v5;
            }
            break;
        case H_USER:
            value = strip(value);
            if (value.empty()) break;
            std::get<5>(filter_tuple) = value;
            is_ok = true;
            break;
        case H_PRG_NAME:
            value = strip(value);
            if (value.empty()) break;
            std::get<6>(filter_tuple) = value;
            is_ok = true;
            break;
    }
    if (!is_ok) {
        renderer.fillRows(5, 5, ' ', styles[3]);
        renderer.setSSFX({5, 0}, "<4>Invalid value! Press any key to continue... ", styles);
        renderer.present();
        Terminal::getKey();
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    auto st = DT::currentTimestamps(), ed = st;
    styles = generate();
    running_main.store(true);
    Renderer::self().setResizeEvent([](Renderer& renderer) {
        if (!filter_mode.load()) {
            from_user.store(true);
        }
        renderer.fillScreen();
    });
    EV::Event ev1(1, "update", []{ return true; }, [](const std::atomic<bool>&) {
        if (!running_main.load() || update_info.load()) return;
        getCPUInfo(cpu, delay_ms);
        getMemory(memory);
        update_info.store(true);
    });
    ev1.setDelayMS(1);
    ev1.setRepeatCount(0);
    ev1.run();
    EV::Event ev2(2, "update_proc", []{ return true; }, [](const std::atomic<bool>&) {
        if (!running_main.load() || update_proc.load()) return;
        if (proc_list.empty()) proc_list = Process::listProcess(true, delay_ms);
        else Process::updateProcessList(proc_list, true, delay_ms);
        std::sort(proc_list.begin(), proc_list.end(), [](const Process::Process& p1, const Process::Process& p2) {
            switch (sorted_header_no) {
                case H_PID:
                    return reversed_sort ? (p1.pid > p2.pid) : (p1.pid < p2.pid);
                case H_PPID:
                    return reversed_sort ? (p1.ppid > p2.ppid) : (p1.ppid < p2.ppid);
                case H_STATES:
                    return reversed_sort ? (p1.status > p2.status) : (p1.status < p2.status);
                case H_CPU:
                    return reversed_sort ? (p1.cpu > p2.cpu) : (p1.cpu < p2.cpu);
                case H_MEM:
                    return reversed_sort ? (p1.memory > p2.memory) : (p1.memory < p2.memory);
                case H_USER:
                    for (size_t i = 0; i < std::min(p1.user_name.size(), p2.user_name.size()); ++i) {
                        if (p1.user_name[i] == p2.user_name[i]) continue;
                        auto c1 = p1.user_name[i];
                        auto c2 = p2.user_name[i];
                        return reversed_sort ? c1 > c2 : c1 < c2;
                    }
                    break;
                case H_PRG_NAME:
                    for (size_t i = 0; i < std::min(p1.exe_path.size(), p2.exe_path.size()); ++i) {
                        if (p1.exe_path[i] == p2.exe_path[i]) continue;
                        return reversed_sort ? p1.exe_path[i] > p2.exe_path[i] : p1.exe_path[i] < p2.exe_path[i];
                    }
                    break;
                default:
                    break;
            }
            return false;
        });
        update_proc.store(true);
    });
    ev2.setDelayMS(1);
    ev2.setRepeatCount(0);
    ev2.run();
    from_user.store(true);
    // update_info.store(true);
    // update_proc.store(true);
    while (running_main.load()) {
        ed = DT::currentTimestamps();
        auto input = Terminal::getInput();
        if (input.type == InputEvent::K && input.input.keyboard.is_pressed) {
            int next{};
            switch (input.input.keyboard.key) {
                case KEY_ESC:
                case KEY_CTRL_C:
                    running_main.store(false);
                    break;
                case KEY_TAB:
                    next = sorted_header_no.load() + 1;
                    if (next >= HEADER_COUNT) next = 0;
                    sorted_header_no.store(next);
                    break;
                case 'R':
                case 'r':
                    reversed_sort.store(!reversed_sort.load());
                    break;
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                    ev1.stop();
                    ev2.stop();
                    look_for(static_cast<Header>(input.input.keyboard.key - '0'));
                    from_user.store(true);
                    ev1.run();
                    ev2.run();
                    break;
                case KEY_SPECIAL:
                    switch (input.input.keyboard.sp_key) {
                        case SP_KEY_F1:
                            sorted_header_no.store(H_PID);
                            break;
                        case SP_KEY_F2:
                            sorted_header_no.store(H_PPID);
                            break;
                        case SP_KEY_F3:
                            sorted_header_no.store(H_STATES);
                            break;
                        case SP_KEY_F4:
                            sorted_header_no.store(H_CPU);
                            break;
                        case SP_KEY_F5:
                            sorted_header_no.store(H_MEM);
                            break;
                        case SP_KEY_F6:
                            sorted_header_no.store(H_USER);
                            break;
                        case SP_KEY_F7:
                            sorted_header_no.store(H_PRG_NAME);
                            break;
                        case SP_KEY_DOWN:
                            skip_index = std::min(skip_index + 1, proc_list.size() - 1);
                            break;
                        case SP_KEY_UP:
                            skip_index = std::max(skip_index - 1, static_cast<uint64_t>(0));
                            break;
                        case SP_KEY_HOME:
                            skip_index = 0;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            from_user.store(true);
        }
        if (ed - st >= 50) {
            st = ed;
            draw();
        }
    }
    return 0;
}
