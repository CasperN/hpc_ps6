#include "grav.h"


int main(int argc, char const *argv[]) {
    /* code */
    int n_par, n_iter, rank, nprocs, threads;
    double start, stop;
    char *savefile;

    if (argc != 4 && argc != 5) {
        printf("Usage: ./nbody num_bodies num_time_steps [num_threads] [savefile]\n");
        printf("Note if you specify a `savefile` you have to also specify `num_threads`\n");
        printf("`num_threads <= 0` means use max threads\n");
        exit(0);
    }

    MPI_Init(&argc, (char ***) &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    n_par    = atoi(argv[1]);
    n_iter   = atoi(argv[2]);
    threads  = atoi(argv[3]);
    savefile = argc == 5 ? (char*) argv[4] : "outfile.bin";

    if (n_par % nprocs != 0) {
        if (rank == 0)
            printf("ERROR: num_bodies must be divisible by num mpi ranks.\n");
        MPI_Finalize();
        exit(1);
    }

    if (rank == 0)
        printf("Running with %d ranks, %d bodies, %d time steps.\n",
            nprocs, n_par, n_iter);
    MPI_Barrier(MPI_COMM_WORLD);

    threads = threads < 1 ? omp_get_max_threads() : threads;
    omp_set_num_threads(threads);
    printf("\tRank %d using %d threads.\n", rank, threads);

    start = MPI_Wtime();
    if (nprocs == 1) {
        run_serial(TIME_STEP, n_par, n_iter, savefile);
    } else {
        run_parallel(TIME_STEP, n_par, n_iter, savefile, rank, nprocs);
    }
    stop = MPI_Wtime();

    if (rank == 0)
        printf("Time: %.2lf seconds\n", stop - start);
        
    MPI_Finalize();
    return 0;
}
