#include <stdio.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
    double x, y;
} vec2;
typedef struct {
    double x, y, z;
} vec3;

#define d2r 3.141592653589793 / 180.0

#define width  120
#define height 30
double aspect = (double)width / (double)height;
double char_aspect = 0.5;

char *grayscale = " .,;(oq#0@";

char screen[width * height + 1];

vec3 light_dir = {0.75, -1.0, 1.0};

double magnitude(vec3 vec) {
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

double dot(vec3 v0, vec3 v1) {
    return (v0.x*v1.x) + (v0.y*v1.y) + (v0.z*v1.z);
}

vec3 normalize(vec3 vec) {
    double magn = magnitude(vec);

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

double sign(double num) {
    return (0 > num) - (0 < num);
}

// hit_torus function is under The MIT License
// Copyright © 2014 Inigo Quilez
// https://www.shadertoy.com/view/4sBGDy
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
double hit_torus(vec3 ro, vec3 rd, vec2 tor)
{
    double po = 1.0;
    
    double Ra2 = tor.x*tor.x;
    double ra2 = tor.y*tor.y;
	
    double m = dot(ro,ro);
    double n = dot(ro,rd);

    // bounding sphere
    {
	double h = n*n - m + (tor.x+tor.y)*(tor.x+tor.y);
	if( h<0.0 ) return -1.0;
	//double t = -n-sqrt(h); // could use this to compute intersections from ro+t*rd
    }
    
	// find quartic equation
    double k = (m - ra2 - Ra2)/2.0;
    double k3 = n;
    double k2 = n*n + Ra2*rd.z*rd.z + k;
    double k1 = k*n + Ra2*ro.z*rd.z;
    double k0 = k*k + Ra2*ro.z*ro.z - Ra2*ra2;
	
    #if 1
    // prevent |c1| from being too close to zero
    if( abs(k3*(k3*k3 - k2) + k1) < 0.01 )
    {
        po = -1.0;
        double tmp=k1; k1=k3; k3=tmp;
        k0 = 1.0/k0;
        k1 = k1*k0;
        k2 = k2*k0;
        k3 = k3*k0;
    }
	#endif

    double c2 = 2.0*k2 - 3.0*k3*k3;
    double c1 = k3*(k3*k3 - k2) + k1;
    double c0 = k3*(k3*(-3.0*k3*k3 + 4.0*k2) - 8.0*k1) + 4.0*k0;

    
    c2 /= 3.0;
    c1 *= 2.0;
    c0 /= 3.0;
    
    double Q = c2*c2 + c0;
    double R = 3.0*c0*c2 - c2*c2*c2 - c1*c1;
    
	
    double h = R*R - Q*Q*Q;
    double z = 0.0;
    if( h < 0.0 )
    {
    	// 4 intersections
        double sQ = sqrt(Q);
        z = 2.0*sQ*cos( acos(R/(sQ*Q)) / 3.0 );
    }
    else
    {
        // 2 intersections
        double sQ = pow( sqrt(h) + abs(R), 1.0/3.0 );
        z = sign(R)*abs( sQ + Q/sQ );
    }		
    z = c2 - z;
	
    double d1 = z   - 3.0*c2;
    double d2 = z*z - 3.0*c0;
    if( abs(d1) < 1.0e-4 )
    {
        if( d2 < 0.0 ) return -1.0;
        d2 = sqrt(d2);
    }
    else
    {
        if( d1 < 0.0 ) return -1.0;
        d1 = sqrt( d1/2.0 );
        d2 = c1/d1;
    }

    //----------------------------------
	
    double result = 1e20;

    h = d1*d1 - z + d2;
    if( h > 0.0 )
    {
        h = sqrt(h);
        double t1 = -d1 - h - k3; t1 = (po<0.0)?2.0/t1:t1;
        double t2 = -d1 + h - k3; t2 = (po<0.0)?2.0/t2:t2;
        if( t1 > 0.0 ) result=t1; 
        if( t2 > 0.0 ) result=fmin(result,t2);
    }

    h = d1*d1 - z - d2;
    if( h > 0.0 )
    {
        h = sqrt(h);
        double t1 = d1 - h - k3;  t1 = (po<0.0)?2.0/t1:t1;
        double t2 = d1 + h - k3;  t2 = (po<0.0)?2.0/t2:t2;
        if( t1 > 0.0 ) result=fmin(result,t1);
        if( t2 > 0.0 ) result=fmin(result,t2);
    }

    return result;
}

char light2char(double light) {
    return grayscale[(int)(light * 9)];
}

int main() {
    vec3 rotation = (vec3){0.0, 0.0, 0.0};
    while (true) {
        rotation.y += 0.05*d2r;

        vec3 trans_light = normalize(
            rot_z(rot_y(rot_x(light_dir, rotation.x), rotation.y), rotation.z)
        );

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double nx = ((double)x / width * 2.0 - 1.0) * aspect * char_aspect;
                double ny = (double)y / height * 2.0 - 1.0;

                vec3 dir = normalize((vec3){nx, ny, 1.0});

                double t = hit_torus((vec3){0.0, 0.0, 2.01}, dir, (vec2){1.0, 0.5});
                if (t > 0.0) {
                    vec3 normal = normalize((vec3){t * dir.x, t * dir.y, t * dir.z + 1.0});
                    double light = dot(normal, trans_light);
                    if (light < 0.0) light = 0.0;
                    screen[y * width + x] = light2char(light);
                }
                else screen[y * width + x] = light2char(0.0);
            }
        }

        screen[width * height] = '\0';
        printf("%s", screen);
    }

    return 0;
}