
#include <tte/app/app.hpp>
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/platform_layer/ttf.hpp>
#include <tte/common/number_types.hpp>
#include <tte/common/assert.hpp>
#include <tte/common/event.hpp>
#include <tte/engine/engine.hpp>
#include <cstdlib>
// TODO(TB): remove this include
#include <filesystem>

namespace tte { namespace app {
    INIT_FUNCTION(init) {
        const std::filesystem::path fonts_directory_path(
            std::filesystem::relative(std::filesystem::path("resources/fonts")));
        if (std::filesystem::directory_entry(fonts_directory_path).is_directory()) {
            for (const auto& x : std::filesystem::directory_iterator(fonts_directory_path)) {
                if (x.is_regular_file() && x.path().extension() == ".ttf") {
                    printf(x.path().c_str());
                    ttf::parse_file(std::filesystem::absolute(x.path()).c_str());
                }
            }
        }

        if (!platform_layer::init(&app->platform_layer)) {
            return false;
        }

        app->font_size = 16;
        app->buffer = &engine::create_buffer();

        if (!engine::insert_empty_line(*app->buffer, 0)) {
            engine::destroy_buffer(*app->buffer);
            platform_layer::deinit(&app->platform_layer);
            return false;
        }

        app->window = platform_layer::create_window(&app->platform_layer, 640, 480);
        if (!app->window) {
            engine::destroy_buffer(*app->buffer);
            platform_layer::deinit(&app->platform_layer);
            return false;
        }

        app->num_fonts = get_fonts(&app->platform_layer, &app->fonts, app->font_size);

    #if TTE_SDL
        if (app->num_fonts == 0) {
            TTE_DBG("Could not find any fonts");
            platform_layer::destroy_window(*app->window);
            engine::destroy_buffer(*app->buffer);
            platform_layer::deinit();
            return false;
        }
    #endif

        app->font = app->fonts;
        return true;
    }

    DEINIT_FUNCTION(deinit) {
        free(app->fonts);
        platform_layer::destroy_window(&app->platform_layer, *app->window);
        engine::destroy_buffer(*app->buffer);
        platform_layer::deinit(&app->platform_layer);
    }

    DRAW_FUNCTION(draw) {
        platform_layer::clear_buffer(&app->platform_layer, *app->window, 0xFF, 0x00, 0xFF, 0xFF);
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

    HANDLE_EVENT_FUNCTION(handle_event) {
        if (event.type == common::Event::Type::Quit) {
            app->running = false;
        }
#if TTE_HOT_RELOAD
        else if (event.type == common::Event::Type::DidHotReload) {
            draw(app);
        }
#endif
        else if (event.type == common::Event::Type::WindowClose) {
            app->running = false;
        } else if (event.type == common::Event::Type::WindowResized) {
            draw(app);
        } else if (event.type == common::Event::Type::KeyDown) {
            if (event.key.keycode == common::KeyCode::Backspace) {
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
            } else if (event.key.keycode == common::KeyCode::Space) {
                if (engine::insert_character(*app->buffer, app->cursor.line, app->cursor.character, ' ')) {
                    ++app->cursor.character;
                    draw(app);
                } else {
                    TTE_ASSERT(false);
                }
            } else if (event.key.keycode == common::KeyCode::Return) {
                const bool result = engine::insert_empty_line(*app->buffer, app->cursor.line + 1);
                TTE_ASSERT(result);
                if (result) {
                    ++app->cursor.line;
                    app->cursor.character = 0;
                    draw(app);
                }
            } else if (event.key.keycode != common::KeyCode::Unknown) {
                if (engine::insert_character(*app->buffer,
                        app->cursor.line,
                        app->cursor.character,
                        platform_layer::get_key_code_character(event.key.keycode))) {
                    ++app->cursor.character;
                    draw(app);
                } else {
                    TTE_ASSERT(false);
                }
            }
        }
    }

    RUN_FUNCTION(run) {
        platform_layer::run(&app->platform_layer);
    }
}}
