#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "../../magic.hpp"

using Int = std::int32_t;
using Position = std::array<Int, 2>;
struct Guard {
    Position pos;
    Position dir;
};
using Positions = std::unordered_set<Position, ArrayHash<Int>>;
using Data = std::tuple<Position, Guard, Positions>;

// Reading in data
Data read() {
    Guard guard;
    Positions obstacles;
    Position shape{};
    Int i = -1;
    for (const std::string& line : LineReader("input")) {
        i++;
        for (auto [_j, c] : std::views::enumerate(line)) {
            Int j = static_cast<Int>(_j);
            shape[1] = std::max(shape[1], j);
            switch (c) {
                case '#':
                    obstacles.insert({i, j});
                    break;
                case '<':
                    guard.pos = {i, j};
                    guard.dir = {0, -1};
                    break;
                case '>':
                    guard.pos = {i, j};
                    guard.dir = {0, +1};
                    break;
                case 'v':
                    guard.pos = {i, j};
                    guard.dir = {+1, 0};
                    break;
                case '^':
                    guard.pos = {i, j};
                    guard.dir = {-1, 0};
                    break;
            }
        }
    }
    shape[0] = i + 1;
    shape[1]++;
    return {shape, guard, obstacles};
}

// Solution to part one
Int solve_one(Data& data) {
    auto& [shape, guard, obstacles] = data;
    static const std::unordered_map<Position, Position, ArrayHash<Int>> turn{
        {{0, -1}, {-1, 0}},
        {{-1, 0}, {0, +1}},
        {{0, +1}, {+1, 0}},
        {{+1, 0}, {0, -1}},
    };
    Positions visited;
    while (0 <= guard.pos[0] and guard.pos[0] < shape[0] and 0 <= guard.pos[1] and
           guard.pos[1] < shape[1]) {
        visited.insert(guard.pos);
        auto pos_next = guard.pos;
        for (auto i : {0, 1}) {
            pos_next[i] += guard.dir[i];
        }
        if (obstacles.contains(pos_next)) {
            guard.dir = turn.at(guard.dir);
        } else {
            guard.pos = pos_next;
        }
    }
    return static_cast<Int>(visited.size());
}

// Solution to part two
Int solve_two(Data& data) {
    auto& [shape, guard, obstacles] = data;
    static const std::unordered_map<Position, Position, ArrayHash<Int>> turn{
        {{0, -1}, {-1, 0}},
        {{-1, 0}, {0, +1}},
        {{0, +1}, {+1, 0}},
        {{+1, 0}, {0, -1}},
    };
    auto patrol = [&](Guard guard,
                      std::unordered_map<Position, Positions, ArrayHash<Int>>& visited) {
        while (0 <= guard.pos[0] and guard.pos[0] < shape[0] and 0 <= guard.pos[1] and
               guard.pos[1] < shape[1]) {
            if (auto it = visited.find(guard.pos); it != visited.end()) {
                auto& [_, dirs] = *it;
                if (dirs.contains(guard.dir)) {
                    return true;
                }
            }
            visited[guard.pos].insert(guard.dir);
            auto pos_next = guard.pos;
            for (auto i : {0, 1}) {
                pos_next[i] += guard.dir[i];
            }
            if (obstacles.contains(pos_next)) {
                guard.dir = turn.at(guard.dir);
            } else {
                guard.pos = pos_next;
            }
        }
        return false;
    };
    std::unordered_map<Position, Positions, ArrayHash<Int>> visited;
    patrol(guard, visited);
    std::unordered_map<Position, Positions, ArrayHash<Int>> visited_obstructed;
    Int possibilities = 0;
    for (const auto& [pos, _] : visited) {
        if (pos == guard.pos) {
            continue;
        }
        obstacles.insert(pos);
        visited_obstructed.clear();
        possibilities += patrol(guard, visited_obstructed);
        obstacles.erase(pos);
    }
    return possibilities;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
