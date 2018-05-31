#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef OMP
#include <omp.h>
#endif


#define GRAV 6.67408
#define SOFTENING 0.00000001
#define EXPECTED_SYSTEM_MASS 100

typedef struct { double x, y, z; } vec_t;

vec_t add(vec_t x1, vec_t x2);
vec_t scale(vec_t v, double c);

// force of y on x
vec_t force(double m1, vec_t x, double m2, vec_t y);

// Due to the force from `num` of `ys` with masses `masses` for `dt` time
// update the velocities on `num` of `xs`
void accelerate(vec_t *vs, vec_t* xs, vec_t* ys, double *mass, double dt, int num);

// Change positions due to velocities
void move(double dt, vec_t *xs, vec_t *vs, int num);
