#pragma once

#include "ttt.hxx"

inline std::ostream &operator <<(std::ostream &os, const Coord &m) {
    return os << '(' << m.r + 1 << ',' << m.c + 1 << ')';
}

inline void stack(const auto &ht, const auto &b) {
    ht();
    for (RC i = 0; i < N; ++i) {
        b(i);
    }
    ht();
}

inline void row(const auto &ht, const auto &b) {
    stack(
        [&](          ) { std::cout << ht  ; },
        [&](const RC c) { std::cout << b(c); }
    );
    std::cout << '\n';
}

constexpr RC d(RC i) noexcept {
    return (i+1)%10;
}

inline void table(const char corner, const auto &cell) {
    stack(
        [&](          ) noexcept { row(corner, [&](const RC c) noexcept { return d(c)      ; }); },
        [&](const RC r) noexcept { row(d(r)  , [&](const RC c) noexcept { return cell(r, c); }); }
    );
}

inline void print_board(const TttState &state) {
    std::cout << "Board:\n";
    table('+', [&](const RC r, const RC c) noexcept { return state[r][c].get(); });
}

inline OX::Raw get_user_mark() {
    again:
    std::cout << "Choose your mark (`o` or `x`): ";
    char m;
    std::cin >> m;
    switch (m) {
        case 'o': return OX::o;
        case 'x': return OX::x;
    }
    std::cerr << "Invalid mark.\n";
    goto again;
}

struct MoveHandler {
    const OX::Raw user;
    const OX::Raw computer;
    TttState &state;

    explicit MoveHandler(TttState &_state) noexcept
        : user COPY_INIT_MIL(get_user_mark())
        , computer COPY_INIT_MIL(static_cast<OX::Raw>(!static_cast<bool>(user)))
        , state COPY_INIT_MIL(_state)
    {
    }

    Coord get_user_move() const {
        RC r, c;
        again:
        std::cout << "1 <= r, c <= " << N << ": ";
        std::cin >> r >> c;
        if (!(1 <= r && r <= N && 1 <= c && c <= N)) {
            std::cerr << "The coordinates are out of range.\n";
            goto again;
        }
        r -= 1;
        c -= 1;
        static_assert(std::is_unsigned<decltype(r)>::value); assert(r < N);
        static_assert(std::is_unsigned<decltype(c)>::value); assert(c < N);
        if (std::as_const(state)[r][c].get()) { // TODO: get rid of `get`
            std::cerr << "The cell is already occupied.\n";
            goto again;
        }
        return Coord{r, c};
    }

    void make_move(const PlayerNo p, const auto &oi) const {
        assert(oi.optimal_move);
        std::cout << "The optimal move is " << *oi.optimal_move << ".\n";

        const OX::Raw current = static_cast<OX::Raw>(p);

        const Coord move = [&]{
            if (current == user) {
                std::cout << "Your move. ";
                return get_user_move();
            } else { assert(current == computer);
                std::cout << "Computer's move. " << *oi.optimal_move << "\n";
                return *oi.optimal_move;
            }
        }();

        assert(move.r < N);
        assert(move.c < N);
        const decltype(auto) cell = state[move.r][move.c];
        assert(!cell.get()); // TODO: get rid of `get`
        cell = current;

        std::cout << '\n';
    }
};

inline bool filled(const TttState &state) noexcept {
    return (state.o | state.x) & ((1U << N*N) - 1);
}

void game(const auto &move) {
    {
        using S = std::ios_base;
        std::cin.exceptions(S::eofbit | S::failbit | S::badbit);
    }

    while (true) {
        TttState state = null_state; // TODO: the board is transformed during the game
        MoveHandler mh{state};

        NOX winner = NOX::n;
        for (MoveNo m = 0; !winner && !filled(state); ++m) {
            std::cout << "Ply #" << m + 1 << ". ";
            print_board(state);

            move(state, mh, m);

            winner = state.get_winner();
        }
        print_board(state);
        std::cout << "The winner is `" << to_char(winner) << "`.\n";
        std::cout << '\n';
    }
}

template<typename Upper_bound, Upper_bound upper_bound>
inline void game2(ProgressLogger<upper_bound> &n_traversed, auto &traverse, const auto &move) {
    n_traversed.begin();
    traverse(null_state, 0);
    n_traversed.end();
    game(move);
}
