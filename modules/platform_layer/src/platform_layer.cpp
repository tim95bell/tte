
#include <tte/platform_layer/platform_layer.hpp>
#include <tte/common/assert.hpp>
#include <stdio.h>
#include <filesystem>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

namespace tte { namespace platform_layer {
    // #region internal
    static void create_quit_event(Event& e) { e.type = EventType::Quit; }

    static void create_key_down_event(Event& e, KeyCode keyCode, U8 repeat) {
        e.type = EventType::KeyDown;
        e.key.keycode = keyCode;
        e.key.repeat = repeat;
    }

    static KeyCode sdl_key_to_tte_key(SDL_Keycode code) {
        if (code >= SDLK_a && code <= SDLK_z) {
            return static_cast<KeyCode>(static_cast<S32>(KeyCode::A) + (code - SDLK_a));
        } else if (code == SDLK_BACKSPACE) {
            return KeyCode::Backspace;
        } else if (code == SDLK_SPACE) {
            return KeyCode::Space;
        } else if (code == SDLK_RETURN) {
            return KeyCode::Return;
        } else {
            return KeyCode::Unknown;
        }
    }
    // #endregion

    struct Window {
        SDL_Window* window;
        SDL_Renderer* renderer;
    };

    struct Font {
        TTF_Font* font;
    };

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            TTE_DBG("Failed to init SDL: %s", SDL_GetError());
            return false;
        }

        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
            TTE_DBG("Linear texture filtering not enabled");
        }

        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            TTE_DBG("Failed to init SDL_image: %s", IMG_GetError());
            SDL_Quit();
            return false;
        }

        if (TTF_Init() == -1) {
            TTE_DBG("Failed to init SDL_ttf: %s", TTF_GetError());
            IMG_Quit();
            SDL_Quit();
            return false;
        }

        return true;
    }

    void deinit() {
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    Window* create_window(U32 width, U32 height) {
        Window* result = static_cast<Window*>(malloc(sizeof(Window)));
        result->window = SDL_CreateWindow("TTE",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            static_cast<S32>(width),
            static_cast<S32>(height),
            SDL_WINDOW_SHOWN);
        if (!result->window) {
            TTE_DBG("Failed to create SDL window: %s", SDL_GetError());
            free(result);
            return nullptr;
        }

        result->renderer = SDL_CreateRenderer(result->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!result->renderer) {
            TTE_DBG("Failed to create SDL renderer: %s", SDL_GetError());
            SDL_DestroyWindow(result->window);
            free(result);
            return nullptr;
        }

        return result;
    }

    void destroy_window(Window& window) {
        SDL_DestroyRenderer(window.renderer);
        SDL_DestroyWindow(window.window);
        free(&window);
    }

    void fill_rect(Window& window, U32 x, U32 y, U32 width, U32 height, U8 r, U8 g, U8 b) {
        SDL_Rect rect{static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height)};
        SDL_SetRenderDrawColor(window.renderer, r, g, b, 0xFF);
        SDL_RenderFillRect(window.renderer, &rect);
    }

    bool poll_events(Event& e) {
        SDL_Event sdl_e;
        while (SDL_PollEvent(&sdl_e)) {
            if (sdl_e.type == SDL_QUIT) {
                create_quit_event(e);
                return true;
            } else if (sdl_e.type == SDL_KEYDOWN) {
                create_key_down_event(e, sdl_key_to_tte_key(sdl_e.key.keysym.sym), sdl_e.key.repeat);
                return true;
            }
        }
        return false;
    }

    void clear_buffer(Window& window, U8 r, U8 g, U8 b) {
        SDL_SetRenderDrawColor(window.renderer, r, g, b, 0xFF);
        SDL_RenderClear(window.renderer);
    }

    void show_buffer(Window& window) { SDL_RenderPresent(window.renderer); }

    void render_text(Window& window, Font& font, const char* text, S32 x, S32 y, U8 r, U8 g, U8 b) {
        TTE_ASSERT(text);
        SDL_Color colour{r, g, b, 0xFF};
        SDL_Surface* textSurface = TTF_RenderUTF8_Solid(font.font, text, colour);
        if (!textSurface) {
            TTE_DBG("Failed to render text surface: %s", TTF_GetError());
            return;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(window.renderer, textSurface);
        if (!texture) {
            TTE_DBG("Failed to create texture from text surface: %s", SDL_GetError());
            SDL_FreeSurface(textSurface);
            return;
        }

        SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
        SDL_RenderCopyEx(window.renderer, texture, nullptr, &renderQuad, 0, nullptr, SDL_FLIP_NONE);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(textSurface);
    }

    Font* open_font(const char* path, U32 size) {
        TTE_ASSERT(path);
        TTF_Font* font = TTF_OpenFont(path, static_cast<S32>(size));
        if (!font) {
            TTE_DBG("Failed to load font: %s", TTF_GetError());
            return nullptr;
        }

        Font* result = static_cast<Font*>(malloc(sizeof(Font)));
        result->font = font;
        return result;
    }

    void close_font(Font& font) {
        TTF_CloseFont(font.font);
        free(&font);
    }

    char get_key_code_character(KeyCode code) {
        if (code >= KeyCode::A && code <= KeyCode::Z) {
            return static_cast<char>(static_cast<U8>('a') + (static_cast<S32>(code) - static_cast<S32>(KeyCode::A)));
        }

        TTE_ASSERT(false);
        return ' ';
    }

    U32 get_cursor_x(Font& font, const char* line, Length cursorIndex) {
        TTE_ASSERT(line);
        S32 w, h;
        if (cursorIndex < strlen(line)) {
            char* substring = static_cast<char*>(malloc(sizeof(char) * cursorIndex + 1));
            memcpy(substring, line, cursorIndex);
            substring[cursorIndex] = '\0';
            const bool success = TTF_SizeUTF8(font.font, substring, &w, &h) == 0;
            free(substring);
            if (!success) {
                TTE_ASSERT(false);
                return 0;
            }
        }

        if (TTF_SizeUTF8(font.font, line, &w, &h) == -1) {
            TTE_ASSERT(false);
            return 0;
        }

        TTE_ASSERT(w >= 0);
        TTE_ASSERT(h >= 0);
        return static_cast<U32>(w);
    }

    Length get_fonts(platform_layer::Font** fonts, const U32 font_size) {
        const std::filesystem::path fonts_directory_path(
            std::filesystem::relative(std::filesystem::path("resources/fonts")));
        if (std::filesystem::directory_entry(fonts_directory_path).is_directory()) {
            Length size = 0;
            Length capacity = 5;
            *fonts = static_cast<Font*>(malloc(sizeof(Font) * capacity));
            for (const auto& x : std::filesystem::directory_iterator(fonts_directory_path)) {
                if (x.is_regular_file() && x.path().extension() == ".ttf") {
                    if (Font* font = open_font(std::filesystem::absolute(x.path()).c_str(), font_size)) {
                        if (size == capacity) {
                            const Length old_capacity = capacity;
                            capacity *= 2;
                            Font* new_fonts = static_cast<Font*>(malloc(sizeof(Font) * capacity));
                            memcpy(new_fonts, *fonts, old_capacity);
                            free(*fonts);
                            *fonts = new_fonts;
                        }

                        (*fonts)[size] = *font;
                        ++size;
                    }
                }
            }
            if (size == 0) {
                free(*fonts);
                *fonts = nullptr;
            }
            return size;
        }

        *fonts = nullptr;
        return 0;
    }
}}
