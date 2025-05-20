#include <ncurses.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>

#include "viewport.hpp"

namespace Var {

    void Viewport::draw(const Buffer& buffer, const Cursor& cursor, bool modified, const std::string& filename) {
        update_dimensions();
        // clear_screen();
        
        const auto [cursor_line, cursor_col] = cursor.position();
        const int text_start_col = calculate_text_start_column(cursor_line, buffer.line_count());
        
        draw_buffer_content(buffer, cursor_line, text_start_col, cursor);
        draw_status_bar(buffer, cursor, modified, filename);
        position_cursor(buffer, cursor, text_start_col);
        
        refresh();
    }

    void Viewport::update_dimensions() {
        getmaxyx(stdscr, height, width);
    }

    // void Viewport::clear_screen() {
    //     attron(COLOR_PAIR(1));
    //     clear();
    // }

    int Viewport::calculate_text_start_column(int cursor_line, int total_lines) const {
        if (show_line_numbers) {
            draw_line_numbers(cursor_line, total_lines);
            return LINE_NUMBERS_WIDTH;
        }
        return 0;
    }

    void Viewport::draw_buffer_content(const Buffer& buffer, int cursor_line, int text_start_col, const Cursor& cursor) {
        for (int screen_row = 0; screen_row < height - 1; ++screen_row) {
            const int buffer_line = viewport_y + screen_row;
            if (buffer_line >= buffer.line_count()) break;
            
            draw_line(buffer, buffer_line, screen_row, text_start_col, 
                    buffer_line == cursor_line, cursor);
        }
    }

    void Viewport::draw_line(const Buffer& buffer, int buffer_line, int screen_row, int start_col, bool is_cursor_line, const Cursor& cursor) {
        const auto& line = buffer.get_line(buffer_line);

        attron(COLOR_PAIR(1));
        mvaddnstr(screen_row, start_col, line.data(), line.size());
        clrtoeol();

        if (is_cursor_line) {
            const auto [_, cursor_col] = cursor.position();
            const int cursor_screen_col = cursor_col + start_col;

            if (cursor_screen_col < width && static_cast<size_t>(cursor_col) < line.size()) {
                attron(COLOR_PAIR(2));
                mvaddch(screen_row, cursor_screen_col, line[cursor_col]);
                attroff(COLOR_PAIR(2));
            }
        }
    }

    void Viewport::position_cursor(const Buffer& buffer, const Cursor& cursor, int text_start_col) {
        const auto [cursor_line, cursor_col] = cursor.position();
        
        if (is_cursor_visible(cursor_line)) {
            const int screen_row = cursor_line - viewport_y;
            const int screen_col = std::min(cursor_col, static_cast<int>(buffer.get_line(cursor_line).size())) + text_start_col;
            move(screen_row, screen_col);
        }
    }

    bool Viewport::is_cursor_visible(int cursor_line) const {
        return cursor_line >= viewport_y && cursor_line < viewport_y + height - 1;
    }

    void Viewport::draw_status_bar(const Buffer& buffer, const Cursor& cursor, bool modified, const std::string& filename) {
        attron(COLOR_PAIR(1) | A_BOLD);
        
        const auto [line, col] = cursor.position();
        const std::string display_name = filename.empty() ? "[No Name]" : filename;
        
        mvhline(height - 1, 0, ' ', width);
        mvprintw(height - 1, 0, " %s | %d/%d | %d:%d %s",
                display_name.c_str(),
                line + 1, buffer.line_count(),
                line + 1, col + 1,
                modified ? "[+]" : "");
        
        std::string version = "VAR 1.1";
        mvprintw(height - 1, width - version.length() - 1, "%s", version.c_str());

        attroff(COLOR_PAIR(1) | A_BOLD);
    }

    void Viewport::draw_line_numbers(int current_line, int total_lines) const {
        for (int screen_row = 0; screen_row < height - 1; ++screen_row) {
            const int line_num = viewport_y + screen_row + 1;
            if (line_num > total_lines) break;

            const bool is_current_line = (line_num == current_line + 1);
            draw_line_number(screen_row, line_num, is_current_line);
        }
    }

    void Viewport::draw_line_number(int screen_row, int line_num, bool is_current_line) const {
        if (is_current_line) {
            attron(A_BOLD);
        }

        mvprintw(screen_row, 0, "%4d ", line_num);
        mvaddch(screen_row, LINE_NUMBERS_SEPARATOR_COL, ACS_VLINE);

        if (is_current_line) {
            attroff(A_BOLD);
        }
    }

    void Viewport::update_size(int w, int h) {
        width = w;
        height = h;
    }

    void Viewport::toggle_line_numbers() {
        show_line_numbers = !show_line_numbers;
    }

    int Viewport::get_y() const {
        return viewport_y;
    }

    void Viewport::set_y(int y) {
        viewport_y = y;
    }

}