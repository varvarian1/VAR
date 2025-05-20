#include <iostream>
#include <getopt.h>

#include <arguments.hpp>

/**
 * Constructs an ArgumentParser object
 * argc Argument count (from main)
 * argv Argument vector (from main)
 * 
 * Initializes the argument parser with command line arguments.
 * The constructor stores references to the arguments for later parsing.
 */

ArgumentParser::ArgumentParser(int argc, char** argv) : argc(argc), argv(argv) {}

/**
 * Parses command line arguments
 * bool - true if arguments are valid and contain files to process,
 *               false if arguments are invalid or no files specified
 * 
 * Processes command line options and stores remaining arguments in vector.
 * Displays usage message if no arguments are provided.
 * 
 * Flow:
 * 1. Processes all command line options using getopt()
 * 2. Stores non-option arguments in member vector 'vec'
 * 3. Returns true if files were specified, false otherwise
 */

bool ArgumentParser::parse() {
    int opt; //current options
    
    while ((opt = getopt(argc, argv, "hV")) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                break;
            case 'V':
                print_version();
                break;
            default:
                std::cerr << "Unknown argument. Use -h for help.\n";
                break;
        }
    }

    for (int i = optind; i < argc; i++) {
        vec.push_back(argv[i]);
        std::cout << argv[i] << std::endl; 
    }

    if (vec.size() > 0) {
        return true;
    }
    printf("Usage: %s <filename>\n", argv[0]);
    return false; 
}

void ArgumentParser::print_help() {
    std::cout << "Usage: var [OPTION]... [FILE]...\n"
              << "Edit text files.\n"
              << "Options:\n"
              << "  -h, --help     display this help and exit\n"
              << "  -V, --version  show program version and exit\n";
}

void ArgumentParser::print_version() {
    std::cout << "program version 1.1\n";
}