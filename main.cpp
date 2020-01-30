// clang++ main.cpp -o tracer -lSDL2

#include "eng.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>

const int WINDOW_WIDTH  = 640;
const int WINDOW_HEIGHT = 480;
const char *title = "raytracer";

SDL_Rect screenRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
SDL_Rect windowRect = { 0, 0, WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2 };

SDL_Window   *window   = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Surface  *surface  = nullptr;
SDL_Texture  *texture  = nullptr;

bool running = true;


bool init()
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cout << "SDL initialization failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH * 2, WINDOW_HEIGHT * 2,
        0
    );

    if (window == nullptr)
    {
        std::cout << "Creation of window failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr)
    {
        std::cout << "Creation of renderer failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    surface = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT, 32, 0, 0, 0, 0);

    if (surface == nullptr)
    {
        std::cout << "Surface creation failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (texture == nullptr)
    {
        std::cout << "Texture creation failed! Error: " << SDL_GetError() << '\n';
        return false;
    }

    return true;
}


void close()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(window);
    SDL_FreeSurface(surface);

    SDL_Quit();
}


void draw()
{
    u8 *buffer = (u8*)(surface->pixels);

    for (int i = 0; i < surface->pitch * surface->h; i += 4)
    {
        buffer[i]     = rand() % 255;
        buffer[i + 1] = rand() % 255;
        buffer[i + 2] = rand() % 255;
        buffer[i + 3] = 255;
    }
}


void show()
{
    SDL_UpdateTexture(texture, &screenRect, surface->pixels, surface->pitch);
    SDL_RenderCopy(renderer, texture, &screenRect, &windowRect);
    SDL_RenderPresent(renderer);
}


int main(int argc, char *argv[])
{
    if (!init()) return 1;

    u32 frameCounter = 0;

    u64 lapsedTime = 0;
    auto time1 = std::chrono::high_resolution_clock::now();

    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0x77, 0x77, 0x77, 0xFF);
        SDL_RenderClear(renderer);

        SDL_LockSurface(surface);

        draw();

        SDL_UnlockSurface(surface);

        show();

        ++frameCounter;
        auto time2 = std::chrono::high_resolution_clock::now();
        lapsedTime += (time2 - time1).count();
        time1 = time2;

        if (lapsedTime > 1000000000)
        {
            std::cout << "frames: " << frameCounter << '\n';
            frameCounter = 0;
            lapsedTime %= 1000000000;
        }
    }

    close();

    return 0;
}

