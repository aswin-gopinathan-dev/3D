#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <bits/stdint-uintn.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "../inc/display.h"
#include "../inc/vector.h"

#define N_POINTS 9*9*9
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];
bool is_running;
float fov_factor = 640;
vec3_t camera_position = {0, 0, -5};
vec3_t cube_rotation = {0, 0, 0};

void setup()
{
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t)*window_width*window_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    int point_count = 0;
    for(float x = -1; x <= 1; x+= 0.25)
    {
        for(float y = -1; y <= 1; y+= 0.25)
        {
            for(float z = -1; z<= 1; z+= 0.25)
            {
                vec3_t new_point = {.x = x, .y = y, .z = z};
                cube_points[point_count++] = new_point;
            }
        }
    }
}

vec2_t project(vec3_t point)
{
    vec2_t projected_point = {point.x * fov_factor/point.z, 
                              point.y * fov_factor/point.z};
    return projected_point;
}

void update()
{
    cube_rotation.y += 0.001;
    cube_rotation.z += 0.001;
    cube_rotation.x += 0.001;
    for(int i=0;i<N_POINTS;i++)
    {
        vec3_t point = cube_points[i];

        vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.x);
        transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
        transformed_point = vec3_rotate_z(transformed_point, cube_rotation.z);
        transformed_point.z -= camera_position.z;
        vec2_t projected_point = project(transformed_point);
        projected_points[i] = projected_point;
    }
}

void render()
{
    for(int i=0;i<N_POINTS;i++)
    {
        vec2_t projected_point = projected_points[i];
        draw_rect(
                projected_point.x + window_width/2, 
                projected_point.y + window_height/2, 
                4, 4, 0xFF4444FF);
    }
    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
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

    destroy_window();
    return 0;
}
