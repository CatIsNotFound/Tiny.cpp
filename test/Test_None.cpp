#include "../src/Tiny.hpp"
#include <atomic>
#include <climits>
#ifndef SIZE_T_MAX
#define SIZE_T_MAX SIZE_MAX
#endif
using namespace Tiny;

struct ColsRange {
    uint16_t start{}, end{};
    ColsRange(uint16_t start, uint16_t end) : start{start}, end{end} {}
};

template <typename T>
T clamp(const T& value, const T& min, const T& max) {
    return value < min ? min : (value > max ? max : value);
}

template <typename T>
T min(const T& v1, const T& v2) {
    return v1 < v2 ? v1 : v2;
}

template <typename T>
T max(const T& v1, const T& v2) {
    return v1 < v2 ? v2 : v1;
}

std::string status(OS::Process::Status status) {
    switch (status) {
    case OS::Process::Status::Unknown:
        // return "😵";
        return "?";
    case OS::Process::Status::Running:
        // return "∑";
        return "R";
    case OS::Process::Status::Idle:
        return "I";
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
    Style s_normal, s_bold, s_err, s_title, s_sorted, s_header, s_low, s_warn, s_high, s_pids;
    s_normal.intensity = 2;
    s_normal.fg_color = Color::Default;
    s_normal.bg_color = Color::Default;
    s_bold.intensity = 2;
    s_bold.fg_color = Color::Blue;
    s_bold.bg_color = Color::Default;
    s_err.intensity = 2;
    s_err.fg_color = Color::Red;
    s_err.bg_color = Color::Black;
    s_title.intensity = 1;
    s_title.fg_color = Color::Black;
    s_title.bg_color = Color::Cyan;
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
    s_pids.fg_color = Color::White;
    s_pids.bg_color = Color::Blue;
    s_pids.intensity = 3;
    style_list.push_back(s_normal);
    style_list.push_back(s_bold);
    style_list.push_back(s_err);
    style_list.push_back(s_title);
    style_list.push_back(s_sorted);
    style_list.push_back(s_header);
    style_list.push_back(s_low);
    style_list.push_back(s_warn);
    style_list.push_back(s_high);
    style_list.push_back(s_pids);
    return style_list;
}

static StyleList styles;
enum Header : uint8_t {
    H_PID, H_PPID, H_STATES, H_CPU, H_MEM, H_USER, H_PRG_NAME, H_NONE
};

#define HEADER_COUNT 7
static std::atomic<bool> show_cursor{}, changed_cur_pos{};
static std::atomic<uint8_t> sorted_header_no{};
static std::atomic<bool> reversed_sort{};
static std::vector<Process::Process> proc_list;
static std::vector<Process::Process> filter_list;
static CPU cpu;
static Memory memory;
static uint64_t delay_ms{1000};
static uint64_t skip_index{}, skip_filter_index{}, skip_help_text{};
static uint8_t skip_cpu_group{};
static std::vector<size_t> selected_pids;
static std::atomic<bool> disabled_mode{}, help_mode{};
static std::atomic<bool> update_proc{}, from_user{}, running_main{}, search_mode{};
static std::atomic<bool> draw_info{}, draw_proc{}, multiselect{}, filter_mode{};
static Header search_header{};
static std::mutex filter_mutex;
static std::condition_variable filter_con_var{};
static Tiny::TUI::Size screen_size{};
static Tiny::TUI::Position mouse_pos{};
static std::string help_infos = "<1>TOP<2> is a simplified version of the process monitor, which belongs to one of the <1>Tiny.cpp<2> projects.\n " \
"<2>This page only introduces its basic operation methods and corresponding functions.\n\n" \
"<2>p.s: This sample program does not guarantee smooth operation. If you can develop a better program than this, please feel free to showcase your own program at any time. :)\n \n" \
"<3>[Keyboard Shortcuts]\n" \
"<1>Tab             <2>Cycle sort column forward\n" \
"<1>Shift+Tab       <2>Cycle sort column backward\n" \
"<1>P/p             <2>Scroll CPU core groups\n" \
"<1>R/r             <2>Toggle ascending/descending sort\n" \
"<1>1-7             <2>Enter column filter mode\n" \
"<1>   1                <2>Search PID\n" \
"<1>   2                <2>Search PPID\n" \
"<1>   3                <2>Search Status\n" \
"<1>   4                <2>Search \%CPU\n" \
"<1>   5                <2>Search Memory usage\n" \
"<1>   6                <2>Search Username\n" \
"<1>   7                <2>Search Programme name (Command line)\n" \
"<1>Backspace/Del   <2>Exit filter mode\n" \
"<1>↑ ↓             <2>Scroll process list\n" \
"<1>Home / End      <2>Jump list top / bottom\n" \
"<1>PageUp/PageDown <2>Page scroll\n" \
"<1>F1 / ?          <2>Show help information\n" \
"<1>F12             <2>Toggle mouse highlight cursor\n" \
"<1>Ctrl+R          <2>Toggle multi-select\n" \
"<1>Ctrl+D          <2>Clear PID selections\n" \
"<1>Esc / Ctrl+C    <2>Quit\n" \
" \n" \
"<3>[Mouse]\n" \
"<1>Move              <2>Hover highlight (<1>F12<2> on)\n" \
"<1>Wheel Up/Down     <2>Scroll process list or CPU groups\n" \
"<1>Left-Click Row    <2>Select / deselect process PID\n" \
"<1>Left-Click Header <2>Set sort column, toggle sort direction\n" \
" \n" \
"<3>[Filter]\n" \
"<1> PID/PPID: <2>numeric <1>0~9<2>; \n" \
"<1>    State: <2>only support input for below: \n" \
"              <1>R<2> (Running)\n" \
"              <1>S<2> (Sleep) \n" \
"              <1>T<2> (Stopped) \n" \
"              <1>Z<2> (Zombie) \n" \
"              <1>U<2> (Uninterruptible Sleep) \n" \
"              <1>I<2> (Idle) \n" \
"              <1>X<2> (Terminated) \n" \
"              <1>?<2> (Unknown) \n" \
"<1>  CPU/Mem: <2>Format: <1><OP><USAGE>\n" \
"              <2>-   OP: <1>> < >= <= =<2> operators\n" \
"              <2>-USAGE: <1>numeric<2> (CPU support for <1>floating-point<2>)\n"
"              e.g: To filter for CPU usage greater than or equal to <1>23.4%<2>, please enter:<1> >=23.4 <2>.\n"
"<1>User/Name: <2>substring match.\n";

struct CpuFilter {
    float val{};
    int8_t opt{};
};

struct MemFilter {
    size_t val{};
    int8_t opt{};
};
static std::tuple<size_t, size_t, Process::Status, CpuFilter, MemFilter, std::string, std::string> filter_tuple{};

static std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> elems;
    std::string temp;
    for (auto& c : s) {
        if (c == delimiter) {
            elems.push_back(temp);
            temp.clear();
            continue;
        }
        temp += c;
    }
    if (!temp.empty()) elems.push_back(temp);
    return elems;
}

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
            return "Mem";
        case H_USER:
            return "User";
        case H_PRG_NAME:
            return "Name";
        default:
            return "";
    }
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

