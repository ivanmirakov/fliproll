#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

// constants
#define WINDOW_W 720
#define WINDOW_H 720

// state
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *canvas;
    float mouse_x, mouse_y;
    float prev_mouse_x, prev_mouse_y; // we will need this for drawing
    bool mouse_down;
} App;

// helpers
static void draw_circle(App *app) {
    // TODO: implement
}

int main(int argc, char **argv) {
    (void)argc; (void)argv; // to shut up the compiler

    App app = {0};

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    if (!SDL_CreateWindowAndRenderer("Fliproll", WINDOW_W, WINDOW_H, 0, &app.window, &app.renderer)) {
        SDL_Log("Window failed: %s", SDL_GetError());
        return 1;
    }
    SDL_SetRenderVSync(app.renderer, 1);

    app.canvas = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_TARGET, WINDOW_W, WINDOW_H);
    
    if (!app.canvas) {
        SDL_Log("Canvas failed: %s", SDL_GetError());
        return 1;
    }

    // clear the canvas once so it starts blank
    SDL_SetRenderTarget(app.renderer, app.canvas);
    SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);
    SDL_RenderClear(app.renderer);
    SDL_SetRenderTarget(app.renderer, NULL);

    bool running = true;
    while (running) {
        // 1. Input
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE) running = false;
            if (e.type == SDL_EVENT_MOUSE_MOTION) {
                app.prev_mouse_x = app.mouse_x;
                app.prev_mouse_y = app.mouse_y;
                app.mouse_x = e.motion.x;
                app.mouse_y = e.motion.y;
            }
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN || e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                app.mouse_down = e.button.down;
            }
        }

        // 2. Update

        // ...

        // 3. Draw
        SDL_SetRenderDrawColor(app.renderer, 30, 30, 40, 255);
        SDL_RenderClear(app.renderer);
        SDL_RenderTexture(app.renderer, app.canvas, NULL, NULL);
        SDL_RenderPresent(app.renderer);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    return 0;
}