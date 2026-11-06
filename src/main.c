#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>

// constants
#define WINDOW_W 120
#define WINDOW_H 120

// state
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *canvas;
    float mouse_x, mouse_y;
    float prev_mouse_x, prev_mouse_y; // we will need this for drawing
    float brush_size;
    bool mouse_down;
} App;

// helpers
static void draw_circle(SDL_Renderer* renderer, int32_t cx, int32_t cy, int32_t radius) { // i stole this from the internet
    const int32_t diameter = (radius * 2 + 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y) {
        SDL_RenderPoint(renderer, cx + x, cy - y);
        SDL_RenderPoint(renderer, cx + x, cy + y);
        SDL_RenderPoint(renderer, cx - x, cy - y);
        SDL_RenderPoint(renderer, cx - x, cy + y);
        SDL_RenderPoint(renderer, cx + y, cy - x);
        SDL_RenderPoint(renderer, cx + y, cy + x);
        SDL_RenderPoint(renderer, cx - y, cy - x);
        SDL_RenderPoint(renderer, cx - y, cy + x);

        if (error <= 0) {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0) {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}


// IDEA: make a brush struct that holds the small bitmap instead of recalculating it every time
void fill_circle(SDL_Renderer *ren, int cx, int cy, int r)
{
    SDL_FRect rows[512];
    int n = 0;
    float rr = (r + 0.5f) * (r + 0.5f);

    for (int dy = -r; dy <= r && n < 512; dy++) {
        int dx = (int)sqrtf(rr - (float)(dy * dy));
        rows[n++] = (SDL_FRect){
            (float)(cx - dx), (float)(cy + dy),
            (float)(2 * dx + 1), 1.0f
        };
    }
    SDL_RenderFillRects(ren, rows, n);
}

static void debug_app_dump(const App *a) {
    SDL_RenderDebugTextFormat(a->renderer, 0, 0,
        "mouse_x=%.1f mouse_y=%.1f\n"
        "prev_mouse_x=%.1f prev_mouse_y=%.1f\n"
        "mouse_down=%d\n",
        a->mouse_x, a->mouse_y,
        a->prev_mouse_x, a->prev_mouse_y,
        a->mouse_down
    );
}

int main(int argc, char **argv) {
    (void)argc; (void)argv; // to shut up the compiler

    App app = {0};
    app.brush_size = 2;

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    if (!SDL_CreateWindowAndRenderer("Fliproll",
        WINDOW_W, WINDOW_H,
        SDL_WINDOW_RESIZABLE,
        &app.window, &app.renderer)
    ) {
        SDL_Log("Window failed: %s", SDL_GetError());
        return 1;
    }

    SDL_SetRenderLogicalPresentation(app.renderer, WINDOW_W, WINDOW_H,
    SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_SetRenderVSync(app.renderer, 1);

    app.canvas = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888,
                                   SDL_TEXTUREACCESS_TARGET, WINDOW_W, WINDOW_H);
    if (!app.canvas) {
        SDL_Log("Canvas failed: %s", SDL_GetError());
        return 1;
    }

    SDL_SetTextureScaleMode(app.canvas, SDL_SCALEMODE_PIXELART);

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
            switch (e.type) {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            
            case SDL_EVENT_KEY_DOWN:
                if (e.key.key == SDLK_ESCAPE) running = false;
                break;

            case SDL_EVENT_MOUSE_MOTION:
                app.prev_mouse_x = app.mouse_x;
                app.prev_mouse_y = app.mouse_y;
                SDL_RenderCoordinatesFromWindow(app.renderer, e.motion.x, e.motion.y, &app.mouse_x, &app.mouse_y);
                break;
            
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                app.mouse_down = e.button.down;
                break;
            
            case SDL_EVENT_MOUSE_WHEEL:
                app.brush_size += e.wheel.y * (1.0f + app.brush_size * 0.15f); // step scales with size
                app.brush_size = SDL_max(app.brush_size, 1.0f);
                break;
            }

        }

        // 2. Update

        if (app.mouse_down) {
            SDL_SetRenderTarget(app.renderer, app.canvas);
            SDL_SetRenderDrawColor(app.renderer, 30, 30, 40, 255);
            fill_circle(app.renderer, app.mouse_x, app.mouse_y, app.brush_size);
            SDL_SetRenderTarget(app.renderer, NULL);
            SDL_Log("AAA");
        }

        // 3. Draw

        // clear
        SDL_SetRenderDrawColor(app.renderer, 30, 30, 40, 255);
        SDL_RenderClear(app.renderer);
        SDL_RenderTexture(app.renderer, app.canvas, NULL, NULL); // draw canvas
        draw_circle(app.renderer, app.mouse_x, app.mouse_y, app.brush_size); // draw brush circle
        debug_app_dump(&app);
        SDL_RenderPresent(app.renderer);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    return 0;
}