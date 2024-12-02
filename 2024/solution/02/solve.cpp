#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <print>
#include <ranges>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using Int = std::int32_t;
using Reports = std::vector<std::vector<Int>>;

// Reading in data
Reports read() {
    const std::string filename = "input";
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    Reports reports;
    std::string line;
    while (std::getline(file, line)) {
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
Int solve_one(Reports reports) {
    auto sgn = [](const Int num) { return (num > 0) - (num < 0); };
    auto check = [&](const std::span<const Int> levels) {
        static const Int min = 1;
        static const Int max = 3;
        const int sign = sgn(levels[0] - levels[1]);
        for (const auto& [left, rght] :
             std::views::zip(levels, std::views::drop(levels, 1))) {
            const Int diff = left - rght;
            if (sgn(diff) != sign || std::abs(diff) < min || max < std::abs(diff)) {
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
Int solve_two(Reports reports) {
    auto sgn = [](const Int num) { return (num > 0) - (num < 0); };
    auto check = [&](const std::span<const Int> levels, std::size_t skip) {
        static const Int min = 1;
        static const Int max = 3;
        static const int sign_unset = -2;
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
            if (sgn(diff) != sign || std::abs(diff) < min || max < std::abs(diff)) {
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
    const int day = 2;
    Reports reports = read();
    std::print("day {}, part {}: {}\n", day, 1, solve_one(reports));
    std::print("day {}, part {}: {}\n", day, 2, solve_two(reports));
    return 0;
}
