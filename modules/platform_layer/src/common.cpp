
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/common/assert.hpp>

namespace tte { namespace platform_layer {
    char get_key_code_character(common::KeyCode code) {
        if (code >= common::KeyCode::A && code <= common::KeyCode::Z) {
            return static_cast<char>(static_cast<U8>('a') + (static_cast<S32>(code) - static_cast<S32>(common::KeyCode::A)));
        }

        TTE_ASSERT(false);
        return ' ';
    }

    static void create_quit_event(common::Event& e) { e.type = common::Event::Type::Quit; }

    static void create_key_down_event(common::Event& e, common::KeyCode keyCode, U8 repeat) {
        e.type = common::Event::Type::KeyDown;
        e.key.keycode = keyCode;
        e.key.repeat = repeat;
    }

    static void create_window_close_event(common::Event& e) { e.type = common::Event::Type::WindowClose; }

    static void create_window_resized_event(common::Event& e) { e.type = common::Event::Type::WindowResized; }
}}
