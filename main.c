#include "grav.h"


int main(int argc, char const *argv[]) {
    /* code */
    int n_par, n_iter, rank, nprocs;
    char *savefile;

    if (argc != 3 && argc != 4) {
        printf("Usage: ./nbody num_bodies num_time_steps [savefile]\n");
        exit(0);
    }

    MPI_Init(&argc, (char ***) &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    n_par    = atoi(argv[1]);
    n_iter   = atoi(argv[2]);
    savefile = argc == 4 ? (char*) argv[3] : "outfile.bin";

    srand(1995 + rank);
    assert(n_par % nprocs == 0);

    if (nprocs == 1) {
        run_serial(TIME_STEP, n_par, n_iter, savefile);
    } else {
        run_parallel(TIME_STEP, n_par, n_iter, savefile, rank, nprocs);
    }

    MPI_Finalize();
    return 0;
}
