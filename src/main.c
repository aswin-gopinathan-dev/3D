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
#include "../inc/mesh.h"

 

bool is_running;
float fov_factor = 640;
vec3_t camera_position = {0, 0, -5};
vec3_t cube_rotation = {0, 0, 0};
triangle_t triangles_to_render[N_MESH_FACES];

void setup()
{
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t)*window_width*window_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
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

    for (int i=0;i<N_MESH_FACES;i++)
    {
        face_t face = mesh_faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh_vertices[face.a-1];
        face_vertices[1] = mesh_vertices[face.b-1];
        face_vertices[2] = mesh_vertices[face.c-1];

        triangle_t projected_triangle;
        for(int j=0; j<3;j++)
        {
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);
            transformed_vertex.z -= camera_position.z;
            vec2_t projected_vertex = project(transformed_vertex);

            projected_vertex.x += window_width/2;
            projected_vertex.y += window_height/2;
            projected_triangle.points[j] = projected_vertex;
        }

        triangles_to_render[i] = projected_triangle;
    }
}

void render()
{
    for(int i=0;i<N_MESH_FACES;i++)
    {
        triangle_t triangle = triangles_to_render[i];
        draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFF4444FF);
        draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFF4444FF);
        draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFF4444FF);
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
