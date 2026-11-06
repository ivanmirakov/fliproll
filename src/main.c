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
static void build_circle_rects(int cx, int cy, int r, SDL_FRect *rows, int *len) {
    int n = 0;
    float rr = (r + 0.5f) * (r + 0.5f);

    for (int dy = -r; dy <= r && n < 512; dy++) {
        int dx = (int)sqrtf(rr - (float)(dy * dy));
        rows[n++] = (SDL_FRect){
            (float)(cx - dx), (float)(cy + dy),
            (float)(2 * dx + 1), 1.0f
        };
    }

    *len = n;
}

// IDEA: make a brush struct that holds the small bitmap instead of recalculating it every time
static void fill_circle(SDL_Renderer *ren, int cx, int cy, int r)
{
    SDL_FRect rows[512];
    int len;

    build_circle_rects(cx, cy, r, rows, &len);
    SDL_RenderFillRects(ren, rows, len);
}

// please bear with me. we have hardcoded brush shapes for sizes 1 to 3 and when we surpass 3 we use our handy formula.
static void build_brush_rects(int cx, int cy, int b, SDL_FRect *rows, int *n)
{
    switch (b) {
    case 1:
        rows[0] = (SDL_FRect){ cx, cy, 1, 1 };
        *n = 1; break;
    case 2:
        rows[0] = (SDL_FRect){ cx, cy,     2, 1 };
        rows[1] = (SDL_FRect){ cx, cy + 1, 2, 1 };
        *n = 2; break;
    case 3:
        rows[0] = (SDL_FRect){ cx,     cy - 1, 1, 1 };
        rows[1] = (SDL_FRect){ cx - 1, cy,     3, 1 };
        rows[2] = (SDL_FRect){ cx,     cy + 1, 1, 1 };
        *n = 3; break;
    default:
        build_circle_rects(cx, cy, b - 2, rows, n);
    }
}

static void fill_brush(SDL_Renderer *ren, int cx, int cy, int r) {
    build_brush_rects(cx, cy, r);
}

// OH MY FUCKING GODDD THAT WAS SO COMPLICATED HOLY SHIT HOLY FUCK. AUGHHH JESUS.
static void draw_brush_ring(SDL_Renderer *ren, int cx, int cy, int r, int t)
{
    SDL_FRect outer[512];
    int len_outer;
    build_brush_rects(cx, cy, r, outer, &len_outer);

    // thick enough to fill the whole disc: no inner circle needed! Such a simple life...
    if (t > r) {
        SDL_RenderFillRects(ren, outer, len_outer);
        return;
    }

    SDL_FRect inner[512];
    int len_inner;
    build_brush_rects(cx, cy, r - t, inner, &len_inner);

    SDL_FRect rows[512];
    int n = 0;

    for (int i = 0; i < len_outer; i++) {
        SDL_FRect o = outer[i];

        // top and bottom rows have no inner partner
        if (i < t || i >= len_outer - t) {
            rows[n++] = o;
            continue;
        }

        SDL_FRect in = inner[i - t];

        rows[n++] = (SDL_FRect){
            (float)(o.x), (float)(o.y),
            (float)(in.x - o.x), 1.0f
        };
        
        rows[n++] = (SDL_FRect){
            (float)(in.x + in.w), (float)(o.y),
            (float)((o.x + o.w) - (in.x + in.w)), 1.0f
        };
    }

    SDL_RenderFillRects(ren, rows, n);
}

static void debug_app_dump(const App *a) {
    SDL_RenderDebugTextFormat(a->renderer, 0, 0, "b=%.0f",
    a->brush_size);
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
    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
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
                app.brush_size += e.wheel.y * (1.0f + app.brush_size * 0.1f); // step scales with size
                app.brush_size = SDL_max(app.brush_size, 1.0f);
                break;
            }

        }

        // 2. Update

        if (app.mouse_down) {
            SDL_SetRenderTarget(app.renderer, app.canvas);
            SDL_SetRenderDrawColor(app.renderer, 30, 30, 40, 255);
            draw_brush(app.renderer, app.mouse_x, app.mouse_y, app.brush_size);
            SDL_SetRenderTarget(app.renderer, NULL);
            SDL_Log("AAA");
        }

        // 3. Draw

        // clear
        SDL_SetRenderDrawColor(app.renderer, 30, 30, 40, 100);
        SDL_RenderClear(app.renderer);
        SDL_RenderTexture(app.renderer, app.canvas, NULL, NULL); // draw canvas
        draw_brush_ring(app.renderer, app.mouse_x, app.mouse_y, app.brush_size, 1); // draw brush circle
        debug_app_dump(&app);
        SDL_RenderPresent(app.renderer);
    }

    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    return 0;
}