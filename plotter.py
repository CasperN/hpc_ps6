import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import mpl_toolkits.mplot3d.axes3d as p3
import sys

limit = 3.0

inputfile = "outfile.bin"
if len(sys.argv) > 1:
    inputfile = sys.argv[1]
    ENDIAN = True


print("loading input file ", inputfile, " ...")
f = open(inputfile)

# Reads header info (nBodies, nIters)
meta = np.fromfile(f, dtype=np.double, count=2)
if ENDIAN:
    meta.byteswap(inplace=True)
nBodies, timesteps = meta
nBodies = int(nBodies)
timesteps = int(timesteps)

# Adjusts marker size based on number of bodies in problem
marker = 1.0
if nBodies > 1000:
    marker = 0.5
if nBodies > 10000:
    marker = 0.1
if nBodies > 100000:
    marker = 0.02

# Allocations array to hold a timestep

# Reads initial conditions
arr = np.fromfile(f, dtype=float, count=nBodies * 3).reshape(nBodies, 3)
if ENDIAN:
    arr.byteswap(inplace=True)

# Create a 3D plot and initialize it with initial particle states
fig, ax = plt.subplots()
ax = p3.Axes3D(fig)

# Build Plot
points, = ax.plot3D(arr[:, 0], arr[:, 1], arr[:, 2], "wo", markersize=marker)
ax.set_ylim(-limit, limit)
ax.set_xlim(-limit, limit)
ax.set_zlim3d(-limit, limit)
ax.set_facecolor("xkcd:black")
plt.axis("off")
ax.grid(False)
ax.set_xticks([])
ax.set_yticks([])
ax.set_zticks([])

# Function that will be called for each frame of the animation
def update(data):
    update.t += 1
    if update.t % 25 == 0:
        print("Processing Time Step ", update.t)
    # Reads a set of bodies into an array
    arr = np.fromfile(f, dtype=float, count=nBodies * 3).reshape(nBodies, 3)
    if ENDIAN:
        arr.byteswap(inplace=True)

    points.set_xdata(arr[:, 0])
    points.set_ydata(arr[:, 1])
    points.set_3d_properties(arr[:, 2])  # No set_zdata, se we use this

    return (points,)


update.t = -1

# Generate the animation
ani = animation.FuncAnimation(fig, update, timesteps - 1)

# Save .mp4 of the animation
# Bitrate may need to be increased for higher particle counts
w = animation.writers["ffmpeg"](
    fps=60, metadata=dict(artist="Me"), bitrate=50000, extra_args=["-s", "1280x1280"]
)

ani.save("simulation.mp4", writer=w)
# plt.show()
