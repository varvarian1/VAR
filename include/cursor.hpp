#ifndef CURSOR
#define CURSOR

#include "buffer.hpp"
#include <utility>

namespace Var {

    class Cursor {
    private:
        int cursor_line = 0;
        int cursor_col = 0;

    public:
        void clamp_line_position(const Buffer& buffer);
        void clamp_column_position(const Buffer& buffer);
        void adjust_viewport(const Buffer& buffer, int& viewport_y) const;
        void move_left(const Buffer& buffer);
        void move_right(const Buffer& buffer);
        void move_up(const Buffer& buffer);
        void move_down(const Buffer& buffer);
        // void tab(const Buffer& buffer);
        void clamp(const Buffer& buffer, int& viewport_y);
        std::pair<int, int> position() const;
        void set_position(int line, int col);
        bool can_move_up() const;
        bool can_move_down(const Buffer& buffer) const;
        bool can_move_left() const;
        bool can_move_right(const Buffer& buffer) const;
        int get_current_line_length(const Buffer& buffer) const;
        void move_to_prev_line_end(const Buffer& buffer);
        void move_to_next_line_start();
        void adjust_col_for_line(const Buffer& buffer);

    };
}

#endif