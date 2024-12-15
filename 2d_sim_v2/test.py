import numpy as np
from numba import njit


pos = np.array([1, 1])
dir = np.array([42, 0])
delta = 0.12

@njit(fastmath=True)
def update(pos: np.ndarray,
           dir: np.ndarray,
           delta) -> tuple[np.ndarray, np.ndarray]:
    pos = pos + dir * delta
    return (pos, dir)

test = pos + dir * delta
print(test)
test, dir = update(pos, dir, delta)
print(test)