StyleList init_help_style() {
    Style bold_style;
    bold_style.fg_color = Color::Green;
    bold_style.intensity = 2;
    Style def_style;
    def_style.intensity = 2;
    Style title_style;
    title_style.fg_color = Color::Blue;
    title_style.intensity = 2;
    return {bold_style, def_style, title_style, {}};
}

void draw_help() {
    auto& renderer = Renderer::self();
    auto help_styles = init_help_style();
    if (from_user.load()) {
        static auto helps = split(help_infos, '\n'); 
        renderer.fillScreen(help_styles[0]);
        skip_help_text = min(skip_help_text, static_cast<uint64_t>(helps.size() - 1));
        for (uint32_t i = 0; i < helps.size(); i++) {
            if (i >= screen_size.height - 2 || i + skip_help_text >= helps.size()) break;
            renderer.setSSFX({i, 0}, helps[i + skip_help_text].data(), help_styles);
        }
        static std::string key_tips_1 = "<4><Esc><5> Quit <4><PgUp>/<PgDn><5> Scroll Page ";
        renderer.fillRows(screen_size.height - 1, screen_size.height, ' ', styles[4]);
        renderer.setSSFX({screen_size.height - 1, 0}, key_tips_1.data(), styles);
        renderer.present();
        from_user.store(false);
    }
}

