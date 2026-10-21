#include "../src/Tiny.hpp"

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

int main(int argc, char *argv[]) {
    int a{1203};
    uint64_t b{1234567};
    const char* c{"hello"};
    std::string d{"world"};
    float f{3.14f};
    TUI::Terminal::printFormat("{:p}, {:p}, {:p}, {:p}, 0x{:*016p}\r\n", a, b, c, d, f);
    Position pos{300.123f, 1232.f};
    TUI::Terminal::printFormat("Pos: {}\r\n", pos);
    return 0;
}


