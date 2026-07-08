
#include "Tetris.hpp"

namespace Tetris {
    Area::Area(uint8_t w, uint8_t h) {
        _data.resize(w * h);
        _size = {w, h};
    }

    void Area::moveUp() {
        for (uint8_t r = 0; r < _size.height; r++) {
            for (uint8_t c = 0; c < _size.width; c++) {
                auto c_idx = r * _size.width + c;
                auto n_idx =  r * (_size.width - 1) + c;
                if (n_idx >= _size.width * _size.height) continue;
                _data[c_idx] = _data[n_idx];
            }
        }
    }

    void Area::pushBlock(BType type, uint8_t col) {
        uint8_t temp[4][4] = {0};
        switch (type) {
            case BType::I:
                temp[0][0] = 1;
                temp[1][0] = 1;
                temp[2][0] = 1;
                temp[3][0] = 1;
                break;
            case BType::RI:
                temp[0][0] = 1;
                temp[0][1] = 1;
                temp[0][2] = 1;
                temp[0][3] = 1;
                break;
            case BType::T:
                temp[0][0] = 1;
                temp[0][1] = 1;
                temp[0][2] = 1;
                temp[1][1] = 1;
                break;
            case BType::L:
                temp[0][0] = 1;
                temp[1][0] = 1;
                temp[2][0] = 1;
                temp[2][1] = 1;
                break;
            case BType::Z:
                temp[0][0] = 1;
                temp[0][1] = 1;
                temp[1][1] = 1;
                temp[1][2] = 1;
                break;
            case BType::S:
                temp[0][1] = 1;
                temp[0][2] = 1;
                temp[1][0] = 1;
                temp[1][1] = 1;
                break;
            case BType::O:
                temp[0][0] = 1;
                temp[0][1] = 1;
                temp[1][0] = 1;
                temp[1][1] = 1;
                break;
        }

    }

    void Area::set(const tui::Position &position) {
        _data.at(position.row * _size.height + position.column) = 1;
    }

    void Area::set(uint8_t row, uint8_t col) {
        _data[row * _size.height + col] = 1;
    }

    void Area::unset(const tui::Position &position) {
        _data[position.row * _size.height + position.column] = 0;
    }

    void Area::unset(uint8_t row, uint8_t col) {
        _data[row * _size.height + col] = 0;
    }

    void Area::unset(uint8_t row) {
        for (uint8_t c = 0; c < _size.width; c++) {
            _data[row * _size.width + c] = 0;
        }
    }

    void Area::reset(uint8_t w, uint8_t h) {
        _data.resize(w * h);
        for (uint8_t & i : _data) {
            i = 0;
        }
    }

    void Area::clear() {
        for (uint8_t & i : _data) {
            i = 0;
        }
    }

    uint8_t Area::get(uint8_t row, uint8_t col) const {
        return _data[row * _size.height + col];
    }

    uint8_t Area::get(const tui::Position &position) const {
        return _data[position.row * _size.height + position.column];
    }

    tui::Size Area::size() const {
        return _size;
    }
}
