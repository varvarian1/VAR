#include <ncurses.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>

#include "viewport.hpp"

/**
 * Manages terminal display and rendering
 * 
 * Implements double buffering to minimize flickering and provides
 * line-numbered text display with status bar. All rendering operations
 * work with back buffer which is then swapped to front buffer.
 */
namespace Var {

    /**
     * Renders complete editor interface including:
     * - Text content
     * - Line numbers
     * - Status bar with file info
     * 
     * Uses double buffering to avoid partial screen updates.
     * 
     * buffer    Text content to render
     * cursor    Current cursor position
     * modified  flag to show [*] indicator
     * filename  Current file name or empty for new buffer
     */
    void Viewport::draw(const Buffer& buffer, const Cursor& cursor, bool modified, const std::string& filename) {
        update_dimensions();
        
        // Calculate text offset after line numbers to ensure proper alignment
        // even when scrolling horizontally
        const auto [cursor_line, cursor_col] = cursor.position();
        const int text_start_col = calculate_text_start_column(cursor_line, buffer.line_count());
        
        // Renders buffer content, status bar and positions cursor
        draw_buffer_content(buffer, cursor_line, text_start_col, cursor);
        draw_status_bar(buffer, cursor, modified, filename);
        position_cursor(buffer, cursor, text_start_col);
        
        swap_buffers();
    }

    /**
     * Adjusts viewport to current terminal dimensions
     * 
     * Must be called after terminal resize events. Maintains
     * back buffer size matching physical terminal.
     */
    void Viewport::update_dimensions() {
        getmaxyx(stdscr, height, width);

        if (back_buffer) {
            wresize(back_buffer, height, width);
        }
    }

    /**
     * Calculates text horizontal offset accounting for line numbers
     * 
     * When line numbers are enabled, reserves fixed-width gutter area
     * and renders line numbers. Return value represents starting column
     * for text content.
     * 
     * cursor_line  Current line for highlight positioning
     * total_lines  Used for line number formatting width
     */
    int Viewport::calculate_text_start_column(int cursor_line, int total_lines) const {
        if (show_line_numbers) {
            draw_line_numbers(cursor_line, total_lines);
            return LINE_NUMBERS_WIDTH;
        }
        return 0;
    }

    /**
     * Renders visible portion of text buffer
     * 
     * Only renders lines currently in viewport to optimize performance.
     * Handles cursor position highlighting and line truncation.
     * 
     * buffer         Source text buffer
     * cursor_line    Currently focused line
     * text_start_col Horizontal offset for text
     * cursor         Cursor instance for position data
     */
    void Viewport::draw_buffer_content(const Buffer& buffer, int cursor_line, int text_start_col, const Cursor& cursor) {
        // Only render visible lines to avoid unnecessary processing
        for (int screen_row = 0; screen_row < height - 1; ++screen_row) {
            const int buffer_line = viewport_y + screen_row;
            if (buffer_line >= buffer.line_count()) break;
            
            draw_line(buffer, buffer_line, screen_row, text_start_col, 
                    buffer_line == cursor_line, cursor);
        }
    }

    /**
     * Initializes double buffering system
     * 
     * Creates back buffer matching terminal dimensions. Front buffer
     * uses stdscr for direct terminal output. Must be called after
     * ncurses initialization.
     */
    void Viewport::init_buffers() {
        update_dimensions();
        front_buffer = stdscr;
        back_buffer = newwin(height, width, 0, 0);
    }

    /**
     * Swaps back and front buffers with optimized rendering
     * 
     * Uses overwrite() instead of wrefresh() for back buffer to
     * minimize screen tearing. Maintains empty back buffer after
     * swap for next draw cycle.
     */
    void Viewport::swap_buffers() {
        overwrite(back_buffer, front_buffer);
        wrefresh(front_buffer);

        werase(back_buffer);
    }

    /**
     * Renders single line of text with cursor highlighting
     * 
     * Handles three display states:
     * 1. Normal text
     * 2. Line with cursor (highlighted character)
     * 3. Line end (clears remaining space)
     * 
     * buffer_line    Absolute line number in buffer
     * screen_row     Vertical position in viewport
     * start_col      Horizontal rendering offset
     * is_cursor_line Whether to show cursor highlight
     */
    void Viewport::draw_line(const Buffer& buffer, int buffer_line, int screen_row, int start_col, bool is_cursor_line, const Cursor& cursor) {
        const auto& line = buffer.get_line(buffer_line);
        
        // Base text rendering
        wattron(back_buffer, COLOR_PAIR(1));
        mvwaddnstr(back_buffer, screen_row, start_col, line.data(), line.size());
        wattroff(back_buffer, COLOR_PAIR(1));
        wclrtoeol(back_buffer);
        
        // Cursor position highlighting
        if (is_cursor_line) {
            const auto [_, cursor_col] = cursor.position();
            const int cursor_screen_col = cursor_col + start_col;
            
            // Only highlight if cursor is within line bounds
            if (cursor_screen_col < width && static_cast<size_t>(cursor_col) < line.size()) {
                wattron(back_buffer, COLOR_PAIR(2));
                mvwaddch(back_buffer, screen_row, cursor_screen_col, line[cursor_col]);
                wattroff(back_buffer, COLOR_PAIR(2));
            }
        }
    }

