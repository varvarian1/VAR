#include <ncurses.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>

#include <editor.hpp>

namespace Var {

    void Editor::build_line_index() {
        line_offsets.clear();
        line_offsets.push_back(0);
        for (size_t i = 0; i < text.size(); ++i) {
            if (text[i] == '\n' && i + 1 < text.size())
                line_offsets.push_back(i + 1);
        }
    }

    std::string_view Editor::get_line(int line_number) {
        if (line_number < 0 || (size_t)line_number >= line_offsets.size())
            return "";
        size_t start = line_offsets[line_number];
        size_t end = text.find('\n', start);
        if (end == std::string::npos) end = text.size();
        return std::string_view(text.data() + start, end - start);
    }

    int Editor::line_count() {
        return line_offsets.size();
    }

    void Editor::update_line_index_from(size_t pos) {
        int line = 0;
        while (line + 1 < (int)line_offsets.size() && line_offsets[line + 1] <= pos) {
            ++line;
        }

        line_offsets.resize(line + 1);

        size_t scan_pos = pos;
        while (true) {
            size_t nl_pos = text.find('\n', scan_pos);
            if (nl_pos == std::string::npos) break;
            if (nl_pos + 1 < text.size())
                line_offsets.push_back(nl_pos + 1);
            scan_pos = nl_pos + 1;
        }
    }

    void Editor::draw() {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        clear();

        for (int i = 0; i < rows; ++i) {
            int line_index = viewport_y + i;
            if (line_index >= line_count()) break;
            auto line = get_line(line_index);
            mvaddnstr(i, 0, line.data(), line.size());
            clrtoeol();
        }

        if (cursor_line >= viewport_y && cursor_line < viewport_y + rows) {
            int cy = cursor_line - viewport_y;
            int cx = std::min(cursor_col, (int)get_line(cursor_line).size());
            move(cy, cx);
        }

        refresh();
    }

    void Editor::clamp_cursor() {
        int max_line = line_count() - 1;
        cursor_line = std::clamp(cursor_line, 0, max_line);
        cursor_col = std::clamp(cursor_col, 0, (int)get_line(cursor_line).size());

        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        if (cursor_line < viewport_y)
            viewport_y = cursor_line;
        else if (cursor_line >= viewport_y + rows)
            viewport_y = cursor_line - rows + 1;
    }

    void Editor::move_cursor_left() {
        if (cursor_col > 0) {
            cursor_col--;
        } else if (cursor_line > 0) {
            cursor_line--;
            cursor_col = get_line(cursor_line).size();
        }
    }

    void Editor::move_cursor_right() {
        if (cursor_col < (int)get_line(cursor_line).size()) {
            cursor_col++;
        } else if (cursor_line + 1 < line_count()) {
            cursor_line++;
            cursor_col = 0;
        }
    }

    void Editor::move_cursor_up() {
        if (cursor_line > 0) {
            cursor_line--;
            cursor_col = std::min(cursor_col, (int)get_line(cursor_line).size());
        }
    }

    void Editor::move_cursor_down() {
        if (cursor_line + 1 < line_count()) {
            cursor_line++;
            cursor_col = std::min(cursor_col, (int)get_line(cursor_line).size());
        }
    }

    void Editor::insert_char(char ch) {
        size_t abs_pos = line_offsets[cursor_line] + cursor_col;
        text.insert(text.begin() + abs_pos, ch);
        update_line_index_from(abs_pos);

        if (ch == '\n') {
            cursor_line++;
            cursor_col = 0;
        } else {
            cursor_col++;
        }
    }

    void Editor::delete_char_before_cursor() {
        if (cursor_line == 0 && cursor_col == 0) return;

        if (cursor_col > 0) {
            size_t abs_pos = line_offsets[cursor_line] + cursor_col - 1;
            text.erase(text.begin() + abs_pos);
            update_line_index_from(abs_pos);
            cursor_col--;
        } else {
            size_t prev_line_end = line_offsets[cursor_line] - 1;
            text.erase(text.begin() + prev_line_end);
            update_line_index_from(prev_line_end);
            cursor_line--;
            cursor_col = get_line(cursor_line).size();
        }
    }

    void Editor::handle_input(int ch) {
        switch (ch) {
            case KEY_UP:    move_cursor_up(); break;
            case KEY_DOWN:  move_cursor_down(); break;
            case KEY_LEFT:  move_cursor_left(); break;
            case KEY_RIGHT: move_cursor_right(); break;
            case KEY_BACKSPACE:
            case 127:
                delete_char_before_cursor();
                modified = true;
                break;
            case 's' & 0x1f: // Ctrl+s
                save_file();
                break;
            case 'x' & 0x1f: // Ctrl+x
                running = false;
                break;
            default:
                if (isprint(ch) || ch == '\n') {
                    insert_char((char)ch);
                    modified = true;
                }
                break;
        }
        clamp_cursor();
    }

    std::string Editor::readFileToString(const std::string& filename) {
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file) {
            throw std::runtime_error("Unable to open file: " + filename);
        }
    
        std::ostringstream contents;
        contents << file.rdbuf();
        return contents.str();
    }

    void Editor::load_file(std::string file_path) {
        filename = file_path;
        text = readFileToString(file_path);
        build_line_index();
    }

    void Editor::run() {
        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();
        curs_set(1);

        draw();

        while (running) {
            int ch = getch();
            handle_input(ch);
            draw();
        }

        endwin();
    }

    void Editor::save_file() {
        std::ofstream file(filename);
        if (!file) {
            mvprintw(0, 0, "Error saving file!");
            return;
        }

        file << text;
        modified = false;
    }
};