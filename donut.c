/*
    the REAL donut.c
    compile with -lm, -lX11
    Copyright (c) 2023 turret. <turret@turret.cyou>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define __USE_MISC
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define BETWEEN(x, min, max) min < x && x< max

const float theta_spacing = 0.07;
const float phi_spacing = 0.02;

int height = 24;
int width = 80;

double utime(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void drawTorus (float *b, const float *Az, const float *Bz) {
    float A = *Az;
    float B = *Bz;

    float *z = malloc(width * height * sizeof(float));//[width * height];
    memset(z, 0, width * height * sizeof(float) / sizeof(char)); // clear z-buffer

    // render torus
    float sinA = sin(A);
    float sinB = sin(B);
    float cosA = cos(A);
    float cosB = cos(B);
    for (float theta = 0; 2*M_PI > theta; theta += theta_spacing) {
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        float h = cosTheta + 2;

        for (float phi = 0; 2*M_PI > phi; phi += phi_spacing) {
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            float depth = 1 / (sinPhi * h * sinA + sinTheta * cosA + 5);
            float t = sinPhi * h * cosA - sinTheta * sinA;
            int x = width/2 + (30 * width / 80) * depth * (cosPhi * h * cosB - t * sinB);
            int y = height/2 + (15 * height / 24) * depth * (cosPhi * h * sinB + t * cosB);
            int index = x + width * y;
            float lum = ((sinTheta * sinA - sinPhi * cosTheta * cosA) * cosB - sinPhi * cosTheta * sinA - sinTheta * cosA - cosPhi * cosTheta * sinB);
            if (BETWEEN(y, 0, height - 2) && BETWEEN(x, 0, width) && depth > z[index]) { //22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                z[index] = depth;
                b[index] = lum + 1;
            }
        }
    }

    free(z);
}

int main() {
    Display *dis = XOpenDisplay((char*)0);
    int screen = DefaultScreen(dis);
    unsigned long black = BlackPixel(dis, screen), white = WhitePixel(dis, screen);

    Window win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0, 640, 480, 5, white, black);
    XSetStandardProperties(dis, win, "DOnut!", "donut", None, NULL, 0, NULL);
    XSelectInput(dis, win, ExposureMask);
    GC gc = XCreateGC(dis, win, 0, 0);
    XSetBackground(dis, gc, white);
    XSetForeground(dis, gc, black);
    XClearWindow(dis, win);
    XMapRaised(dis, win);

    // torus rotation floats
    float A = 0; // x-axis
    float B = 0; // z-axis
    while(1){
        double stime = utime();

        XSync(dis, 1);

        XWindowAttributes attr;
        XGetWindowAttributes(dis, win, &attr);
        width = attr.width / SCALE;
        height = attr.height / SCALE;

        float *b = malloc(width * height * sizeof(float));
        memset(b, 0, width * height * sizeof(float) / sizeof(char));
        drawTorus(b, &A, &B);

        // old ascii console generation code. kept for presevation sakes
//#define LUMINANCE_SET " .,-~:;=!*#$@"
        //printf("\x1b[H");
        //for (int k = 0; width * height >= k; k++){
        //    putchar(k % width ? (b[k] > 0 ? LUMINANCE_SET[(int)MAX((b[k] - 1) * 8, 0)] : ' ') : 10);
        //}

        for(int x = 0; width >= x; x++){
            for(int y = 0; height >= y; y++){
                int color = (b[x + width * y] - 1 + M_SQRT2) / sqrt(8) * 255;
                XSetForeground(dis, gc, color << 16 | color << 8 | color);
                XFillRectangle(dis, win, gc, x * SCALE, y * SCALE, SCALE, SCALE);
            }
        }

        free(b);

        double etime = utime();

        if(etime < stime + 1000/FPS_MAX)
            usleep((1000/FPS_MAX - (etime - stime)) * 1000);
        double rtime = utime();

        A += (rtime-stime) * (0.04f * ROTATION_SPEED_FACTOR) / (1000/FPS_MAX);// / (1000/FPS_MAX), 0.04f * ROTATION_SPEED_FACTOR);
        B += (rtime-stime) * (0.02f * ROTATION_SPEED_FACTOR) / (1000/FPS_MAX);// / (1000/FPS_MAX), 0.04f * ROTATION_SPEED_FACTOR);

        printf("%.1f %d  ", (rtime-stime), 1000/(FPS_MAX));
        printf("fps: %.1f %d     \r", 1000/(rtime-stime), FPS_MAX);
    }
}

