#include <cstddef>
#include <cstdint>
#include <deque>
#include <ranges>
#include <sstream>
#include <unordered_map>
#include <utility>

#include "../../magic.h"

using Int = std::uint64_t;

// Reading in data
std::deque<Int> read() {
    std::deque<Int> stones;
    for (const std::string& line : LineReader("input")) {
        std::istringstream stream(line);
        Int stone;
        while (stream >> stone) {
            stones.push_back(stone);
        }
    }
    return stones;
}

// Solution to part one
std::size_t solve_one(std::deque<Int>& stones) {
    constexpr int num_blinks = 25;
    constexpr Int factor = 2024;
    for (auto _ [[maybe_unused]] : std::ranges::iota_view(0, num_blinks)) {
        const std::size_t count = stones.size();
        for (std::size_t i = 0; i < count; i++) {
            Int stone = stones[0];
            if (stone == 0) {
                stones.push_back(1);
            } else if (const std::string str = std::to_string(stone); not(str.size() & 1)) {
                std::size_t mid = str.size() / 2;
                std::istringstream(str.substr(0, mid)) >> stone;
                stones.push_back(stone);
                std::istringstream(str.substr(mid)) >> stone;
                stones.push_back(stone);
            } else {
                stones.push_back(factor * stone);
            }
            stones.pop_front();
        }
    }
    return stones.size();
}

// Solution to part two
std::size_t solve_two(std::deque<Int>& stones) {
    constexpr int num_blinks = 75;
    constexpr Int factor = 2024;
    auto blink_at_stone = [&](const Int& stone) -> std::vector<Int> {
        if (stone == 0) {
            return {1};
        } else if (const std::string str = std::to_string(stone); not(str.size() & 1)) {
            Int stone0, stone1;
            const std::size_t mid = str.size() / 2;
            std::istringstream(str.substr(0, mid)) >> stone0;
            std::istringstream(str.substr(mid)) >> stone1;
            return {stone0, stone1};
        } else {
            return {factor * stone};
        }
    };
    std::unordered_map<Int, std::size_t> counter;
    for (const Int& stone : stones) {
        counter[stone] += 1;
    }
    std::unordered_map<Int, std::size_t> counter_next;
    for (const auto& _ [[maybe_unused]] : std::ranges::iota_view(0, num_blinks)) {
        for (const auto& [value, count] : counter) {
            for (const Int& value_next : blink_at_stone(value)) {
                counter_next[value_next] += count;
            }
        }
        counter.clear();
        std::swap(counter, counter_next);
    }
    std::size_t count_tot = 0;
    for (const auto& [value, count] : counter) {
        count_tot += count;
    }
    return count_tot;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
