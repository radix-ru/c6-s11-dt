#include <limits>
#include <vector>
#include <random>
#include <iostream>

/// @file
/// Pseudo code for alpha-beta algorithm and its trivial testing.

template<typename T>
static constexpr T inf = std::numeric_limits<T>::infinity();

namespace alpha_beta {
    template<class Node, typename Payoff> static inline Payoff minimize(const Node &node, Payoff a, Payoff b);
    template<class Node, typename Payoff> static inline Payoff maximize(const Node &node, Payoff a, Payoff b);

    template<class Node, typename Payoff>
    static inline Payoff minimize(const Node &node, const Payoff a, Payoff b)
    {
        Payoff v = +inf<Payoff>;
        for (const auto &child: node.get_children_or_set_terminal_payoff(&v)) {
            const Payoff c = maximize(child, a, b);
            if (c <  v) v = c;
            if (v <  b) b = v;
            if (b <= a) break;
        }
        return v;
    }

    template<class Node, typename Payoff>
    static inline Payoff maximize(const Node &node, Payoff a, const Payoff b)
    {
        Payoff v = -inf<Payoff>;
        for (const auto &child: node.get_children_or_set_terminal_payoff(&v)) {
            const Payoff c = minimize(child, a, b);
            if (c >  v) v = c;
            if (v >  a) a = v;
            if (a >= b) break;
        }
        return v;
    }
}

namespace test {
    using Payoff = float;
    static constexpr Payoff min_payoff = -100;
    static constexpr Payoff max_payoff = +100;

    static std::random_device rd;
    static std::bernoulli_distribution branching(0.5);
    static std::uniform_real_distribution<Payoff> payoff(min_payoff, max_payoff);

    struct Node {
        std::vector<Node> get_children_or_set_terminal_payoff(Payoff * const v) const noexcept {
            std::cout << this << " ";
            if (branching(rd)) {
                std::cout << "branched\n";
                return {Node(), Node()};
            } else {
                std::cout << "terminated\n";
                *v = payoff(rd);
                return {};
            }
        }
    };
}

int main() {
    const test::Node root;
    [[maybe_unused]] const test::Payoff g = alpha_beta::maximize(root, -inf<test::Payoff>, +inf<test::Payoff>);
}
