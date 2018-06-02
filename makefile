CC = mpicc
CFLAGS = -O3 -Wall -fopenmp -DOMP

N = 1000
T = 600


info :
	@ echo "todo info"

clean :
	rm nbody grav.o *.bin

show : simulation.mp4
	open simulation.mp4

simulation.mp4 : plotter.py outfile.bin
	time python3 plotter.py

outfile.bin : nbody
	mpirun -n 4 ./nbody $N $T
	./nbody $N $T sout.bin

test : nbody
	mpirun -n 4 ./nbody $N $T "a.bin"
	./nbody $N $T "b.bin"
	python3 test_identical.py a.bin b.bin



nbody : main.c grav.o serial.o parallel.o
	$(CC) $(CFLAGS) main.c grav.o serial.o parallel.o -o nbody

# Library

parallel.o : grav.h parallel.c
	$(CC) $(CFLAGS) parallel.c -c -o parallel.o

serial.o : grav.h serial.c
	$(CC) $(CFLAGS) serial.c -c -o serial.o

grav.o : grav.h grav.c makefile
	$(CC) $(CFLAGS) grav.c -c -o grav.o
