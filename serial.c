#include "grav.h"

typedef struct {
    vec_t *pos, *vel;
    double *mass;
    int n_par;
} system_t;


// samples points on unit sphere
vec_t sample_direction(){
    double phi, cos_th, sin_th;
    vec_t v;
    phi    = (double) rand() / (double) RAND_MAX * 2 * M_PI;
    cos_th = (double) rand() / (double) RAND_MAX * 2 - 1;
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


void init_system(system_t *sys, int n_par){

    sys->n_par = n_par;
    sys->pos = malloc(sizeof(vec_t) * n_par);
    sys->vel = malloc(sizeof(vec_t) * n_par);
    sys->mass = malloc(sizeof(double) * n_par);

    for(int i=0; i<n_par; i++){
        // uniformly distributed on unit sphere
        sys->pos[i] = sample_direction();
        // outward with rotation
        sys->vel[i] = add(sys->vel[i], scale(rotate_about_z(sys->pos[i]), 100));
        // Initialize mass while maintaining EXPECTED_SYSTEM_MASS
        sys->mass[i] = (double) rand() / (double) RAND_MAX
                        * EXPECTED_SYSTEM_MASS * 2 / n_par;
    }
}



void write_positions(FILE* f, system_t* bodies){
    fwrite(bodies->pos, sizeof(vec_t), bodies->n_par, f);
}

void write_header(FILE* f, int n_par, int n_iters){
    double a = n_par, b = n_iters;
    fwrite(&a, sizeof(double), 1, f);
    fwrite(&b, sizeof(double), 1, f);
}


void run_serial(system_t *bodies, double dt, int n_iters, const char* file){

    FILE* f = fopen(file, "w");
    write_header(f, bodies->n_par, n_iters);

    for (int t=0; t<n_iters; t++) {
        write_positions(f, bodies);
        accelerate(bodies->vel, bodies->pos, bodies->pos, bodies->mass, dt, bodies->n_par);
        move(dt, bodies->pos, bodies->vel, bodies->n_par);
    }

    write_positions(f, bodies);
    fclose(f);
}


int main(int argc, char const *argv[]) {
    /* code */
    system_t bodies;
    double dt = 0.001;
    int n_par = 50;

    init_system(&bodies, n_par);
    run_serial(&bodies, dt, 500, "outfile.bin");

    return 0;
}