void draw() {
    auto& renderer = Renderer::self();
    if (screen_size.width < 64 || screen_size.height < 8) {
        renderer.fillScreen();
        renderer.setSSF({0, 0}, "To continue, please resize the terminal size!", styles[8]);
        renderer.setSSFX({1, 0}, "<9>Type <8><Esc><9> or <8><Ctrl+C><9> to quit.", styles);
        renderer.setSSF({2, 0}, "Current size: {:>#3d}x{:<#3d}", styles[8], screen_size.width, screen_size.height);
        renderer.present();
        if (!disabled_mode.load()) disabled_mode.store(true);
        return;
    }
    if (disabled_mode.load()) disabled_mode.store(false);
    if (draw_info.load()) {
        auto now = DT::currentTimestamps(true);
        auto mem_usage = static_cast<double>(memory.used_ram) / static_cast<double>(memory.total_ram) * 100.f;
        auto mem_avail = static_cast<double>(memory.available_ram) / static_cast<double>(memory.total_ram) * 100.f;
        renderer.setStrF({0, 0},
            "HTOP {:12s}, CPU: {:#2d} core(s), {:5.1f}% total, Memory: {:5.1f}% used, {:5.1f}% available",
            DT::formatTime("d:hh:mm:ss", now),  cpu.cores, cpu.total_usage, mem_usage, mem_avail);
        auto cpu_width = screen_size.width / 2 - 1;
        auto cpu_bar_width = cpu_width - 4;
        /// 01[||||||||]  02[|||||||||]
        renderer.fillRows(1, 4);
        for (uint32_t i = 0; i < 8; ++i) {
            size_t idx = skip_cpu_group * 2 + i;
            if (idx >= cpu.cores) break;
            auto c = static_cast<uint32_t>((static_cast<float>(cpu_bar_width) * (cpu.usages[idx] * 0.01f)));
            auto prg = Terminal::formatString("{:{}c}", c, '|').substr(0, cpu_bar_width);
            renderer.setSSFX({i / 2 + 1, (i + 1) % 2 ? 0 : cpu_width}, "<2>{:*0>2#d}:<{}>{:<{}s}", styles,
                idx + 1, cpu.usages[idx] >= 85.f ? 9 : (cpu.usages[idx] >= 60.f ? 8 : 7), cpu_bar_width, prg);
        }
        draw_info.store(false);
    }
    if (from_user.load()) {
        uint32_t col = 0;
        const uint32_t PW = screen_size.width - 59;
        for (uint32_t i = 0; i < HEADER_COUNT; ++i) {
            uint32_t col_width = 8;
            if (i == H_MEM) col_width = 10;
            if (i == H_USER) col_width = 16;
            if (i == H_PRG_NAME) col_width = PW;
            if (sorted_header_no.load() == i) {
                renderer.setSSFX({6, col}, "<5>{}{:{}{}s}", styles,
                    reversed_sort ? "▼" : "▲", i < HEADER_COUNT - 1 ? '>' : '<', col_width - 1,
                    headerName(static_cast<Header>(i)));
            } else {
                renderer.setSSFX({6, col}, "<4> {:{}{}s}", styles,
                    i < HEADER_COUNT - 1 ? '>' : '<', col_width - 1, headerName(static_cast<Header>(i)));
            }
            col += col_width;
        }
        
        static std::string key_tips_1 = "<4><F1>/<?><5> Help <4><Tab><5> Sort Header <4>1~7<5> Column Filter <4><R><5> Reversed Sort <4><P><5> Scroll CPU Usage <4><Esc><5> Quit";
        static std::string key_tips_2 = "<4><F1>/<?><5> Help <4><Backspace><5> Leave Filter Mode <4>1~7<5> Column Filter <4><R><5> Reversed Sort <4><P><5> Scroll CPU Usage <4><Esc><5> Quit";
        renderer.fillRows(screen_size.height - 1, screen_size.height, ' ', styles[4]);
        renderer.setSSFX({screen_size.height - 1, 0}, filter_mode.load() ? key_tips_2.data() : key_tips_1.data(), styles);
        from_user.store(false);
    }

    if (draw_proc.load()) {
        const uint32_t PW = screen_size.width - 60;
        const uint32_t PH = screen_size.height - 8;
        size_t skip_my_index{};
        std::vector<Process::Process>* my_proc_list{};
        if (filter_mode.load()) {
            skip_my_index = skip_filter_index;
            my_proc_list = &filter_list;
        } else {
            skip_my_index = skip_index;
            my_proc_list = &proc_list;
        }
        for (auto i = 0; i < PH; ++i) {
            if (skip_my_index + i >= my_proc_list->size()) {
                renderer.fillRows(7 + i, 7 + i);
                continue;
            }
            DataUnit data_unit;
            auto& process = my_proc_list->at(skip_my_index + i);
            auto sz = autoConvertDataSize(process.memory, data_unit);
            bool is_selected = std::find(selected_pids.begin(), selected_pids.end(), process.pid) != selected_pids.end();
            renderer.setSSFX({static_cast<uint32_t>(7 + i), 0}, "{}{:>8s}{:>8s}{:>8s}{:>8.1f} {:>6.1f}{:<3s}{:>16s} {:<{}s}", styles,
                is_selected ? "<10>" : "", process.pid, process.ppid, status(process.status), process.cpu, sz, dataUnitName(data_unit), process.user_name.substr(0, 16),
                PW, process.command.substr(0, PW));
        }
        
        if (screen_size.height > 6) {
            renderer.fillRows(5, 5);
            renderer.setSSFX({5, 0}, "<10>{}<7>{:>#5d}/{:<#5d} <2>Process(es), <7>{:#5d} <2>selected", styles, 
                            filter_mode.load() ? "FILTER_MODE" : "", skip_my_index, my_proc_list->size(), selected_pids.size());
        }
        draw_proc.store(false);
    }

    if (show_cursor.load() && changed_cur_pos.load() && renderer.isValid(mouse_pos)) {
        auto temp_style = renderer.styleAt(mouse_pos);
        temp_style.property |= Style::Reverse;
        renderer.setStyle(mouse_pos, temp_style);
        changed_cur_pos.store(false);
    }
    renderer.present();
}

