#include "../inc/vector.h"
#include <math.h>


vec3_t vec3_new(float x, float y, float z)
{
    vec3_t result = { x, y, z };
    return result;
}

vec3_t vec3_clone(vec3_t* v)
{
    vec3_t result = { v->x, v->y, v->z };
    return result;
}

vec3_t vec3_rotate_x(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
        v.x,
        v.y * cos(angle) - v.z * sin(angle),
        v.y * sin(angle) + v.z * cos(angle)
    };

    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
        v.x * cos(angle) - v.z * sin(angle),
        v.y,
        v.x * sin(angle) + v.z * cos(angle)
    };

    return rotated_vector;
}

vec3_t vec3_rotate_z(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
        v.x * cos(angle) - v.y * sin(angle),
        v.x * sin(angle) + v.y * cos(angle),
        v.z
    };

    return rotated_vector;
}


vec2_t vec2_new(float x, float y)
{
    vec2_t result = { x, y };
    return result;
}

float vec2_length(vec2_t v)
{
    return sqrt(v.x*v.x + v.y*v.y);
}

float vec3_length(vec3_t v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

vec2_t vec2_add(vec2_t a, vec2_t b)
{
    vec2_t result = { a.x + b.x, a.y + b.y };
    return result;
}

vec2_t vec2_sub(vec2_t a, vec2_t b)
{
    vec2_t result = { a.x - b.x, a.y - b.y };
    return result;
}

vec2_t vec2_mul(vec2_t v, float factor)
{
    vec2_t result = { v.x * factor, v.y * factor };
    return result;
}

vec2_t vec2_div(vec2_t v, float factor)
{
    vec2_t result = { v.x / factor, v.y / factor };
    return result;
}

float vec2_dot(vec3_t a, vec3_t b)
{
    return (a.x * b.x + a.y * b.y);
}

void vec2_normalize(vec2_t* normal)
{
    float mag = vec2_length(*normal);
    normal->x = normal->x / mag;
    normal->y = normal->y / mag;
}

vec2_t vec2_from_vec4(vec4_t v)
{
    vec2_t result = { v.x, v.y};
    return result;
}

vec3_t vec3_add(vec3_t a, vec3_t b)
{
    vec3_t result = { a.x + b.x, a.y + b.y, a.z + b.z };
    return result;
}

vec3_t vec3_sub(vec3_t a, vec3_t b)
{
    vec3_t result = { a.x - b.x, a.y - b.y, a.z - b.z };
    return result;
}

vec3_t vec3_mul(vec3_t v, float factor)
{
    vec3_t result = { v.x * factor, v.y * factor, v.z * factor };
    return result;
}

vec3_t vec3_div(vec3_t v, float factor)
{
    vec3_t result = { v.x / factor, v.y / factor, v.z / factor };
    return result;
}

vec3_t vec3_cross(vec3_t a, vec3_t b)
{
    vec3_t result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

float vec3_dot(vec3_t a, vec3_t b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

void vec3_normalize(vec3_t* normal)
{
    float mag = vec3_length(*normal);
    normal->x = normal->x / mag;
    normal->y = normal->y / mag;
    normal->z = normal->z / mag;
}

vec3_t vec3_from_vec4(vec4_t v)
{
    vec3_t result = { v.x, v.y, v.z };
    return result;
}

vec4_t vec4_from_vec3(vec3_t v)
{
    vec4_t result = { v.x, v.y, v.z, 1 };
    return result;
}
