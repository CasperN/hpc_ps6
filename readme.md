---
geometry: margin=2cm
---

# High Performance Computing Problem Set 6

## Usage
* To compile `make nbody`
* To run `mpirun -n $n_ranks ./nbody $n_bodies $n_iters $n_threads [$savefile]`
* `python plotter.py` will render `outfile.bin` into `simulation.mp4`
* `make show` will run and plot based on the setting at the top of the makefile

### Parallel vs Serial
Compiling with `-DIDENTICAL` makes the 0 rank initialize the bodies for every
other rank, otherwise each rank will initialize their own bodies. This should
only make a small difference as initialization is $O(N)$ for $N$ bodies while
force computation is $O(N^2 T)$ for $T$ time steps. Testing with 128 bodies for
10 time steps with this flag yields results within 10 degrees of precision. This
can be confirmed with `make test` (be sure to set the parameters at the top of
the file).

### Midway Scaling
This is a weird result. Somehow using hybrid OMP and MPI was slower as I
increased nodes. This may have to do with how Midway's memory is laid out.
If each cpu on midway has separate RAM then the hybrid parallelism should be
worse, though that won't explain why it slows down with more Nodes.

Nodes    Hybrid   MPI-Only
------  -------  ---------
1        101.16     102.13
2        105.36      53.20
4        111.63      29.40
8        139.94      20.23

### Hybrid vs Pure MPI Vesta
Like in Midway, hybrid parallelism is dramatically slower on my implementation.
I'm not sure why,


### Vesta
Scaling tests were run with $2^{19}=524,288$ particles using MPI-Only 64 ranks
per node. Somewhere between 256 and 512 nodes, the cost of moving the particles
became significant compared to the gains of having more nodes. 1024 nodes are
actually slower than before. However as the production simulation doubles the
particles and quadruples the number of comparisons, 1024 nodes should work fine.

 Nodes    Seconds       bodies/rank
------  ---------   ---------------
    32     360.47               256
    64     183.29               128
   128      98.26                64
   256      58.84                32
   512      48.08                16
  1024      55.42                 8

### Production simulation
It turned out that my simulation took over an hour to run so it stopped at
iteration 395. This breaks the plotter file and you have to hardcode the
number of time steps to render the final simulation.
