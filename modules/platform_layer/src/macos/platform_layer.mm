
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/common/assert.hpp>
#include <AppKit/AppKit.h>
#include <cstdlib>
#include <algorithm>
#include <unistd.h>

#include "../platform_layer_common.cpp"
#include "../ttf_common.cpp"

#define TTE_BITMAP_BITS_PER_COMPONENT 8
#define TTE_BITMAP_BYTES_PER_PIXEL 4

@class TTEWindowDelegate;

namespace tte { namespace platform_layer {
    struct Window {
        PlatformLayer* platform_layer;
        NSWindow* ns_window;
        TTEWindowDelegate* ns_window_delegate;
        U8* buffer;
        Length buffer_width;
        Length buffer_height;
    };

    struct Font {};

    static void resize_buffer(Window*);
}}

@interface TTEWindowDelegate : NSObject<NSWindowDelegate>
{
    @public tte::platform_layer::Window* window;
}
@end

@implementation TTEWindowDelegate 
- (instancetype)initWithWindow:(tte::platform_layer::Window*)inWindow {
    if (self = [super init]) {
        self->window = inWindow;
    }
    return self;
}
- (void)windowWillClose:(id)sender {
    tte::common::Event e;
    tte::platform_layer::create_window_close_event(e);
    self->window->platform_layer->handle_event(e);
}
- (void)windowDidResize:(NSNotification*)notification {
    resize_buffer(self->window);
    tte::common::Event e;
    tte::platform_layer::create_window_resized_event(e);
    self->window->platform_layer->handle_event(e);
}
@end

@interface TTEView : NSView
{
    @public tte::platform_layer::Window* window;
}
@end

@implementation TTEView
- (instancetype)initWithFrame:(NSRect)frameRect window:(tte::platform_layer::Window*)inWindow {
    if (self = [super initWithFrame: frameRect]) {
        self->window = inWindow;
    }
    return self;
}
- (void)drawRect:(NSRect)dirtyRect {
    TTE_ASSERT(self->window->buffer_width == self->window->ns_window.contentView.bounds.size.width);
    TTE_ASSERT(self->window->buffer_height == self->window->ns_window.contentView.bounds.size.height);
    CGRect bounds = CGRectMake(0, 0, self->window->buffer_width, self->window->buffer_height);

    CGColorSpaceRef colour_space = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    CGContextRef bitmap_context = CGBitmapContextCreate(self->window->buffer,
        self->window->buffer_width,
        self->window->buffer_height,
        TTE_BITMAP_BITS_PER_COMPONENT,
        self->window->buffer_width * TTE_BITMAP_BYTES_PER_PIXEL,
        colour_space,
        kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colour_space);

    CGImageRef image = CGBitmapContextCreateImage(bitmap_context);
    CGContextRef graphics_context = [[NSGraphicsContext currentContext] CGContext];

    CGContextDrawImage(graphics_context, bounds, image);

    TTE_ASSERT(CGBitmapContextGetData(bitmap_context) == self->window->buffer);

    CGContextRelease(bitmap_context);
    CGImageRelease(image);
}
@end

namespace tte { namespace platform_layer {
    static void init_buffer(Window* window) {
        window->buffer_width = static_cast<U64>(window->ns_window.contentView.bounds.size.width);
        window->buffer_height = static_cast<U64>(window->ns_window.contentView.bounds.size.height);
        const Length new_buffer_num_bytes = window->buffer_width * window->buffer_height * TTE_BITMAP_BYTES_PER_PIXEL;
        window->buffer = static_cast<U8*>(malloc(new_buffer_num_bytes));
        // TOOD(TB): memset could be compiled away?
        memset(window->buffer, 0, new_buffer_num_bytes);
    }

    static void resize_buffer(Window* window) {
        const Length old_width = window->buffer_width;
        const Length old_height = window->buffer_height;
        U8* old_buffer = window->buffer;
        window->buffer_width = static_cast<U64>(window->ns_window.contentView.bounds.size.width);
        window->buffer_height = static_cast<U64>(window->ns_window.contentView.bounds.size.height);
        const Length new_buffer_num_bytes = window->buffer_width * window->buffer_height * TTE_BITMAP_BYTES_PER_PIXEL;
        window->buffer = static_cast<U8*>(malloc(new_buffer_num_bytes));
        // TOOD(TB): memset could be compiled away?
        memset(window->buffer, 0, new_buffer_num_bytes);
        const Length min_width = std::min(old_width, window->buffer_width);
        const Length min_height = std::min(old_height, window->buffer_height);

        // TODO(TB): this assumes dragging from bottom left corner
        for (Length y = 0; y < min_height; ++y) {
            memcpy(window->buffer + (y * window->buffer_width * TTE_BITMAP_BYTES_PER_PIXEL), old_buffer + (y * old_width * TTE_BITMAP_BYTES_PER_PIXEL), min_width * TTE_BITMAP_BYTES_PER_PIXEL);
        }

        free(old_buffer);
    }

