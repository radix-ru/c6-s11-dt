import random

import numpy as np

from utils import GameSP, Player, TotalGainMatrix

random.seed()


class Setup:
    pass


odd_vs_even: Setup = Setup()
rps: Setup = Setup()  # Rock-Paper-Scissors
random3vs3: Setup = Setup()
random: Setup = Setup()

setup: Setup = rps

n: Player.Count  # the number of players
s: GameSP  # given a player, the number of strategies they have
π: TotalGainMatrix.Type  # given a profile and a player, the gain of that player

if setup is odd_vs_even:
    π0: Player.GainMatrix = np.array((
        (0, +1),
        (+1, 0),
    ), dtype=np.int8)
    π1: Player.GainMatrix = np.array((
        (+1, 0),
        (0, +1),
    ), dtype=np.int8)

    n = 2
    s = (2, 2)
    π = np.array((π0, π1))
elif setup is rps:
    π0: Player.GainMatrix = np.array((
        (0, -1, +1),
        (+1, 0, -1),
        (-1, +1, 0),
    ), dtype=np.int8)

    n = 2
    s = (3, 3)
    π = np.array((+π0, -π0))
elif setup is random3vs3:
    n = 2
    s = (3, 3)
    π = np.random.randint(-100, 100, (n,) + s, dtype=np.int8)
elif setup is random:
    n = random.randint(1, 3)  # TODO: min=0 ?
    s = tuple(random.randint(1, 3) for _ in range(n))  # TODO: min=0 ?
    π = np.random.randint(-100, +100, (n,) + s, dtype=np.int8)
else:
    raise Exception("No setup selected")

"""
n = 2
s = (1, 2)
π = np.array((
    ((21, 22),),
    ((98, 59),),
))
"""

assert len(s) == n
assert π.ndim == 1 + n
assert π.shape == (n,) + s

print("n =", n)
print("s =", s)
print("π = {")
for p in range(n):
    print(p, ":", sep="")
    print(π[p])
print("}")
