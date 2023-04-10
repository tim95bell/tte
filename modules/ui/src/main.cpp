
#include <tte/engine/engine.hpp>
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/common/assert.hpp>
#include <iostream>

namespace tte { namespace ui {
    struct Cursor {
        Length line;
        Length character;
    };

    void run() {
        engine::Buffer& buffer = engine::create_buffer();
        Cursor cursor{};
        engine::insert_empty_line(buffer, 0);

        platform_layer::Window* window = platform_layer::create_window(640, 480);
        if (!window) {
            engine::destroy_buffer(buffer);
            return;
        }

        if (!platform_layer::init()) {
            platform_layer::destroy_window(*window);
            engine::destroy_buffer(buffer);
            return;
        }

        const U32 font_size = 16;
        platform_layer::Font* fonts;
        const Length num_fonts = get_fonts(&fonts, font_size);

        if (num_fonts == 0) {
            TTE_DBG("Could not find any fonts");
            platform_layer::deinit();
            platform_layer::destroy_window(*window);
            engine::destroy_buffer(buffer);
            return;
        }

        platform_layer::Font* font = fonts;

        bool running = true;
        while (running) {
            {
                platform_layer::clear_buffer(*window, 0xFF, 0xFF, 0xFF);
                Length buffer_length = engine::get_buffer_length(buffer);

                if (cursor.line < buffer_length) {
                    char* line = engine::line_to_c_string(buffer, cursor.line);
                    TTE_ASSERT(line);
                    U32 x = platform_layer::get_cursor_x(*font, line, cursor.character);
                    free(static_cast<void*>(line));
                    U32 y = static_cast<U32>(cursor.line * font_size);
                    platform_layer::fill_rect(*window, x, y, static_cast<U32>(font_size * 0.1f), font_size, 0xFF, 0, 0);
                }

                for (Length i = 0; i < buffer_length; ++i) {
                    if (!engine::line_empty(buffer, i)) {
                        char* line = engine::line_to_c_string(buffer, i);
                        TTE_ASSERT(line);
                        platform_layer::render_text(*window,
                            *font,
                            line,
                            0,
                            static_cast<S32>(i * font_size),
                            0xFF,
                            0x39,
                            0xA1);
                        free(static_cast<void*>(line));
                    }
                }

                platform_layer::show_buffer(*window);
            }

            platform_layer::Event e;
            while (platform_layer::poll_events(e)) {
                if (e.type == platform_layer::EventType::Quit) {
                    running = false;
                    break;
                } else if (e.type == platform_layer::EventType::KeyDown) {
                    if (e.key.keycode == platform_layer::KeyCode::Backspace) {
                        if (cursor.character == 0) {
                            if (cursor.line != 0) {
                                const Length old_line_length = engine::get_line_length(buffer, cursor.line - 1);
                                if (engine::merge_lines(buffer, cursor.line - 1)) {
                                    cursor.line = cursor.line - 1;
                                    cursor.character = old_line_length;
                                }
                            }
                        } else {
                            engine::delete_character(buffer, cursor.line, cursor.character - 1);
                            --cursor.character;
                        }
                    } else if (e.key.keycode == platform_layer::KeyCode::Space) {
                        engine::insert_character(buffer, cursor.line, cursor.character, ' ');
                        ++cursor.character;
                    } else if (e.key.keycode == platform_layer::KeyCode::Return) {
                        const bool result = engine::insert_empty_line(buffer, cursor.line + 1);
                        TTE_ASSERT(result);
                        if (result) {
                            ++cursor.line;
                            cursor.character = 0;
                        }
                    } else if (e.key.keycode != platform_layer::KeyCode::Unknown) {
                        engine::insert_character(buffer,
                            cursor.line,
                            cursor.character,
                            platform_layer::get_key_code_character(e.key.keycode));
                        ++cursor.character;
                    }
                }
            }
        }

        free(fonts);
        platform_layer::deinit();
        platform_layer::destroy_window(*window);
        engine::destroy_buffer(buffer);
    }
}}

int main() {
tte::ui::run();
return 0;
}
