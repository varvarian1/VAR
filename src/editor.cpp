#include <ncurses.h>
#include <string>
#include <vector>
#include <fstream>

#include <editor.hpp>

namespace Var {

    Editor::Editor(const std::string& file) : filename(file) {
        load_file();
    }

    void Editor::load_file() {
        std::ifstream file(filename); 
        if (!file) {
            lines.push_back("");
            return;
        }

        std::string line;
        while (getline(file, line)) {
            lines.push_back(line);
        }
        if (lines.empty()) lines.push_back("");
    }

    void Editor::refresh_screen() {
        for (int i = 0; i < lines.size(); i++) {
            mvprintw(i, 0, "%s", lines[i].c_str());
        }
        move(cursor_y, cursor_x);
    }

    void Editor::run() {
        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();
        curs_set(1);

        refresh_screen();

        bool running = true;
        while (running) {
            int ch = getch();
            handle_input(ch, running);
            refresh_screen();
        }

        endwin();
    }

    void Editor::handle_input(int ch, bool &running) {
        switch (ch) {
            case KEY_UP:
                if (cursor_y > 0) cursor_y--;
                break;
            case KEY_LEFT:
                if (cursor_x > 0) cursor_x--;
                break;
            case KEY_DOWN:
                if (cursor_y < lines.size() - 1) cursor_y++;
                break;
            case KEY_RIGHT:
                if (cursor_x < lines[cursor_y].length()) cursor_x++;
                break;
            case KEY_BACKSPACE: //Backspace
                //backspace
                break;
            case '\n': //Enter
            case '\r':
                //enter
            case 's' & 0x1f: // Ctrl+s
                save_file();
                break;
            case 'x' & 0x1f: // Ctrl+x
                running = false;
                break;
            default:
                if (isprint(ch)) { 
                    lines[cursor_y].insert(cursor_x, 1, ch);
                    cursor_x++;
                    modified = true;
                }
                break;
        }
    }

    void Editor::save_file() {
        std::ofstream file(filename);
        if (!file) {
            mvprintw(0, 0, "Error saving file!");
            return;
        }
        for (const auto &line : lines) {
            file << line << '\n';
        }
        modified = false;
    }
};