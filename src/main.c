#include <stdio.h>
#include <math.h>
#include <string.h>

typedef struct {
    double x, y, z;
} vec3_t;

typedef struct {
    double R, r;
} torus_t;

#define W 120
#define H 30

double sign(double num) {
    return (0 < num) - (0 > num);
}

double dot(vec3_t v0, vec3_t v1) {
    return (v0.x*v1.x) + (v0.y*v1.y) + (v0.z*v1.z);
}

double magnitude(vec3_t vec) {
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

vec3_t normalize(vec3_t vec) {
    double magn = magnitude(vec);

    if (magn == 0) return vec;
    return (vec3_t){vec.x / magn, vec.y / magn, vec.z / magn};
}

vec3_t get_rd(double x, double y, double aspect) {
    vec3_t rd = {0.0, 0.0, 1.0};

    rd.x = (x / W * 2.0 - 1.0) * aspect;
    rd.y = y / H * 2.0 - 1.0;

    return normalize(rd);
}

char light2char(double light, char *grayscale) {
    return grayscale[(int)(light * strlen(grayscale))];
}

// iTorus function is under The MIT License
// Copyright © 2014 Inigo Quilez
// https://www.shadertoy.com/view/4sBGDy
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
double iTorus( vec3_t ro, vec3_t rd, torus_t tor )
{
    double po = 1.0;
    
    double Ra2 = tor.R*tor.R;
    double ra2 = tor.r*tor.r;
	
    double m = dot(ro,ro);
    double n = dot(ro,rd);

    // bounding sphere
    {
	double h = n*n - m + (tor.R+tor.r)*(tor.R+tor.r);
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

int main() {
    char screen[W * H + 1];
    double aspect = (double)W / (double)H * 0.5; // 0.5 — char aspect
    
    vec3_t light_dir = normalize((vec3_t){0.75, -1.0, 1.0});
    char *grayscale = " .,;(oq#0@";

    vec3_t ro = {0.0, 0.0, 2.0}; // #FIXME: point in center

    torus_t torus = {1.0, 0.5};

    while (1) {
        for (int y = 0; y < H; y++) {
            for (int x = 0; x < W; x++) {
                vec3_t rd = get_rd((double)x, (double)y, aspect);

                double t = iTorus(ro, rd, torus);
                if (t > 0.0) {
                    vec3_t normal = normalize((vec3_t){t * rd.x, t * rd.y, t * rd.z});
                    double light = dot(normal, light_dir);

                    if (light < 0.0) light = 1 / strlen(grayscale);
                    screen[y * W + x] = light2char(light, grayscale);
                }
                else screen[y * W + x] = light2char(0.0, grayscale);
            }
        }

        screen[W * H] = '\0';
        printf("%s", screen);
    }

    return 0;
}