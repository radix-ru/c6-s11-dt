#pragma once

#include <algorithm>

#include "experimental/ranges/concepts"
#include "experimental/ranges/algorithm"

#include "utils.hxx"
#include "general.hxx"

/// @file
/// [8] 5. Symmetries aware game solving algorithm.
/// Requirements:
/// - Symmetries aware
/// - Partial

template<class State>
concept bool HasSymmetries =
    std::experimental::ranges::CopyConstructible<State> // [S:CC]
    && requires(const State &state) {
        state.symmetries(); // [S::s]
        requires InputRangeOf<decltype(state.symmetries()), State>; // [S::s/RT]
    }
    && LessThanComparable<State> // [S:LTC]
;

template<class State>
requires HasSymmetries<State>
inline State normalize(const State &state) {
    const auto &symmetries = state.symmetries(); // [S::s]
    assert(!ranges::empty(symmetries)); // [S::s/RT]
    const auto &b = ranges::cbegin(symmetries); // [S::s/RT]
    const auto &e = ranges::cend  (symmetries); // [S::s/RT]
    if constexpr (ranges::EqualityComparable<State>) { // provide [S:EC]
        assert(ranges::find(b, e, state) != e); // [S::s/RT], use [S:EC]
    }
    return */* TODO: std -> ranges */std::min_element(b, e); // [S:LTC], [S:CC]
}

template<class State>
concept bool Normalizable = requires(const State &state) {
    {normalize(state)} -> State;
};

template<typename Move, class State>
requires StateConcept<State, Move> && Normalizable<State>
struct NormalizedTraversal: Traverse<Move, State> {
private:
    using _m_base = Traverse<Move, State>;

public:
    NormalizedTraversal(
        typename _m_base::Cache &_cache,
        typename _m_base::On_node &_on_node,
        typename _m_base::On_edge &_on_edge
    ): _m_base{_cache, _on_node, _on_edge, normalize<State>} {
    }

    Payoffs operator ()(const State &state, const PlayerNo current_player) {
        return _m_base::operator ()(normalize(state), current_player); // [S:CC]
    }
};
