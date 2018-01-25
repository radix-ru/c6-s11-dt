#include <cstdlib>
#include <cassert>

#include <algorithm>
#include <array>
#include <vector>
#include <ostream>
#include <iostream>

#include "utils.hxx"

/// @file
/// [4] 2. Generation of the impartial variant of the game tree for Nim.
/// Properties:
/// - Player-cyclic
/// - Symmetries oblivious
/// - Impartial
/// - Without cache

enum class Player: bool {
    current,
    opponent,
};

struct Mark_and_color {
    char mark;
    const char *color;
};

static constexpr Mark_and_color to_mark_and_color(const Player p) noexcept {
    switch (p) {
        case Player::current : return {'c', "#008000"};
        case Player::opponent: return {'o', "#800000"};
    }
    __builtin_unreachable();
}

using HeapNo   = unsigned short;
using HeapSize = unsigned short;

static constexpr HeapNo n_heaps = 3;

struct Move {
    HeapNo heap_no;
    HeapSize n_taken;
};

static constexpr bool misere = true;

struct NimState: std::array<HeapSize, n_heaps> {
    std::vector<Move> possible_moves(Player &winner) const {
        std::vector<Move> pt;
        for (HeapNo h = 0; h < size(); ++h) {
            for (HeapSize t = 1; t <= SELF[h]; ++t) {
                pt.push_back({h, t});
            }
        }
        if (pt.empty()) {
            winner = misere ? Player::current : Player::opponent;
        }
        return pt;
    }

    constexpr NimState apply(const Move &move) const noexcept {
        NimState next = SELF;
        assert(move.heap_no < size());
        assert(move.n_taken > 0);
        HeapSize &heap = next[move.heap_no];
        assert(move.n_taken <= heap);
        heap -= move.n_taken;
        return next;
    }
};

static inline std::ostream &operator <<(std::ostream &os, const NimState &state) {
    os << state[0];
    for (HeapNo h = 1; h < state.size(); ++h) {
        os << ',' << state[h];
    }
    return os;
}

using NodeId = unsigned;

static inline void on_node(const NodeId id, const NimState &state, const Player winner) {
    const Mark_and_color mac = to_mark_and_color(winner);
    std::cout << "\t" << id << " [label=\"{" << state << '|' << mac.mark << "}\", color=\"" << mac.color << "\"]\n";
}

static inline void on_edge(const NodeId id, const NimState &, const NodeId nid, const NimState &) {
    std::cout << "\t" << id << " -> " << nid << "\n";
}

static NodeId counter = 0;

Player traverse(const NodeId id, const NimState &state) {
    Player winner = Player::opponent;
    for (const Move &m: state.possible_moves(winner)) {
        const Player child = [&]{
            const NimState &next = state.apply(m);
            const NodeId nid = counter++;
            on_edge(id, state, nid, next);
            return traverse(nid, next);
        }();
        if (child == Player::opponent) {
            winner = Player::current;
        }
    }
    on_node(id, state, winner);
    return winner;
}

int main(const int _argc, const char * const argv[]) {
    NimState state;

    command_line_processing: {
        const unsigned n = [&]() noexcept -> unsigned {
            assert(_argc >= 0);
            const unsigned argc = static_cast<unsigned>(_argc);
            assert(argv[argc] == nullptr);
            return std::max(1U, argc) - 1;
        }();

        if (n > state.size()) {
            std::cerr << "Too many heaps.\n";
            return 1;
        }

        for (unsigned i = 0; i < n; ++i) {
            char *p;
            assert(argv[1 + i]);
            const long h = std::strtol(argv[1 + i], &p, 0);
            if (*p != '\0') {
                std::cerr << "Failed to interpret argument #" << (1 + i) << ".\n";
                return 2;
            }
            if (h < 0) {
                std::cerr << "Heap #" << i << " has negative size " << h << ".\n";
                return 3;
            }
            if (h > 9) {
                std::cerr << "Heap #" << i << " has size too big " << h << ".\n";
                return 4;
            }
            state[i] = static_cast<HeapSize>(h);
        }
        std::fill(state.begin() + n, state.end(), 0);
    }

    std::cout << "digraph {\n";
    traverse(counter++, state);
    std::cout << "}\n";

    return 0;
}
