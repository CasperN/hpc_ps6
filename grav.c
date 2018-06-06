#include "grav.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

vec_t add(vec_t x1, vec_t x2) {
    x1.x += x2.x;
    x1.y += x2.y;
    x1.z += x2.z;
    return x1;
}


vec_t scale(vec_t v, double c) {
    v.x *= c;
    v.y *= c;
    v.z *= c;
    return v;
}

double dot(vec_t x1, vec_t x2){
    return x1.x * x2.x + x1.y * x2.y + x1.z * x2.z;
}


// add force of every y to every x
void accelerate(vec_t *vs, vec_t* xs, vec_t* ys, double *mass, double dt, int num){
    #pragma omp parallel for schedule(static) shared(xs, ys, vs, mass, dt, num)
    for(int i=0; i<num; i++){
        vec_t acc = {0,0,0};
        for(int j=0; j<num; j++){
            double dist;
            vec_t diff;
            diff = add(ys[j], scale(xs[i], -1));
            dist = sqrt(dot(diff, diff) + SOFTENING);
            acc  = add(acc, scale(diff, GRAV * mass[j] / (dist * dist * dist)));
        }
        vs[i] = add(vs[i], scale(acc, dt));
    }
}


void move(double dt, vec_t *xs, vec_t *vs, int num) {
    #pragma omp parallel for default(none) schedule(static) \
        shared(xs, vs, dt, num)
    for(int i=0; i<num; i++){
        xs[i] = add(xs[i], scale(vs[i], dt));
    }
}


vec_t sample_direction(){
    double phi, cos_th, sin_th;
    vec_t v;
    phi    = (double) rand_r() / (double) RAND_MAX * 2 * M_PI;
    cos_th = (double) rand_r() / (double) RAND_MAX * 2 - 1;
    sin_th = sqrt(1 - cos_th * cos_th);

    v.x = sin_th * cos(phi);
    v.y = sin_th * sin(phi);
    v.z = cos_th;
    return v;
}


vec_t rotate_about_z(vec_t x){
    double tmp = x.y;
    x.y = x.x;
    x.x = -tmp;
    x.z = 0;
    return x;
}


void init_3body(vec_t* pos, vec_t*vel, double*mass, int n){

    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    #pragma omp parallel for schedule(static) shared(pos, vel, mass, n, nprocs)
    for(int i=0; i<n; i++){
        vec_t cluster;

        // Just a little bit off perfect symmetry
        if (i % 3 == 0)
            cluster = (vec_t) {-1 , -sqrt(3), 0.1};
        else if (i % 3 == 1)
            cluster = (vec_t) {-1 , +sqrt(3), 0};
        else
            cluster = (vec_t) { 2.1, 0, 0};

        pos[i] = add(cluster, scale(sample_direction(),(double) rand_r()/RAND_MAX));
        vel[i] = scale(rotate_about_z(pos[i]), 0.02);
        mass[i] = ((double) rand_r() / (double) RAND_MAX) * 2
            * EXPECTED_SYSTEM_MASS / n / nprocs;

    }
}


void init_crosses(vec_t* pos, vec_t*vel, double*mass, int n_par){
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    for(int i=0; i<n_par; i++){

        int j = i + n_par * rank;
        double x = (double) j / n_par / nprocs;
        pos[i] = (vec_t) {
            j % 2 == 0 ? x : -x,
            j % 3 == 0 ? x : -x,
            j % 5 == 0 ? x : -x
        };
        mass[i] = EXPECTED_SYSTEM_MASS / n_par / nprocs;
        vel[i] = (vec_t) {0,0,0};
    }
}


void init_system(bodies_t *sys, int n_par){

    sys->n_par = n_par;
    sys->pos  = malloc(n_par * sizeof(vec_t));
    sys->vel  = malloc(n_par * sizeof(vec_t));
    sys->mass = malloc(n_par * sizeof(double));


    #ifdef IDENTICAL
        init_crosses(sys->pos, sys->vel, sys->mass, sys->n_par);
    #else
        init_3body(sys->pos, sys->vel, sys->mass, sys->n_par);
    #endif
}

void destroy_system(bodies_t *sys){
    free(sys->pos);
    free(sys->vel);
    free(sys->mass);
}
