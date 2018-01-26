from typing import List, Sequence

import numpy as np

from common import n, s, π
from utils import Strategy, Player

k: Sequence = np.zeros(s, dtype=np.int16)  # for each profile, the count of players for which it is the best response

# TODO: optimize?
i = np.nditer(k, flags=["multi_index"], op_flags=["readwrite"])  # a profile
while not i.finished:
    # a player
    for p in range(n):  # type: Player.No
        br: bool = True  # whether the profile is the best response (or one of them) for the player
        # an alternative strategy
        for a in range(s[p]):  # type: Strategy.No
            if a == i.multi_index[p]:
                continue
            mi: List[int] = list(i.multi_index)  # the alternative profile
            mi[p] = a
            if π[p][tuple(mi)] > π[p][i.multi_index]:
                br = False
                break
        if br:
            i[0] += 1
    i.iternext()

for x in np.nditer(k, op_flags=["readwrite"]):
    x[...] = (x == n)

print("k = ")
print(k)
