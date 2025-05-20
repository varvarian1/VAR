#ifndef VIEWPORT
#define VIEWPORT

#include "buffer.hpp"
#include "cursor.hpp"

namespace Var {

    class Viewport {
    private:
        int viewport_y = 0;
        int width = 0;
        int height = 0;
        bool show_line_numbers = true;

        static constexpr int LINE_NUMBERS_WIDTH = 6;
        static constexpr int LINE_NUMBERS_SEPARATOR_COL = 5;
            
    public:
        void draw(const Buffer& buffer, const Cursor& cursor, bool modified, const std::string& filename);
        void update_dimensions();
        void clear_screen();
        int calculate_text_start_column(int cursor_line, int total_lines) const;
        void draw_buffer_content(const Buffer& buffer, int cursor_line, int text_start_col, const Cursor& cursor);
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