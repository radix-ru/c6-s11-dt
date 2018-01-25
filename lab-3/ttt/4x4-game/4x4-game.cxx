#include <cmath>
#include <cassert>

#include <type_traits>
#include <ostream>
#include <iostream>

#include "general.hxx"
#include "symmetries.hxx"
#define N 4
#include "../ttt.hxx"
#include "../interactive.hxx"

/// @file
/// [10] 6. Human vs Computer game of TTT on 4x4 field.
/// Properties:
/// - Player-cyclic
/// - Symmetries aware
/// - Partial

static ProgressLogger<ipow(3, N*N)> n_traversed;

static inline void on_node(const TttState &, const Payoffs &) {
    n_traversed++;
}

static inline void on_edge(const TttState &, const PlayerNo, const TttState &) noexcept {
}

using Traversal = NormalizedTraversal<Coord, TttState>;
static Traversal::Cache cache;
static Traversal traversal{cache, on_node, on_edge};

static inline void move(TttState &state, const MoveHandler &mh, const MoveNo m) {
    const auto &oi = cache.at(state);
    std::cout << "The optimal winner is `";
    switch (oi.ultimate_payoffs[0]) {
        case +1: assert(oi.ultimate_payoffs[1] == -1); std::cout << to_char(static_cast<OX::Raw>(0)); break;
        case -1: assert(oi.ultimate_payoffs[1] == +1); std::cout << to_char(static_cast<OX::Raw>(1)); break;
        case  0: assert(oi.ultimate_payoffs[1] ==  0); std::cout << to_char(           NOX::n      ); break;
    }
    std::cout << "`. ";

    mh.make_move(m%2, oi);
    state = normalize(state);
}

int main() {
    game2(n_traversed, traversal, move);
}
