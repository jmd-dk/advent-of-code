#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "helper.hpp"

using Int = std::int64_t;
using Coord = std::array<Int, 2>;
using CoordSet = std::unordered_set<Coord, aoc::ArrayHash>;
struct Map {
    CoordSet splitters{};
    Coord shape{};
};
using Data = std::pair<Map, Coord>;

// Reading in data
Data read() {
    Map map{};
    Coord start{};
    std::ifstream file{"input"};
    std::string line{};
    for (Int i = 0; std::getline(file, line); i++) {
        for (auto [_j, c] : std::views::enumerate(line)) {
            Int j = static_cast<Int>(_j);
            switch (c) {
                case '^':
                    map.splitters.insert({i, j});
                    map.shape[0] = std::max(map.shape[0], i + 1);
                    map.shape[1] = std::max(map.shape[1], j + 1);
                    break;
                case 'S':
                    start = {i, j};
                    break;
            }
        }
    }
    return {map, start};
}

// Solution to part one
Int solve_one(const Data& data) {
    const auto& [map, start] = data;
    Int count{};
    CoordSet splitters_visited{};
    auto emit_tachyon_beam = [&](this const auto& self, const Coord& start_coord) {
        Int j = start_coord[1];
        for (Int i = start_coord[0]; i < map.shape[0]; i++) {
            Coord coord{i, j};
            if (map.splitters.contains(coord)) {
                if (splitters_visited.contains(coord)) {
                    return;
                }
                splitters_visited.insert(coord);
                count++;
                self({i, j - 1});
                self({i, j + 1});
                return;
            }
        }
    };
    emit_tachyon_beam(start);
    return count;
}

// Solution to part two
Int solve_two(const Data& data) {
    const auto& [map, start] = data;
    std::unordered_map<Int, Int> beams{
        {start[1], 1},
    };
    for (Int i = start[0]; i < map.shape[0]; i++) {
        std::unordered_map<Int, Int> beams_next{};
        for (auto [j, multiplicity] : beams) {
            if (map.splitters.contains({i, j})) {
                beams_next[j - 1] += multiplicity;
                beams_next[j + 1] += multiplicity;
            } else {
                beams_next[j] += multiplicity;
            }
        }
        std::swap(beams, beams_next);
    }
    return aoc::sum(std::views::values(beams));
}

// Solve
int main() {
    aoc::analyze(read, solve_one);
    aoc::analyze(read, solve_two);
}
