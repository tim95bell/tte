
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

    extern void draw(App*);
    extern void handle_event(App*, common::Event);

}}
