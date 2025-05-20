#ifndef BUFFER
#define BUFFER

#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <string_view>

namespace Var {

    class Buffer {
    private:
        std::string text;
        std::vector<size_t> line_offsets;

    public:
        void load_file(const std::string& file_path, std::string& filename);
        void reset_buffer_state();
        void load_and_process_file(const std::string& file_path, std::string& filename);
        void load_file_content(const std::string& file_path);
        void ensure_minimum_buffer_state();
        void initialize_with_empty_line();
        void handle_load_error(std::string& filename);
        void save_file(const std::string& filename) const;
        std::string_view get_line(int line_number) const;
        int line_count() const;
        void insert_char(int line, int col, char ch);
        void delete_char_before_cursor(int& line, int& col);
        const std::string& get_text() const;
        void build_line_index();
        void update_line_index_from(size_t pos);
        bool is_invalid_line(int line) const;
        bool is_at_beginning(int line, int col) const;
        bool has_next_position(size_t i) const;
        std::pair<size_t, size_t> get_line_boundaries(int line) const;
        int find_line_for_position(size_t pos) const;
        void scan_for_newlines(size_t start_pos);
        void delete_char_in_line(int line, int& col);
        std::string read_file_to_string(const std::string& filename);
        size_t calculate_absolute_position(int line, int col) const;
        void handle_line_deletion(int& line, int& col);

    };
}

#endif