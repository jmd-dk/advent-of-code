#include <array>
#include <cstdint>
#include <cstdlib>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../magic.hpp"

using Int = std::int64_t;
struct Machine {
    std::array<Int, 2> button_a;
    std::array<Int, 2> button_b;
    std::array<Int, 2> prize;
};

// Reading in data
std::vector<Machine> read() {
    static const std::regex pattern(R"((Button A|Button B|Prize): .+?(\d+), .+?(\d+))");
    std::vector<Machine> machines;
    std::array<Int, 2> button_a;
    std::array<Int, 2> button_b;
    std::array<Int, 2> prize;
    for (const std::string& line : LineReader("input")) {
        std::smatch match;
        if (not std::regex_match(line, match, pattern)) {
            continue;
        }
        Int x, y;
        std::istringstream(match[2].str()) >> x;
        std::istringstream(match[3].str()) >> y;
        std::string type{match[1].str()};
        if (type == "Button A") {
            button_a = {x, y};
        } else if (type == "Button B") {
            button_b = {x, y};
        } else {
            prize = {x, y};
            machines.emplace_back(button_a, button_b, prize);
        }
    }
    return machines;
}

// Solution to part one
namespace Costs {
constexpr Int A = 3;
constexpr Int B = 1;
}  // namespace Costs
Int solve_one(const std::vector<Machine>& machines) {
    auto compute_cost = [](Int m, Int n) { return m * Costs::A + n * Costs::B; };
    Int cost_tot = 0;
    for (const auto& [a, b, p] : machines) {
        Int denom = b[0] * a[1] - a[0] * b[1];
        if (denom == 0) {
            // Degenerate case
            // This case is not present within the input data, so we can ignore it
            throw std::runtime_error("Buttons A and B are degenerate");
            continue;
        }
        // Unique solution case
        auto [quot_a, rem_a] = std::div(b[0] * p[1] - p[0] * b[1], denom);
        if (rem_a != 0) {
            // Fractional A-button presses required
            continue;
        }
        auto [quot_b, rem_b] = std::div(p[0] - quot_a * a[0], b[0]);
        if (rem_b != 0) {
            // Fractional B-button presses required
            continue;
        }
        // Integer solution exists
        cost_tot += compute_cost(quot_a, quot_b);
    }
    return cost_tot;
}

// Solution to part two
Int solve_two(std::vector<Machine>& machines) {
    static constexpr Int offset = 10'000'000'000'000;
    for (auto& [a, b, p] : machines) {
        p[0] += offset;
        p[1] += offset;
    }
    auto compute_cost = [](Int m, Int n) { return m * Costs::A + n * Costs::B; };
    Int cost_tot = 0;
    for (const auto& [a, b, p] : machines) {
        Int denom = b[0] * a[1] - a[0] * b[1];
        if (denom == 0) {
            // Degenerate case
            // This case is not present within the input data, so we can ignore it
            throw std::runtime_error("Buttons A and B are degenerate");
            continue;
        }
        // Unique solution case
        auto [quot_a, rem_a] = std::div(b[0] * p[1] - p[0] * b[1], denom);
        if (rem_a != 0) {
            // Fractional A-button presses required
            continue;
        }
        auto [quot_b, rem_b] = std::div(p[0] - quot_a * a[0], b[0]);
        if (rem_b != 0) {
            // Fractional B-button presses required
            continue;
        }
        // Integer solution exists
        cost_tot += compute_cost(quot_a, quot_b);
    }
    return cost_tot;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
