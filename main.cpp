#include <iostream>
#include <complex>
#include <tuple>
#include <SDL2/SDL.h>

uint16_t MAX_ITER = 200;
double offsetX = 0.0f;
double offsetY = 0.0f;
double zoom = 0.004f;
//constexpr const std::complex<double> julia_c(-0.74543f, 0.11301f);
constexpr const std::complex<double> julia_c(-0.1f, 0.68f);
constexpr const int16_t WINDOW_X = 1920;
constexpr const int16_t WINDOW_Y = 1080;
constexpr const double ZOOM_MULTIPLIER = 1.9f;

inline uint32_t generate_set(std::complex<double>&&);
std::tuple<uint8_t, uint8_t, uint8_t> getColor(uint16_t);

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow("WERGUR", 0, 0, WINDOW_X, WINDOW_Y, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ren == nullptr) {
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    bool quit = false, refresh_picture = true;
    SDL_Event e;
    while(!quit) {
        while(SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    offsetY -= 70 * zoom;
                    refresh_picture = true;
                    break;
                case SDLK_DOWN:
                    offsetY += 70 * zoom;
                    refresh_picture = true;
                    break;
                case SDLK_LEFT:
                    offsetX -= 70 * zoom;
                    refresh_picture = true;
                    break;
                case SDLK_RIGHT:
                    offsetX += 70 * zoom;
                    refresh_picture = true;
                    break;
                case SDLK_n: // closer
                    zoom /= ZOOM_MULTIPLIER;
                    refresh_picture = true;
                    break;
                case SDLK_m: // farther
                    zoom *= ZOOM_MULTIPLIER;
                    refresh_picture = true;
                    break;
                case SDLK_k: // + max_iter
                    MAX_ITER += 5;
                    refresh_picture = true;
                    break;
                case SDLK_l: // - max_iter
                    MAX_ITER -= 5;
                    refresh_picture = true;
                    break;
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
            }
        }

        if(refresh_picture) {
            double real = -WINDOW_X / 2.0f * zoom + offsetX;
            const double imagstart = -WINDOW_Y / 2.0f * zoom + offsetY;
            for(uint16_t x = 0; x < WINDOW_X; ++x, real += zoom) {
                double imag = imagstart;
                for(uint16_t y = 0; y < WINDOW_Y; ++y, imag += zoom) {
                    auto[ r, g, b ] = getColor(generate_set(std::complex<double>(real, imag)));
                    SDL_SetRenderDrawColor(ren, r, g, b, 255);
                    SDL_RenderDrawPoint(ren, x, y);
                }
            }
            refresh_picture = false;
        }
        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

inline uint32_t generate_set(std::complex<double>&& point) {
    uint32_t n;
    std::complex<double> z = 0;
    for (n = 0; std::abs(z) <= 2 && n < MAX_ITER; ++n)
        z = z*z + point;
    return n;
}

std::tuple<uint8_t, uint8_t, uint8_t> getColor(uint16_t iterations) {
    switch(iterations * 7 / MAX_ITER) {
    case 0:
        return { 255, 0, 0 }; // red
    case 1:
        return { 255, 128, 0 }; // orange
    case 2:
        return { 255, 128, 0 }; // yellow
    case 3:
        return { 0, 255, 0 }; // green
    case 4:
        return { 0, 255, 128 }; // blue-greenish
    case 5:
        return { 0, 255, 255 }; // light blue
    case 6:
        return { 0, 0, 255 }; // dark blue
    case 7:
        return { 0, 0, 0 }; // black
    }
    return { 0, 0, 0 };
}
