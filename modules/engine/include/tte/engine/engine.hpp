
#pragma once

#include <tte/common/number_types.hpp>

namespace tte { namespace engine {
    using Char = char;
    struct Buffer;

    [[nodiscard]] extern Buffer& create_buffer();
    extern void destroy_buffer(Buffer&);
    [[nodiscard]] extern bool insert_empty_line(Buffer&, const Length line_index);
    [[nodiscard]] extern bool insert_line(Buffer&, const Length line_index, const Char* data);
    [[nodiscard]] extern bool insert_line(Buffer&, const Length line_index, const Char* data, const Length data_length);
    [[nodiscard]] extern bool insert_empty_lines(Buffer&, const Length number_of_lines, const Length line_index);
    [[nodiscard]] extern bool insert_lines(Buffer&,
        const Length number_of_lines,
        const Length line_index,
        Char const* const* const data_array,
        const Length* data_length_array);
    [[nodiscard]] extern bool
    insert_lines(Buffer&, const Length number_of_lines, const Length line_index, Char const* const* const data_array);
    [[nodiscard]] extern bool
    insert_character(Buffer&, const Length line_index, const Length character_index, const Char character);
    [[nodiscard]] extern bool insert_characters(Buffer&,
        const Length line_index,
        const Length character_index,
        const Char* data,
        const Length data_length);
    [[nodiscard]] extern bool
    insert_characters(Buffer&, const Length line_index, const Length character_index, const Char* data);
    [[nodiscard]] extern bool delete_line(Buffer&, const Length line_index);
    [[nodiscard]] extern bool delete_lines(Buffer&, const Length number_of_lines, const Length line_index);
    [[nodiscard]] extern bool delete_character(Buffer&, const Length line_index, const Length character_index);
    [[nodiscard]] extern bool delete_characters(Buffer&,
        const Length number_of_characters,
        const Length line_index,
        const Length character_index);
    [[nodiscard]] extern bool merge_lines(Buffer&, const Length line_index);
    [[nodiscard]] extern Length get_buffer_length(Buffer&);
    [[nodiscard]] extern Length get_line_length(Buffer&, const Length line_index);
    // line_to_c_string
    // caller owns returned memory
    // may return nullptr
    [[nodiscard]] extern char* line_to_c_string(Buffer&, const Length line_index);
    // buffer_to_c_string
    // caller owns returned memory
    [[nodiscard]] extern char* buffer_to_c_string(Buffer&);
    [[nodiscard]] extern bool line_empty(Buffer& buffer, const Length line_index);
}}
