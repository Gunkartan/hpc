import numpy as np
from multiprocessing import Pool
import time
import os

def multiply_row(args):
    row, b = args

    return np.dot(row, b)

if __name__ == '__main__':
    n = 3000
    a = np.random.rand(n, n)
    b = np.random.rand(n, n)
    num_processes = int(os.environ.get('SLURM_CPUS_PER_TASK', 1))
    start = time.time()

    with Pool(num_processes) as p:
        c = p.map(multiply_row, [(a[i], b) for i in range(n)])

    c = np.array(c)
    end = time.time()
    print(f'Running with {num_processes} cores')
    print(f'The matrix size is {n}')
    print(f'The program ran for {end - start} seconds')