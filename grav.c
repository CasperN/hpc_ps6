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

double rand_unif(double low, double high) {
    return (double) rand() / RAND_MAX * (high - low) + low;
}

// add force of every y to every x
void accelerate(bodies_t *xs, vec_t* ys, double *mass, double dt){
    #pragma omp parallel for schedule(static) shared(xs, ys, mass, dt)
    for(int i=0; i<xs->n_par; i++){
        vec_t acc = {0,0,0};
        for(int j=0; j<xs->n_par; j++){
            double dist;
            vec_t diff;
            diff = add(ys[j], scale(xs->pos[i], -1));
            dist = sqrt(dot(diff, diff) + SOFTENING);
            acc  = add(acc, scale(diff, GRAV * mass[j] / (dist * dist * dist)));
        }
        xs->vel[i] = add(xs->vel[i], scale(acc, dt));
    }
}


void move(bodies_t *xs, double dt) {
    #pragma omp parallel for default(none) schedule(static) shared(xs, dt)
    for(int i=0; i<xs->n_par; i++){
        xs->pos[i] = add(xs->pos[i], scale(xs->vel[i], dt));
    }
}


vec_t sample_direction(){
    double phi, cos_th, sin_th;
    vec_t v;
    phi    = rand_unif(0,2 * M_PI);
    cos_th = rand_unif(-1, 1);
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


void init_3body(bodies_t *xs){

    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    #ifndef IDENTICAL // disallow race conditions over random state in IDENTICAL
    #pragma omp parallel for schedule(static) shared(xs, nprocs)
    #endif
    for(int i=0; i<xs->n_par; i++){
        int r = rand() % 3;
        vec_t cluster;
        // just a bit off of the radius 2 circle
        cluster = r == 0 ? (vec_t) {-1 , -sqrt(3), - DISPLACEMENT}
                : r == 1 ? (vec_t) {-1 , sqrt(3) - DISPLACEMENT, 0}
                :          (vec_t) { 2.0 -DISPLACEMENT, 0, 0 };

        xs->pos[i]  = add(cluster, scale(sample_direction(), rand_unif(0,1)));
        xs->vel[i]  = scale(rotate_about_z(xs->pos[i]), ROTATION);
        xs->mass[i] = rand_unif(0, 2) * EXPECTED_SYSTEM_MASS / xs->n_par / nprocs;
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

}

void destroy_system(bodies_t *sys){
    free(sys->pos);
    free(sys->vel);
    free(sys->mass);
}
