#include "../src/Tiny.hpp"

using namespace Tiny;

int main() {
    auto start = DT::currentTimestamps();
    std::string s, o;
#ifdef TINY_CPP_MY_OS_WINDOWS
    TUI::Terminal::print() << "Running: " << TUI::TStyle::fg(TUI::Color::Blue) << R"(.\assets\keep_cmd)"
                           << TUI::TStyle::reset() << "\n";
    int ret = OS::exec(R"(.\assets\keep_cmd.bat)", 3000, &s, &o);
#else
    TUI::Terminal::print() << "Running: " << "./assets/keep_bash" << "\n";
    OS::exec("chmod u+x ./assets/keep_bash");
    int ret = OS::exec("./assets/keep_bash", 5000, &s, &o);
#endif
    TUI::Terminal::print() << "Ret: " << ret << "\n";
    TUI::Terminal::print() << TUI::TStyle::bold() << "Output: \n"
                           << TUI::TStyle::bold(false) 
                           << TUI::TStyle::fg(TUI::Color::Green) << s << TUI::TStyle::reset() << "\n";
    TUI::Terminal::print() << TUI::TStyle::bold() << "Error: \n" << TUI::TStyle::bold(false)
                           << TUI::TStyle::fg(TUI::Color::Yellow) << o << TUI::TStyle::reset() << "\n";
    auto end = DT::currentTimestamps();
    TUI::Terminal::print() << TUI::TStyle::reset() << "Time taken: " << (end - start) << " ms\n";
    return ret;
}