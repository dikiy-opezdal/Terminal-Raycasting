#include <stdio.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
    float x, y, z;
} vec3;

#define width  120
#define height 30
const float aspect = width / height;
const float char_aspect = 0.5f;

char screen[width * height + 1];

vec3 sphere_center = {0.0f, 0.0f, 5.0f};
float sphere_radius = 1.0f;

float magnitude(vec3 vec) {
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

float dot(vec3 v0, vec3 v1) {
    float angle = cos((v0.x*v1.x) + (v0.y*v1.y) + (v0.z*v1.z));

    return magnitude(v0) * magnitude(v1) * angle;
}

vec3 normalize(vec3 vec) {
    float magn = magnitude(vec);

    if (magn == 0) return vec;
    return (vec3){vec.x / magn, vec.y / magn, vec.z / magn};
}

bool hit_sphere(vec3 center, float radius, vec3 ray) {
    float a = dot(ray, ray);
    float b = -2.0f * dot(ray, center);
    float c = dot(center, center) - radius*radius;
    return (b*b - 4*a*c) >= 0;
}

int main() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = ((float)x / width * 2.0f - 1.0f) * aspect * char_aspect;
            float ny = (float)y / height * 2.0f - 1.0f;

            vec3 dir = normalize((vec3){nx, ny, 1.0f});

            if (hit_sphere(sphere_center, sphere_radius, dir)) {
                screen[y * width + x] = '@';
            }
            else screen[y * width + x] = ' ';
        }
    }

    screen[width * height] = '\0';
    printf("%s", screen);

    return 0;
}