#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <ranges>
#include <span>
#include <sstream>
#include <string>
#include <vector>

#include "../../magic.h"

using Int = std::int32_t;
using Reports = std::vector<std::vector<Int>>;

// Reading in data
Reports read() {
    Reports reports;
    for (const std::string& line : LineReader("input")) {
        std::istringstream stream(line);
        std::vector<Int> levels;
        Int level;
        while (stream >> level) {
            levels.push_back(level);
        }
        reports.push_back(levels);
    }
    return reports;
}

// Solution to part one
Int solve_one(const Reports& reports) {
    auto sgn = [](const Int num) { return (num > 0) - (num < 0); };
    auto check = [&](const std::span<const Int> levels) {
        static constexpr Int min = 1;
        static constexpr Int max = 3;
        const int sign = sgn(levels[0] - levels[1]);
        for (const auto& [left, rght] :
             std::views::zip(levels, std::views::drop(levels, 1))) {
            const Int diff = left - rght;
            if (sgn(diff) != sign or std::abs(diff) < min or max < std::abs(diff)) {
                return false;
            }
        }
        return true;
    };
    Int num_safe = 0;
    for (auto levels : reports) {
        num_safe += check(levels);
    }
    return num_safe;
}

// Solution to part two
Int solve_two(const Reports& reports) {
    auto sgn = [](const Int num) { return (num > 0) - (num < 0); };
    auto check = [&](const std::span<const Int> levels, std::size_t skip) {
        static constexpr Int min = 1;
        static constexpr Int max = 3;
        static constexpr int sign_unset = -2;
        int sign = sign_unset;
        for (std::size_t index = 0; index < levels.size() - 1 - (levels.size() - 1 == skip);
             index++) {
            if (index == skip) {
                continue;
            }
            const Int left = levels[index];
            const Int rght = levels[index + 1 + (index + 1 == skip)];
            const Int diff = left - rght;
            if (sign == sign_unset) {
                sign = sgn(diff);
            }
            if (sgn(diff) != sign or std::abs(diff) < min or max < std::abs(diff)) {
                return false;
            }
        }
        return true;
    };
    Int num_safe = 0;
    for (auto levels : reports) {
        for (std::size_t skip = 0; skip < levels.size(); skip++) {
            if (check(levels, skip)) {
                num_safe += 1;
                break;
            }
        }
    }
    return num_safe;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
