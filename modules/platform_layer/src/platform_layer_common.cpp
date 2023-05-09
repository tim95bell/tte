
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/common/assert.hpp>
#include <cstdlib>

namespace tte { namespace platform_layer {
    [[nodiscard]] U8* read_file(const char* file_name, Length* file_size) {
        if (FILE* file = fopen(file_name, "rb")) {
            fseek(file, 0, SEEK_END);
            *file_size = ftell(file);
            fseek(file, 0, SEEK_SET);

            U8* file_content = static_cast<U8*>(malloc(*file_size + 1));
            Length read_amount = fread(file_content, *file_size, 1, file);
            file_content[*file_size] = '\0';
            fclose(file);

            if (read_amount) {
                return file_content;
            }

            free(file_content);
        }
        return nullptr;
    }

    [[nodiscard]] bool has_enough_space_from_at(platform_layer::FileIter* file_iter, Length required_size) {
        const U8* const begin_bound = file_iter->begin;
        const U8* const end_bound = file_iter->begin + file_iter->size;
        TTE_ASSERT(begin_bound <= end_bound);
        TTE_ASSERT(file_iter->at >= file_iter->begin && file_iter->at <= file_iter->begin + file_iter->size);
        const U8* const begin = file_iter->at;
        const U8* const end = file_iter->at + required_size;
        return end >= begin && end <= end_bound;
    }

    [[nodiscard]] bool has_enough_space_from_at(platform_layer::FileIter* file_iter, Length offset, Length required_size) {
        const U8* const begin_bound = file_iter->begin;
        const U8* const end_bound = file_iter->begin + file_iter->size;
        TTE_ASSERT(begin_bound <= end_bound);
        TTE_ASSERT(file_iter->at >= file_iter->begin && file_iter->at <= file_iter->begin + file_iter->size);
        const U8* const begin = file_iter->at + offset;
        const U8* const end = begin + required_size;
        return end >= begin && begin >= begin_bound && end <= end_bound;
    }

    [[nodiscard]] bool has_enough_space_from_begin(platform_layer::FileIter* file_iter, Length required_size) {
        const U8* const begin_bound = file_iter->begin;
        const U8* const end_bound = file_iter->begin + file_iter->size;
        TTE_ASSERT(begin_bound <= end_bound);
        TTE_ASSERT(file_iter->at >= file_iter->begin && file_iter->at <= file_iter->begin + file_iter->size);
        const U8* const begin = file_iter->begin;
        const U8* const end = begin + required_size;
        return end >= begin && end <= end_bound;
    }

    [[nodiscard]] bool has_enough_space_from_begin(platform_layer::FileIter* file_iter, Length offset, Length required_size) {
        const U8* const begin_bound = file_iter->begin;
        const U8* const end_bound = file_iter->begin + file_iter->size;
        TTE_ASSERT(begin_bound <= end_bound);
        TTE_ASSERT(file_iter->at >= file_iter->begin && file_iter->at <= file_iter->begin + file_iter->size);
        const U8* const begin = file_iter->begin + offset;
        const U8* const end = begin + required_size;
        return end >= begin && begin >= begin_bound && end <= end_bound;
    }

    [[nodiscard]] inline bool has_enough_space(const U8* const memory, Length required_size, const U8* const end_bound) {
        TTE_ASSERT(memory + required_size > memory);
        return memory <= end_bound && memory + required_size <= end_bound;
    }

    [[nodiscard]] inline bool has_enough_space(const U8* const memory, Length required_size, const U8* const start_bound, const U8* const end_bound) {
        TTE_ASSERT(memory + required_size > memory);
        return memory >= start_bound && memory + required_size <= end_bound && start_bound <= end_bound;
    }

    template <>
    inline void read_big_endian(U8* const memory, U8* result) {
        *result = memory[0];
    }

    template <>
    inline void read_big_endian(U8* const memory, U16* result) {
        *result = TTE_READ_BIG_ENDIAN_16(memory);
    }

    template <>
    inline void read_big_endian(U8* const memory, U32* result) {
        *result = TTE_READ_BIG_ENDIAN_32(memory);
    }

    template <>
    inline void read_big_endian(U8* const memory, U64* result) {
        *result = TTE_READ_BIG_ENDIAN_64(memory);
    }

    template <>
    inline void read_big_endian(U8* const memory, S8* result) {
        *result = memory[0];
    }

    template <>
    inline void read_big_endian(U8* const memory, S16* result) {
        *result = TTE_READ_BIG_ENDIAN_16(memory);
    }

    template <>
    inline void read_big_endian(U8* const memory, S32* result) {
        *result = TTE_READ_BIG_ENDIAN_32(memory);
    }

    template <>
    inline void read_big_endian(U8* const memory, S64* result) {
        *result = TTE_READ_BIG_ENDIAN_64(memory);
    }

    inline void read_big_endian_unchecked(FileIter* file_iter, U8* result) {
        *result = file_iter->at[0];
    }

