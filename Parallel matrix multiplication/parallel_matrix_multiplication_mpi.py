from mpi4py import MPI
import numpy as np
import time

if __name__ == '__main__':
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()
    n = 6000

    if rank == 0:
        np.random.seed(42)
        a = np.random.rand(n, n)
        b = np.random.rand(n, n)
        start = time.time()
        counts = [n // size + (1 if i < n % size else 0) for i in range(size)]
        displs = [sum(counts[:i]) for i in range(size)]

    else:
        a = None
        b = None
        counts = None
        displs = None

    b = comm.bcast(b, root=0)
    counts = comm.bcast(counts, root=0)
    displs = comm.bcast(displs, root=0)
    local_rows = counts[rank]
    a_local = np.zeros((local_rows, n), dtype=np.float64)

    if rank == 0:
        send_buf = [a.flatten(), [count * n for count in counts], [d * n for d in displs], MPI.DOUBLE]

    else:
        send_buf = None

    recv_buf = np.empty(local_rows * n, dtype=np.float64)
    comm.Scatterv(send_buf, recv_buf, root=0)
    a_local = recv_buf.reshape(local_rows, n)
    c_local = np.dot(a_local, b)

    if rank == 0:
        c = np.zeros((n, n), dtype=np.float64)
        recv_buf = [c.flatten(), [count * n for count in counts], [d * n for d in displs], MPI.DOUBLE]

    else:
        recv_buf = None

    send_buf = c_local.reshape(-1)
    comm.Gatherv(send_buf, recv_buf, root=0)

    if rank == 0:
        end = time.time()
        print(f'Running with {size} processes')
        print(f'The matrix size is {n}')
        print(f'The program ran for {end - start:.6f} seconds')