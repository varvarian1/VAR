#include <iostream>

#include <editor.hpp>
#include <arguments.hpp>

int main(int argc, char *argv[]) {

    ArgumentParser argument(argc, argv);
    if (!argument.parse()) {
        return 1;
    }

    if (argument.vec.size() > 0) {
        Var::Editor::get().load_file(argument.vec[0]);
    }
    Var::Editor::get().run();
    
    return 0;
}