    inline void read_big_endian_unchecked(FileIter* file_iter, U16* result) {
        *result = TTE_READ_BIG_ENDIAN_16(file_iter->at);
    }

    inline void read_big_endian_unchecked(FileIter* file_iter, U32* result) {
        *result = TTE_READ_BIG_ENDIAN_32(file_iter->at);
    }

    inline void read_big_endian_unchecked(FileIter* file_iter, S16* result) {
        *result = TTE_READ_BIG_ENDIAN_16(file_iter->at);
    }

    inline void read_big_endian_unchecked(FileIter* file_iter, S64* result) {
        *result = TTE_READ_BIG_ENDIAN_64(file_iter->at);
    }

    inline void read_big_endian_and_move_unchecked(FileIter* file_iter, U8* result) {
        read_big_endian_unchecked(file_iter, result);
        file_iter->at += sizeof(U8);
    }

    inline void read_big_endian_and_move_unchecked(FileIter* file_iter, U16* result) {
        read_big_endian_unchecked(file_iter, result);
        file_iter->at += sizeof(U16);
    }

    inline void read_big_endian_and_move_unchecked(FileIter* file_iter, U32* result) {
        read_big_endian_unchecked(file_iter, result);
        file_iter->at += sizeof(U32);
    }

    inline void read_big_endian_and_move_unchecked(FileIter* file_iter, S16* result) {
        read_big_endian_unchecked(file_iter, result);
        file_iter->at += sizeof(S16);
    }

    inline void read_big_endian_and_move_unchecked(FileIter* file_iter, S64* result) {
        read_big_endian_unchecked(file_iter, result);
        file_iter->at += sizeof(S64);
    }

    [[nodiscard]] inline bool read_big_endian(FileIter* file_iter, U8* result) {
        if (file_iter->at + sizeof(U8) <= file_iter->begin + file_iter->size) {
            read_big_endian_unchecked(file_iter, result);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool read_big_endian(FileIter* file_iter, U16* result) {
        if (file_iter->at + sizeof(U16) <= file_iter->begin + file_iter->size) {
            read_big_endian_unchecked(file_iter, result);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool read_big_endian(FileIter* file_iter, U32* result) {
        if (file_iter->at + sizeof(U32) <= file_iter->begin + file_iter->size) {
            read_big_endian_unchecked(file_iter, result);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool read_big_endian(FileIter* file_iter, S16* result) {
        if (file_iter->at + sizeof(U32) <= file_iter->begin + file_iter->size) {
            read_big_endian_unchecked(file_iter, result);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool read_big_endian(FileIter* file_iter, S64* result) {
        if (file_iter->at + sizeof(U32) <= file_iter->begin + file_iter->size) {
            read_big_endian_unchecked(file_iter, result);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool read_big_endian_and_move(FileIter* file_iter, U8* result) {
        if (file_iter->at + sizeof(U8) <= file_iter->begin + file_iter->size) {
            read_big_endian_and_move_unchecked(file_iter, result);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool read_big_endian_and_move(FileIter* file_iter, U16* result) {
        if (file_iter->at + sizeof(U16) <= file_iter->begin + file_iter->size) {
            read_big_endian_and_move_unchecked(file_iter, result);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool read_big_endian_and_move(FileIter* file_iter, U32* result) {
        if (file_iter->at + sizeof(U32) <= file_iter->begin + file_iter->size) {
            read_big_endian_and_move_unchecked(file_iter, result);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool read_big_endian_and_move(FileIter* file_iter, S16* result) {
        if (file_iter->at + sizeof(U32) <= file_iter->begin + file_iter->size) {
            read_big_endian_and_move_unchecked(file_iter, result);
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool read_big_endian_and_move(FileIter* file_iter, S64* result) {
        if (file_iter->at + sizeof(U32) <= file_iter->begin + file_iter->size) {
            read_big_endian_and_move_unchecked(file_iter, result);
            return true;
        }
        return false;
    }

    [[nodiscard]] char get_key_code_character(common::KeyCode code) {
        if (code >= common::KeyCode::A && code <= common::KeyCode::Z) {
            return static_cast<char>(static_cast<U8>('a') + (static_cast<S32>(code) - static_cast<S32>(common::KeyCode::A)));
        }

        TTE_ASSERT(false);
        return ' ';
    }

    void create_quit_event(common::Event& e) { e.type = common::Event::Type::Quit; }

    void create_key_down_event(common::Event& e, common::KeyCode keyCode, U8 repeat) {
        e.type = common::Event::Type::KeyDown;
        e.key.keycode = keyCode;
        e.key.repeat = repeat;
    }

    void create_window_close_event(common::Event& e) { e.type = common::Event::Type::WindowClose; }

    void create_window_resized_event(common::Event& e) { e.type = common::Event::Type::WindowResized; }
}}
