#ifndef EDITOR
#define EDITOR

#include <string>
#include <vector>

namespace Var {
    class Editor {
    private:
        std::string filename;
        std::vector<std::string> lines;
        int cursor_x = 0, cursor_y = 0;
        bool modified = false;
        
    public:
        Editor(const std::string& file);
        void load_file();
        void run();
        void handle_input(int ch, bool &running);
        void refresh_screen();
        void save_file();
    };
}

#endif