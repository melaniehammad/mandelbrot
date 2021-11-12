CC = gcc
CFLAGS = -lm -lX11
MANDELBROT = xmandelbrot
JULIA = xjulia

default: all

all: x11_main.o mandelbrot.o x11_julia.o julia.o
	$(CC) $(CFLAGS) x11_main.o mandelbrot.o -o $(MANDELBROT) 
	$(CC) $(CFLAGS) x11_julia.o julia.o -o $(JULIA)

$(MANDELBROT): x11_main.o mandelbrot.o
	$(CC) $(CFLAGS) x11_main.o mandelbrot.o -o $(MANDELBROT) 

$(JULIA): x11_julia.o julia.o
	$(CC) $(CFLAGS) x11_julia.o julia.o -o $(JULIA)

mandelbrot.o: mandelbrot.c mandelbrot.h
	$(CC) $(CFLAGS) -c mandelbrot.c mandelbrot.h

julia.o: julia.c julia.h
	$(CC) $(CFLAGS) -c julia.c julia.h

x11_main.o: x11_main.c
	$(CC) $(CFLAGS) -c x11_main.c 

x11_julia.o: x11_julia.c
	$(CC) $(CFLAGS) -c x11_julia.c 

clean: 
	$(RM) $(TARGET) *.o *~ *h.gch
