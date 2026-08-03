
#include "../src/Tiny.hpp"

using namespace Tiny;
using namespace TUI;

int main() {
    Terminal::print() << "16 colors\r\n";
    Terminal::print() << TStyle::bg(Color::Black) << "  " << TStyle::bg(Color::Red) << "  "
                      << TStyle::bg(Color::Green) << "  " << TStyle::bg(Color::Yellow) << "  "
                      << TStyle::bg(Color::Blue) << "  " << TStyle::bg(Color::Magenta) << "  "
                      << TStyle::bg(Color::Cyan) << "  " << TStyle::bg(Color::White) << "  \r\n";
    Terminal::print() << TStyle::bg(Color::Black, true) << "  " << TStyle::bg(Color::Red, true) << "  "
                      << TStyle::bg(Color::Green, true) << "  " << TStyle::bg(Color::Yellow, true) << "  "
                      << TStyle::bg(Color::Blue, true) << "  " << TStyle::bg(Color::Magenta, true) << "  "
                      << TStyle::bg(Color::Cyan, true) << "  " << TStyle::bg(Color::White, true) << "  \r\n";
    Terminal::print() << TStyle::reset() << "RGB Color (256 colors)\r\n";
    for (uint32_t i = 0; i < 256; i++) {
        Terminal::print() << TStyle::bg(i, 0, 0) << "  ";
        if ((i + 1) % 16 == 0) {
            Terminal::reset();
            Terminal::printLine();
        }
    }
    Terminal::printLine();
    Terminal::print() << TStyle::reset() << "Press any key to continue...\r\n";
    Terminal::getKey();
    for (uint32_t i = 0; i < 256; i++) {
        Terminal::print() << TStyle::bg(0, i, 0) << "  ";
        if ((i + 1) % 16 == 0) {
            Terminal::reset();
            Terminal::printLine();
        }
    }
    Terminal::printLine();
    Terminal::print() << TStyle::reset() << "Press any key to continue...\r\n";
    Terminal::getKey();
    for (uint32_t i = 0; i < 256; i++) {
        Terminal::print() << TStyle::bg(0, 0, i) << "  ";
        if ((i + 1) % 16 == 0) {
            Terminal::reset();
            Terminal::printLine();
        }
    }
    Terminal::printLine();
    Terminal::print() << TStyle::reset() << "Press any key to continue...\r\n";
    Terminal::getKey();
    std::string sample = "Where is a will, there is a way!";
    Terminal::print() << "Normal:       " << sample << "\r\n";
    Terminal::print() << "Bold:         " << TStyle::bold() << sample << TStyle::reset() << "\r\n";
    Terminal::print() << "Italic:       " << TStyle::italic() << sample << TStyle::reset() << "\r\n";
    Terminal::print() << "Underline:    " << TStyle::underline() << sample << TStyle::reset() << "\r\n";
    Terminal::print() << "Strikethrough: " << TStyle::striketh() << sample << TStyle::reset() << "\r\n";
    Terminal::print() << TStyle::reset() << "Press any key to continue...\r\n";
    Terminal::getKey();
    return 0;
}

