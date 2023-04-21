
#include <tte/engine/engine.hpp>
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/common/assert.hpp>
#include <tte/common/event.hpp>
// TODO(TB): make this include library style
//#include <tte/app/app.hpp>
#include "../include/app/app.hpp"
#include <cstdlib>

namespace tte { namespace app {
    static App app;

    void handle_event(common::Event event) {
        handle_event(&app, event);
    }

    void run() {
        app.platform_layer.handle_event = &handle_event;
        if (!platform_layer::init(&app.platform_layer)) {
            return;
        }

        app.font_size = 16;
        app.buffer = &engine::create_buffer();

        if (!engine::insert_empty_line(*app.buffer, 0)) {
            engine::destroy_buffer(*app.buffer);
            platform_layer::deinit(&app.platform_layer);
            return;
        }

        app.window = platform_layer::create_window(&app.platform_layer, 640, 480);
        if (!app.window) {
            engine::destroy_buffer(*app.buffer);
            platform_layer::deinit(&app.platform_layer);
            return;
        }

        app.num_fonts = get_fonts(&app.platform_layer, &app.fonts, app.font_size);

    #if TTE_SDL
        if (app.num_fonts == 0) {
            TTE_DBG("Could not find any fonts");
            platform_layer::destroy_window(*app.window);
            engine::destroy_buffer(*app.buffer);
            platform_layer::deinit();
            return;
        }
    #endif

        app.font = app.fonts;

        draw(&app);

        app.running = true;
        while (app.running) {
            platform_layer::run(&app.platform_layer);
        }

        free(app.fonts);
        platform_layer::destroy_window(&app.platform_layer, *app.window);
        engine::destroy_buffer(*app.buffer);
        platform_layer::deinit(&app.platform_layer);
    }
}}

int main() {
    tte::app::run();
    return 0;
}
