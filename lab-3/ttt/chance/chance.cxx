#include <cassert>

#include <optional>
#include <array>
#include <map>
#include <random>
#include <iostream>

#include <boost/coroutine/asymmetric_coroutine.hpp>

#include "utils.hxx"
#include "general.hxx"
#include "symmetries.hxx"
#define N 3
#include "../ttt.hxx"
#include "../interactive.hxx"

/// @file
/// [11] 7. Human vs Computer game of TTT on 3x3 field with cells excluded by Nature.
/// Properties:
/// - Player-cyclic
/// - Symmetries aware
/// - Partial
/// - With moves by Nature

static constexpr Coord nature_move = {N, N};

struct TttStateInfo {
    std::array<float, n_players> payoffs;
    std::optional<Coord> optimal_move;
};

static std::map<TttState, TttStateInfo> cache;

static ProgressLogger<ipow(4, N*N)> n_traversed;

static constexpr double nature_probability = 0.5;

/// Calculates the answer for `state` and puts it into `tree`.
TttStateInfo traverse(const TttState &state, const unsigned k) {
    const auto i = cache.lower_bound(state);
    if (i != cache.cend()) {
        assert(!(i->first < state));
        if (!(state < i->first)) {
            return i->second;
        }
    }

    const auto remember = [&](const TttStateInfo &si) {
        #ifndef NDEBUG
        const auto old_size = cache.size();
        #endif
        cache.emplace_hint(i, state, si);
        assert(cache.size() == old_size + 1);
        n_traversed++;
        return si;
    };

    end_of_game: {
        const NOX winner = state.get_winner();
        if (winner || filled(state)) {
            switch (winner) {
                // TODO: check `k`?
                case static_cast<NOX::Raw>(1 << 0): return remember({{+1, -1}, std::nullopt});
                case static_cast<NOX::Raw>(1 << 1): return remember({{-1, +1}, std::nullopt});
                case             NOX::Raw::n      : return remember({{ 0,  0}, std::nullopt});
            }
        }
    }

    using MoveProviding = boost::coroutines::asymmetric_coroutine<Coord>;

    MoveProviding::pull_type moves(
        [&](MoveProviding::push_type &sink) {
            for (RC r = 0; r < N; ++r) {
                for (RC c = 0; c < N; ++c) {
                    if (state[r][c].get() == NOXF::n) { // TODO: get rid of `get`
                        sink(Coord{r, c});
                    }
                }
            }
        }
    );

    assert(state.current_player == k%3);
    const PlayerNo next_player = (k + 1)%3;
    if (state.current_player == n_players) {
        TttStateInfo si{{0, 0}, nature_move};
        unsigned npm = 0;

        for (const Coord &m: moves) {
            TttState next = normalize(state.with_filled(m.r, m.c));
            next.current_player = next_player;
            const TttStateInfo current = traverse(next, k + 1);
            si.payoffs[0] += current.payoffs[0];
            si.payoffs[1] += current.payoffs[1];
            npm++;
        }

        assert(npm > 0);
        si.payoffs[0] /= npm;
        si.payoffs[1] /= npm;
        si.payoffs[0] *= nature_probability;
        si.payoffs[1] *= nature_probability;

        {
            TttState next = state;
            next.current_player = next_player;
            const TttStateInfo current = traverse(next, k + 1);
            constexpr double q = 1 - nature_probability;
            si.payoffs[0] += q*current.payoffs[0];
            si.payoffs[1] += q*current.payoffs[1];
        }

        return remember(si);
    } else {
        const PlayerNo cp = state.current_player;
        TttStateInfo best{{-1, -1}, std::nullopt};
        for (const Coord &m: moves) {
            TttState next = normalize(state.with_set(m.r, m.c, static_cast<OX::Raw>(cp)));
            next.current_player = next_player;
            const TttStateInfo current = traverse(next, k + 1);
            if (current.payoffs[cp] > best.payoffs[cp]) {
                best.payoffs = current.payoffs;
                best.optimal_move = m;
            }
        }
        assert(best.optimal_move);
        return remember(best);
    }
}

static std::random_device rd;
static std::bernoulli_distribution nature(nature_probability);
static std::uniform_int_distribution<RC> rc(0, N-1);

static inline void move(TttState &state, const MoveHandler &mh, const MoveNo m) {
    const TttStateInfo si = cache.at(state);
    std::cout << "The ultimate payoffs are [" << si.payoffs[0] << ", " << si.payoffs[1] << "].\n";
    if (m%3 == n_players) {
        std::cout << "Nature's move. ";
        if (nature(rd)) {
            while (true) {
                const RC r = rc(rd);
                const RC c = rc(rd);
                if (state[r][c].get() == NOXF::n) { // TODO: get rid of `get`
                    state[r][c] = NOXF::f;
                    std::cout << Coord{r, c};
                    break;
                }
            }
        } else {
            std::cout << "Skipped";
        }
        std::cout << "\n";
    } else {
        mh.make_move(m%3, si);
    }
    state = normalize(state);
    state.current_player = static_cast<PlayerNo>((m + 1)%3);
}

int main() {
    game2(n_traversed, traverse, move);
}
