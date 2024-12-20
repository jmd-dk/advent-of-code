#include <algorithm>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../magic.hpp"

using Int = std::uint32_t;
using Position = std::array<Int, 2>;
using Data = std::tuple<Position, std::unordered_map<char, std::vector<Position>>>;

// Reading in data
Data read() {
    std::unordered_map<char, std::vector<Position>> antennas;
    Int i = -1;
    Position shape{};
    for (const auto& line : LineReader("input")) {
        i++;
        for (auto [_j, freq] : std::views::enumerate(line)) {
            Int j = static_cast<Int>(_j);
            shape[1] = std::max(shape[1], j);
            if (freq != '.') {
                antennas[freq].push_back({i, j});
            }
        }
    }
    shape[0] = i + 1;
    shape[1]++;
    return {shape, antennas};
}

// Solution to part one
Int solve_one(const Data& data) {
    const auto& [shape, antennas] = data;
    std::unordered_set<Position, ArrayHash<Int>> antinodes;
    auto add_antinode = [&](const Position& antinode) {
        if (antinode[0] < shape[0] && antinode[1] < shape[1]) {
            antinodes.insert(antinode);
        }
    };
    for (const auto& [freq, positions] : antennas) {
        for (const auto& [i, pos_i] : std::views::enumerate(positions)) {
            for (const auto& pos_j : std::views::drop(positions, i + 1)) {
                Position diff{pos_j[0] - pos_i[0], pos_j[1] - pos_i[1]};
                add_antinode({pos_j[0] + diff[0], pos_j[1] + diff[1]});
                add_antinode({pos_i[0] - diff[0], pos_i[1] - diff[1]});
            }
        }
    }
    return antinodes.size();
}

// Solution to part two
Int solve_two(const Data& data) {
    const auto& [shape, antennas] = data;
    std::unordered_set<Position, ArrayHash<Int>> antinodes;
    auto add_antinodes = [&](Position pos, const Position& diff) {
        while (pos[0] < shape[0] && pos[1] < shape[1]) {
            antinodes.insert(pos);
            pos[0] += diff[0];
            pos[1] += diff[1];
        }
    };
    for (const auto& [freq, positions] : antennas) {
        for (const auto& [i, pos_i] : std::views::enumerate(positions)) {
            for (const auto& pos_j : std::views::drop(positions, i + 1)) {
                Position diff{pos_j[0] - pos_i[0], pos_j[1] - pos_i[1]};
                add_antinodes(pos_j, diff);
                diff[0] *= -1;
                diff[1] *= -1;
                add_antinodes(pos_i, diff);
            }
        }
    }
    return antinodes.size();
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
