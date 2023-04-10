
#include <tte/engine/engine.hpp>
#include <tte/common/assert.hpp>
#include <cassert>
#include <cstring>
#include <algorithm>

namespace tte { namespace engine {
    // #region internal
    struct Line {
        Char* data;
        Length length;
        Line* next;
    };

    struct Buffer {
        Line* first_line;
    };

    static inline void insert_empty_line_internal(Line** line) {
        TTE_ASSERT(line)
        // next could be nullptr
        Line* const next = *line;
        *line = static_cast<Line*>(malloc(sizeof(Line)));
        memset(*line, 0, sizeof(Line));
        (*line)->next = next;
        TTE_ASSERT(*line);
    }

    static inline void insert_line_internal(Line** line, const Char* data, const Length data_length) {
        TTE_ASSERT(line)
        // when data is nullptr, data_length must be 0. data_length could be 0 when data is not nullptr.
        TTE_ASSERT(data != nullptr || data_length == 0);
        insert_empty_line_internal(line);
        TTE_ASSERT(*line);
        (*line)->data = static_cast<Char*>(malloc(sizeof(const Char) * data_length));
        memcpy((*line)->data, static_cast<const void*>(data), data_length);
        (*line)->length = data_length;
    }

    [[nodiscard]] static inline Line** get_line_internal(Buffer& buffer, const Length line_index) {
        Length i = 0;
        Line** line = &buffer.first_line;
        while (line) {
            if (i == line_index) {
                return line;
            } else if (*line) {
                line = &(*line)->next;
                ++i;
            } else {
                break;
            }
        }

        return nullptr;
    }

    [[maybe_unused, nodiscard]] static inline Line** get_closest_line_internal(Buffer& buffer,
        const Length line_index) {
        Line** line = &buffer.first_line;
        for (Length i = 0; i < line_index && line && *line; ++i) {
            line = &(*line)->next;
        }
        return line;
    }

    [[nodiscard]] static bool
    insert_characters(Line& line, const Length character_index, const Char* data, const Length data_length) {
        if (character_index <= line.length) {
            Char* new_data = static_cast<Char*>(malloc(sizeof(Char) * (line.length + data_length)));
            memcpy(new_data, line.data, character_index);
            memcpy(new_data + character_index, data, data_length);
            memcpy(new_data + character_index + data_length,
                line.data + character_index,
                line.length - character_index);
            free(line.data);
            line.data = new_data;
            line.length = line.length + data_length;
            return true;
        }
        return data_length == 0;
    }

    static void destroy_line(Line& line) {
        free(line.data);
        free(&line);
    }

    static void destroy_lines(Line* line) {
        Line* next;
        while (line) {
            next = line->next;
            destroy_line(*line);
            line = next;
        }
    }

    static void delete_line(Line** line) {
        TTE_ASSERT(line);
        TTE_ASSERT(*line);
        Line& old_line = **line;
        *line = (*line)->next;
        destroy_line(old_line);
    }

    // #endregion

    Buffer& create_buffer() {
        Buffer* buffer = static_cast<Buffer*>(malloc(sizeof(Buffer)));
        memset(buffer, 0, sizeof(Buffer));
        return *buffer;
    }

    void destroy_buffer(Buffer& buffer) {
        destroy_lines(buffer.first_line);
        free(&buffer);
    }

    bool insert_empty_line(Buffer& buffer, const Length line_index) {
        if (Line** line = get_line_internal(buffer, line_index)) {
            insert_empty_line_internal(line);
            return true;
        }
        return false;
    }

    bool insert_line(Buffer& buffer, const Length line_index, const Char* data) {
        return insert_line(buffer, line_index, data, strlen(data));
    }

    bool insert_line(Buffer& buffer, const Length line_index, const Char* data, const Length data_length) {
        if (Line** line = get_line_internal(buffer, line_index)) {
            insert_line_internal(line, data, data_length);
            return true;
        }

        return false;
    }

    bool insert_empty_lines(Buffer& buffer, const Length number_of_lines, const Length line_index) {
        if (Line** line = get_line_internal(buffer, line_index)) {
            for (Length i = 0; i < number_of_lines; ++i) {
                insert_empty_line_internal(line);
                TTE_ASSERT(line);
            }
            return true;
        }

        return number_of_lines == 0;
    }

    bool insert_lines(Buffer& buffer,
        const Length number_of_lines,
        const Length line_index,
        Char const* const* const data_array) {
        Length* line_lengths = static_cast<Length*>(malloc(sizeof(Length) * number_of_lines));
        for (Length i = 0; i < number_of_lines; ++i) {
            line_lengths[i] = strlen(data_array[i]);
        }
        const bool result = insert_lines(buffer, number_of_lines, line_index, data_array, line_lengths);
        free(static_cast<void*>(line_lengths));
        return result;
    }

    bool insert_lines(Buffer& buffer,
        const Length number_of_lines,
        const Length line_index,
        Char const* const* const data_array,
        const Length* data_length_array) {
        if (Line** line = get_line_internal(buffer, line_index)) {
            for (Length i = 0; i < number_of_lines; ++i) {
                insert_line_internal(line, data_array[i], data_length_array[i]);
                line = &(*line)->next;
            }
            return true;
        }

        return number_of_lines == 0;
    }

