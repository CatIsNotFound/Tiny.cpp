
#include "../src/Tiny.hpp"
#include <random>
using namespace Tiny;
using namespace TUI;

static void resizeWindow(TermBuffer& buffer) {
    TermStyle style;
    style.colors.color.bg = Color::Green;
    style.colors.color.fg = Color::Black;
    buffer.fillScreen(style);
}

int main(int argc, char *argv[]) {
    TermStyle style;
    style.colors.color.bg = Color::Cyan;
    style.colors.color.fg = Color::Black;
    TermBuffer::self().setResizeEvent(&resizeWindow);
    TermBuffer::self().fillScreen(style);
    TermBuffer::self().fillRows(1, 4, '*', style);
    std::random_device rd;
    std::mt19937 gen(rd());
    auto ts = DT::currentTimestamps(), now = ts, st = ts;
    size_t fps{}, real_cnt{};
    TermBuffer::self().fillScreen(style);
    TermStyleList my_list;
    my_list.push_back(style);
    my_list.emplace_back();
    my_list.back().colors.color.bg = Color::Cyan;
    my_list.back().colors.color.fg = Color::Blue;
    my_list.emplace_back();
    my_list.back().colors.color.bg = Color::Red;
    my_list.back().colors.color.fg = Color::Yellow;
    my_list.emplace_back();
    my_list.back().colors.color.bg = Color::Blue;
    my_list.back().colors.color.fg = Color::Yellow;
    while (true) {
        now = DT::currentTimestamps();
        if (now - ts >= 0) {
            TermBuffer::self().present();
            // style.intensity = rand() % 3;
            style.colors.color.bg = static_cast<Color>(gen() % 9);
            style.colors.color.fg = static_cast<Color>(gen() % 9);
            TermBuffer::self().fillScreen(style);
            auto size = TermBuffer::self().size();
            // TermBuffer::self().fillRows(gen() % size.height, gen() % size.height, '*', style);
            // TermBuffer::self().fillCols(gen() % size.width, gen() % size.width, '#', style);
            Position st_pos = {size.height / 2 - 4, size.width / 2 - 4};
            Position ed_pos = {size.height / 2 + 4, size.width / 2 + 4};
            // TermBuffer::self().fillRect(st_pos, ed_pos, '&', style);
            Border bd;
            bd.makeRoundedRect();

            TermBuffer::self().drawBorder(st_pos, ed_pos, bd, style);
            now = DT::currentTimestamps();
            std::string datetime = Terminal::formatString("<1>延迟: <0>{:>#4d}<2>毫秒, 帧率: <0>{}<3>帧/秒, </>{}", now - ts, fps, now);
            my_list.front().colors.color.bg = style.colors.color.bg;
            my_list.front().colors.color.fg = style.colors.color.fg;
            TermBuffer::self().drawString({1, 1}, datetime, my_list);
            TermBuffer::self().drawString({}, " Please Press Esc to quit! ");
            real_cnt += 1;
            ts = now;
        }
        if (now - st >= 1000) {
            st = now;
            fps = real_cnt;
            real_cnt = 0;
        }
        auto in = Terminal::getInput();
        if (in.type == InputEvent::K && in.input.keyboard.key == KEY_ESC) {
            break;
        }
        if (in.type == InputEvent::M) {
            TermBuffer::self().setStyle(in.input.mouse.position, style);
        }
    }
    return 0;
}

