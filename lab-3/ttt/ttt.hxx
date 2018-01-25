#pragma once

#include <climits>
#include <cassert>

#include <type_traits>
#include <utility>
#include <array>
#include <vector>
#include <ostream>

#include <boost/integer.hpp>

#include "utils.hxx"
#include "general.hxx"

#ifndef N
#   warning N is not defined
#   define N 0
#endif

using RC = unsigned short;

struct Coord {
    RC r;
    RC c;
};

namespace Bases {
    enum class OX: bool {
        o,
        x,
    };

    enum class NOX {
        n = 0,
        o = 1 << raw(OX::o),
        x = 1 << raw(OX::x),
    };

    enum class NOXF {
        n = raw(NOX::n),
        o = raw(NOX::o),
        x = raw(NOX::x),
        f = o | x,
    };
}

struct OX {
    using Raw = Bases::OX;

    Raw value;

    static constexpr Raw o = Raw::o;
    static constexpr Raw x = Raw::x;

    constexpr OX(const Raw v): value COPY_INIT_MIL(v) {
    }

    constexpr operator const Raw &() const noexcept { return value; }
    constexpr operator       Raw &()       noexcept { return value; }

    constexpr operator Bases::NOX() const noexcept {
        return static_cast<Bases::NOX>(1 << raw(value));
    }
};

static_assert(!std::is_convertible_v<OX::Raw, bool>);
static_assert(!std::is_convertible_v<OX     , bool>);

struct NOX {
    using Raw = Bases::NOX;

    Raw value;

    static constexpr Raw n = Raw::n;
    static constexpr Raw o = Raw::o;
    static constexpr Raw x = Raw::x;

    constexpr NOX(const Raw m): value COPY_INIT_MIL(m) {
    }

    constexpr NOX(const OX::Raw v): NOX COPY_INIT_MIL(static_cast<Raw>(1 << raw(v))) {
    }

    constexpr operator const Raw &() const noexcept { return value; }
    constexpr operator       Raw &()       noexcept { return value; }

    constexpr explicit operator bool() const noexcept {
        return value != n;
    }

    constexpr bool operator !() const noexcept {
        return !operator bool();
    }
};

struct NOXF {
    using Raw = Bases::NOXF;

    Raw value;

    static constexpr Raw n = Raw::n;
    static constexpr Raw o = Raw::o;
    static constexpr Raw x = Raw::x;
    static constexpr Raw f = Raw::f;

    constexpr NOXF(const Raw m): value COPY_INIT_MIL(m) {
    }

    constexpr operator const Raw &() const noexcept { return value; }
    constexpr operator       Raw &()       noexcept { return value; }

    constexpr explicit operator NOX() const noexcept {
        return static_cast<NOX::Raw>(value);
    }

    constexpr explicit operator bool() const noexcept {
        return value != n;
    }

    constexpr bool operator !() const noexcept {
        return !static_cast<bool>(SELF);
    }
};

struct TttState {
    using Mask = unsigned;

    Mask o = 0;
    Mask x = 0;
    PlayerNo current_player = 0;

    static constexpr Mask make_mask(const RC r, const RC c) noexcept {
        static_assert(sizeof(Mask)*CHAR_BIT >= N*N);
        return 1U << (r*N + c);
    }

    constexpr unsigned combined() const noexcept {
        static_assert(sizeof(unsigned)*CHAR_BIT >= 2*N*N);
        return (x << N*N) + o; // TODO: intermediate typing?
    }

    template<typename CState>
    struct Reference {
        CState &target;
        const Mask mask;

        constexpr Reference(CState &target_, const RC r, const RC c) noexcept
            : target COPY_INIT_MIL(target_)
            , mask COPY_INIT_MIL(make_mask(r, c))
        {
        }

        constexpr NOXF get() const noexcept {
            const bool has_o = target.o & mask;
            const bool has_x = target.x & mask;
            if (has_o) {
                if (has_x) {
                    return NOXF::f;
                } else {
                    return NOXF::o;
                }
            } else {
                if (has_x) {
                    return NOXF::x;
                } else {
                    return NOXF::n;
                }
            }
        }

        constexpr operator NOXF() const noexcept {
            return get();
        }

