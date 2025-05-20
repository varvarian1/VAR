#include <ncurses.h>
#include <algorithm>

#include "cursor.hpp"

namespace Var {

    void Cursor::clamp_line_position(const Buffer& buffer) {
        const int max_line = buffer.line_count() - 1;
        cursor_line = std::clamp(cursor_line, 0, max_line);
    }
    
    void Cursor::clamp_column_position(const Buffer& buffer) {
        const int max_col = get_current_line_length(buffer);
        cursor_col = std::clamp(cursor_col, 0, max_col);
    }
    
    void Cursor::adjust_viewport(const Buffer& buffer, int& viewport_y) const {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        
        if (cursor_line < viewport_y) {
            viewport_y = cursor_line;
        } else if (cursor_line >= viewport_y + rows) {
            viewport_y = cursor_line - rows + 1;
        }
    }

    void Cursor::move_left(const Buffer& buffer) {
        if (can_move_left()) {
            cursor_col--;
        } else if (can_move_up()) {
            move_to_prev_line_end(buffer);
        }
    }
    
    void Cursor::move_right(const Buffer& buffer) {
        if (can_move_right(buffer)) {
            cursor_col++;
        } else if (can_move_down(buffer)) {
            move_to_next_line_start();
        }
    }
    
    void Cursor::move_up(const Buffer& buffer) {
        if (can_move_up()) {
            cursor_line--;
            adjust_col_for_line(buffer);
        }
    }
    
    void Cursor::move_down(const Buffer& buffer) {
        if (can_move_down(buffer)) {
            cursor_line++;
            adjust_col_for_line(buffer);
        }
    }
    
    void Cursor::clamp(const Buffer& buffer, int& viewport_y) {
        clamp_line_position(buffer);
        clamp_column_position(buffer);
        adjust_viewport(buffer, viewport_y);
    }
    
    std::pair<int, int> Cursor::position() const {
        return {cursor_line, cursor_col};
    }
    
    void Cursor::set_position(int line, int col) {
        cursor_line = line;
        cursor_col = col;
    }

    bool Cursor::can_move_up() const {
        return cursor_line > 0;
    }
    
    bool Cursor::can_move_down(const Buffer& buffer) const {
        return cursor_line + 1 < buffer.line_count();
    }
    
    bool Cursor::can_move_left() const {
        return cursor_col > 0;
    }
    
    bool Cursor::can_move_right(const Buffer& buffer) const {
        return cursor_col < get_current_line_length(buffer);
    }
    
    int Cursor::get_current_line_length(const Buffer& buffer) const {
        return static_cast<int>(buffer.get_line(cursor_line).size());
    }
    
    void Cursor::move_to_prev_line_end(const Buffer& buffer) {
        cursor_line--;
        cursor_col = get_current_line_length(buffer);
    }
    
    void Cursor::move_to_next_line_start() {
        cursor_line++;
        cursor_col = 0;
    }
    
    void Cursor::adjust_col_for_line(const Buffer& buffer) {
        cursor_col = std::min(cursor_col, get_current_line_length(buffer));
    }

}