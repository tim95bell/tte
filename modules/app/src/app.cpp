
// TODO(TB): make this include library style
//#include <tte/app/app.hpp>
#include "../include/app/app.hpp"
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/common/number_types.hpp>
#include <tte/common/assert.hpp>
#include <tte/common/event.hpp>
#include <tte/engine/engine.hpp>
#include <cstdlib>

namespace tte { namespace app {
    void draw(App* app) {
        platform_layer::clear_buffer(&app->platform_layer, *app->window, 0xFF, 0xFF, 0xFF, 0xFF);
        Length buffer_length = engine::get_buffer_length(*app->buffer);

        if (app->cursor.line < buffer_length) {
            char* line = engine::line_to_c_string(*app->buffer, app->cursor.line);
            TTE_ASSERT(line);
            U32 x = platform_layer::get_cursor_x(&app->platform_layer, *app->font, line, app->cursor.character);
            free(static_cast<void*>(line));
            U32 y = static_cast<U32>(app->cursor.line * app->font_size);
            platform_layer::fill_rect(&app->platform_layer,
                *app->window,
                x,
                y,
                static_cast<U32>(app->font_size * 0.1f),
                app->font_size,
                0xFF,
                0,
                0);
        }

        for (Length i = 0; i < buffer_length; ++i) {
            if (!engine::line_empty(*app->buffer, i)) {
                char* line = engine::line_to_c_string(*app->buffer, i);
                TTE_ASSERT(line);
#if TTE_SDL
                platform_layer::render_text(*app->window,
                    *app->font,
                    line,
                    0,
                    static_cast<S32>(i * app->font_size),
                    0xFF,
                    0x39,
                    0xA1);
#endif
                free(static_cast<void*>(line));
            }
        }

        platform_layer::show_buffer(&app->platform_layer, *app->window);
    }

    void handle_event(App* app, common::Event e) {
        if (e.type == common::Event::Type::Quit) {
            app->running = false;
        } else if (e.type == common::Event::Type::WindowClose) {
            app->running = false;
        } else if (e.type == common::Event::Type::WindowResized) {
            draw(app);
        } else if (e.type == common::Event::Type::KeyDown) {
            if (e.key.keycode == common::KeyCode::Backspace) {
                if (app->cursor.character == 0) {
                    if (app->cursor.line != 0) {
                        const Length old_line_length =
                            engine::get_line_length(*app->buffer, app->cursor.line - 1);
                        if (engine::merge_lines(*app->buffer, app->cursor.line - 1)) {
                            app->cursor.line = app->cursor.line - 1;
                            app->cursor.character = old_line_length;
                            draw(app);
                        }
                    }
                } else {
                    if (engine::delete_character(*app->buffer, app->cursor.line, app->cursor.character - 1)) {
                        --app->cursor.character;
                        draw(app);
                    } else {
                        TTE_ASSERT(false);
                    }
                }
            } else if (e.key.keycode == common::KeyCode::Space) {
                if (engine::insert_character(*app->buffer, app->cursor.line, app->cursor.character, ' ')) {
                    ++app->cursor.character;
                    draw(app);
                } else {
                    TTE_ASSERT(false);
                }
            } else if (e.key.keycode == common::KeyCode::Return) {
                const bool result = engine::insert_empty_line(*app->buffer, app->cursor.line + 1);
                TTE_ASSERT(result);
                if (result) {
                    ++app->cursor.line;
                    app->cursor.character = 0;
                    draw(app);
                }
            } else if (e.key.keycode != common::KeyCode::Unknown) {
                if (engine::insert_character(*app->buffer,
                        app->cursor.line,
                        app->cursor.character,
                        platform_layer::get_key_code_character(e.key.keycode))) {
                    ++app->cursor.character;
                    draw(app);
                } else {
                    TTE_ASSERT(false);
                }
            }
        }
    }
}}
