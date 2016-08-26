#include <SDL2/SDL.h>

#include <iostream>
#include <complex>
#include <cmath>

namespace
{
    constexpr int WINDOW_WIDTH  = 640;
    constexpr int WINDOW_HEIGHT = 480;

    inline void set_pixel(SDL_Surface* sdl_surface, int x, int y, Uint8 r, Uint8 g, Uint8 b) noexcept
    {
        *(static_cast<Uint32*>(sdl_surface->pixels) + sdl_surface->w * y + x) =
            ::SDL_MapRGB(sdl_surface->format, r, g, b);
    }

    void render_fractal(SDL_Surface* sdl_surface) noexcept
    {
        static constexpr unsigned max_iterations = 500;

        for (int x = 0; x < WINDOW_WIDTH; ++x)
        {
            for (int y = 0; y < WINDOW_HEIGHT; ++y)
            {
                const auto nx = x * 3.0F / WINDOW_WIDTH  - 2.0F;
                const auto ny = y * 2.0F / WINDOW_HEIGHT - 1.0F;

                const std::complex<float> C{nx, ny};
                std::complex<float> Z{};

                unsigned iterations = 0;

                while (Z.real() * Z.real() + Z.imag() * Z.imag() <= 4.0F && iterations < max_iterations)
                {
                    Z = Z * Z + C;
                    ++iterations;
                }

                const auto normalized_iterations_count = static_cast<float>(iterations) / max_iterations;

                const auto sin_ = std::sin(normalized_iterations_count * 180.0F);
                const auto cos_ = std::cos(normalized_iterations_count * 180.0F);
                const auto sincos = sin_ * cos_;

                const auto r = sin_   < 0.0F ? normalized_iterations_count :   sin_;
                const auto g = cos_   < 0.0F ? normalized_iterations_count :   cos_;
                const auto b = sincos < 0.0F ? normalized_iterations_count : sincos;

                ::set_pixel(sdl_surface, x, y, r * 255, g * 255, b * 255);
            }
        }
    }
}

int main()
{
    if (::SDL_Init(SDL_INIT_VIDEO) >= 0)
    {
        SDL_Window* const sdl_window = ::SDL_CreateWindow("Fractal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (sdl_window)
        {
            SDL_Surface* const sdl_surface = ::SDL_GetWindowSurface(sdl_window);
            if (sdl_surface)
            {
                ::SDL_LockSurface(sdl_surface);
                ::render_fractal(sdl_surface);
                ::SDL_UnlockSurface(sdl_surface);

                SDL_Event sdl_event;

                bool running = true;
                while (running)
                {
                    while (::SDL_PollEvent(&sdl_event))
                    {
                        if (sdl_event.type == SDL_QUIT)
                            running = false;
                    }

                    ::SDL_UpdateWindowSurface(sdl_window);
                    ::SDL_Delay(50);
                }
            }
            else
                std::cerr << ::SDL_GetError() << std::endl;
        }
        else
            std::cerr << ::SDL_GetError() << std::endl;
    }
    else
        std::cerr << ::SDL_GetError() << std::endl;

    return 0;
}
