
#include <tte/engine/engine.hpp>
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/common/assert.hpp>
#include <tte/common/event.hpp>
#include <tte/app/app.hpp>
#include <cstdlib>
#include <dlfcn.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctime>
#include <thread>

#if !TTE_HOT_RELOAD
#include "app.cpp"
#endif

namespace tte { namespace app {
#if TTE_HOT_RELOAD
    struct AppLib {
        void* dll;
        init_function* init;
        deinit_function* deinit;
        draw_function* draw;
        handle_event_function* handle_event;
        run_function* run;
        time_t last_modification_time;
    };

    static AppLib app_lib;
    static const constexpr char* APP_LIB_DLL_PATH = "build/tte/modules/app/Debug/libtte.dylib.copy";

    INIT_FUNCTION(init_stub) { return true; }
    DEINIT_FUNCTION(deinit_stub) {}
    DRAW_FUNCTION(draw_stub) {}
    HANDLE_EVENT_FUNCTION(handle_event_stub) {}
    RUN_FUNCTION(run_stub) {}

    [[nodiscard]] bool get_dll_last_modification_time(const char* dll_path, time_t* result) {
        struct stat data;
        if (stat(dll_path, &data) == 0) {
            *result = data.st_mtime;
            return true;
        }

        *result = 0;
        return false;
    }

    [[nodiscard]] static bool load_app_lib_dll(time_t last_modification_time) {
        if (last_modification_time <= app_lib.last_modification_time) {
            return false;
        }

        if (app_lib.dll) {
            if (dlclose(app_lib.dll) != 0) {
                TTE_DBG("Failed to close app lib dll");
                return false;
            }
        }

        app_lib.dll = nullptr;
        app_lib.init = init_stub;
        app_lib.deinit = deinit_stub;
        app_lib.draw = draw_stub;
        app_lib.handle_event = handle_event_stub;
        app_lib.run = run_stub;

        if (app_lib.dll = dlopen(APP_LIB_DLL_PATH, RTLD_NOW)) {
            init_function* init = (init_function*)dlsym(app_lib.dll, "init");
            deinit_function* deinit = (deinit_function*)dlsym(app_lib.dll, "deinit");
            draw_function* draw = (draw_function*)dlsym(app_lib.dll, "draw");
            handle_event_function* handle_event = (handle_event_function*)dlsym(app_lib.dll, "handle_event");
            run_function* run = (run_function*)dlsym(app_lib.dll, "run");
            if (init && deinit && draw && handle_event && run) {
                app_lib.init = init;
                app_lib.deinit = deinit;
                app_lib.draw = draw;
                app_lib.handle_event = handle_event;
                app_lib.run = run;
                app_lib.last_modification_time = last_modification_time;
                return true;
            }

            if (dlclose(app_lib.dll) != 0) {
                TTE_DBG("Failed to close app lib dll");
            }
            app_lib.dll = nullptr;
        }

        return false;
    }

    [[nodiscard]] static bool load_app_lib_dll() {
        time_t time;
        if (get_dll_last_modification_time(APP_LIB_DLL_PATH, &time)) {
            return load_app_lib_dll(time);
        }
    }
#endif

    static App app;

    void handle_event(common::Event event) {
#if TTE_HOT_RELOAD
        TTE_ASSERT(app_lib.dll && app_lib.handle_event);
        app_lib.handle_event(&app, event);
#else
        handle_event(&app, event);
#endif
    }
}}

int main() {
#if TTE_HOT_RELOAD
    if (!tte::app::load_app_lib_dll()) {
        TTE_DBG("Failed to load app lib dll");
        return 0;
    }
#endif

    tte::app::app.platform_layer.handle_event = &tte::app::handle_event;

#if TTE_HOT_RELOAD
    tte::app::app_lib.init(&tte::app::app);
#else
    tte::app::init(&tte::app::app);
#endif

    // TODO(TB): initial draw before event loop begin?
#if TTE_HOT_RELOAD
    tte::app::app_lib.draw(&tte::app::app);
#else
    tte::app::draw(&tte::app::app);
#endif

    tte::app::app.running = true;
    while (tte::app::app.running) {
#if TTE_HOT_RELOAD
        tte::app::app_lib.run(&tte::app::app);
        if (tte::app::load_app_lib_dll()) {
            tte::common::Event event;
            event.type = tte::common::Event::Type::DidHotReload;
            tte::app::app_lib.handle_event(&tte::app::app, event);
        }
#else
        tte::app::run(&tte::app::app);
#endif
        // TODO(TB): sleep?
    }

#if TTE_HOT_RELOAD
    tte::app::app_lib.deinit(&tte::app::app);
#else
    tte::app::deinit(&tte::app::app);
#endif

    return 0;
}
