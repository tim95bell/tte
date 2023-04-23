
#pragma once

#include <tte/common/number_types.hpp>
#include <tte/common/event.hpp>
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/engine/engine.hpp>

namespace tte { namespace app {
    struct Cursor {
        Length line;
        Length character;
    };

    struct App {
        bool running;
        U32 font_size;
        platform_layer::Window* window;
        engine::Buffer* buffer;
        Cursor cursor;
        Length num_fonts;
        platform_layer::Font* fonts;
        platform_layer::Font* font;
        platform_layer::PlatformLayer platform_layer;
    };

#define INIT_FUNCTION(name) bool name(App* app)
#define DEINIT_FUNCTION(name) void name(App* app)
#define DRAW_FUNCTION(name) void name(App* app)
#define HANDLE_EVENT_FUNCTION(name) void name(App* app, common::Event event)
#define RUN_FUNCTION(name) void name(App* app)

#if TTE_HOT_RELOAD
    extern "C" {
        typedef INIT_FUNCTION(init_function);
        extern init_function init;

        typedef DEINIT_FUNCTION(deinit_function);
        extern deinit_function deinit;

        typedef DRAW_FUNCTION(draw_function);
        extern draw_function draw;

        typedef HANDLE_EVENT_FUNCTION(handle_event_function);
        extern handle_event_function handle_event;

        typedef RUN_FUNCTION(run_function);
        extern run_function run;
    }
#endif
}}
