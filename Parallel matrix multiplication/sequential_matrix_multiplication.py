import numpy as np
import time

if __name__ == '__main__':
    n = 6000
    np.random.seed(42)
    a = np.random.rand(n, n)
    b = np.random.rand(n, n)
    start = time.time()
    c = np.dot(a, b)
    end = time.time()
    print(f'The matrix size is {n}')
    print(f'The program ran for {end - start:.6f} seconds')