bool look_for(Header header) {
    auto& renderer = Renderer::self();
    // auto scr_size = Terminal::screenSize();
    static std::array<std::string, HEADER_COUNT> range_tips = {
        "",
        "",
        "[R = running, S = sleeping, T = stopped, Z = zombie, ...]",
        "[e.g: =3.1, >=3.1, <=3.1, ...]",
        "[e.g: =30k, >=40m, <=50g, ...]",
        "",
        ""
    };
    renderer.fillRows(5, 5, ' ', styles[3]);
    std::string h_name = headerName(header);
    renderer.setSSFX({5, 0}, "<4>Search {}: {} ", styles, h_name, range_tips[header]);
    renderer.present();
    Terminal::setCursorVisible(true);
    size_t st_pos_col = 10 + h_name.size() + range_tips[header].size();
    Terminal::moveCursor(5, st_pos_col);
    auto value = strip(Terminal::readLine());
    Terminal::setCursorVisible(false);
    size_t v1{}, v2{};
    Process::Status v3{};
    CpuFilter v4{};
    MemFilter v5{};
    std::string v6, v7;
    uint8_t off{1};
    bool is_ok{};
    if (!value.empty()) {
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
            for (char c: value) {
                if (c == 'R' || c == 'r') {
                    v3 = Process::Status::Running;
                    is_ok = true;
                    break;
                } else if (c == 'I' || c == 'i') {
                    v3 = Process::Status::Idle;
                    is_ok = true;
                    break;  
                } else if (c == 'T' || c == 't') {
                    v3 = Process::Status::Stopped;
                    is_ok = true;
                    break;    
                } else if (c == 'S' || c == 's') {
                    v3 = Process::Status::Sleeping;
                    is_ok = true;
                    break;
                } else if (c == 'U' || c == 'u') {
                    v3 = Process::Status::UninterruptibleSleep;
                    is_ok = true;
                    break;
                } else if (c == 'Z' || c == 'z') {
                    v3 = Process::Status::Zombie;
                    is_ok = true;
                    break;
                } else if (c == 'X' || c == 'x') {
                    v3 = Process::Status::Terminated;
                    is_ok = true;
                    break;
                } else if (c == '?') {
                    v3 = Process::Status::Unknown;
                    is_ok = true;
                    break;
                }
            }
            if (is_ok) {
                std::get<2>(filter_tuple) = v3;
            }
            break;
        case H_CPU:
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
    }
    if (!is_ok) {
        renderer.fillRows(5, 5, ' ', styles[3]);
        renderer.setSSFX({5, 0}, "<4>Invalid value! Press any key to continue... ", styles);
        renderer.present();
        Terminal::getKey();
        renderer.fillRows(5, 5, ' ', styles[0]);
        return false;
    }
    search_header = header;
    skip_filter_index = 0;
    renderer.fillRows(5, 5, ' ', styles[0]);
    return true;
}

