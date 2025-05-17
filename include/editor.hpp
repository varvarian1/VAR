#ifndef EDITOR
#define EDITOR

#include <string>
#include <vector>

namespace Var {
    class Editor {
    private:
        Editor() = default;
        std::string filename = "";
        std::string text;
        std::vector<size_t> line_offsets;
        int cursor_line = 0;
        int cursor_col = 0;
        int viewport_y = 0;
        bool running = true;
        bool modified = false;
        
    public:
        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        static Editor& get() {
            static Editor instance;
            return instance;
        }

        std::string readFileToString(const std::string& filename);
        std::string_view get_line(int line_number);

        void build_line_index();
        int line_count();
        void draw();
        void clamp_cursor();
        void move_cursor_left();
        void move_cursor_right();
        void update_line_index_from(size_t pos);
        void move_cursor_up();
        void move_cursor_down();
        void insert_char(char ch);
        void delete_char_before_cursor();
        void load_file(std::string file_path);
        void run();
        void handle_input(int ch);
        void save_file();
    };
}

#endif