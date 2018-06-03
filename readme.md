---
geometry: margin=2cm
---

# High Performance Computing Problem Set 6

## Usage
* To compile `make nbody`
* To run `mpirun -n $n_ranks ./nbody $n_bodies $n_iters $n_threads [$savefile]`
* `python plotter.py` will render `outfile.bin` into `simulation.mp4`
* `make show` will run and plot based on the setting at the top of the makefile

### Midway Scaling
This is a weird result. Somehow using hybrid OMP and MPI was slower as I
increased nodes.  

Nodes    Hybrid   MPI-Only
------  -------  ---------
1        128.74     128.79
2        130.46      51.04
4        146.24      29.54
8        175.30      18.56
