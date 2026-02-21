#pragma once
#include <cmath>
#include <math.h>

typedef struct Vector2f
{
    float x, y;
} Vector2f;

float Length(Vector2f v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

Vector2f Normalize(Vector2f v)
{
    float length = Length(v);
    return (Vector2f){v.x/length, v.y / length};
}

Vector2f Scale(Vector2f v, float scalar)
{
    return (Vector2f){v.x * scalar, v.y * scalar};
}

Vector2f Rotate(Vector2f v, float radians)
{
    float c = cosf(radians);
    float s = sinf(radians);

    return (Vector2f){
      v.x * c - v.y * s,
      v.x * s + v.y * c
    };
}
