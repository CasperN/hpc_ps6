import sys
import numpy as np

a = np.fromfile(sys.argv[1], dtype=np.float64)
b = np.fromfile(sys.argv[2], dtype=np.float64)


if a.shape != b.shape:
    print("shapes don't align {} != {}".format(a.shape, b.shape))
    exit(0)

diff = np.abs(a - b) > 1e-10
if diff.any():
    diff_idx = np.where(diff)
    print(f"Differs at indices: {diff_idx}")
    print(a[diff_idx])
    print(b[diff_idx])
    print((a-b)[diff_idx])
