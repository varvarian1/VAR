#ifndef VIEWPORT
#define VIEWPORT

#include <ncurses.h>

#include "buffer.hpp"
#include "cursor.hpp"

namespace Var {

    /**
     * Handles terminal display rendering
     *
     * Implements:
     * - Double-buffered display to prevent flickering
     * - Line number gutter
     * - Status bar with file information
     * - Cursor position highlighting
     * - Viewport scrolling
     */
    class Viewport {
    private:
        // Vertical scroll offset (lines scrolled down)
        int viewport_y = 0;

        // Current dimensions of visible area
        int width = 0; // in characters
        int height = 0; // in lines

        // Toggle for line numbers display
        bool show_line_numbers = true;

        // Double buffering system
        WINDOW* front_buffer; // Primary buffer (stdscr)
        WINDOW* back_buffer; // Secondary buffer for rendering

        // Line numbers gutter formatting
        static constexpr int LINE_NUMBERS_WIDTH = 6; // Total gutter width
        static constexpr int LINE_NUMBERS_SEPARATOR_COL = 5; // Position of '|' separator
            
    public:
        void draw(const Buffer& buffer, const Cursor& cursor, bool modified, const std::string& filename);
        void update_dimensions();
        int calculate_text_start_column(int cursor_line, int total_lines) const;
        void draw_buffer_content(const Buffer& buffer, int cursor_line, int text_start_col, const Cursor& cursor);
        void init_buffers();
        void swap_buffers();
        void draw_line(const Buffer& buffer, int buffer_line, int screen_row, int start_col, bool is_cursor_line, const Cursor& cursor);
        void position_cursor(const Buffer& buffer, const Cursor& cursor, int text_start_col);
        bool is_cursor_visible(int cursor_line) const;
        void draw_status_bar(const Buffer& buffer, const Cursor& cursor, bool modified, const std::string& filename);
        void draw_line_numbers(int current_line, int total_lines) const;
        void draw_line_number(int screen_row, int line_num, bool is_current_line) const;
        void update_size(int w, int h);
        void toggle_line_numbers();
        int get_y() const;
        void set_y(int y);

    };
}

#endif