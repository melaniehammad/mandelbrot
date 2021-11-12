#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>

#include "julia.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define WIN_HEIGHT 1000
#define WIN_WIDTH 1000
int ITERATIONS = 0;

Display *display;
int screen;
Window win;
GC gc;
u_int32_t black, white;

struct coord {
	int x, y;
} dot;

struct rect {
	double top_y, left_x, bottom_y, right_x;
} scope;

void close_window();

void draw_julia(complex *c){
	for(int x=0; x<WIN_WIDTH; x++){
		for(int y=0; y<WIN_HEIGHT; y++){
			complex z = {
				.re = ((double)x * ((4.0)/WIN_WIDTH)) - 2.0,
				.im = ((double)(-y + WIN_HEIGHT) * ((4.0)/WIN_HEIGHT)) - 2.0
			};
			int iterations = julia(&z, c, ITERATIONS);
			
			if(iterations == ITERATIONS){
				XSetForeground(display, gc, black);
			}
			else{
				XSetForeground(display, gc, colour_palette[iterations%colour_palette_size]);
			}
			XDrawPoint(display, win, gc, x, y);
		}
	}
}

int main(int argc, char **argv) {
	complex c;
	if(argc == 4) {
		c.re = atof(argv[1]);
		c.im = atof(argv[2]);
		ITERATIONS = atoi(argv[3]);
	}
	else if(argc == 3){
		c.re = atof(argv[1]);
		c.im = atof(argv[2]);
		ITERATIONS = 500;
	}
	else{
		c.re = 0.0, c.im = 0.0;
		ITERATIONS = 500;
		printf("Unproper arguments provided\n\n");
	}
	
	display = XOpenDisplay(NULL);
	screen = DefaultScreen(display);

	black = BlackPixel(display, screen);
	white = WhitePixel(display, screen);

	char julia_win_title[50];
	sprintf(julia_win_title, "Julia Set for c=%5lf+%5lfi", c.re, c.im);
	
	win = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, WIN_HEIGHT, WIN_WIDTH, 0, white, white);
	XSetStandardProperties(display, win, julia_win_title, "Hi", None, NULL, 0, NULL);
	XSelectInput(display, win, ExposureMask | KeyPressMask);
	
	gc = XCreateGC(display, win, 0, 0);
	XSetBackground(display, gc, white);
	XSetForeground(display, gc, black);
	XClearWindow(display, win);
	XMapRaised(display, win);
	
    XEvent event;
    KeySym key;
    char text[255];
	
	while (1){
		
		XNextEvent(display, &event);
		if(event.type==Expose && event.xexpose.count==0) {
			draw_julia(&c);
		}
		if(event.type==KeyPress && XLookupString(&event.xkey, text, 255, &key,0)==1) {
			if(text[0]=='q') {
				close_window();
			}
		}
    }
    return 0;
}

void close_window() {
	XFreeGC(display, gc);
	XDestroyWindow(display, win);
	XCloseDisplay(display);
	exit(0);
}
