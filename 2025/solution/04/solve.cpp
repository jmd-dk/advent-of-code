#include <array>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <unordered_set>
#include <vector>

#include "helper.hpp"

using Int = std::int32_t;
using Coord = std::array<Int, 2>;
using Data = std::unordered_set<Coord, helper::ArrayHash>;

// Reading in data
Data read() {
    Data map{};
    std::ifstream file{"input"};
    std::string line;
    for (Int i{}; std::getline(file, line); i++) {
        for (auto [j, c] : std::views::enumerate(line)) {
            if (c == '@') {
                map.insert({i, static_cast<Int>(j)});
            }
        }
    }
    return map;
}

// Solution to part one
Int solve_one(const Data& map) {
    constexpr Int limit{4};
    auto accessable = [&](const Coord& coord) {
        auto [i, j] = coord;
        Int count{};
        for (Int di : {-1, 0, 1}) {
            for (Int dj : {-1, 0, 1}) {
                if (di == 0 and dj == 0) {
                    continue;
                }
                count += map.contains({i + di, j + dj});
                if (count == limit) {
                    return false;
                }
            }
        }
        return true;
    };
    Int numAccessable{};
    for (const Coord& coord : map) {
        numAccessable += accessable(coord);
    }
    return numAccessable;
}

// Solution to part two
Int solve_two(Data map) {
    constexpr Int limit{4};
    auto accessable = [&](const Coord& coord) {
        auto [i, j] = coord;
        Int count{};
        for (Int di : {-1, 0, 1}) {
            for (Int dj : {-1, 0, 1}) {
                if (di == 0 and dj == 0) {
                    continue;
                }
                count += map.contains({i + di, j + dj});
                if (count == limit) {
                    return false;
                }
            }
        }
        return true;
    };
    Int numOri{static_cast<Int>(map.size())};
    std::vector<Coord> remove;
    do {
        remove.clear();
        for (const Coord& coord : map) {
            if (accessable(coord)) {
                remove.push_back(coord);
            }
        }
        for (const Coord& coord : remove) {
            map.erase(coord);
        }
    } while (!remove.empty());
    return numOri - static_cast<Int>(map.size());
}

// Solve
int main() {
    helper::analyze(read, solve_one);
    helper::analyze(read, solve_two);
}
