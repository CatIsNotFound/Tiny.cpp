
#include "../src/Tiny.hpp"

using namespace Tiny;
using namespace TUI;

int main() {
    Terminal::print() << "16 colors\r\n";
    Terminal::print() << Term::bg(Color::Black) << "  " << Term::bg(Color::Red) << "  "
                      << Term::bg(Color::Green) << "  " << Term::bg(Color::Yellow) << "  "
                      << Term::bg(Color::Blue) << "  " << Term::bg(Color::Magenta) << "  "
                      << Term::bg(Color::Cyan) << "  " << Term::bg(Color::White) << "  \r\n";
    Terminal::print() << Term::bg(Color::Black, true) << "  " << Term::bg(Color::Red, true) << "  "
                      << Term::bg(Color::Green, true) << "  " << Term::bg(Color::Yellow, true) << "  "
                      << Term::bg(Color::Blue, true) << "  " << Term::bg(Color::Magenta, true) << "  "
                      << Term::bg(Color::Cyan, true) << "  " << Term::bg(Color::White, true) << "  \r\n";
    Terminal::print() << Term::reset() << "RGB Color (256 colors)\r\n";
    for (uint32_t i = 0; i < 256; i++) {
        Terminal::print() << Term::bg(i, 0, 0) << "  ";
        if ((i + 1) % 16 == 0) {
            Terminal::reset();
            Terminal::printLine();
        }
    }
    Terminal::printLine();
    Terminal::print() << Term::reset() << "Press any key to continue...\r\n";
    Terminal::getKey();
    for (uint32_t i = 0; i < 256; i++) {
        Terminal::print() << Term::bg(0, i, 0) << "  ";
        if ((i + 1) % 16 == 0) {
            Terminal::reset();
            Terminal::printLine();
        }
    }
    Terminal::printLine();
    Terminal::print() << Term::reset() << "Press any key to continue...\r\n";
    Terminal::getKey();
    for (uint32_t i = 0; i < 256; i++) {
        Terminal::print() << Term::bg(0, 0, i) << "  ";
        if ((i + 1) % 16 == 0) {
            Terminal::reset();
            Terminal::printLine();
        }
    }
    Terminal::printLine();
    Terminal::print() << Term::reset() << "Press any key to continue...\r\n";
    Terminal::getKey();
    std::string sample = "Where is a will, there is a way!";
    Terminal::print() << "Normal:       " << sample << "\r\n";
    Terminal::print() << "Bold:         " << Term::bold() << sample << Term::reset() << "\r\n";
    Terminal::print() << "Italic:       " << Term::italic() << sample << Term::reset() << "\r\n";
    Terminal::print() << "Underline:    " << Term::underline() << sample << Term::reset() << "\r\n";
    Terminal::print() << "Strikethrough: " << Term::striketh() << sample << Term::reset() << "\r\n";
    Terminal::print() << Term::reset() << "Press any key to continue...\r\n";
    Terminal::getKey();
    return 0;
}

