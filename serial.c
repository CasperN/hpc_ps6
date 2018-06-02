#include "grav.h"


void write_positions(FILE* f, bodies_t* bodies){
    fwrite(bodies->pos, sizeof(vec_t), bodies->n_par, f);
}

void write_header(FILE* f, int n_par, int n_iters){
    double a = n_par, b = n_iters;
    fwrite(&a, sizeof(double), 1, f);
    fwrite(&b, sizeof(double), 1, f);
}

void run_serial(double dt, int n_par, int n_iters, const char* file){

    bodies_t bodies;
    init_system(&bodies, n_par);

    FILE* f = fopen(file, "w");
    write_header(f, bodies.n_par, n_iters);

    for (int t=0; t<n_iters; t++) {
        write_positions(f, &bodies);
        accelerate(bodies.vel, bodies.pos, bodies.pos, bodies.mass, dt, bodies.n_par);
        move(dt, bodies.pos, bodies.vel, bodies.n_par);
    }

    write_positions(f, &bodies);
    destroy_system(&bodies);
    fclose(f);
}
