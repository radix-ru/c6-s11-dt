#pragma once

#include <cassert>

#include <optional>
#include <array>
#include <map>

#include "experimental/ranges/concepts"

#include "utils.hxx"

/// @file
/// [5] 4. General game solving algorithm.
/// Requirements:
/// - Symmetries oblivious
/// - Partial

using NPlayers = unsigned short;
using PlayerNo = unsigned short;

constexpr NPlayers n_players = 2;

using Payoff = int;
using Payoffs = std::array<Payoff, n_players>;

template<class State, typename Move>
concept bool StateConcept =
    requires(const State &state, const PlayerNo &current_player, Payoffs &terminal_payoffs) { // [S::pm]
        state.possible_moves(current_player, terminal_payoffs);
        requires InputRangeOf<decltype(state.possible_moves(current_player, terminal_payoffs)), Move>;
    }
    && requires(const State &state, const PlayerNo &current_player, const Move &move) { // [S::a]
        {state.apply(current_player, move)} -> State;
    }
    && LessThanComparable<State> // [s:LTC]
    && std::experimental::ranges::Copyable<Move> // [M:C]
    && std::experimental::ranges::CopyConstructible<State> // [S:CC]
    && CopyAssignable<State> // [S:CA]
;

template<typename Move>
struct StateInfo {
    Payoffs ultimate_payoffs;
    std::optional<Move> optimal_move; ///< Empty if the state is terminal.
};

template<typename Move, class State>
requires StateConcept<State, Move>
struct Traverse {
    using Cache = std::map<State, StateInfo<Move>>;
    using On_node   =  void(const State &state, const Payoffs &payoffs);
    using On_edge   =  void(const State &state, PlayerNo current_player, const State &next);
    using Normalize = State(const State &state);

    Cache &cache;
    On_node &on_node;
    On_edge &on_edge;
    Normalize * const normalize;

    Payoffs operator ()(const State &state, const PlayerNo current_player) {
        const auto j = cache.lower_bound(state); // [s:LTC]
        if (j != cache.cend()) {
            assert(!(j->first < state)); // [s:LTC]
            if (!(state < j->first)) { // [s:LTC]
                return j->second.ultimate_payoffs;
            }
        }

        StateInfo<Move> best;
        {
            const auto next_player = (current_player + 1)%n_players;
            assert(!best.optimal_move);
            for (const Move &m: state.possible_moves(current_player, best.ultimate_payoffs)) { // [S::pm]
                const Payoffs current = [&]{
                    State next = state.apply(current_player, m); // [S::a], [S:CC]
                    if (normalize) {
                        next = normalize(next); // [S:CA]
                    }
                    on_edge(state, current_player, next);
                    return SELF(next, next_player);
                }();
                if (!best.optimal_move || current[current_player] > best.ultimate_payoffs[current_player]) {
                    best.ultimate_payoffs = current;
                    best.optimal_move = m; // [M:C]
                }
            }
        }

        #ifndef NDEBUG
        const auto old_size = cache.size();
        #endif
        cache.try_emplace(j, state, best); // [S:CC], [M:C]
        assert(cache.size() == old_size + 1);
        on_node(state, best.ultimate_payoffs);
        return best.ultimate_payoffs;
    }
};
