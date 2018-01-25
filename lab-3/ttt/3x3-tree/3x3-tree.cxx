#include <cassert>

#include <tuple>
#include <set>
#include <iostream>

#include "symmetries.hxx"
#define N 3
#include "../ttt.hxx"

/// @file
/// [3] 1. Generation of the game tree for TTT on 3x3 field.
/// Properties:
/// - Player-cyclic
/// - Symmetries aware
/// - Partial

static inline std::ostream &operator <<(std::ostream &os, const TttState::RowRefToConst &row) {
    for (RC c = 0; c < N; ++c) {
        const NOXF m = row[c];
        assert(m != NOXF::f);
        os << to_char(static_cast<NOX>(m));
    }
    return os;
}

static inline std::ostream &operator <<(std::ostream &os, const TttState &state) {
    assert(N > 0);
    os << state[0];
    for (RC r = 1; r < N; ++r) {
        os << "\\n";
        os << state[r];
    }
    return os;
}

static inline void on_node(const TttState &state, const Payoffs &payoffs) {
    std::cout << "\t" << state.combined() << " [label=\"" << state << "\"";

    if (payoffs[0] == +1) {
        assert(payoffs[1] == -1);
        std::cout << ", color=\"blue\"";
    } else if (payoffs[0] == -1) {
        assert(payoffs[1] == +1);
        std::cout << ", color=\"red\" ";
    } else { assert(payoffs[0] == 0);
        assert(payoffs[1] == 0);
    }
    std::cout << "]\n";
}

static std::set<std::tuple<TttState, PlayerNo, TttState>> edges;

static inline void on_edge(const TttState &state, const PlayerNo current_player, const TttState &next) {
    edges.insert({state, current_player, next});
}

int main() {
    std::cout << "digraph {\n";

    using Traversal = NormalizedTraversal<Coord, TttState>;
    Traversal::Cache cache;
    Traversal traversal{cache, on_node, on_edge};
    traversal(null_state, 0);

    for (const auto &edge: edges) {
        const TttState &state = std::get<0>(edge);
        const OX::Raw cp = static_cast<OX::Raw>(std::get<1>(edge));
        const TttState &next = std::get<2>(edge);

        const auto &om = cache.at(state).optimal_move;
        assert(om);

        const TttState optimal_next = normalize(state.apply(static_cast<unsigned short>(cp), *om));

        std::cout << "\t" << state.combined() << " -> " << next.combined();
        if (next == optimal_next) {
            std::cout << "[color=\"";
            switch (cp) {
                case OX::Raw::o: std::cout << "blue"; break;
                case OX::Raw::x: std::cout << "red" ; break;
            }
            std::cout << "\"]";
        }
        std::cout << "\n";
    }

    std::cout << "}\n";
}
