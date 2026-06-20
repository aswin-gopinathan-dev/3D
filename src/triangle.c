#include "../inc/display.h"
#include "../inc/triangle.h"
#include <bits/stdint-uintn.h>

void swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void swap_f(float* a, float* b)
{
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int Mx, int My, uint32_t color)
{
    float inv_slope1 = (float)(x1 - x0)/(y1 - y0);
    float inv_slope2 = (float)(Mx - x0)/(My - y0);

    float x_start = x0;
    float x_end = x0;
    // Loop all scanlines from top to bottom
    for(int y=y0; y<=y1; y++)
    {
        draw_line((int)x_start, y, (int)x_end, y, color);
        x_start += inv_slope1;
        x_end += inv_slope2;
    }
}

void fill_flat_top_triangle(int x1, int y1, int Mx, int My, int x2, int y2, uint32_t color)
{
    float inv_slope1 = (float)(x2 - x1)/(y2 - y1);
    float inv_slope2 = (float)(x2 - Mx)/(y2 - My);

    float x_start = x2;
    float x_end = x2;
    // Loop all scanlines from top to bottom
    for(int y=y2; y>=y1; y--)
    {
        draw_line((int)x_start, y, (int)x_end, y, color);
        x_start -= inv_slope1;
        x_end -= inv_slope2;
    }
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    // Sort vertices by y coordinate
    if(y0 > y1)
    {
        swap(&y0, &y1);
        swap(&x0, &x1); 
    }
    if(y1 > y2)
    {
        swap(&y1, &y2);
        swap(&x1, &x2);
    }
    if(y0 > y1)
    {
        swap(&y0, &y1);
        swap(&x0, &x1);
    }

    if(y1 == y2)
    {
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    }
    else if(y0 == y1)
    {
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    }
    else 
    {
        int My = y1;
        int Mx =  ((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0;

        fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);
        fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);
    }
}


vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
    // Find vectors between the vertices ABC & point P 
    vec2_t ac = vec2_sub(c, a);
    vec2_t ab = vec2_sub(b, a);
    vec2_t ap = vec2_sub(p, a);
    vec2_t pc = vec2_sub(c, p);
    vec2_t pb = vec2_sub(b, p);

    // Compute area of triangle ABC, using cross product of the two sides of the triangle, 
    // which gives the area of the parallelogram that the triangles makes one half of
    float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x);

    // Alpha is the area of the small triangle  divided by the area of the bigger triangle
    float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;
    float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;
    float gamma = 1 - alpha - beta;

    vec3_t weights = { alpha, beta, gamma };
    return weights;
}


void draw_texel(int x, int y, uint32_t* texture, 
                vec4_t point_a, vec4_t point_b, vec4_t point_c, 
                float u0, float v0, float u1, float v1, float u2, float v2)
{
    vec2_t p = { x, y };
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);
    vec3_t weights = barycentric_weights(a, b, c, p);
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    interpolated_u = (u0 / point_a.w) * alpha + (u1 / point_b.w) * beta + (u2 / point_c.w) * gamma;
    interpolated_v = (v0 / point_a.w) * alpha + (v1 / point_b.w) * beta + (v2 / point_c.w) * gamma;

    interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
    int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;
    draw_pixel(x, y, texture[tex_y * texture_width + tex_x]);
}


void draw_textured_triangle(int x0, int y0, float z0, float w0, float u0, float v0, 
                            int x1, int y1, float z1, float w1, float u1, float v1, 
                            int x2, int y2, float z2, float w2, float u2, float v2,
                            uint32_t* texture)
{
    if(y0 > y1)
    {
        swap(&y0, &y1);
        swap(&x0, &x1); 
        swap_f(&z0, &z1);
        swap_f(&w0, &w1);
        swap_f(&u0, &u1);
        swap_f(&v0, &v1);
    }
    if(y1 > y2)
    {
        swap(&y1, &y2);
        swap(&x1, &x2);
        swap_f(&z1, &z2);
        swap_f(&w1, &w2);
        swap_f(&u1, &u2);
        swap_f(&v1, &v2);
    }
    if(y0 > y1)
    {
        swap(&y0, &y1);
        swap(&x0, &x1);
        swap_f(&z0, &z1);
        swap_f(&w0, &w1);
        swap_f(&u0, &u1);
        swap_f(&v0, &v1);
    }

    // Flip v component to account for inverted uv coordinates
    v0 = 1 - v0;
    v1 = 1 - v1;
    v2 = 1 - v2;

    vec4_t point_a = { x0, y0, z0, w0 };
    vec4_t point_b = { x1, y1, z1, w1 };
    vec4_t point_c = { x2, y2, z2, w2 };

    float inv_slope1 = 0;
    float inv_slope2 = 0;

    inv_slope1 = ((y1 - y0) != 0) ? (float)(x1 - x0) / abs(y1 - y0): 0;
    inv_slope2 = ((y2 - y0) != 0) ? (float)(x2 - x0) / abs(y2 - y0): 0;

    if(y1 - y0 != 0)
    {
        for(int y=y0; y<=y1; y++)
        {
            int x_start = x1 + (y - y1) * inv_slope1;
            int x_end = x0 + (y - y0) * inv_slope2;

            if(x_end < x_start) swap(&x_start, &x_end);

            for (int x = x_start; x < x_end; x++)
            {
                draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
            }
        }
    }


    inv_slope1 = ((y2 - y1) != 0) ? (float)(x2 - x1) / abs(y2 - y1): 0;
    inv_slope2 = ((y2 - y0) != 0) ? (float)(x2 - x0) / abs(y2 - y0): 0;

    if(y2 - y1 != 0)
    {
        for(int y=y1; y<=y2; y++)
        {
            int x_start = x1 + (y - y1) * inv_slope1;
            int x_end = x0 + (y - y0) * inv_slope2;

            if(x_end < x_start) swap(&x_start, &x_end);

            for (int x = x_start; x < x_end; x++)
            {
                draw_texel(x, y, texture, point_a, point_b, point_c, u0, v0, u1, v1, u2, v2);
            }
        }
    }
}

