#include "mandelbrot.h"
#include <math.h>

uint32_t colour_palette[] = {
	0xff0000,
	0xff8000,
	0xffff00,
	0x80ff00,
	0x00ff00,
	0x00ff80,
	0x00ffff,
	0x0080ff,
	0x0000ff,
	0x8000ff,
	0xff00ff,
	0xff0080
};
int colour_palette_size = 12;

void complex_mult(const complex *x, const complex *y, complex *result){
	result->re = (x->re * y->re) - (x->im * y->im);
	result->im = (x->re * y->im) + (x->im * y->re);
}

void complex_add(const complex *x, const complex *y, complex *result){
	result->re = x->re + y->re;
	result->im = x->im + y->im;
}

double complex_dist(const complex *x){
	return sqrt(x->re*x->re + x->im*x->im);
}

unsigned int mandelbrot(const complex *c, const unsigned int ITERATIONS){
	complex z = {.re = 0, .im = 0};
	
	for(int i=0; i<ITERATIONS; i++){
		if(complex_dist(&z) > 2.0){
			return i;
		}
		complex result;
		complex_mult(&z, &z, &result);
		complex_add(&result, c, &z);
	}
	return ITERATIONS;
}
