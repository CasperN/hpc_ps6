CC = mpicc
CFLAGS = -O2 -std=c99 -Wall -fopenmp -DIDENTICAL

N = 1024
T = 400
TH = 1

nbody : main.c grav.o serial.o parallel.o
	$(CC) $(CFLAGS) main.c grav.o serial.o parallel.o -o nbody

Readme.pdf : readme.md
	pandoc readme.md -o Readme.pdf

clean :
	rm -f nbody grav.o serial.o parallel.o *.bin

show : simulation.mp4
	open simulation.mp4

simulation.mp4 : plotter.py outfile.bin
	time python3 plotter.py

outfile.bin : nbody
	mpirun -n 8 ./nbody $N $T $(TH)

test : nbody
	mpirun -n 4 ./nbody $N $T $(TH) "a.bin"
	./nbody $N $T $(TH) "b.bin"
	python3 test_identical.py a.bin b.bin


# Library

parallel.o : grav.h parallel.c
	$(CC) $(CFLAGS) parallel.c -c -o parallel.o

serial.o : grav.h serial.c
	$(CC) $(CFLAGS) serial.c -c -o serial.o

grav.o : grav.h grav.c makefile
	$(CC) $(CFLAGS) grav.c -c -o grav.o
