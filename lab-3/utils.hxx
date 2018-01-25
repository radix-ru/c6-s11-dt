#pragma once

#include <cmath>

#include <iostream>

#include "experimental/ranges/concepts"
#include "experimental/ranges/iterator"

#define SELF (*this)

#define COPY_INIT_MIL(EXPR) (EXPR)

namespace ranges = std::experimental::ranges;

template<typename T>
concept bool CopyAssignable = ranges::Assignable<T &, const T &>;

template<typename T>
concept bool LessThanComparable = requires(const T &a, const T &b) {
    {a < b} -> ranges::Boolean;
};

template<typename R, typename T>
concept bool InputRangeOf =
    std::experimental::ranges::InputRange<R>
    && ranges::Same<ranges::value_type_t<ranges::iterator_t<R>>, T>
;

constexpr unsigned long long ipow(unsigned long long a, unsigned char n) noexcept {
    unsigned long long r = 1;
    while (n) {
        if (n&1) {
            r *= a;
        }
        a *= a;
        n >>= 1;
    }
    return r;
}

template<auto upper_bound>
struct ProgressLogger {
    using Raw = decltype(upper_bound);

    static const Raw logging_m = std::sqrt(upper_bound);

    Raw counter = 0;

    void operator ++(int) {
        counter++;
        if (counter%logging_m == 0) {
            std::clog << counter << "/~" << upper_bound << std::endl;
        }
    }

    void begin() {
        std::clog << "Preprocessing:\n";
    }

    void end() {
        std::clog << "Total = " << counter << "\n";
        std::clog << "\n";
    }
};

template<typename T>
requires std::is_enum_v<T>
constexpr auto raw(const T t) {
    return static_cast<std::underlying_type_t<T>>(t);
}
