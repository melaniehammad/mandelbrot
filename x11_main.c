#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>
#include "mandelbrot.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define WIN_HEIGHT 1000
#define WIN_WIDTH 1000
int ITERATIONS = 0;

Display *display;
int screen;
Window win, julia_win;
GC gc, julia_gc;
uint32_t black, white;

struct coord {
	int x, y;
} dot;

struct rect {
	double top_y, left_x, bottom_y, right_x;
} scope;

void init();
void close_window();

void draw_mandelbrot(){
	for(int x=0; x<WIN_WIDTH; x++){
		for(int y=0; y<WIN_HEIGHT; y++){
			complex c = {
				.re = ((double)x * ((scope.right_x - scope.left_x)/WIN_WIDTH)) + scope.left_x,
				.im = (((double)(-y + WIN_HEIGHT) * ((scope.top_y - scope.bottom_y)/WIN_HEIGHT)) + scope.bottom_y)
			};
			int iterations = mandelbrot(&c, ITERATIONS);
			
			if(iterations == ITERATIONS){
				XSetForeground(display, gc, black);
			}
			else{
				XSetForeground(display, gc, colour_palette[iterations%colour_palette_size]);
			}
			XDrawPoint(display, win, gc, x, y);
		}
	}
	printf("Done Drawing for scope %lf %lf - %lf %lf\n", scope.left_x, scope.top_y, scope.right_x, scope.bottom_y);
}

int main(int argc, char **argv) {
	if(argc == 2) ITERATIONS = atoi(argv[1]);
	else ITERATIONS = 500;
	
    init();
    XEvent event;
    KeySym key;
    char text[255];
	
	
	scope.top_y = 2.0,
	scope.left_x = -2.0,
	scope.bottom_y = -2.0,
	scope.right_x = 2.0;
	
	
	while (1){
		XNextEvent(display, &event);
		if(event.type==Expose && event.xexpose.count==0) {
			draw_mandelbrot();
		}
		if(event.type==KeyPress && XLookupString(&event.xkey, text, 255, &key,0)==1) {
			if(text[0]=='q') {
				close_window();
			}
			else if(text[0]=='r'){
				//reset
				scope.top_y = 2.0,
				scope.left_x = -2.0,
				scope.bottom_y = -2.0,
				scope.right_x = 2.0;
				dot.x = 0;
				dot.y = 0;
				draw_mandelbrot();
			}
			else if(text[0] == 'c'){
				//cancel
				dot.x = 0;
				dot.y = 0;
			}
			else if(text[0] == 'j'){
				//julia set for position of last mouseclick
				if(dot.x != 0 || dot.y != 0){
				
					pid_t pid = fork();
					if(pid == -1) {
						fprintf(stderr, "Error occured while forking process.\n");
						exit(-1);
					}
					else if(pid == 0){
						
						double x = ((double)dot.x * ((scope.right_x - scope.left_x)/WIN_WIDTH)) + scope.left_x;
						
						double y = ((double)(-dot.y + WIN_HEIGHT) * ((scope.top_y - scope.bottom_y)/WIN_HEIGHT)) + scope.bottom_y;
						
						printf("Julia Child: Attemtpting to exec for %lf+%lfi\n", x, y);
						
						char _x[20], _y[20], _iterations[10];
						sprintf(_x, "%lf", x);
						sprintf(_y, "%lf", y);
						sprintf(_iterations, "%d", ITERATIONS);
						execl("./xjulia", "./xjulia", _x, _y, _iterations, NULL);
					}
					else{
						dot.x = 0;
						dot.y = 0;
					}
				}
			}
			printf("You pressed the %c key!\n", text[0]);
        }
        if(event.type == ButtonPress) {
			int x = event.xbutton.x;
			int y = event.xbutton.y;
			printf("Display clicked at %d %d\n", x, y);
			if(dot.x == 0 && dot.y == 0){
				dot.x = x;
				dot.y = y;
			}
			else{
				int top_y = MIN(dot.y, y);
				int bottom_y = MAX(dot.y, y);
				int left_x = MIN(dot.x, x);
				int right_x = MAX(dot.x, x);
				printf("Rectangle is %d %d, %d %d\n", left_x, top_y, right_x, bottom_y);
				
				int distance = MAX(bottom_y-top_y, right_x-left_x);
				bottom_y = top_y + distance;
				right_x = left_x + distance;
				printf("Square is    %d %d, %d %d (distance %d)\n", left_x, top_y, right_x, bottom_y, distance);
				
				struct rect new_scope = {
					.top_y = (((double)(-top_y + WIN_HEIGHT) * ((scope.top_y - scope.bottom_y)/WIN_HEIGHT)) + scope.bottom_y),
					.left_x = ((double)left_x * ((scope.right_x - scope.left_x)/WIN_WIDTH)) + scope.left_x,
					.bottom_y = (((double)(-bottom_y + WIN_HEIGHT) * ((scope.top_y - scope.bottom_y)/WIN_HEIGHT)) + scope.bottom_y),
					.right_x = ((double)right_x * ((scope.right_x - scope.left_x)/WIN_WIDTH)) + scope.left_x
				};
				scope = new_scope;
				dot.x = 0;
				dot.y = 0;
				draw_mandelbrot();
			}
        }
    }
    return 0;
}

void init() {
	display = XOpenDisplay(NULL);
	screen = DefaultScreen(display);

	black = BlackPixel(display, screen);
	white = WhitePixel(display, screen);

	win = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, WIN_HEIGHT, WIN_WIDTH, 0, black, white);
	XSetStandardProperties(display, win, "Mandelbrot", "Hi", None, NULL, 0, NULL);
	XSelectInput(display, win, ExposureMask | ButtonPressMask | KeyPressMask);
	
	gc = XCreateGC(display, win, 0, 0);
	XSetBackground(display, gc, white);
	XSetForeground(display, gc, black);
	XClearWindow(display, win);
	XMapRaised(display, win);
}

void close_window() {
	XFreeGC(display, gc);
	XDestroyWindow(display, win);
	XCloseDisplay(display);
	exit(0);
}
