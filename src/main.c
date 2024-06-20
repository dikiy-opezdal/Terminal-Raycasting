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

#define d2r 3.141592653589793f / 180.0f

#define width  120
#define height 30
float aspect = width / height;
float char_aspect = 0.5f;

char *grayscale = " .,;(oq#0@";

char screen[width * height + 1];

sphere_t sphere = {{0.0f, 0.0f, 3.0f}, 2.0f};

vec3 light_dir = {0.75f, -1.0f, 1.0f};

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

vec3 rot_x(vec3 v0, double angle)
{
    vec3 v1 = v0;
    v1.z = v0.z * cos(angle) - v0.y * sin(angle);
    v1.y = v0.z * sin(angle) + v0.y * cos(angle);
    return v1;
}
vec3 rot_y(vec3 v0, double angle)
{
    vec3 v1 = v0;
    v1.x = v0.x * cos(angle) - v0.z * sin(angle);
    v1.z = v0.x * sin(angle) + v0.z * cos(angle);
    return v1;
}
vec3 rot_z(vec3 v0, double angle)
{
    vec3 v1 = v0;
    v1.x = v0.x * cos(angle) - v0.y * sin(angle);
    v1.y = v0.x * sin(angle) + v0.y * cos(angle);
    return v1;
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
    vec3 rotation = (vec3){0.0f, 0.0f, 0.0f};
    while (true) {
        rotation.y += 0.05f*d2r;

        vec3 trans_light = normalize(
            rot_z(rot_y(rot_x(light_dir, rotation.x), rotation.y), rotation.z)
        );

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float nx = ((float)x / width * 2.0f - 1.0f) * aspect * char_aspect;
                float ny = (float)y / height * 2.0f - 1.0f;

                vec3 dir = normalize((vec3){nx, ny, 1.0f});

                float t = hit_sphere(sphere.center, sphere.radius, dir);
                if (t > 0.0f) {
                    vec3 normal = normalize((vec3){t * dir.x, t * dir.y, t * dir.z + 1.0f});
                    float light = dot(normal, trans_light);
                    if (light < 0.0f) light = 0.0f;
                    screen[y * width + x] = light2char(light);
                }
                else screen[y * width + x] = light2char(0.0f);
            }
        }

        screen[width * height] = '\0';
        printf("%s", screen);
    }

    return 0;
}