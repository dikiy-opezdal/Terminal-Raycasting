#include <stdio.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
    float x, y, z;
} vec3;

typedef struct {
    vec3 center;
    float radius;
} sphere_t;

#define width  120
#define height 30
float aspect = width / height;
float char_aspect = 0.5f;

char *grayscale = " .:-=oa#%@";

char screen[width * height + 1];

sphere_t sphere = {{0.0f, 0.0f, 2.0f}, 1.0f};

vec3 light_dir = {1.0f, -1.0f, 1.0f};

float magnitude(vec3 vec) {
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

float dot(vec3 v0, vec3 v1) {
    return (v0.x*v1.x) + (v0.y*v1.y) + (v0.z*v1.z);
}

vec3 normalize(vec3 vec) {
    float magn = magnitude(vec);

    if (magn == 0) return vec;
    return (vec3){vec.x / magn, vec.y / magn, vec.z / magn};
}

float hit_sphere(vec3 center, float radius, vec3 ray) {
    float a = pow(magnitude(ray), 2.0);
    float h = dot(ray, center);
    float c = pow(magnitude(center), 2.0) - radius*radius;

    float discr = h*h - a*c;
    if (discr < 0.0f) return -1.0f;
    else {
        return (h - sqrt(discr)) / a;
    }
}

char light2char(float light) {
    return grayscale[(int)(light * 9)];
}

int main() {
    light_dir = normalize(light_dir);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = ((float)x / width * 2.0f - 1.0f) * aspect * char_aspect;
            float ny = (float)y / height * 2.0f - 1.0f;

            vec3 dir = normalize((vec3){nx, ny, 1.0f});

            float t = hit_sphere(sphere.center, sphere.radius, dir);
            if (t > 0) {
                vec3 normal = normalize((vec3){t * dir.x, t * dir.y, t * dir.z + 1.0f});
                float light = dot(normal, light_dir);
                if (light < 0.0f) light = 0.0f;
                screen[y * width + x] = light2char(light);
            }
            else screen[y * width + x] = ' ';
        }
    }

    screen[width * height] = '\0';
    printf("%s", screen);

    return 0;
}