    bool insert_character(Buffer& buffer, const Length line_index, const Length character_index, const Char character) {
        if (Line** line = get_line_internal(buffer, line_index); line && *line) {
            if (character_index <= (*line)->length) {
                Char* new_data = static_cast<Char*>(malloc(sizeof(Char) * ((*line)->length + 1)));
                memcpy(new_data, (*line)->data, character_index);
                new_data[character_index] = character;
                memcpy(new_data + character_index + 1,
                    (*line)->data + character_index,
                    (*line)->length - character_index);
                free((*line)->data);
                (*line)->data = new_data;
                (*line)->length = (*line)->length + 1;
                return true;
            }
        }
        return false;
    }

    bool insert_characters(Buffer& buffer, const Length line_index, const Length character_index, const Char* data) {
        return insert_characters(buffer, line_index, character_index, data, strlen(data));
    }

    bool insert_characters(Buffer& buffer,
        const Length line_index,
        const Length character_index,
        const Char* data,
        const Length data_length) {
        if (data_length == 0) {
            return true;
        }

        if (Line** line = get_line_internal(buffer, line_index); line && *line) {
            return insert_characters(**line, character_index, data, data_length);
        }
        return false;
    }

    bool delete_line(Buffer& buffer, const Length line_index) {
        if (Line** line = get_line_internal(buffer, line_index); line && *line) {
            delete_line(line);
            return true;
        }
        return false;
    }

    bool delete_lines(Buffer& buffer, const Length number_of_lines, const Length line_index) {
        if (Line** line = get_line_internal(buffer, line_index); line && *line) {
            for (Length i = 0; i < number_of_lines; ++i) {
                if (*line) {
                    Line& old_line = **line;
                    *line = (*line)->next;
                    destroy_line(old_line);
                } else {
                    break;
                }
            }
        } else {
            return number_of_lines == 0;
        }
        return true;
    }

    bool delete_character(Buffer& buffer, const Length line_index, const Length character_index) {
        if (Line** line = get_line_internal(buffer, line_index); line && *line) {
            if (character_index < (*line)->length) {
                Char* oldData = (*line)->data;
                (*line)->data = static_cast<Char*>(malloc(sizeof(Char) * ((*line)->length - 1)));
                memcpy((*line)->data, oldData, character_index);
                memcpy((*line)->data + character_index,
                    oldData + character_index + 1,
                    (*line)->length - character_index - 1);
                free(oldData);
                (*line)->length = (*line)->length - 1;
                return true;
            }
        }
        return false;
    }

    bool delete_characters(Buffer& buffer,
        const Length number_of_characters,
        const Length line_index,
        const Length character_index) {
        if (Line** line = get_line_internal(buffer, line_index); line && *line) {
            if (character_index < (*line)->length) {
                const Length actual_number_of_characters =
                    std::min((*line)->length - character_index, number_of_characters);
                Char* oldData = (*line)->data;
                (*line)->data =
                    static_cast<Char*>(malloc(sizeof(Char) * ((*line)->length - actual_number_of_characters)));
                memcpy((*line)->data, oldData, character_index);
                memcpy((*line)->data + character_index,
                    oldData + character_index + actual_number_of_characters,
                    (*line)->length - character_index - actual_number_of_characters);
                free(oldData);
                (*line)->length = (*line)->length - actual_number_of_characters;
                return true;
            }
        }
        return number_of_characters == 0;
    }

    bool merge_lines(Buffer& buffer, const Length line_index) {
        if (Line** line = get_line_internal(buffer, line_index); line && *line && (*line)->next) {
            if (insert_characters(**line, (*line)->length, (*line)->next->data, (*line)->next->length)) {
                delete_line(&(*line)->next);
                return true;
            }
        }
        return false;
    }

    Length get_buffer_length(Buffer& buffer) {
        Length length = 0;
        for (Line* line = buffer.first_line; line; line = line->next) {
            ++length;
        }
        return length;
    }

    Length get_line_length(Buffer& buffer, const Length line_index) {
        if (Line** line = get_line_internal(buffer, line_index); line && *line) {
            return (*line)->length;
        }
        return 0;
    }

    char* line_to_c_string(Buffer& buffer, const Length line_index) {
        if (Line** line = get_line_internal(buffer, line_index); line && *line) {
            char* result = static_cast<char*>(malloc(sizeof(char) * ((*line)->length + 1)));
            memcpy(result, (*line)->data, (*line)->length);
            result[(*line)->length] = '\0';
            return result;
        }

        return nullptr;
    }

    char* buffer_to_c_string(Buffer& buffer) {
        Length length = 0;
        for (Line* line = buffer.first_line; line; line = line->next) {
            length += line->length + 1;
        }

        char* result = static_cast<char*>(malloc(sizeof(char) * (length + 1)));
        result[length] = '\0';
        Length index = 0;
        for (Line* line = buffer.first_line; line; line = line->next) {
            memcpy(result + index, line->data, line->length);
            result[index + line->length] = '\n';
            index += line->length + 1;
        }
        return result;
    }

    bool line_empty(Buffer& buffer, const Length line_index) {
        if (Line** line = get_line_internal(buffer, line_index); line && *line) {
            return (*line)->length == 0;
        }
        return true;
    }
}}
