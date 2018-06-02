import sys
import numpy as np

a = np.fromfile(sys.argv[1], dtype=np.float64)
b = np.fromfile(sys.argv[2], dtype=np.float64)


if a.shape != b.shape:
    print("shapes don't align {} != {}".format(a.shape, b.shape))
    exit(0)

if (a!=b).any():
    diff = np.where(a!=b)
    print(f"Differs at indices: {diff}")
    print(a[diff])
    print(b[diff])
    print((a-b)[diff])
