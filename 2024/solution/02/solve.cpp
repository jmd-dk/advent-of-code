#include <cmath>
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
        const Int min = 1;
        const Int max = 3;
        const int sign = sgn(levels[0] - levels[1]);
        for (const auto& [left, right] :
             std::views::zip(levels, std::views::drop(levels, 1))) {
            const Int diff = left - right;
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
    std::function<bool(const std::span<Int>, bool)> check;
    check = [&](const std::span<Int> levels, bool recurse) {
        const Int min = 1;
        const Int max = 3;
        const int sign = sgn(levels[0] - levels[1]);
        for (auto it_left = levels.begin(), it_rght = levels.begin() + 1;
             it_rght != levels.end(); it_left++, it_rght++) {
            const Int diff = *it_left - *it_rght;
            if (sgn(diff) == sign && min <= std::abs(diff) && std::abs(diff) <= max) {
                continue;
            }
            if (!recurse) {
                return false;
            }
            // Try leaving out left
            const Int left = *it_left;
            *it_left = *(it_left - 1);
            if (check(std::span<Int>(it_left, levels.end()), false)) {
                return true;
            }
            // Try leaving out rght
            *it_rght = left;
            return check(std::span<Int>(it_rght, levels.end()), false);
        }
        return true;
    };
    Int num_safe = 0;
    for (auto levels : reports) {
        num_safe += check(levels, true);
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
