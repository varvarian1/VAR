#include <ncurses.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>

#include "editor.hpp"

namespace Var {
    
    Editor& Editor::get() {
        static Editor instance;
        return instance;
    }
    
    void Editor::load_file(const std::string& file_path) {
        buffer.load_file(file_path, filename);
        cursor.set_position(0, 0);
        viewport.set_y(0);
        modified = false;
    }
    
    void Editor::run() {
        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();
        curs_set(3);

        start_color();
        use_default_colors();
        
        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
        init_pair(3, COLOR_CYAN, COLOR_BLACK);

        attron(COLOR_PAIR(2)); 
        bkgd(COLOR_PAIR(1));
    
        while (running) {
            viewport.draw(buffer, cursor, modified, filename);
            int ch = getch();
            handle_input(ch);
            
            if (ch == KEY_RESIZE) {
                int rows, cols;
                getmaxyx(stdscr, rows, cols);
                viewport.update_size(cols, rows);
            }
        }
    
        endwin();
    }

    void Editor::handle_input(int ch) {
        auto [cursor_line, cursor_col] = cursor.position();
        int viewport_y = viewport.get_y();
    
        switch (ch) {
            case KEY_UP:    
                cursor.move_up(buffer); 
                break;
            case KEY_DOWN:  
                cursor.move_down(buffer); 
                break;
            case KEY_LEFT:  
                cursor.move_left(buffer); 
                break;
            case KEY_RIGHT: 
                cursor.move_right(buffer); 
                break;
            case KEY_BACKSPACE:
            case 127:
                buffer.delete_char_before_cursor(cursor_line, cursor_col);
                cursor.move_left(buffer);
                modified = true;
                break;
            case 's' & 0x1f: // Ctrl+S
                try {
                    buffer.save_file(filename);
                    modified = false;
                } catch (const std::runtime_error& e) {
                    attron(COLOR_PAIR(2));
                    mvprintw(0, 0, "Error: %s", e.what());
                    attroff(COLOR_PAIR(2));
                    refresh();
                    getch();
                }
                break;
            case 'l' & 0x1f: // Ctrl+L
                viewport.toggle_line_numbers();
                break;
            case 'x' & 0x1f: // Ctrl+X
                running = false;
                break;
            default:
                if (isprint(ch) || ch == '\n') {
                    buffer.insert_char(cursor_line, cursor_col, (char)ch);
                    if (ch == '\n') {
                        cursor.set_position(cursor_line + 1, 0);
                    } else {
                        cursor.move_right(buffer);
                    }
                    modified = true;
                }
                break;
        }
        cursor.clamp(buffer, viewport_y);
        viewport.set_y(viewport_y);
    }
};