bool check_prg(const Process::Process& prg) {
    if (search_header == H_PID) {
        return prg.pid == std::get<0>(filter_tuple);
    }
    if (search_header == H_PPID) {
        return prg.ppid == std::get<1>(filter_tuple) || prg.pid == std::get<1>(filter_tuple); 
    }
    if (search_header == H_STATES) {
        return prg.status == std::get<2>(filter_tuple);
    }
    if (search_header == H_CPU) {
        CpuFilter cpu_filter = std::get<3>(filter_tuple);
        if (cpu_filter.opt == 0) {
            return prg.cpu == cpu_filter.val;
        } else if (cpu_filter.opt == 1) {
            return prg.cpu < cpu_filter.val;
        } else if (cpu_filter.opt > 1) {
            return prg.cpu <= cpu_filter.val;
        } else if (cpu_filter.opt == -1) {
            return prg.cpu > cpu_filter.val;
        } else if (cpu_filter.opt < -1) {
            return prg.cpu >= cpu_filter.val;
        }
    }
    if (search_header == H_MEM) {
        auto mem_filter = std::get<4>(filter_tuple);
        if (mem_filter.opt == 0) {
            return prg.memory == mem_filter.val;
        } else if (mem_filter.opt == 1) {
            return prg.memory < mem_filter.val;
        } else if (mem_filter.opt > 1) {
            return prg.memory <= mem_filter.val;
        } else if (mem_filter.opt == -1) {
            return prg.memory > mem_filter.val;
        } else if (mem_filter.opt < -1) {
            return prg.memory >= mem_filter.val;
        }
    }
    if (search_header == H_USER) {
        return prg.user_name.find(std::get<5>(filter_tuple)) != std::string::npos;
    }
    if (search_header == H_PRG_NAME) {
        return prg.command.find(std::get<6>(filter_tuple)) != std::string::npos;
    }
    return false;
}

bool look_for_from_proc_list(const Process::Process& prg) {
    return check_prg(prg);
}

bool sort_proc(const Process::Process& p1, const Process::Process& p2) {
    switch (sorted_header_no.load()) {
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
            for (size_t i = 0; i < min(p1.user_name.size(), p2.user_name.size()); ++i) {
                if (p1.user_name[i] == p2.user_name[i]) continue;
                auto c1 = p1.user_name[i];
                auto c2 = p2.user_name[i];
                return reversed_sort ? c1 > c2 : c1 < c2;
            }
            break;
        case H_PRG_NAME:
            for (size_t i = 0; i < min(p1.command.size(), p2.command.size()); ++i) {
                if (p1.command[i] == p2.command[i]) continue;
                return reversed_sort ? p1.command[i] > p2.command[i] : p1.command[i] < p2.command[i];
            }
            break;
        default:
            break;
    }
    return false;
}