        constexpr Reference operator =(const NOX v) const noexcept {
            switch (v) {
                case NOX::o: target.o |= mask; break;
                case NOX::x: target.x |= mask; break;
                case NOX::n:
                    target.o &= ~mask;
                    target.x &= ~mask;
                    break;
            }
            return SELF;
        }

        constexpr Reference operator =(const NOXF v) const noexcept {
            switch (v) {
                case NOXF::o: target.o |= mask; break;
                case NOXF::x: target.x |= mask; break;
                case NOXF::n:
                    target.o &= ~mask;
                    target.x &= ~mask;
                    break;
                case NOXF::f:
                    target.o |= mask;
                    target.x |= mask;
                    break;
            }
            return SELF;
        }
    };

    template<typename CState>
    struct RowReference {
        CState &target;
        const RC r;

        constexpr Reference<CState> operator [](const RC c) const noexcept {
            return Reference{target, r, c};
        }
    };

    using    RefToMutable =    Reference<      TttState>;
    using    RefToConst   =    Reference<const TttState>;
    using RowRefToMutable = RowReference<      TttState>;
    using RowRefToConst   = RowReference<const TttState>;

    constexpr RowRefToMutable operator [](const RC r)       noexcept { return RowRefToMutable{SELF, r}; }
    constexpr RowRefToConst   operator [](const RC r) const noexcept { return RowRefToConst  {SELF, r}; }

    NOX check(const auto &r, const auto &c) const {
        const NOXF m = SELF[r(0)][c(0)];
        if (m == NOXF::n || m == NOXF::f) {
            return NOX::n;
        }
        for (RC j = 1; j < N; ++j) {
            if (SELF[r(j)][c(j)].get() != m) { // TODO: get rid of `get`
                return NOX::n;
            }
        }
        switch (m) {
            case NOXF::Raw::o: return NOX::o;
            case NOXF::Raw::x: return NOX::x;
            default:
                __builtin_unreachable();
        }
    }

    static constexpr RC inv(const RC i) noexcept {
        assert(i < N);
        return static_cast<RC>(N - 1 - i);
    }

    NOX get_winner() const noexcept {
        #define RC(EXPR) [&]([[maybe_unused]] const RC j) noexcept { return EXPR; }
        for (RC i = 0; i < N; ++i) {
            if (const NOX m = check(RC(i), RC(j))) return m; // row
            if (const NOX m = check(RC(j), RC(i))) return m; // column
        }
        // diagonals
        if (const NOX m = check(RC(j), RC(    j ))) return m; //   primary
        if (const NOX m = check(RC(j), RC(inv(j)))) return m; // secondary
        #undef RC
        return NOX::n;
    }

    std::vector<Coord> possible_moves(const PlayerNo, Payoffs &payoffs) const {
        std::vector<Coord> moves;
        const NOX w = get_winner();
        if (w) {
            unsigned w0;
            switch (w) {
                case NOX::Raw::o: w0 = raw(OX::Raw::o); break;
                case NOX::Raw::x: w0 = raw(OX::Raw::x); break;
                case NOX::Raw::n: __builtin_unreachable();
            }
            payoffs[w0] = +1;
            payoffs[1 - w0] = -payoffs[w0];
        } else {
            for (RC r = 0; r < N; ++r) {
                for (RC c = 0; c < N; ++c) {
                    if (!SELF[r][c].get()) { // TODO: get rid of `get`
                        moves.push_back({r, c});
                    }
                }
            }
            if (moves.empty()) {
                payoffs[0] = payoffs[1] = 0;
            }
        }
        return moves;
    }

    constexpr TttState transform(Coord (&f)(RC r, RC c) noexcept) const noexcept {
        TttState s;
        for (RC r = 0; r < N; ++r) {
            for (RC c = 0; c < N; ++c) {
                const Coord x = f(r, c);
                s[r][c] = SELF[x.r][x.c].get(); // TODO: get rid of `get`
            }
        }
        return s;
    }

    static constexpr Coord   flip(const RC r, const RC c) noexcept { return {r, inv(c)}; }
    static constexpr Coord rotate(const RC r, const RC c) noexcept { return {inv(c), r}; }

    constexpr TttState get_flipped() const noexcept { return transform(  flip); }
    constexpr TttState get_rotated() const noexcept { return transform(rotate); }

