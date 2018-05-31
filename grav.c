#include "grav.h"


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

    #ifdef OMP
    #pragma omp parallel for default(none) schedule(static) shared(xs, ys, vs mass)
    #endif
    for(int i=0; i<num; i++){
        for(int j=0; j<num; j++){
            vec_t dist = add(ys[j], scale(xs[i], -1));
            vec_t dv = scale(dist, pow(dot(dist, dist) + SOFTENING, 1.5)
                                    * GRAV * mass[j] * dt);
            vs[i] = add(vs[i], dv);
        }
    }
}

void move(double dt, vec_t *xs, vec_t *vs, int num) {
    #ifdef OMP
    #pragma omp parallel for default(none) schedule(static) shared(xs, ys, vs mass)
    #endif
    for(int i=0; i<num; i++){
        xs[i] = add(xs[i], scale(vs[i], dt));
    }
}
