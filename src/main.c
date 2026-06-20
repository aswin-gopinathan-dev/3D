#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
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
#include "../inc/matrix.h"
#include "../inc/light.h"
#include "../inc/triangle.h"
#include "../inc/texture.h"
#include "../inc/upng.h"
 
 
bool is_running;
vec3_t camera_position = {0, 0, 0};
triangle_t* triangles_to_render = NULL;
mat4_t proj_matrix;

// Initialize color buffer, color buffer texture and load obj file
void setup()
{
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    color_buffer = (uint32_t*)malloc(sizeof(uint32_t)*window_width*window_height);
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    float fov = 60 * 3.14 / 180;
    float aspect = (float)window_height / (float)window_width;
    float znear = 0.1;
    float zfar = 100.0;
    proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

/*    
    mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
    texture_width = 64;
    texture_height = 64;
*/

    //load_cube_mesh_data();
    load_png_texture_data("./assets/crab.png");
    load_obj_file_data("./assets/crab.obj");
}
 

void update()
{
    // Increment rotation angle
    mesh.rotation.y += 0.008;
    //mesh.rotation.z += 0.001;
    //mesh.rotation.x += 0.001;
    //mesh.scale.x += 0.002;
    //mesh.translation.x += 0.01;
    mesh.translation.z = 5.0;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    triangles_to_render = NULL;

    // Get the list of faces. A face is a collection of 3 vertices that forms a triangle in 3D space
    int num_faces = array_length(mesh.faces);
    for (int i=0;i<num_faces;i++)
    {
        face_t face = mesh.faces[i];
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[face.a];
        face_vertices[1] = mesh.vertices[face.b];
        face_vertices[2] = mesh.vertices[face.c];

        // For each vertex that forms the face, apply transformation - rotation, translate along z (to place it at a visible distance from camera). 
        // Project the transformed vertex into 2D.
        // Add the collection of projected triangles into an array
        vec4_t transformed_vertices[3];
        for(int j=0; j<3;j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            mat4_t world_matrix = mat4_identity();
            world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
            world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
            world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
            transformed_vertices[j] = transformed_vertex;
        }

        // Check backface culling
        vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
        vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
        vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

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

        if(cull_method == CULL_BACKFACE)
        {
            // Bypass triangles that are facing away from camera
            if(dot_normal_camera < 0)
                continue;
        }

        vec4_t projected_points[3];
        // Loop 3 vertices to perform projection
        for(int j=0;j<3;j++)
        {
            projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

            // Scale into the view
            projected_points[j].x *= (window_width/2);
            projected_points[j].y *= (window_height/2);

            // Invert y values to account for flipped screen y coordinate. In obj files, y value 
            // increase upwards whereas on screen its downwards
            projected_points[j].y *= -1;

            // Translate the projected points to the middle of the screen
            projected_points[j].x += window_width/2;
            projected_points[j].y += window_height/2;

        }

        // Calculate average depth for each face based on the vertices after the transformation
        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z)/3;
        float light_intensity_factor = -vec3_dot(normal, light.direction);
        uint32_t triangle_color = light_apply_intensity(face.color, light_intensity_factor);
        
        triangle_t projected_triangle = { 
                                         .points = {{projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                                                    {projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                                                    {projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w }},
                                         .texcoords = {
                                             {face.a_uv.u, face.a_uv.v},
                                             {face.b_uv.u, face.b_uv.v},
                                             {face.c_uv.u, face.c_uv.v},
                                         },
                                         .color = triangle_color,
                                         .avg_depth = avg_depth
        };
        array_push(triangles_to_render, projected_triangle);
    }

    // Sort triangles to render by their average depth
    int num_triangles = array_length(triangles_to_render);
    for(int i=0;i<num_triangles;i++)
    {
        for(int j=i;j<num_triangles;j++)
        {
            if(triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth)
            {
                triangle_t temp = triangles_to_render[i];
                triangles_to_render[i] = triangles_to_render[j];
                triangles_to_render[j] = temp;
            }
        }
    }
}

void render()
{
    draw_grid();
    
    // Iterate the array of projected triangle and draw it on the color buffer
    int num_triangles = array_length(triangles_to_render);
    for(int i=0;i<num_triangles;i++)
    {
        triangle_t triangle = triangles_to_render[i];

        if(render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
        {
            draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
                      triangle.points[1].x, triangle.points[1].y,
                      triangle.points[2].x, triangle.points[2].y,
                      triangle.color);
        }

        if(render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX 
           || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE)
        {
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                    triangle.points[1].x, triangle.points[1].y,
                    triangle.points[2].x, triangle.points[2].y,
                    0xFFFFFFFF);
        }

        if(render_method == RENDER_WIRE_VERTEX)
        {
            draw_rect(triangle.points[0].x-3, triangle.points[0].y-3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[1].x-3, triangle.points[1].y-3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[2].x-3, triangle.points[2].y-3, 6, 6, 0xFFFF0000);
        }

        if(render_method == RENDER_TEXTURED_WIRE || render_method == RENDER_TEXTURED)
        {
            draw_textured_triangle(triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                                   triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
                                   triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
                                   mesh_texture);
        }
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
            if(event.key.keysym.sym == SDLK_1)
                render_method = RENDER_WIRE_VERTEX;
            if(event.key.keysym.sym == SDLK_1)
                render_method = RENDER_WIRE_VERTEX;
            if(event.key.keysym.sym == SDLK_2)
                render_method = RENDER_WIRE;
            if(event.key.keysym.sym == SDLK_3)
                render_method = RENDER_FILL_TRIANGLE;
            if(event.key.keysym.sym == SDLK_4)
                render_method = RENDER_FILL_TRIANGLE_WIRE;
       
            if(event.key.keysym.sym == SDLK_5) 
                render_method = RENDER_TEXTURED;
            if(event.key.keysym.sym == SDLK_6)
                render_method = RENDER_TEXTURED_WIRE;
            
            if(event.key.keysym.sym == SDLK_c)
                cull_method = CULL_BACKFACE;
            if(event.key.keysym.sym == SDLK_d)
                cull_method = CULL_NONE;
            break;
    }
}

void free_resources()
{
    free(color_buffer);
    upng_free(png_texture);
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
