#include "../inc/display.h"


#define MAX(a, b) (((a) > (b)) ? (a) : (b))

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL;
int window_width = 800;
int window_height = 600;
enum cull_method cull_method = CULL_BACKFACE;
enum render_method render_method = RENDER_WIRE;

bool initialize_window()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error initializing SDL.\n");
        return false;
    }

    //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
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

void draw_grid() {
    for (int y = 0; y < window_height; y += 10) {
        for (int x = 0; x < window_width; x += 10) {
            color_buffer[(window_width * y) + x] = 0xFF444444;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color)
{
    if(x >= 0 && x < window_width && y >= 0 && y < window_height)
        color_buffer[window_width * y + x] = color;
}

void draw_rect(int x, int y, int width, int height, uint32_t color)
{
    for (int i=0;i<width;i++)
    {
        for (int j=0;j<height;j++)
        {
            int cur_x = x + i;
            int cur_y = y + j;
            draw_pixel(cur_x, cur_y, color); 
        }
    }
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);

    int side_length = MAX(abs(delta_x), abs(delta_y));
    float x_inc = (float)delta_x/side_length;
    float y_inc = (float)delta_y/side_length;

    float cur_x = x0;
    float cur_y = y0;
    for(int i=0; i<=side_length;i++)
    {
        draw_pixel(round(cur_x), round(cur_y), color);
        cur_x += x_inc;
        cur_y += y_inc;
    }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}


void render_color_buffer()
{
    SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, window_width*sizeof(uint32_t));
    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void destroy_window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


