#include "../inc/display.h"
#include "../inc/triangle.h"
#include <bits/stdint-uintn.h>

void swap(int* a, int* b)
{
    int tmp = *a;
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




