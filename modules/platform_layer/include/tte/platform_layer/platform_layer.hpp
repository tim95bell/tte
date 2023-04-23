
#pragma once

#include <tte/common/number_types.hpp>
#include <tte/common/event.hpp>

namespace tte { namespace platform_layer {
    struct Window;
    struct Font;

    struct PlatformLayer {
        void (*handle_event)(common::Event);
    };


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
