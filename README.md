# VAR - Minimalist Terminal Text Editor

### About VAR

VAR is a lightweight, terminal-based text editor written in C++ using ncurses. It's designed for quick file editing with minimal dependencies.

### Features
- [x] Basic text editing
- [x] File saving/loading
- [ ] Syntax highlighting
- [ ] Multiple tabs
- [ ] Configuration file

### Usage

```bash
var filename.txt
```

```bash
Options:
  -h, --help       Show this help message
  -v, --version    Display version information

Controls:
  Arrow keys       Move cursor
  Ctrl+S           Save file
  Ctrl+X           Exit
  Ctrl+L           Show or hide line numbers
```

### Prerequisites

- GCC (GNU Compiler Collection)
- CMake (3.10 or higher)
- Ncurses library

On Ubuntu/Debian:
```bash
sudo apt install build-essential cmake libncurses5-dev
```

### Building from source
```bash
mkdir build && cd build
cmake ..
make
sudo make install
```