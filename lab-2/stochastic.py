from array import array
from math import sqrt, exp
from typing import Callable, Iterable, List

import numpy as np

from projection_simplex import projection_simplex_sort
from utils import GamePP, Namespace, Player, PlayerG, PlayerPP, Prob, Strategy, TotalGainMatrix, Vector


def equal(n, *args):
    for m in args:
        assert m == n
    return n


def calc_d_yx_2(g_y: Player.GainMatrix, p: GamePP.Type, x: Player.No) -> PlayerG:
    """ Calculate the gain gradient of player #y against the strategy change of player #x at point p. """
    n: Player.Count = equal(g_y.ndim, len(p))
    r: np.array = g_y
    assert x >= 0
    assert x < n
    for j in range(0, x):  # type: Player.No
        r = np.tensordot(p[j], r, 1)
    for j in range(n - 1, x, -1):  # type: Player.No
        r = np.tensordot(r, p[j], 1)
    return r


# def calc_d_yx(π: TotalGainMatrix, p: GamePP.Type, y: Player.No, x: Player.No) -> PlayerG:
#     return calc_d_yx_2(π[y], p, x)
#
#
# def calc_d(π: TotalGainMatrix, p: GamePP.Type, h: Player.No) -> PlayerG:
#     return calc_d_yx(π, p, h, h)
#
#
# def calc_d_2(π: Player.GainMatrix, p: GamePP.Type, h: Player.No) -> PlayerG:
#     return calc_d_yx_2(π, p, h)


class Iteration(metaclass=Namespace):
    class Count(int):
        pass

    class No(int):
        pass


class StepMultiplier(float):
    pass


def uniform(t: Strategy.Count) -> PlayerPP.Type:
    assert t >= 1
    return (Prob(1 / t),) * t


def unit(t: Strategy.Count) -> PlayerPP.Type:
    assert t >= 1
    return (Prob(0),) * (t - 1) + (Prob(1),)


def uniform_eps(t: Strategy.Count) -> PlayerPP.Type:
    assert t >= 1
    return (Prob(1 / t - EPS),) * (t - 1) + (Prob(1 / t + (t - 1) * EPS),)


def random(t: Strategy.Count) -> PlayerPP.Type:
    assert t >= 1
    return np.random.dirichlet(np.ones(t) * D_MULT, size=1)[0]


def iterate(n: int, c: Callable[[int], None]):
    m: int = int(sqrt(n))
    f: str = "i = %%d/%d" % n
    for i in range(1, n + 1):  # type: int
        if i % m == 0:
            print(f % i, flush=True)
        c(i)


class Coordinate(int):
    pass


def print_p(p: GamePP.Type):
    for h in range(len(p)):  # type: Player.No
        print("%d: " % h, end="")
        for x in p[h]:  # type: Prob
            assert x >= 0
            assert x <= 1
            print("%7.2f%%" % (x * 100), end="")
        print()


N_ITERATIONS: Iteration.Count = Iteration.Count(10000)
K: StepMultiplier = StepMultiplier(1e-4)

EPS: Prob = Prob(1e-4)
D_MULT: float = 1 / 2
INITIAL_DISTRIBUTION: Callable[[Strategy.Count], PlayerPP.Type] = random

SHUFFLE: bool = True  # shuffle order of player selection
INPLACE: bool = True
TOTAL: bool = False


def alpha(i: Iteration.No) -> float:
    return exp(i / N_ITERATIONS - 1/2)


def main():
    from common import n, s, π

    g_summed: TotalGainMatrix = π.sum(axis=0)

    p: GamePP.Type = [INITIAL_DISTRIBUTION(t) for t in s]
    print_p(p)

    with open("path.bin", "bw") as f:
        def iteration(i: Iteration.No):
            nonlocal p
            o: Iterable[Player.No] = np.random.permutation(n) if SHUFFLE else range(n)
            q: GamePP.Type = p if INPLACE else [None] * n
            for h in o:  # type: Player.No
                w: PlayerPP = np.empty_like(p[h])
                gs: List[PlayerG] = [calc_d_yx_2(π[j], p, h) for j in range(n)]
                for x in range(len(p)):  # type: Coordinate
                    xm_0: float = 0
                    xp_0: float = 0
                    for j in range(n):  # type: Player.No
                        if j == h:
                            continue
                        gsjx: float = gs[j][x]
                        if gsjx > 0:
                            xm_0 += gsjx
                        elif gsjx < 0:
                            xp_0 += gsjx
                        else:
                            assert gsjx == 0
                    xm: float = gs[h][x] - xm_0 * alpha(i)
                    xz: float = 0
                    xp: float = gs[h][x] + xp_0 * alpha(i)
                    # if xp + xm < 0:
                    #     d = -K
                    # elif xp + xm > 0:
                    #     d = +K
                    # else:
                    #     assert xp + xm == 0
                    #     pass
                    if -xm > xz and -xm > xp:
                        d = -K * gs[h][x]
                    elif xp > xz and xp > -xm:
                        d = +K * gs[h][x]
                    else:
                        d = 0
                    # if xm > xz and xm > xp:
                    #     d = -K
                    # elif xp > xz and xp > xm:
                    #     d = +K
                    # else:
                    #     d = 0
                    w[x] = p[h][x] + d
                # g: PlayerG = gs[h]
                # for j in range(n):
                #     if j == h:
                #         continue
                #     g += gs[j]*alpha(i)
                # g: PlayerG = calc_d_yx_2(g_summed if TOTAL else π[h], p, h)
                # if i % 100 == 0:
                #     print(g)
                # r: Vector = p[h] + K * g
                # q[h] = projection_simplex_sort(r)
                q[h] = projection_simplex_sort(w)
                # if h == 0:
                #   array('d', g).tofile(f)
            p = q
            array('d', p[0]).tofile(f)

        array('d', p[0]).tofile(f)
        iterate(N_ITERATIONS, iteration)

    print_p(p)


if __name__ == "__main__":
    main()
