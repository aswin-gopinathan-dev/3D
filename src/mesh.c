#include "../inc/mesh.h"
#include "../inc/array.h"
#include <stdio.h>
#include <string.h>

mesh_t mesh = {NULL, NULL, {0,0,0}, {1.0,1.0,1.0}, {0,0,0}};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {-1, -1, -1},   // 1
    {-1,  1, -1},   // 2
    { 1,  1, -1},   // 3
    { 1, -1, -1},   // 4
    { 1,  1,  1},   // 5
    { 1, -1,  1},   // 6
    {-1,  1,  1},   // 7
    {-1, -1,  1},   // 8
};

#define  N_CUBE_FACES (6*2)
face_t cube_faces[N_CUBE_FACES] = {
    // front
    {1, 2, 3, 0xFFFFFFFF},
    {1, 3, 4, 0xFFFFFFFF},
    // right
    {4, 3, 5, 0xFFFFFFFF},
    {4, 5, 6, 0xFFFFFFFF},
    // back
    {6, 5, 7, 0xFFFFFFFF},
    {6, 7, 8, 0xFFFFFFFF},
    // left
    {8, 7, 2, 0xFFFFFFFF},
    {8, 2, 1, 0xFFFFFFFF},
    // top
    {2, 7, 5, 0xFFFFFFFF},
    {2, 5, 3, 0xFFFFFFFF},
    // bottom
    {6, 8, 1, 0xFFFFFFFF},
    {6, 1, 4, 0xFFFFFFFF}
};

void load_cube_mesh_data()
{
    for(int i=0;i<N_CUBE_VERTICES;i++)
    {
        array_push(mesh.vertices, cube_vertices[i]);
    }
    for(int i=0;i<N_CUBE_FACES;i++)
    {
        array_push(mesh.faces, cube_faces[i]);
    }
}

void load_obj_file_data(char* filename)
{
    FILE* file;
    file = fopen(filename, "r");

    char line[1024];
    while(fgets(line, 1024, file))
    {
        // Vertex information
        if(strncmp(line, "v ", 2) == 0)
        {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }

        // Face information
        if(strncmp(line, "f ", 2) == 0)
        {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
                    &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                    &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                    &vertex_indices[2], &texture_indices[2], &normal_indices[2]);
            face_t face = {vertex_indices[0], vertex_indices[1], vertex_indices[2], 0xFFFFFFFF};
            array_push(mesh.faces, face);

        }
    }
}
