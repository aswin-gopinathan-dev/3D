#include "../inc/light.h"
#include <bits/stdint-uintn.h>

light_t light = {
    .direction = {0, 0, 1}
};

uint32_t light_apply_intensity(uint32_t color, float factor)
{
    if(factor < 0)
        factor = 0;
    if(factor > 1)
        factor = 1;

    uint32_t A = (color & 0xFF000000);
    uint32_t R = (color & 0x00FF0000) * factor;
    uint32_t G = (color & 0x0000FF00) * factor;
    uint32_t B = (color & 0x000000FF) * factor;

    uint32_t result_color = A | (R & 0x00FF0000) | (G & 0x0000FF00) | (B & 0x000000FF);

    return result_color;
}
