#ifndef EDITOR
#define EDITOR

#include <string>
#include <vector>

#include "cursor.hpp"
#include "buffer.hpp"
#include "viewport.hpp"

namespace Var {
        
    class Editor {
    private:
        Buffer buffer;
        Cursor cursor;
        Viewport viewport;
        std::string filename;
        bool running = true;
        bool modified = false;
            
    public:
        static Editor& get();
        void load_file(const std::string& file_path);
        void run();
        void handle_input(int ch);
        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;
            
    private:
        Editor() = default;
    };
}

#endif