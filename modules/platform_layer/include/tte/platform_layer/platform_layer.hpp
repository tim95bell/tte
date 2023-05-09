
#pragma once

#include <tte/common/number_types.hpp>
#include <tte/common/event.hpp>

#define TTE_READ_BIG_ENDIAN_16(memory) (((memory)[0] << 8) | ((memory)[1] << 0))
#define TTE_READ_BIG_ENDIAN_32(memory) (((memory)[0] << 24) | ((memory)[1] << 16) | ((memory)[2] << 8) | ((memory)[3] << 0))
#define TTE_READ_BIG_ENDIAN_64(memory) (((memory)[0] << 56) | ((memory)[0] << 48) | ((memory)[0] << 40) | ((memory)[0] << 32) | ((memory)[0] << 24) | ((memory)[1] << 16) | ((memory)[2] << 8) | ((memory)[3] << 0))

namespace tte { namespace platform_layer {
    struct Window;
    struct Font;

    struct PlatformLayer {
        void (*handle_event)(common::Event);
    };

    struct FileIter {
        U8* begin;
        U8* at;
        Length size;
    };

    [[nodiscard]] extern U8* read_file(const char* file_name, Length* file_size);
    [[nodiscard]] extern bool has_enough_space_from_at(platform_layer::FileIter* file_iter, Length required_size);
    [[nodiscard]] extern bool has_enough_space_from_at(platform_layer::FileIter* file_iter, Length offset, Length required_size);
    [[nodiscard]] extern bool has_enough_space_from_begin(platform_layer::FileIter* file_iter, Length required_size);
    [[nodiscard]] extern bool has_enough_space_from_begin(platform_layer::FileIter* file_iter, Length offset, Length required_size);

    template <typename T>
    extern inline void read_big_endian(U8* const memory, T* result);

    template <typename T>
    [[nodiscard]] inline U8* move(U8* const memory) {
        return memory + sizeof(T);
    }

    template <typename T>
    [[nodiscard]] inline U8* read_big_endian_and_move(U8* const memory, T* result) {
        read_big_endian(memory, result);
        return move<T>(memory);
    }

    [[nodiscard]] inline bool has_enough_space(const U8* const memory, Length required_size, const U8* const end_bound);
    [[nodiscard]] inline bool has_enough_space(const U8* const memory, Length required_size, const U8* const start_bound, const U8* const end_bound);

    extern inline void read_big_endian_unchecked(FileIter* file_iter, U16* result);
    extern inline void read_big_endian_unchecked(FileIter* file_iter, U32* result);
    extern inline void read_big_endian_unchecked(FileIter* file_iter, S16* result);
    extern inline void read_big_endian_unchecked(FileIter* file_iter, S64* result);
    extern inline void read_big_endian_and_move_unchecked(FileIter* file_iter, U16* result);
    extern inline void read_big_endian_and_move_unchecked(FileIter* file_iter, U32* result);
    extern inline void read_big_endian_and_move_unchecked(FileIter* file_iter, S16* result);
    extern inline void read_big_endian_and_move_unchecked(FileIter* file_iter, S64* result);
    [[nodiscard]] extern inline bool read_big_endian(FileIter* file_iter, U16* result);
    [[nodiscard]] extern inline bool read_big_endian(FileIter* file_iter, U32* result);
    [[nodiscard]] extern inline bool read_big_endian(FileIter* file_iter, S16* result);
    [[nodiscard]] extern inline bool read_big_endian(FileIter* file_iter, S64* result);
    [[nodiscard]] extern inline bool read_big_endian_and_move(FileIter* file_iter, U16* result);
    [[nodiscard]] extern inline bool read_big_endian_and_move(FileIter* file_iter, U32* result);
    [[nodiscard]] extern inline bool read_big_endian_and_move(FileIter* file_iter, S16* result);
    [[nodiscard]] extern inline bool read_big_endian_and_move(FileIter* file_iter, S64* result);

    [[nodiscard]] extern bool init(PlatformLayer*);
    extern void deinit(PlatformLayer*);
    [[nodiscard]] extern Window* create_window(PlatformLayer*, U32 width, U32 height);
    extern void destroy_window(PlatformLayer*, Window& window);
    extern void fill_rect(PlatformLayer*, Window& window, U32 x, U32 y, U32 width, U32 height, U8 r, U8 g, U8 b);
    extern void render_text(PlatformLayer*, Window& window, Font& font, const char* text, S32 x, S32 y, U8 r, U8 g, U8 b);
    extern void clear_buffer(PlatformLayer*, Window& window, U8 r, U8 g, U8 b, U8 a);
    extern void show_buffer(PlatformLayer*, Window& window);
    [[nodiscard]] extern Font* open_font(PlatformLayer*, const char* path, U32 size);
    extern void close_font(PlatformLayer*, Font& font);
    [[nodiscard]] extern char get_key_code_character(common::KeyCode code);
    [[nodiscard]] extern U32 get_cursor_x(PlatformLayer*, Font& font, const char* line, U64 cursorIndex);
    [[nodiscard]] extern Length get_fonts(PlatformLayer*, platform_layer::Font** fonts, const U32 size);
    extern void run(PlatformLayer*);
    extern void sleep(U64 milliseconds);
}}
