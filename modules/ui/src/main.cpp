
#include <tte/engine/engine.hpp>
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/common/assert.hpp>
#include <cstdlib>

namespace tte { namespace ui {
    struct Cursor {
        Length line;
        Length character;
    };

    struct App {
        U32 font_size;
        platform_layer::Window* window;
        engine::Buffer* buffer;
        Cursor cursor;
        Length num_fonts;
        platform_layer::Font* fonts;
        platform_layer::Font* font;
    };

    void draw(App* app) {
        platform_layer::clear_buffer(*app->window, 0xFF, 0xFF, 0xFF, 0xFF);
        Length buffer_length = engine::get_buffer_length(*app->buffer);

        if (app->cursor.line < buffer_length) {
            char* line = engine::line_to_c_string(*app->buffer, app->cursor.line);
            TTE_ASSERT(line);
            U32 x = platform_layer::get_cursor_x(*app->font, line, app->cursor.character);
            free(static_cast<void*>(line));
            U32 y = static_cast<U32>(app->cursor.line * app->font_size);
            platform_layer::fill_rect(*app->window,
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

        platform_layer::show_buffer(*app->window);
    }

    void run() {
        App app;
        app.font_size = 16;
        app.buffer = &engine::create_buffer();

        if (!engine::insert_empty_line(*app.buffer, 0)) {
            engine::destroy_buffer(*app.buffer);
            return;
        }

        if (!platform_layer::init()) {
            engine::destroy_buffer(*app.buffer);
            return;
        }

        app.window = platform_layer::create_window(640, 480);
        if (!app.window) {
            platform_layer::deinit();
            engine::destroy_buffer(*app.buffer);
            return;
        }

        app.num_fonts = get_fonts(&app.fonts, app.font_size);

#if TTE_SDL
        if (app.num_fonts == 0) {
            TTE_DBG("Could not find any fonts");
            platform_layer::destroy_window(*app.window);
            platform_layer::deinit();
            engine::destroy_buffer(*app.buffer);
            return;
        }
#endif

        app.font = app.fonts;

        draw(&app);

        bool running = true;
        while (running) {
            platform_layer::Event e;
            while (platform_layer::poll_events(e)) {
                if (e.type == platform_layer::EventType::Quit) {
                    running = false;
                    break;
                } else if (e.type == platform_layer::EventType::WindowClose) {
                    running = false;
                    break;
                } else if (e.type == platform_layer::EventType::WindowResized) {
                    draw(&app);
                    break;
                } else if (e.type == platform_layer::EventType::KeyDown) {
                    if (e.key.keycode == platform_layer::KeyCode::Backspace) {
                        if (app.cursor.character == 0) {
                            if (app.cursor.line != 0) {
                                const Length old_line_length =
                                    engine::get_line_length(*app.buffer, app.cursor.line - 1);
                                if (engine::merge_lines(*app.buffer, app.cursor.line - 1)) {
                                    app.cursor.line = app.cursor.line - 1;
                                    app.cursor.character = old_line_length;
                                    draw(&app);
                                }
                            }
                        } else {
                            if (engine::delete_character(*app.buffer, app.cursor.line, app.cursor.character - 1)) {
                                --app.cursor.character;
                                draw(&app);
                            } else {
                                TTE_ASSERT(false);
                            }
                        }
                    } else if (e.key.keycode == platform_layer::KeyCode::Space) {
                        if (engine::insert_character(*app.buffer, app.cursor.line, app.cursor.character, ' ')) {
                            ++app.cursor.character;
                            draw(&app);
                        } else {
                            TTE_ASSERT(false);
                        }
                    } else if (e.key.keycode == platform_layer::KeyCode::Return) {
                        const bool result = engine::insert_empty_line(*app.buffer, app.cursor.line + 1);
                        TTE_ASSERT(result);
                        if (result) {
                            ++app.cursor.line;
                            app.cursor.character = 0;
                            draw(&app);
                        }
                    } else if (e.key.keycode != platform_layer::KeyCode::Unknown) {
                        if (engine::insert_character(*app.buffer,
                                app.cursor.line,
                                app.cursor.character,
                                platform_layer::get_key_code_character(e.key.keycode))) {
                            ++app.cursor.character;
                            draw(&app);
                        } else {
                            TTE_ASSERT(false);
                        }
                    }
                }
            }
        }

        free(app.fonts);
        platform_layer::destroy_window(*app.window);
        platform_layer::deinit();
        engine::destroy_buffer(*app.buffer);
    }
}}

int main() {
tte::ui::run();
return 0;
}
