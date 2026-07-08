

#ifndef TINY_CPP_DEMO_TETRIS_HPP
#define TINY_CPP_DEMO_TETRIS_HPP
#include "../src/Tiny.hpp"


namespace Tetris {
    using namespace Tiny;
    namespace tui = TUI;
    namespace dt  = DT;
    using term = TUI::Terminal;

    enum class BType : uint8_t {
        I,
        RI,
        T,
        L,
        Z,
        S,
        O
    };

    class Area {
    public:
        Area(uint8_t w, uint8_t h);
        void moveUp();
        void pushBlock(BType type, uint8_t col);
        void set(const tui::Position& position);
        void set(uint8_t row, uint8_t col);
        void unset(const tui::Position& position);
        void unset(uint8_t row, uint8_t col);
        void unset(uint8_t row);
        void reset(uint8_t w, uint8_t h);
        void clear();
        uint8_t get(uint8_t row, uint8_t col) const;
        uint8_t get(const tui::Position& position) const;
        tui::Size size() const;
    private:
        std::vector<uint8_t> _data{};
        tui::Size _size{};
    };



}

#endif //TINY_TETRIS_HPP