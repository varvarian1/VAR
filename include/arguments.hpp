#ifndef ARGUMENTS
#define ARGUMENTS

#include <vector>
class ArgumentParser {
private:
    int argc;
    char** argv;

public:
    std::vector<std::string> vec;

    ArgumentParser(int argc, char** argv);

    bool parse();
    void print_help();
    void print_version();
    
};

#endif