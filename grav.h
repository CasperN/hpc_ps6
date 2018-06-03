#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>
#include <omp.h>


#define GRAV 6.67408e-3
#define SOFTENING 1.0e-4
#define TIME_STEP 0.2
#define EXPECTED_SYSTEM_MASS 1.0

typedef struct { double x, y, z; } vec_t;

typedef struct {
    vec_t *pos, *vel;
    double *mass;
    int n_par;
} bodies_t;


// Due to the force from `num` of `ys` with masses `masses` for `dt` time
// update the velocities on `num` of `xs`
void accelerate(vec_t *vs, vec_t* xs, vec_t* ys, double *mass, double dt, int num);

// Update positions due to velocities
void move(double dt, vec_t *xs, vec_t *vs, int num);

// Initialize / destroy an n body system struct
void init_system(bodies_t *sys, int n_par);
void destroy_system(bodies_t *sys);

// Run the N Body problem on 1 node
void run_serial(double dt, int n_par, int n_iters, const char* file);

// Run the N Body problem on nprocs nodes
void run_parallel(double dt, int n_par, int n_iters, const char* file,
                  int rank, int nprocs);
