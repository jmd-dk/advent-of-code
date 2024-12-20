#include <array>
#include <cstdint>
#include <queue>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "../../magic.hpp"

using Int = std::int32_t;
using Coord = std::array<Int, 2>;
using Space = std::unordered_set<Coord, ArrayHash<Int>>;

// Reading in data
std::vector<Coord> read() {
    static const std::regex pattern(R"((\d+),(\d+))");
    std::vector<Coord> coordinates;
    for (const std::string& line : LineReader("input")) {
        std::smatch match;
        if (std::regex_match(line, match, pattern)) {
            Int x, y;
            std::istringstream(match[1].str()) >> x;
            std::istringstream(match[2].str()) >> y;
            coordinates.push_back({x, y});
        }
    }
    return coordinates;
}

// Solution to part one
Int solve_one(std::vector<Coord>& coordinates) {
    constexpr Coord shape{70 + 1, 70 + 1};
    constexpr Int time = 1024;
    auto corrupt = [](std::vector<Coord>& coordinates, Int num) {
        Space space;
        for (const auto& coord : std::views::take(coordinates, num)) {
            space.insert(coord);
        }
        return space;
    };
    auto bfs = [&](const Space& space) {
        constexpr Coord end{shape[0] - 1, shape[1] - 1};
        constexpr std::array<Coord, 4> steps{{{-1, 0}, {+1, 0}, {0, -1}, {0, +1}}};
        std::queue<std::tuple<Coord, Int>> visit{{{{0, 0}, 0}}};
        std::unordered_set<Coord, ArrayHash<Int>> visited(space);
        while (not visit.empty()) {
            auto [coord, length] = visit.front();
            visit.pop();
            if (visited.contains(coord)) {
                continue;
            }
            visited.insert(coord);
            if (coord == end) {
                return length;
            }
            for (const auto& step : steps) {
                Coord coord_next{coord[0] + step[0], coord[1] + step[1]};
                if (not(0 <= coord_next[0] && coord_next[0] < shape[0])) {
                    continue;
                }
                if (not(0 <= coord_next[1] && coord_next[1] < shape[1])) {
                    continue;
                }
                if (visited.contains(coord_next)) {
                    continue;
                }
                visit.push({coord_next, length + 1});
            }
        }
        return -1;
    };
    Space space = corrupt(coordinates, time);
    return bfs(space);
}

// Solution to part two
std::string solve_two(std::vector<Coord>& coordinates) {
    constexpr Coord shape{70 + 1, 70 + 1};
    auto corrupt = [](std::vector<Coord>& coordinates, Int num) {
        Space space;
        for (const auto& coord : std::views::take(coordinates, num)) {
            space.insert(coord);
        }
        return space;
    };
    auto bfs = [&](const Space& space) {
        constexpr Coord end{shape[0] - 1, shape[1] - 1};
        constexpr std::array<Coord, 4> steps{{{-1, 0}, {+1, 0}, {0, -1}, {0, +1}}};
        std::queue<std::tuple<Coord, Int>> visit{{{{0, 0}, 0}}};
        std::unordered_set<Coord, ArrayHash<Int>> visited(space);
        while (not visit.empty()) {
            auto [coord, length] = visit.front();
            visit.pop();
            if (visited.contains(coord)) {
                continue;
            }
            visited.insert(coord);
            if (coord == end) {
                return length;
            }
            for (const auto& step : steps) {
                Coord coord_next{coord[0] + step[0], coord[1] + step[1]};
                if (not(0 <= coord_next[0] && coord_next[0] < shape[0])) {
                    continue;
                }
                if (not(0 <= coord_next[1] && coord_next[1] < shape[1])) {
                    continue;
                }
                if (visited.contains(coord_next)) {
                    continue;
                }
                visit.push({coord_next, length + 1});
            }
        }
        return -1;
    };
    auto search = [&](Int num) { return bfs(corrupt(coordinates, num)); };
    // Exponential search
    Int lower = 1024;
    Int upper = lower * 2;
    while (search(upper) != -1) {
        upper *= 2;
    }
    lower = upper / 2;
    // Binary search
    while (lower < upper) {
        Int middl = lower + (upper - lower) / 2;
        if (search(middl) == -1) {
            upper = middl - 1;
        } else {
            lower = middl + 1;
        }
    }
    Coord coord = coordinates[upper - 1];
    std::ostringstream stream;
    stream << coord[0] << "," << coord[1];
    return stream.str();
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
