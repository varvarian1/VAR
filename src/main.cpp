#include <iostream>
#include <unistd.h>
#include <getopt.h>

#include <editor.hpp>
#include <arguments.hpp>

int main(int argc, char *argv[]) {
    
    static struct option long_options[] = {
        {"help",    no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "hV", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return 0;
            case 'V':
                print_version();
                return 0;
            default:
                std::cout << "Unknown argument...\n";
                return 1;
        }
    }

    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    Var::Editor editor(argv[1]);
    editor.run();
    
    return 0;
}