from typing import MutableSequence, Sequence, Tuple

import numpy as np


class Namespace(type):
    pass


class Prob(float):
    pass


class Gain(float):
    pass


class Player(metaclass=Namespace):
    class Count(int):
        pass

    class No(int):
        pass

    class GainMatrix(np.ndarray):
        pass


class Strategy(metaclass=Namespace):
    class Count(int):
        pass

    class No(int):
        pass


class Vector(np.ndarray):
    Type = Sequence[float]


class PlayerPP(Vector):
    Type = Sequence[Prob]


class GamePP(tuple):
    Type = MutableSequence[PlayerPP]


class GameSP(tuple):
    """ Strategy profile. """
    Type = Tuple[Strategy.Count]


# class TotalGainMatrix(tuple, Tuple[Player.GainMatrix]):
#     pass

class TotalGainMatrix(np.ndarray):
    Type = Tuple[Player.GainMatrix]


class PlayerG(Vector):
    Type = Sequence[Gain]
