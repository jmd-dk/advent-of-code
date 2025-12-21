#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

#include "helper.hpp"

using Int = std::uint64_t;
using Bank = std::vector<Int>;
using Data = std::vector<Bank>;

// Reading in data
Data read() {
    Data banks{};
    auto char2int = [](char c) { return static_cast<Int>(c - '0'); };
    std::ifstream file{"input"};
    std::string line{};
    while (std::getline(file, line)) {
        banks.push_back(line | std::views::transform(char2int) | std::ranges::to<std::vector>());
    }
    return banks;
}

// Solution to part one
Int solve_one(const Data& banks) {
    auto compute_max_joltage = [](const Bank& bank) {
        Int max_left = std::ranges::max(bank | std::views::take(bank.size() - 1));
        std::ptrdiff_t index = std::ranges::find(bank, max_left) - bank.begin();
        Int max_rght = std::ranges::max(bank | std::views::drop(1 + index));
        return max_left * 10 + max_rght;
    };
    Int sum{};
    for (const Bank& bank : banks) {
        sum += compute_max_joltage(bank);
    }
    return sum;
}

// Solution to part two
Int solve_two(const Data& banks) {
    constexpr Int num_batteries{12};
    using Key = std::array<std::size_t, 2>;
    using Cache = std::unordered_map<Key, Int, aoc::ArrayHash>;
    auto compute_max_joltage =
        [](this const auto& self, const Bank& bank, Cache& cache, std::size_t remaining, std::size_t index = 0) -> Int {
        if (remaining == 0) {
            // All available digits spent
            return 0;
        }
        if (index + remaining > bank.size()) {
            // Not enough remaining batteries (i.e. we skipped too many)
            return 0;
        }
        // Look up in cache
        Key key{remaining, index};
        if (auto it = cache.find(key); it != cache.end()) {
            return it->second;
        }
        // Option A: Accept digit at this index
        Int pow10 = aoc::ipow(Int{10}, remaining - 1);
        Int joltageA = bank[index] * pow10 + self(bank, cache, remaining - 1, index + 1);
        // Option B: Skip this index
        Int joltageB = self(bank, cache, remaining, index + 1);
        // Pick the option with maximum joltage, memoize and return
        Int max = std::max(joltageA, joltageB);
        cache.emplace(key, max);
        return max;
    };
    Int sum{};
    for (const Bank& bank : banks) {
        Cache cache{};
        sum += compute_max_joltage(bank, cache, num_batteries);
    }
    return sum;
}

// Solve
int main() {
    aoc::analyze(read, solve_one);
    aoc::analyze(read, solve_two);
}
