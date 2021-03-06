#include "grav.h"

void pwrite_header(MPI_File f, int n_par, int n_iters, int rank, int nprocs){
    double a = n_par, b = n_iters;
    if(rank == 0){
        MPI_File_write(f, &a, 1, MPI_DOUBLE, MPI_STATUS_IGNORE);
        MPI_File_write(f, &b, 1, MPI_DOUBLE, MPI_STATUS_IGNORE);
    }
}

void pwrite_positions(MPI_File f, bodies_t *bodies, int iter, int rank, int nprocs){
    MPI_Offset offset = sizeof(double) * 2              // header
        + sizeof(vec_t) * nprocs * bodies->n_par * iter // previous iterations
        + sizeof(vec_t) * rank * bodies->n_par;         // other ranks in this iteration

    MPI_File_write_at_all(f, offset, bodies->pos, bodies->n_par * sizeof(vec_t),
        MPI_BYTE, MPI_STATUS_IGNORE);
}


void share_bodies(bodies_t *mine, int rank, int nprocs){
    if(rank){
        MPI_Recv(
            mine->pos, mine->n_par * sizeof(vec_t), MPI_BYTE, 0, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(
            mine->vel, mine->n_par * sizeof(vec_t), MPI_BYTE, 0, 1,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(
            mine->mass, mine->n_par, MPI_DOUBLE, 0, 2,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        bodies_t theirs;
        init_3body(mine);
        init_system(&theirs, mine->n_par);
        for(int i=1; i<nprocs; i++){
            init_3body(&theirs);
            MPI_Send(
                theirs.pos, theirs.n_par * sizeof(vec_t), MPI_BYTE, i, 0,
                MPI_COMM_WORLD);
            MPI_Send(
                theirs.vel, theirs.n_par * sizeof(vec_t), MPI_BYTE, i, 1,
                MPI_COMM_WORLD);
            MPI_Send(
                theirs.mass, theirs.n_par, MPI_DOUBLE, i, 2,
                MPI_COMM_WORLD);
        }
        destroy_system(&theirs);
    }
}


void run_parallel(double dt, int n_par, int n_iters, const char* file, int rank, int nprocs){
    MPI_File f;
    bodies_t bodies;
    vec_t *guest_pos;
    double *guest_mass;
    init_system(&bodies, n_par / nprocs);

    #ifdef IDENTICAL
    share_bodies(&bodies, rank, nprocs);
    #else
    srand(rank + rand());
    init_3body(&bodies);
    #endif

    guest_pos = malloc(sizeof(vec_t) * bodies.n_par);
    guest_mass = malloc(sizeof(double) * bodies.n_par);

    MPI_File_open(MPI_COMM_WORLD, (char*) file,
            MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &f);
    pwrite_header(f, n_par, n_iters, rank, nprocs);

    for (int t=0; t<n_iters; t++) {
        pwrite_positions(f, &bodies, t, rank, nprocs);
        // Acceleration due to own particles
        accelerate(&bodies, bodies.pos, bodies.mass, dt);

        for(int i=1; i<nprocs; i++){
            // Acceleration due to neighbor i's particles
            int sendr, recvr;
            sendr = (rank - i + nprocs) % nprocs;
            recvr = (rank + i + nprocs) % nprocs;
            MPI_Sendrecv(
                bodies.pos, bodies.n_par * sizeof(vec_t), MPI_BYTE, recvr, i,
                guest_pos,  bodies.n_par * sizeof(vec_t), MPI_BYTE, sendr, i,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Sendrecv(
                bodies.mass, bodies.n_par, MPI_DOUBLE, recvr, i,
                guest_mass,  bodies.n_par, MPI_DOUBLE, sendr, i,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            accelerate(&bodies, guest_pos, guest_mass, dt);
        }
        move(&bodies, dt);
    }

    pwrite_positions(f, &bodies, n_iters, rank, nprocs);
    destroy_system(&bodies);
    MPI_File_close(&f);
    free(guest_mass);
    free(guest_pos);
}