    constexpr TttState with_set(const RC r, const RC c, const OX m) const noexcept {
        const Mask b = make_mask(r, c);
        assert(!(o & b));
        assert(!(x & b));
        switch (m.value) {
            case OX::o: return TttState{.o = o | b, .x = x    };
            case OX::x: return TttState{.o = o    , .x = x | b};
        }
        __builtin_unreachable();
    }

    constexpr TttState with_filled(const RC r, const RC c) const noexcept {
        const Mask b = make_mask(r, c);
        assert(!(o & b));
        assert(!(x & b));
        return TttState{.o = o | b, .x = x | b};
    }

    #if 0
    constexpr OX current_player() const {
        unsigned counts[2] = {0, 0};
        for (RC r = 0; r < N; ++r) {
            for (RC c = 0; c < N; ++c) {
                switch (SELF[r][c].get()) { // TODO: get rid of `get`
                    case NOXF::o: counts[raw(OX::Raw::o)]++; break;
                    case NOXF::x: counts[raw(OX::Raw::x)]++; break;
                    default: break;
                }
            }
        }
        if (counts[0] > counts[1]) {
            assert(counts[0] == counts[1] + 1);
            return static_cast<OX::Raw>(1);
        } else {
            assert(counts[0] == counts[1]);
            return static_cast<OX::Raw>(0);
        }
    }
    #endif

    const TttState apply(const PlayerNo cp, const Coord &move) const {
        TttState next = SELF;
        const OX::Raw cp2 = static_cast<OX::Raw>(cp);
//        assert(cp2 == current_player());
//        next._m_current_player = !_m_current_player;
        const auto &m = next[move.r][move.c];
        assert(!m.get()); // TODO: get rid of `get`
        m = cp2;
        return next;
    }

    static constexpr unsigned NR = 4;

    static constexpr std::size_t n_symmetries = 2*NR;

    using Symmetries = std::array<TttState, 2*NR>;

    constexpr Symmetries symmetries() const {
        TttState state = SELF;
        Symmetries symmetries;
        const auto fill = [&](const unsigned b) {
            symmetries[b + 0] = state;
            for (unsigned j = 1; j < NR; ++j) {
                state = state.get_rotated();
                symmetries[b + j] = state;
            }
        };
        fill(0*NR);
        state = state.get_flipped();
        fill(1*NR);
        return symmetries;
    }
};

#if 0
constexpr bool operator <(const TttState a, const TttState b) noexcept {
    return
        a.o != b.o ?
        a.o <  b.o :
        a.x <  b.x ;
}
#endif

static inline constexpr bool operator <(const TttState &a, const TttState &b) {
    if (a.o < b.o) return  true;
    if (a.o > b.o) return false;
    if (a.x < b.x) return  true;
    if (a.x > b.x) return false;
    return a.current_player < b.current_player;
}

constexpr bool operator ==(const TttState a, const TttState b) noexcept {
    return a.o == b.o && a.x == b.x;
}

constexpr char to_char(const OX::Raw m) noexcept {
    switch (m) {
        case OX::Raw::o: return 'o';
        case OX::Raw::x: return 'x';
    }
    __builtin_unreachable();
}

constexpr char to_char(const NOX::Raw m) noexcept {
    switch (m) {
        case NOX::Raw::n: return ' ';
        case NOX::Raw::o: return 'o';
        case NOX::Raw::x: return 'x';
    }
    __builtin_unreachable();
}

constexpr char to_char(const NOXF::Raw m) noexcept {
    switch (m) {
        case NOXF::Raw::n: return ' ';
        case NOXF::Raw::o: return 'o';
        case NOXF::Raw::x: return 'x';
        case NOXF::Raw::f: return '#';
    }
    __builtin_unreachable();
}

constexpr char to_char  (const  OX  m) noexcept { return to_char  (m.value); }
constexpr char to_char  (const NOX  m) noexcept { return to_char  (m.value); }
constexpr char to_char  (const NOXF m) noexcept { return to_char  (m.value); }

inline std::ostream &operator <<(std::ostream &os, const  OX  &m) { return os << to_char(m); }
inline std::ostream &operator <<(std::ostream &os, const NOX  &m) { return os << to_char(m); }
inline std::ostream &operator <<(std::ostream &os, const NOXF &m) { return os << to_char(m); }

constexpr TttState null_state = TttState{};

using MoveNo = boost::uint_value_t<N*N>::fast;
