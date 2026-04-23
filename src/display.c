#include "../inc/display.h"


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error initializing SDL.\n");
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w;
    window_height = display_mode.h;

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, 
                              SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_BORDERLESS);
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

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);     

    return true;
}

void clear_color_buffer(uint32_t color)
{
    for(int y =0;y<window_height;y++)
        for(int x=0;x<window_width;x++)
            color_buffer[window_width*y + x] = color;
}

void draw_grid(uint32_t color)
{
    for (int y=0;y<window_height;y++)
    {
        for (int x=0;x<window_width;x++)
        {
            if( y%75 == 0 || x %75==0)
                color_buffer[window_width*y + x] = color;
        }
    }
}


void render_color_buffer()
{
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, window_width*sizeof(uint32_t));
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderClear(renderer);

    render_color_buffer();

    clear_color_buffer(0xFF000000);
    draw_grid(0xFFFFFFFF);

    SDL_RenderPresent(renderer);
}


void destroy_window()
{
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


