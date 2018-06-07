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
#define EXPECTED_SYSTEM_MASS 10.0
#define ROTATION 0.04
#define DISPLACEMENT 0.5

typedef struct { double x, y, z; } vec_t;

typedef struct {
    vec_t *pos, *vel;
    double *mass;
    int n_par;
} bodies_t;


// Due to the force from `ys` with masses `masses` for `dt` time update the
// velocities of `xs`
void accelerate(bodies_t *xs, vec_t* ys, double *mass, double dt);

// Position the bodies
void init_3body(bodies_t *xs);

// Update positions due to velocities
void move(bodies_t *xs, double dt);

// Initialize / destroy memory within an n body system struct
void init_system(bodies_t *sys, int n_par);
void destroy_system(bodies_t *sys);

// Run the N Body problem on 1 node
void run_serial(double dt, int n_par, int n_iters, const char* file);

// Run the N Body problem on nprocs nodes
void run_parallel(double dt, int n_par, int n_iters, const char* file,
                  int rank, int nprocs);