    bool init(PlatformLayer*) {
        @autoreleasepool {
            // TODO(TB): should there be a top level auto release pool that is created here and released in 'deinit'?
            [NSApplication sharedApplication];
        }
        return true;
    }

    void deinit(PlatformLayer*) {}

    Window* create_window(PlatformLayer* platform_layer, U32 width, U32 height) {
        Window* result = static_cast<Window*>(malloc(sizeof(Window)));
        result->buffer = nullptr;
        result->platform_layer = platform_layer;
        @autoreleasepool {
            NSRect screen_rect = [[NSScreen mainScreen] frame];
            NSRect initial_frame = NSMakeRect((screen_rect.size.width - static_cast<double>(width)) * 0.5,
                (screen_rect.size.height - static_cast<double>(height)) * 0.5,
                static_cast<double>(width),
                static_cast<double>(height));
            
            result->ns_window = [[NSWindow alloc] initWithContentRect: initial_frame
                styleMask: NSWindowStyleMaskTitled |
                    NSWindowStyleMaskClosable |
                    NSWindowStyleMaskMiniaturizable |
                    NSWindowStyleMaskResizable 
                backing: NSBackingStoreBuffered
                defer: true];
            TTE_ASSERT(result->ns_window.contentView.bounds.size.width == initial_frame.size.width && result->ns_window.contentView.bounds.size.height == initial_frame.size.height);
            [result->ns_window setContentView: [[TTEView alloc] initWithFrame: initial_frame window: result]];
            result->ns_window_delegate = [[TTEWindowDelegate alloc] initWithWindow: result];

            [result->ns_window setDelegate: result->ns_window_delegate];
            [result->ns_window setTitle: @"TTE"];
            [result->ns_window makeKeyAndOrderFront: nil];
            [result->ns_window orderFrontRegardless];
            result->ns_window.releasedWhenClosed = false;

            init_buffer(result);
        }

        return result;
    }

    void destroy_window(PlatformLayer*, Window& window) {
        [window.ns_window_delegate release];
        [window.ns_window release];
        free(&window);
    }

    void fill_rect(PlatformLayer*, Window& window, U32 x, U32 y, U32 width, U32 height, U8 r, U8 g, U8 b) {
        if (x + width < 0 || x > window.buffer_width || y + height < 0 || y > window.buffer_height) {
            // whole rectangle off screen
            return;
        }

        // TODO(TB): make Window::buffer_width and Window::buffer_height U32?
        const Length until_y = std::min(y + height, static_cast<U32>(window.buffer_height));
        const Length until_x = std::min(x + width, static_cast<U32>(window.buffer_width));
        for (U32 y_index = std::max<U32>(y, 0); y_index < until_y; ++y_index) {
            for (U32 x_index = std::max<U32>(x, 0); x_index < until_x; ++x_index) {
                U8* component = window.buffer + (((y_index * window.buffer_width) + x_index) * TTE_BITMAP_BYTES_PER_PIXEL);
                *component++ = r;
                *component++ = g;
                *component++ = b;
                // TODO(TB): alpha
            }
        }
    }

    void render_text(PlatformLayer*, Window& window, Font& font, const char* text, S32 x, S32 y, U8 r, U8 g, U8 b) {
        // TODO(TB): missing implementation
    }

    void clear_buffer(PlatformLayer*, Window& window, U8 r, U8 g, U8 b, U8 a) {
        const U64 width = window.buffer_width;
        const U64 height = window.buffer_height;
        U8* component = window.buffer;
        for (Length y = 0; y < height; ++y) {
            for (Length x = 0; x < width; ++x) {
                *component++ = r;
                *component++ = g;
                *component++ = b;
                *component++ = a;
            }
        }
    }

    void show_buffer(PlatformLayer*, Window& window) {
        [window.ns_window.contentView setNeedsDisplay: true];
    }

    Font* open_font(PlatformLayer*, const char* path, U32 size) {
        // TODO(TB): missing implementation
        return nullptr;
    }

    void close_font(PlatformLayer*, Font& font) {
        // TODO(TB): missing implementation
    }

    U32 get_cursor_x(PlatformLayer*, Font& font, const char* line, U64 cursorIndex) {
        // TODO(TB): missing implementation
        return 0;
    }

    Length get_fonts(PlatformLayer*, platform_layer::Font** fonts, const U32 size) {
        // TODO(TB): missing implementation
        *fonts = nullptr;
        return 0;
    }

    void run(PlatformLayer*) {
        @autoreleasepool {
            if (NSEvent* ns_event = [NSApp nextEventMatchingMask: NSEventMaskAny
                untilDate: nil
                inMode: NSDefaultRunLoopMode
                dequeue: true]) {
                switch ([ns_event type]) {
                    default:
                        [NSApp sendEvent: ns_event];
                }

                // TODO(TB): call handle_event
            }
        }
    }

    void sleep(U64 milliseconds) {
        usleep(milliseconds * 1000);
    }
}}