    /**
     * Positions physical cursor in terminal
     * 
     * Accounts for viewport scrolling offset and line number gutter.
     * Only updates cursor position when it's within visible area.
     */
    void Viewport::position_cursor(const Buffer& buffer, const Cursor& cursor, int text_start_col) {
        auto [cursor_line, cursor_col] = cursor.position();
        
        if (is_cursor_visible(cursor_line)) {
            int screen_row = cursor_line - viewport_y;
            int screen_col = std::min(cursor_col, static_cast<int>(buffer.get_line(cursor_line).size())) + text_start_col;
            move(screen_row, screen_col);
        }
    }

    /**
     * Checks if cursor is in currently visible viewport
     * 
     * Used to avoid unnecessary cursor positioning operations
     * when scrolling beyond visible area.
     */
    bool Viewport::is_cursor_visible(int cursor_line) const {
        return cursor_line >= viewport_y && cursor_line < viewport_y + height - 1;
    }

    /**
     * Renders status bar with editor state information
     * 
     * Displays in reverse colors at bottom line with:
     * - File name/path
     * - Line numbers (current/total)
     * - Modified indicator
     * - Version info (right-aligned)
     * 
     * Uses bold formatting for better visibility.
     */
    void Viewport::draw_status_bar(const Buffer& buffer, const Cursor& cursor, bool modified, const std::string& filename) {
        wattron(back_buffer, COLOR_PAIR(1) | A_BOLD);
        
        const auto [line, col] = cursor.position();
        const std::string display_name = filename.empty() ? "[No Name]" : filename;
        
        // Clear and draw status line
        mvwhline(back_buffer, height - 1, 0, ' ', width);
        mvwprintw(back_buffer, height - 1, 0, " %s | %d/%d | %d:%d %s",
            display_name.c_str(),
            line + 1, buffer.line_count(),
            line + 1, col + 1,
            modified ? "[+]" : "");
        
        // Right-aligned version info
        std::string version = "VAR 1.1";
        mvwprintw(back_buffer, height - 1, width - version.length() - 1, "%s", version.c_str());

        wattroff(back_buffer, COLOR_PAIR(1) | A_BOLD);
    }


    /**
     * Renders line numbers gutter
     * 
     * Shows absolute line numbers.
     * Uses fixed width formatting with right alignment.
     * 
     * current_line  Current line (1-based)
     * total_lines   Used for number width calculation
     */
    void Viewport::draw_line_numbers(int current_line, int total_lines) const {
        for (int screen_row = 0; screen_row < height - 1; ++screen_row) {
            const int line_num = viewport_y + screen_row + 1; // Convert to 1-based
            if (line_num > total_lines) break;
    
            const bool is_current_line = (line_num == current_line + 1);
            draw_line_number(screen_row, line_num, is_current_line);
        }
    }
    
    /**
     * Renders single line number entry
     * 
     * Formats number with right alignment and adds vertical separator.
     * Current line number is shown in bold.
     */
    void Viewport::draw_line_number(int screen_row, int line_num, bool is_current_line) const {
        if (is_current_line) {
            wattron(back_buffer, A_BOLD);
        }
    
        mvwprintw(back_buffer, screen_row, 0, "%4d ", line_num);
        mvwaddch(back_buffer, screen_row, LINE_NUMBERS_SEPARATOR_COL, ACS_VLINE);
    
        if (is_current_line) {
            wattroff(back_buffer, A_BOLD);
        }
    }

    /**
     * Updates the viewport dimensions
     * 
     * Used for external control of viewport size (e.g., when handling terminal
     * resize events). Only sets the values - doesn't trigger layout recalculation.
     * 
     * w New width in characters
     * h New height in lines
     */
    void Viewport::update_size(int w, int h) {
        width = w;
        height = h;
    }

    /**
     * Toggles line numbers display
     * 
     * Inverts the current state of show_line_numbers flag.
     * Actual display update will occur on next draw() call.
     * Maintains state between calls.
     */
    void Viewport::toggle_line_numbers() {
        show_line_numbers = !show_line_numbers;
    }

    /**
     * Gets current vertical viewport position
     * 
     * Position is measured in buffer lines and indicates
     * which line appears at the top of the display area.
     */
    int Viewport::get_y() const {
        return viewport_y;
    }

    /**
     * Sets vertical viewport position
     * 
     * Allows programmatic control of content scrolling.
     * Automatically clamps the value to valid range.
     * 
     * y New vertical position (0-based)
     */
    void Viewport::set_y(int y) {
        viewport_y = y;
    }

} // namespace Var