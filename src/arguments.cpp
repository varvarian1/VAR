#include <iostream>
#include <getopt.h>

void print_help() {
    std::cout << "Usage: var [OPTION]... [FILE]...\n"
              << "Edit text files.\n"
              << "Options:\n"
              << "  -h, --help     display this help and exit\n"
              << "  -V, --version  show program version and exit\n";
}

void print_version() {
    std::cout << "program version 1.0\n";
}