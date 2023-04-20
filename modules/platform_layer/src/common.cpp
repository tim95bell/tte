
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/common/assert.hpp>

namespace tte { namespace platform_layer {
    char get_key_code_character(KeyCode code) {
        if (code >= KeyCode::A && code <= KeyCode::Z) {
            return static_cast<char>(static_cast<U8>('a') + (static_cast<S32>(code) - static_cast<S32>(KeyCode::A)));
        }

        TTE_ASSERT(false);
        return ' ';
    }

    static void create_quit_event(Event& e) { e.type = EventType::Quit; }

    static void create_key_down_event(Event& e, KeyCode keyCode, U8 repeat) {
        e.type = EventType::KeyDown;
        e.key.keycode = keyCode;
        e.key.repeat = repeat;
    }

    static void create_window_close_event(Event& e) { e.type = EventType::WindowClose; }

    static void create_window_resized_event(Event& e) { e.type = EventType::WindowResized; }
}}
