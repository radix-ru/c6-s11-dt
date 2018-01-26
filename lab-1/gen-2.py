from math import exp, nan, isnan
from numbers import Number
from typing import Callable, Sequence, TypeVar, Union

import matplotlib.pyplot as pp
import numpy as np

PMF = Callable[[int], float]
PDF = Callable[[float], float]


class UniformIntD:  # is a PMF
    def __init__(self, a: int, b: int):
        self.a = a
        self.b = b

    def __call__(self, x: int) -> float:
        if x < self.a:
            return 0
        elif x > self.b:
            return 0
        else:
            return 1 / (self.b - self.a + 1)


class ExpD:  # is a PDF
    def __init__(self, l: float):
        self.l = l

    def __call__(self, x: float) -> float:
        return self.l * exp(-self.l * x)


def p_2(p: PMF, x: int) -> float:
    return p(x // 2) if x % 2 == 0 else 0


ml = 0.5  # type: float
mg = 1 - ml  # type: float


def div(a: float, b: float) -> float:
    if b == 0:
        return nan
    else:
        return a / b


def z_d(p: PMF, y: int) -> float:
    return div(2 * y * ml * p(y) + y / 2 * mg * p_2(p, y), ml * p(y) + mg * p_2(p, y))


def z_c(p: PDF, y: float) -> float:
    return div(2 * y * ml * p(y) + y / 2 * mg * p(y / 2), ml * p(y) + mg * p(y / 2))


T = TypeVar("T")


def plot(p: Callable[[T], float], ys: Sequence[T], z_x: Callable[[Callable[[T], float], T], float], a: bool = False):
    zs = tuple(map(lambda y: z_x(p, y) - y, ys))
    if a:
        f = pp.figure()
        ax = f.add_subplot(111)  # TODO: 111?
        for yz in zip(ys, zs):
            if isnan(yz[1]):
                continue
            ax.annotate("%.2f" % yz[1], xy=yz, textcoords="data")
    pp.plot(ys, zs)


def plot_d(p: PMF, l: int, r: int):
    plot(p, tuple(range(l, r + 1)), z_d, True)


def plot_c(p: PDF, l: float, r: float):
    plot(p, np.linspace(l, r, 1000), z_c)


# plot_d(UniformIntD(1, 10), 0, 12)
plot_c(ExpD(1), 0, 12)
plot_c(ExpD(2), 0, 12)
plot_c(ExpD(4), 0, 12)
pp.grid()
pp.show()
