
#pragma once

#include <tte/common/number_types.hpp>

namespace tte { namespace engine {
    using Char = char;
    struct Buffer;

    extern Buffer& create_buffer();
    extern void destroy_buffer(Buffer&);
    extern bool insert_empty_line(Buffer&, const Length line_index);
    extern bool insert_line(Buffer&, const Length line_index, const Char* data);
    extern bool insert_line(Buffer&, const Length line_index, const Char* data, const Length data_length);
    extern bool insert_empty_lines(Buffer&, const Length number_of_lines, const Length line_index);
    extern bool insert_lines(Buffer&,
        const Length number_of_lines,
        const Length line_index,
        const Char** data_array,
        const Length* data_length_array);
    extern bool insert_lines(Buffer&, const Length number_of_lines, const Length line_index, const Char** data_array);
    extern bool insert_character(Buffer&, const Length line_index, const Length character_index, const Char character);
    extern bool insert_characters(Buffer&,
        const Length line_index,
        const Length character_index,
        const Char* data,
        const Length data_length);
    extern bool insert_characters(Buffer&, const Length line_index, const Length character_index, const Char* data);
    extern bool delete_line(Buffer&, const Length line_index);
    extern bool delete_lines(Buffer&, const Length number_of_lines, const Length line_index);
    extern bool delete_character(Buffer&, const Length line_index, const Length character_index);
    extern bool delete_characters(Buffer&,
        const Length number_of_characters,
        const Length line_index,
        const Length character_index);
    extern bool merge_lines(Buffer&, const Length line_index);
    extern Length get_buffer_length(Buffer&);
    extern Length get_line_length(Buffer&, const Length line_index);
    // line_to_c_string
    // caller owns returned memory
    // may return nullptr
    extern const char* line_to_c_string(Buffer&, const Length line_index);
    // buffer_to_c_string
    // caller owns returned memory
    extern const char* buffer_to_c_string(Buffer&);
    extern bool line_empty(Buffer& buffer, const Length line_index);
}}
