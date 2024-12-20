#include <array>
#include <cstdint>
#include <cstdlib>
#include <unordered_map>

#include "../../magic.hpp"

using Int = std::int32_t;
using Position = std::array<Int, 2>;
using Map = std::unordered_map<Position, char, ArrayHash<Int>>;

// Reading in data
Map read() {
    Map map;
    Int i = -1;
    for (const std::string& line : LineReader("input")) {
        i++;
        for (auto [j, c] : std::views::enumerate(line)) {
            map[{i, static_cast<Int>(j)}] = c;
        }
    }
    return map;
}

// Solution to part one
Int solve_one(Map& map) {
    auto lookup = [&](const Position& pos) {
        if (auto it = map.find(pos); it != map.end()) {
            const auto& [_, c] = *it;
            return c;
        } else {
            return ' ';
        }
    };
    auto locate = [&](char symbol) -> Position {
        for (const auto& [pos, c] : map) {
            if (c == symbol) {
                return pos;
            }
        }
        return {-1, -1};
    };
    Position start = locate('S');
    Position end = locate('E');
    map[end] = '.';
    auto race = [&]() {
        constexpr std::array<Position, 4> steps{{{-1, 0}, {+1, 0}, {0, -1}, {0, +1}}};
        Int time = 0;
        Position pos = start;
        std::unordered_map<Position, Int, ArrayHash<Int>> times{{pos, time}};
        Position step_prev_neg{0, 0};
        while (pos != end) {
            for (const auto& step : steps) {
                if (step == step_prev_neg) {
                    continue;
                }
                Position pos_next{pos[0] + step[0], pos[1] + step[1]};
                char c = lookup(pos_next);
                if (c == '.') {
                    step_prev_neg = {-step[0], -step[1]};
                    pos = pos_next;
                    break;
                }
            }
            times[pos] = ++time;
        }
        return times;
    };
    auto manhattan = [](Position pos0, Position pos1) {
        return std::abs(pos1[0] - pos0[0]) + std::abs(pos1[1] - pos0[1]);
    };
    auto times = race();
    auto count_saves = [&](Int num_cheats, Int saved_min) {
        Int count = 0;
        for (auto it0 = times.begin(); it0 != times.end(); it0++) {
            const auto& [pos0, time0] = *it0;
            for (auto it1 = std::next(it0); it1 != times.end(); it1++) {
                const auto& [pos1, time1] = *it1;
                Int dist = manhattan(pos0, pos1);
                if (dist <= num_cheats) {
                    Int saved = std::abs(time1 - time0) - dist;
                    count += (saved >= saved_min);
                }
            }
        }
        return count;
    };
    return count_saves(2, 100);
}

// Solution to part two
Int solve_two(Map& map) {
    auto lookup = [&](const Position& pos) {
        if (auto it = map.find(pos); it != map.end()) {
            const auto& [_, c] = *it;
            return c;
        } else {
            return ' ';
        }
    };
    auto locate = [&](char symbol) -> Position {
        for (const auto& [pos, c] : map) {
            if (c == symbol) {
                return pos;
            }
        }
        return {-1, -1};
    };
    Position start = locate('S');
    Position end = locate('E');
    map[end] = '.';
    auto race = [&]() {
        constexpr std::array<Position, 4> steps{{{-1, 0}, {+1, 0}, {0, -1}, {0, +1}}};
        Int time = 0;
        Position pos = start;
        std::unordered_map<Position, Int, ArrayHash<Int>> times{{pos, time}};
        Position step_prev_neg{0, 0};
        while (pos != end) {
            for (const auto& step : steps) {
                if (step == step_prev_neg) {
                    continue;
                }
                Position pos_next{pos[0] + step[0], pos[1] + step[1]};
                char c = lookup(pos_next);
                if (c == '.') {
                    step_prev_neg = {-step[0], -step[1]};
                    pos = pos_next;
                    break;
                }
            }
            times[pos] = ++time;
        }
        return times;
    };
    auto manhattan = [](Position pos0, Position pos1) {
        return std::abs(pos1[0] - pos0[0]) + std::abs(pos1[1] - pos0[1]);
    };
    auto times = race();
    auto count_saves = [&](Int num_cheats, Int saved_min) {
        Int count = 0;
        for (auto it0 = times.begin(); it0 != times.end(); it0++) {
            const auto& [pos0, time0] = *it0;
            for (auto it1 = std::next(it0); it1 != times.end(); it1++) {
                const auto& [pos1, time1] = *it1;
                Int dist = manhattan(pos0, pos1);
                if (dist <= num_cheats) {
                    Int saved = std::abs(time1 - time0) - dist;
                    count += (saved >= saved_min);
                }
            }
        }
        return count;
    };
    return count_saves(20, 100);
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