int main(int argc, char *argv[]) {
    auto st = DT::currentTimestamps(true), ed = st;
    styles = generate();
    screen_size = Terminal::screenSize();
    auto shown_proc_count = screen_size.height - 8;
    running_main.store(true);
    Renderer::self().setResizeEvent([&shown_proc_count](Renderer& renderer) {
        screen_size = Terminal::screenSize();
        shown_proc_count = screen_size.height - 8;
        if (show_cursor.load()) {
            mouse_pos.row = 0; 
            mouse_pos.column = 0;
            changed_cur_pos.store(true);
        }
        from_user.store(true);
        renderer.fillScreen();
    });
    EV::Event ev1(1, "update", []{ return true; }, [](const std::atomic<bool>&) {
        if (!running_main.load() || draw_info.load() || search_mode.load()) return;
        static bool first_running = true;
        if (first_running) {
            first_running = false;
        } else {
            getCPUInfo(cpu, delay_ms);
        }
        getMemory(memory);
        draw_info.store(true);
    });
    ev1.setDelayMS(1);
    ev1.setRepeatCount(0);
    ev1.run();
    EV::Event ev2(2, "update_proc", []{ return true; }, [](const std::atomic<bool>&) {
        if (!running_main.load() || !update_proc.load() || draw_proc.load() || search_mode.load()) return;
        static bool first_running = true;
        if (filter_mode.load()) {
            if (filter_list.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
            } else {
                Process::updateProcessList(filter_list, true, delay_ms);
                std::sort(filter_list.begin(), filter_list.end(), &sort_proc);
            }
        } else {
            if (proc_list.empty()) proc_list = Process::listProcess(!first_running, delay_ms);
            else Process::listProcess(proc_list, true, delay_ms);
            std::sort(proc_list.begin(), proc_list.end(), &sort_proc);
        }
        update_proc.store(false);
        draw_proc.store(true);
        if (first_running) first_running = false;
    });
    ev2.setDelayMS(1);
    ev2.setRepeatCount(0);
    ev2.run();
    bool for_proc_list = false;
    EV::Event ev3(3, "look_for_procs", []{ return true; }, [&for_proc_list](const std::atomic<bool>& ok) {
        std::unique_lock<std::mutex> uni_lock(filter_mutex);
        if (!filter_list.empty()) filter_list.clear();
        if (for_proc_list) {
            auto start_iter = proc_list.begin();
            do {
                if (!ok.load()) break;
                auto iter = std::find_if(start_iter, proc_list.end(), &look_for_from_proc_list);
                if (iter == proc_list.end()) break;
                filter_list.push_back(*iter);
                start_iter = iter + 1;
            } while (true);
        } else {
            std::vector<Process::Process> temp_list;
            auto start_iter = filter_list.begin();
            do {
                if (!ok.load()) break;
                auto iter = std::find_if(start_iter, filter_list.end(), &look_for_from_proc_list);
                if (iter == filter_list.end()) break;
                temp_list.push_back(*iter);
                start_iter = iter + 1;
            } while (true);
            filter_list = temp_list;
        }
        draw_proc.store(true);
    });
    ev3.setDelayMS(1);
    draw_info.store(true);
    update_proc.store(true);
    from_user.store(true);
#if defined(__linux__) && defined(TINY_CPP_USE_GPM)
    if (!strcmp(getenv("TERM"), "linux")) {
        show_cursor.store(true);
    }
#endif
    std::array<ColsRange, HEADER_COUNT> mlabels = {
        {
           {0, 7},
           {8, 15},
           {16, 23},
           {24, 31},
           {32, 41},
           {42, 57},
           {57, 255}
        }
    };

    auto last = DT::currentTimestamps(true), cur = last;
    while (running_main.load()) {
        ed = DT::currentTimestamps(true);
        last = ed;
        if (ed - st >= 20) {
            st = ed;
            if (help_mode.load()) {
                draw_help();
            } else {
                draw();
            }
        }
        if (cur - last >= delay_ms) {
            last = cur;
            update_proc.store(true);
        }
        auto input = Terminal::getInput();
        
        if (disabled_mode.load()) {
            ev1.stop();
            ev2.stop();
            if (input.type == InputEvent::K && input.input.keyboard.is_pressed) {
                if (input.input.keyboard.key == KEY_ESC || input.input.keyboard.key == KEY_CTRL_C) {
                    break;
                }
            }
            continue;
        } else if (help_mode.load()) {
            ev1.stop();
            ev2.stop();
            if (input.type == InputEvent::K && input.input.keyboard.is_pressed) {
                if (input.input.keyboard.key == KEY_ESC) {
                    help_mode.store(false);
                    from_user.store(true);
                } else if (input.input.keyboard.key == KEY_SPECIAL) {
                    if (input.input.keyboard.sp_key == SP_KEY_PAGE_DOWN) {
                        skip_help_text += screen_size.height - 1;
                    } else if (input.input.keyboard.sp_key == SP_KEY_PAGE_UP) {
                        if (skip_help_text <= screen_size.height - 1) {
                            skip_help_text = 0;
                        } else {
                            skip_help_text -= screen_size.height - 1;
                        }
                    } else if (input.input.keyboard.sp_key == SP_KEY_UP) {
                        if (skip_help_text >= 1) skip_help_text -= 1; 
                    } else if (input.input.keyboard.sp_key == SP_KEY_DOWN) {
                        skip_help_text += 1;
                    }
                    from_user.store(true);
                }
            } else if (input.type == InputEvent::M) {
                if (input.input.mouse.button == SP_MOUSE_WHEEL_UP) {
                    if (skip_help_text >= 1) skip_help_text -= 1; 
                } else if (input.input.mouse.button == SP_MOUSE_WHEEL_DOWN) {
                    skip_help_text += 1;
                }
                from_user.store(true);
            }
            continue;
        } else {
            ev1.run();
            ev2.run();
        }
        if (input.type == InputEvent::K && input.input.keyboard.is_pressed) {
            int next{};
            switch (input.input.keyboard.key) {
                case KEY_TAB:
                    next = (sorted_header_no.load() + 1) % HEADER_COUNT;
                    sorted_header_no.store(next);
                    break;
                case 'P':
                    if (skip_cpu_group == 0) {
                        skip_cpu_group = cpu.cores / 2 - 1;
                    } else {
                        skip_cpu_group -= 1;
                    }
                    break;
                case 'p':
                    if (cpu.cores > 8) {
                        skip_cpu_group = (skip_cpu_group + 1) % (cpu.cores / 2);
                    }
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
                    if (look_for(static_cast<Header>(input.input.keyboard.key - '0' - 1))) {
                        if (!filter_mode.load()) {
                            filter_mode.store(true);
                            for_proc_list = true;
                        } else {
                            for_proc_list = false;
                        }
                        ev3.run();
                    }
                    from_user.store(true);
                    ev1.run();
                    ev2.run();
                    break;
                case '/':
                case '?':
                    help_mode.store(true);
                    break;
                case KEY_DEL:
                case KEY_BK:
                    filter_mode.store(false);
                    draw_proc.store(true);
                    from_user.store(true);
                    ev3.stop();
                    break;
                case KEY_SPECIAL:
                    switch (input.input.keyboard.sp_key) {
                        case SP_KEY_F1:
                            help_mode.store(true);
                            break;
                        case SP_SHIFT_TAB:
                            if (sorted_header_no.load() == 0) {
                                sorted_header_no.store(H_PRG_NAME);
                            } else {
                                next = (sorted_header_no.load() - 1) % HEADER_COUNT;
                                sorted_header_no.store(next);
                            }
                            break;
                        
                        case SP_KEY_F12:
                            show_cursor.store(!show_cursor.load());
                            mouse_pos = {0, 0};
                            break;
                        case SP_KEY_DOWN:
                            if (filter_mode.load()) {
                                skip_filter_index = (skip_filter_index + 1) % filter_list.size();
                            } else {
                                skip_index = (skip_index + 1) % proc_list.size();
                            }
                            draw_proc.store(true);
                            break;
                        case SP_KEY_UP:
                            if (filter_mode.load()) {
                                skip_filter_index -= 1;
                                if (skip_filter_index >= SIZE_T_MAX - 1) skip_filter_index = filter_list.size() - 1;
                            } else {
                                skip_index -= 1;
                                if (skip_index >= SIZE_T_MAX - 1) skip_index = proc_list.size() - 1;
                            }
                            draw_proc.store(true);
                            break;
                        case SP_KEY_HOME:
                            if (filter_mode.load()) {
                                skip_filter_index = 0;
                            } else {
                                skip_index = 0;
                            }
                            draw_proc.store(true);
                            break;
                        case SP_KEY_END:
                            if (filter_mode.load()) {
                                skip_filter_index = filter_list.size() - 1;
                            } else {
                                skip_index = proc_list.size() - 1;
                            }
                            draw_proc.store(true);
                            break;
                        case SP_KEY_PAGE_DOWN:
                            if (filter_mode.load()) { 
                                if (skip_filter_index + shown_proc_count < filter_list.size()) {
                                    skip_filter_index += shown_proc_count;
                                    draw_proc.store(true);
                                }
                            } else if (skip_index + shown_proc_count < proc_list.size()) {
                                skip_index += shown_proc_count;
                                draw_proc.store(true);
                            }
                            break;
                        case SP_KEY_PAGE_UP:
                            if (filter_mode.load()) {
                                if (static_cast<int64_t>(skip_filter_index) - static_cast<int64_t>(shown_proc_count) >= 0) {
                                    skip_filter_index -= shown_proc_count;
                                    draw_proc.store(true);
                                }
                            } else if (static_cast<int64_t>(skip_index) - static_cast<int64_t>(shown_proc_count) >= 0) {
                                skip_index -= shown_proc_count;
                                draw_proc.store(true);
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case KEY_CTRL_R:
                    multiselect.store(!multiselect.load());
                    if (selected_pids.size() > 1) {
                        selected_pids.clear();
                        draw_proc.store(true);
                    }
                    break;
                case KEY_CTRL_D:
                    selected_pids.clear();
                    draw_proc.store(true);
                    break;
                case KEY_ESC:
                case KEY_CTRL_C:
                    running_main.store(false);
                    break;
                default:
                    break;
            }
            from_user.store(true);
        } else if (input.type == InputEvent::M) {
            if (show_cursor.load() && input.input.mouse.button == MOUSE_MOVED && 
                    mouse_pos != input.input.mouse.position) {
                if (Renderer::self().isValid(mouse_pos)) {
                    auto temp_style = Renderer::self().styleAt(mouse_pos);
                    temp_style.property &= ~(Style::Reverse);
                    Renderer::self().setStyle(mouse_pos, temp_style);
                }
                mouse_pos = input.input.mouse.position;
                changed_cur_pos.store(true);
            }
            if (input.input.mouse.button == MOUSE_WHEEL_DOWN) {
                if (input.input.mouse.position.row >= 7) {
                    if (filter_mode.load()) {
                        skip_filter_index = min(skip_filter_index + 1, filter_list.size() - 1);
                    } else {
                        skip_index = min(skip_index + 1, proc_list.size() - 1);
                    }
                    draw_proc.store(true);
                } else if (cpu.cores > 8 && input.input.mouse.position.row >= 1 && input.input.mouse.position.row < 6) {
                    if (skip_cpu_group < cpu.cores / 2 - 1) skip_cpu_group += 1;
                    draw_info.store(true);
                }
            } else if (input.input.mouse.button == MOUSE_WHEEL_UP) {
                if (input.input.mouse.position.row >= 7) {
                    if (filter_mode.load()) {
                        if (skip_filter_index > 0) skip_filter_index -= 1;
                    } else {
                        if (skip_index > 0) skip_index -= 1;
                    }
                    draw_proc.store(true);
                } else if (cpu.cores > 8 && input.input.mouse.position.row >= 1 && input.input.mouse.position.row < 6) {
                    if (skip_cpu_group > 0) skip_cpu_group -= 1;
                    draw_info.store(true);
                }
            } else if (input.input.mouse.button == MOUSE_LEFT_BUTTON && input.input.mouse.is_pressed) {
                if (input.input.mouse.position.row >= 7 && input.input.mouse.position.row <= screen_size.height - 2) {
                    uint32_t C = input.input.mouse.position.row - 7;
                    size_t pid{};
                    if (filter_mode.load()) {
                        if (skip_filter_index + C < filter_list.size()) {
                            pid = filter_list[skip_filter_index + C].pid;
                        }
                    } else {
                        if (skip_index + C < proc_list.size()) {
                            pid = proc_list[skip_index + C].pid;
                        }
                    }
                    if (pid > 0) {
                        if (multiselect.load()) {
                            auto iter = std::find(selected_pids.begin(), selected_pids.end(), pid);
                            if (iter == selected_pids.end()) {
                                selected_pids.push_back(pid);
                            } else {
                                selected_pids.erase(iter);
                            }
                        } else {
                            if (selected_pids.empty()) selected_pids.push_back(pid);
                            else selected_pids.front() = pid;
                        }
                        draw_proc.store(true);
                    }
                } else if (input.input.mouse.position.row == 6) {
                    auto col = input.input.mouse.position.column;
                    size_t ID = 0;
                    for (auto& cols_range: mlabels) {
                        if (col >= cols_range.start && col <= cols_range.end) {
                            if (sorted_header_no.load() == ID) {
                                reversed_sort.store(!reversed_sort.load());
                            } else {
                                sorted_header_no.store(ID);
                            }
                            from_user.store(true);
                            break;
                        }
                        ID += 1;
                    }
                }
            }
        }
    }
    return 0;
}


//
// int main(int argc, char const *argv[])
// {
//     // auto ts = DT::currentTimestamps(true);
//     // auto prg_lst = OS::Process::listProcess(true, 1000);
//     // auto ed = DT::currentTimestamps(true);
//     // for (auto& f : prg_lst) {
//     //     if (f.pid == 0) continue;
//     // TUI::Terminal::printFormat("PID {}, PPID {}, Status {}, CPU {:.1f}%, MEM {} bytes, User '{}', Name '{}', Exe '{}'\r\n",
//     //     f.pid, f.ppid, status(f.status), f.cpu, f.memory, f.user_name, f.name, f.command);
//     // }
//     // TUI::Terminal::printFormat("Total: {} process(s), Running {} ms.\r\n", prg_lst.size(), ed - ts);
//     return 0;
// }
