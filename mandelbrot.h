#include <stdint.h>

uint32_t colour_palette[12];
int colour_palette_size;

typedef struct _complex{
	double re;
	double im;
} complex;

void complex_mult(const complex *x, const complex *y, complex *result);
void complex_add(const complex *x, const complex *y, complex *result);
double complex_dist(const complex *x);
unsigned int mandelbrot(const complex *c, const unsigned int ITERATIONS);
