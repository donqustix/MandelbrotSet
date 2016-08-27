#include <SDL2/SDL.h>

#include <iostream>
#include <complex>

namespace
{
    constexpr int WINDOW_WIDTH  = 640;
    constexpr int WINDOW_HEIGHT = 480;

    inline void set_pixel(SDL_Surface* surface, int x, int y, Uint8 r, Uint8 g, Uint8 b) noexcept
    {
        *(static_cast<Uint32*>(surface->pixels) + surface->w * y + x) = ::SDL_MapRGB(surface->format, r, g, b);
    }

    void render_fractal(SDL_Surface* surface, float x1, float y1, float x2, float y2,
            unsigned max_iterations = 100) noexcept
    {
        if (!max_iterations) return;

        static constexpr Uint8 palette[]
        {
             66,  30,  15,
             25,   7,  26,
              9,   1,  47,
              4,   4,  73,
              0,   7, 100,
             12,  44, 138,
             24,  82, 177,
             57, 125, 209,
            134, 181, 229,
            211, 236, 248,
            241, 233, 191,
            248, 201,  95,
            255, 170,   0,
            204, 128,   0,
            153,  87,   0,
            106,  52,   3
        };

        for (int y = 0; y < WINDOW_HEIGHT; ++y)
        {
            for (int x = 0; x < WINDOW_WIDTH; ++x)
            {
                const auto nx = x * (x2 - x1) / WINDOW_WIDTH  + x1;
                const auto ny = y * (y2 - y1) / WINDOW_HEIGHT + y1;

                const std::complex<float> C{nx, ny};
                std::complex<float> Z;

                unsigned iterations = 0;

                do
                {
                    Z = Z * Z + C;
                    ++iterations;
                }
                while (Z.real() * Z.real() + Z.imag() * Z.imag() <= 4.0F && iterations < max_iterations);

                if (iterations < max_iterations)
                {
                    const auto r = palette[3 * (iterations * 100 / max_iterations % 16)    ];
                    const auto g = palette[3 * (iterations * 100 / max_iterations % 16) + 1];
                    const auto b = palette[3 * (iterations * 100 / max_iterations % 16) + 2];

                    ::set_pixel(surface, x, WINDOW_HEIGHT - 1 - y, r, g, b);
                }
                else
                    ::set_pixel(surface, x, WINDOW_HEIGHT - 1 - y, 0, 0, 0);
            }
        }
    }
}

int main()
{
    if (::SDL_Init(SDL_INIT_VIDEO) >= 0)
    {
        SDL_Window* const window = ::SDL_CreateWindow("Fractal", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (window)
        {
            SDL_Surface* const surface = ::SDL_GetWindowSurface(window);
            if (surface)
            {
                SDL_Event sdl_event;

                ::SDL_LockSurface(surface);
                ::render_fractal(surface, -0.7463F - 0.005F, 0.1102F - 0.005F,
                                          -0.7463F + 0.005F, 0.1102F + 0.005F, 1000);
                ::SDL_UnlockSurface(surface);

                bool running = true;
                while (running)
                {
                    while (::SDL_PollEvent(&sdl_event))
                    {
                        if (sdl_event.type == SDL_QUIT)
                            running = false;
                    }

                    ::SDL_UpdateWindowSurface(window);
                    ::SDL_Delay(50);
                }
            }
            else
                std::cerr << "SDL_Surface getting error: " << ::SDL_GetError() << std::endl;
        }
        else
            std::cerr << "SDL_Window creation error: " << ::SDL_GetError() << std::endl;
    }
    else
        std::cerr << "SDL2 initialization error: " << ::SDL_GetError() << std::endl;

    return 0;
}
