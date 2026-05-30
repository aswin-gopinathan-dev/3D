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
#include "../inc/array.h"

 

bool is_running;
float fov_factor = 640;
vec3_t camera_position = {0, 0, 0};
triangle_t* triangles_to_render = NULL;

// Initialize color buffer, color buffer texture and load obj file
void setup()
{
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t)*window_width*window_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
    //load_cube_mesh_data();
    load_obj_file_data("./assets/cube.obj");//f22.obj");
}
 

vec2_t project(vec3_t point)
{
    // Project a point in 3D to 2D by doing a perspective divide
    vec2_t projected_point = {point.x * fov_factor/point.z, 
                              point.y * fov_factor/point.z};
    return projected_point;
}

void update()
{
    // Increment rotation angle
    mesh.rotation.y += 0.001;
    mesh.rotation.z += 0.001;
    mesh.rotation.x += 0.001;

    triangles_to_render = NULL;

    // Get the list of faces. A face is a collection of 3 vertices that forms a triangle in 3D space
    int num_faces = array_length(mesh.faces);
    for (int i=0;i<num_faces;i++)
    {
        face_t face = mesh.faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[face.a-1];
        face_vertices[1] = mesh.vertices[face.b-1];
        face_vertices[2] = mesh.vertices[face.c-1];

        // For each vertex that forms the face, apply transformation - rotation, translate along z (to place it at a visible distance from camera). 
        // Project the transformed vertex into 2D.
        // Add the collection of projected triangles into an array
        vec3_t transformed_vertices[3];
        for(int j=0; j<3;j++)
        {
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            // Translate vertex away from the camera
            transformed_vertex.z += 5;
            transformed_vertices[j] = transformed_vertex;
        }

        // Check backface culling
        vec3_t vector_a = transformed_vertices[0];
        vec3_t vector_b = transformed_vertices[1];
        vec3_t vector_c = transformed_vertices[2];

        // Get the two vectors that forms the sides of the face
        vec3_t vector_ab = vec3_sub(vector_b, vector_a);
        vec3_t vector_ac = vec3_sub(vector_c, vector_a);
        
        // Compute face normal
        vec3_t normal = vec3_cross(vector_ab, vector_ac);

        // Normalize face normal vector
        vec3_normalize(&normal);
        
        // Find the camera ray 
        vec3_t camera_ray = vec3_sub(camera_position, vector_a);
        vec3_normalize(&camera_ray);

        // Calculate how aligned camera ray is with the face normal
        float dot_normal_camera = vec3_dot(camera_ray, normal);
        
        // Bypass triangles that are facing away from camera
        if(dot_normal_camera < 0)
            continue;
        
        triangle_t projected_triangle;
        // Loop 3 vertices to perform projection
        for(int j=0;j<3;j++)
        {
            vec2_t projected_vertex = project(transformed_vertices[j]);
            projected_vertex.x += window_width/2;
            projected_vertex.y += window_height/2;
            projected_triangle.points[j] = projected_vertex;
        }

        array_push(triangles_to_render, projected_triangle);
    }
}

void render()
{
    // Iterate the array of projected triangle and draw it on the color buffer
    int num_triangles = array_length(triangles_to_render);
    for(int i=0;i<num_triangles;i++)
    {
        triangle_t triangle = triangles_to_render[i];
        draw_triangle(triangle.points[0].x, triangle.points[0].y,
                      triangle.points[1].x, triangle.points[1].y,
                      triangle.points[2].x, triangle.points[2].y,
                      0xFF00FF00);
    }
    array_free(triangles_to_render);
    // Render the color buffer
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

void free_resources()
{
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
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
    free_resources();

    return 0;
}
