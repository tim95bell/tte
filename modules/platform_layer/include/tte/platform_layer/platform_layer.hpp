
#pragma once

#include <tte/common/number_types.hpp>

namespace tte { namespace platform_layer {
    struct Window;
    struct Font;

    // TODO(TB): should WindowClose and WindowResized be a sub event of type Window?
    enum class EventType { Quit, KeyDown, KeyUp, WindowClose, WindowResized };

    enum class KeyCode {
        Unknown,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Backspace,
        Space,
        Return,
    };

    struct KeyEventData {
        U8 repeat;
        KeyCode keycode;
    };

    struct Event {
        EventType type;

        union {
            KeyEventData key;
        };
    };

    [[nodiscard]] extern bool init();
    extern void deinit();
    [[nodiscard]] extern Window* create_window(U32 width, U32 height);
    extern void destroy_window(Window& window);
    extern void fill_rect(Window& window, U32 x, U32 y, U32 width, U32 height, U8 r, U8 g, U8 b);
    extern void render_text(Window& window, Font& font, const char* text, S32 x, S32 y, U8 r, U8 g, U8 b);
    [[nodiscard]] extern bool poll_events(Event&);
    extern void clear_buffer(Window& window, U8 r, U8 g, U8 b, U8 a);
    extern void show_buffer(Window& window);
    [[nodiscard]] extern Font* open_font(const char* path, U32 size);
    extern void close_font(Font& font);
    [[nodiscard]] extern char get_key_code_character(KeyCode code);
    [[nodiscard]] extern U32 get_cursor_x(Font& font, const char* line, U64 cursorIndex);
    [[nodiscard]] extern Length get_fonts(platform_layer::Font** fonts, const U32 size);
}}
