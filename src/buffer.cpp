#include <ncurses.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <algorithm>

#include "buffer.hpp"

namespace Var {

    void Buffer::load_file(const std::string& file_path, std::string& filename) {
        reset_buffer_state();
        try {
            load_and_process_file(file_path, filename);
        } catch (const std::exception& e) {
            handle_load_error(filename);
            throw;
        }
    }

    void Buffer::reset_buffer_state() {
        text.clear();
        line_offsets.clear();
    }
    
    void Buffer::load_and_process_file(const std::string& file_path, std::string& filename) {
        load_file_content(file_path);
        filename = file_path;
        ensure_minimum_buffer_state();
    }
    
    void Buffer::load_file_content(const std::string& file_path) {
        text = read_file_to_string(file_path);
        build_line_index();
    }
    
    void Buffer::ensure_minimum_buffer_state() {
        if (line_offsets.empty()) {
            initialize_with_empty_line();
        }
    }
    
    void Buffer::initialize_with_empty_line() {
        line_offsets.push_back(0);
        text = "";
    }
    
    void Buffer::handle_load_error(std::string& filename) {
        initialize_with_empty_line();
        filename.clear();
    }
    
    void Buffer::save_file(const std::string& filename) const {
        if (filename.empty()) {
            throw std::runtime_error("No filename provided");
        }
    
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + filename);
        }
    
        try {
            if (!text.empty()) {
                file.write(text.data(), text.size());
            }

            if (!file.good()) {
                throw std::runtime_error("Failed to write to file: " + filename);
            }
        } catch (const std::exception& e) {
            file.close();
            throw;
        }
    
        file.close();
    }
    
    std::string_view Buffer::get_line(int line_number) const {
        if (is_invalid_line(line_number)) {
            return "";
        }
        
        const auto [start, end] = get_line_boundaries(line_number);
        return {text.data() + start, end - start};
    }
    
    int Buffer::line_count() const {
        return static_cast<int>(line_offsets.size());
    }
    
    void Buffer::insert_char(int line, int col, char ch) {
        const size_t pos = calculate_absolute_position(line, col);
        text.insert(text.begin() + pos, ch);
        update_line_index_from(pos);
    }
    
    void Buffer::delete_char_before_cursor(int& line, int& col) {
        if (is_at_beginning(line, col)) return;
        
        if (col > 0) {
            delete_char_in_line(line, col);
        } else {
            handle_line_deletion(line, col);
        }
    }
    
    const std::string& Buffer::get_text() const {
        return text;
    }

    void Buffer::build_line_index() {
        line_offsets = {0};
        
        for (size_t i = 0; i < text.size(); ++i) {
            if (text[i] == '\n' && has_next_position(i)) {
                line_offsets.push_back(i + 1);
            }
        }
    }

    void Buffer::update_line_index_from(size_t pos) {
        const int line = find_line_for_position(pos);
        line_offsets.resize(line + 1);
        scan_for_newlines(pos);
    }

    bool Buffer::is_invalid_line(int line) const {
        return line < 0 || static_cast<size_t>(line) >= line_offsets.size();
    }

    bool Buffer::is_at_beginning(int line, int col) const {
        return line == 0 && col == 0;
    }

    bool Buffer::has_next_position(size_t i) const {
        return i + 1 < text.size();
    }

    std::pair<size_t, size_t> Buffer::get_line_boundaries(int line) const {
        const size_t start = line_offsets[line];
        const size_t end = text.find('\n', start);
        return {start, end != std::string::npos ? end : text.size()};
    }

    int Buffer::find_line_for_position(size_t pos) const {
        int line = 0;
        while (line + 1 < static_cast<int>(line_offsets.size()) && 
            line_offsets[line + 1] <= pos) {
            ++line;
        }
        return line;
    }

    void Buffer::scan_for_newlines(size_t start_pos) {
        size_t pos = start_pos;
        while (true) {
            const size_t nl_pos = text.find('\n', pos);
            if (nl_pos == std::string::npos) break;
            
            if (has_next_position(nl_pos)) {
                line_offsets.push_back(nl_pos + 1);
            }
            pos = nl_pos + 1;
        }
    }

    void Buffer::delete_char_in_line(int line, int& col) {
        const size_t pos = calculate_absolute_position(line, col - 1);
        text.erase(text.begin() + pos);
        update_line_index_from(pos);
        col--;
    }

    std::string Buffer::read_file_to_string(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Unable to open file: " + filename);
        }
        
        return {std::istreambuf_iterator<char>(file), 
                std::istreambuf_iterator<char>()};
    }

    size_t Buffer::calculate_absolute_position(int line, int col) const {
        return line_offsets[line] + col;
    }
    
    void Buffer::handle_line_deletion(int& line, int& col) {
        const size_t prev_line_end = line_offsets[line] - 1;
        text.erase(text.begin() + prev_line_end);
        update_line_index_from(prev_line_end);
        line--;
        col = get_line(line).size();
    }
}