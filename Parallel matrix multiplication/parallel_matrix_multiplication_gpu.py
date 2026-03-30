import cupy as cp
import time

if __name__ == '__main__':
    n = 6000
    a = cp.random.rand(n, n)
    b = cp.random.rand(n, n)
    cp.cuda.Stream.null.synchronize()
    start = time.time()
    c = cp.dot(a, b)
    cp.cuda.Stream.null.synchronize()
    end = time.time()
    print(f'Running on a GPU')
    print(f'The matrix size is {n}')
    print(f'The program ran for {end - start:.6f} seconds')