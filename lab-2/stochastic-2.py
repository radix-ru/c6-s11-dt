from typing import List, Sequence, Tuple

import numpy as np

from common import n, s, π
from utils import distribute, hstack, increment, ones, vstack, zeros, Strategy



def increment(a: MutableSequence[bool], l: int, r: int) -> bool:
    if l == r:
        return False
    if a[l]:
        a[l] = False
        return increment(a, l + 1, r)
    else:
        a[l] = True
        return True


T = TypeVar("T")


def distribute(xc: Sequence[T], m: Sequence[bool]) -> List[T]:
    """ Distributes `xc` over `m`.

    :param xc: `x` compressed
    :param m: the distribution
    :return: `x` uncompressed
    """
    n: int = len(m)
    x: List[T] = [None] * n
    k: int = 0
    for i in range(n):  # type: int
        if m[i]:
            x[i] = xc[k]
            k += 1
        else:
            x[i] = 0
    return x


def zeros(*args):
    return np.zeros(args, dtype=np.int8)


def ones(*args):
    return np.ones(args, dtype=np.int8)


def vstack(*args):
    return np.concatenate(args, axis=0)


def hstack(*args):
    return np.concatenate(args, axis=1)


if n != 2:
    print("stochastic, but n != 2, exiting")
    exit(0)

a: Strategy.Count = s[0]
b: Strategy.Count = s[1]

aa = zeros(a, a)
bb = zeros(b, b)
ab = π[0]
ba = π[1].T
al1 = ones(a, 1)
al2 = zeros(a, 1)
bl1 = zeros(b, 1)
bl2 = ones(b, 1)
l1a = al1.T
l1b = bl1.T
l2a = al2.T
l2b = bl2.T
ll = zeros(2, 2)
abB = zeros(a + b, 1)
lB = ones(2, 1)
aA = hstack(aa, ab, al1, al2)
bA = hstack(ba, bb, bl1, bl2)
l1A = hstack(l1a, l1b)
l2A = hstack(l2a, l2b)
lA = hstack(vstack(l1A, l2A), ll)
A = vstack(aA, bA, lA)
B = vstack(abB, lB)

print("A,B =")
print(hstack(A, B))

m: List[bool] = np.zeros((a + b + n), dtype=bool)  # mask
m[-2] = 1
m[-1] = 1

# TODO: check that the stationary point is a minimum

v: List[Tuple] = []
while increment(m, 0, a):
    while increment(m, a, a + b):
        print("m =", m)
        Am = A[m][:, m]  # TODO: ?
        Bm = B[m]
        print("Am,Bm =")
        print(hstack(Am, Bm))
        try:
            xm = np.linalg.solve(Am, Bm).T[0]
        except np.linalg.LinAlgError:
            print("Singular")
        else:
            print("xm =", xm)
            x: Sequence = distribute(xm, m)
            p1 = x[0:a]
            p2 = x[a:a + b]
            if any(p < 0 for p in p1) or any(p < 0 for p in p2):
                continue
            p = np.outer(p1, p2)
            e1 = np.sum(np.multiply(p, π[0]))
            e2 = np.sum(np.multiply(p, π[1]))
            v.append((p1, p2, e1, e2))
            print("p1 =", p1)
            print("p2 =", p2)
            # print("l1 =", x[-2])
            # print("l2 =", x[-1])
            print("e1 =", e1)
            print("e2 =", e2)
        print()

v.sort(key=lambda t: t[2:4])
for t in v:
    for p in t[0:2]:
        print("[", end="")
        for x in p:
            print("%7.2f%%" % (x * 100), end="")
        print("] ", end="")
    print("%8.2f %8.2f" % t[2:4])
