CC = mpicc
CFLAGS = -O3 -Wall




nbody : serial.c grav.o
	$(CC) $(CFLAGS) serial.c grav.o -o nbody

grav.o : grav.h grav.c
	$(CC) $(CFLAGS) grav.c -c -o grav.o
