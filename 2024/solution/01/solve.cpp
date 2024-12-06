#include <array>
#include <cmath>
#include <cstdint>
#include <ranges>
#include <span>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../../magic.h"

using Int = std::int32_t;
using Data = std::tuple<std::vector<Int>, std::vector<Int>>;

// Reading in data
Data read() {
    std::vector<Int> list_left;
    std::vector<Int> list_rght;
    for (const std::string& line : LineReader("input")) {
        std::istringstream stream(line);
        Int num;
        stream >> num;
        list_left.push_back(num);
        stream >> num;
        list_rght.push_back(num);
    }
    return {list_left, list_rght};
}

// Solution to part one
Int solve_one(Data& data) {
    auto& [list_left, list_rght] = data;
    std::ranges::sort(list_left);
    std::ranges::sort(list_rght);
    Int distance = 0;
    for (const auto& [left, rght] : std::views::zip(list_left, list_rght)) {
        distance += std::abs(left - rght);
    }
    return distance;
}

// Solution to part two
Int solve_two(const Data& data) {
    auto& [list_left, list_rght] = data;
    auto count = [](std::span<const Int> list) {
        std::unordered_map<Int, Int> counter;
        for (const Int& n : list) {
            counter[n] += 1;
        }
        return counter;
    };
    const auto counter_left = count(list_left);
    const auto counter_rght = count(list_rght);
    Int score = 0;
    for (const auto& [key, val_left] : counter_left) {
        if (auto it = counter_rght.find(key); it != counter_rght.end()) {
            const auto& [_, val_rght] = *it;
            score += key * val_left * val_rght;
        }
    }
    return score;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
