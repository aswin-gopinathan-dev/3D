#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

SDL_Window* window;
SDL_Renderer* renderer;
bool is_running;

bool initialize_window()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, 
                              SDL_WINDOWPOS_CENTERED, 800, 600, 
                              SDL_WINDOW_BORDERLESS);
    if(!window)
    {
        printf("Error creating SDL window\n");
        return false;
    }

    bool is_wsl = system("grep -qi microsoft /proc/version") == 0;
    Uint32 flags = is_wsl ? SDL_RENDERER_SOFTWARE : SDL_RENDERER_ACCELERATED;

    renderer = SDL_CreateRenderer(window, -1, flags);
    if(!renderer)
    {
        printf("Error creating SDL renderer");
        return false;
    }

    return true;
}

void setup()
{

}

void update()
{

}

void process_input()
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type)
    {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break;
    }
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
}


int main()
{
    is_running = initialize_window();

    setup();
    while(is_running)
    {
        process_input();
        update();
        render();
    }
    return 0;
}
