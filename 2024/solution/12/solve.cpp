#include <array>
#include <ranges>
#include <unordered_set>

#include "../../magic.h"

using Int = std::int32_t;
using Grid = std::vector<std::vector<char>>;
using Position = std::array<Int, 2>;
using Region = std::unordered_set<Position, ArrayHash<Int>>;

// Reading in data
Grid read() {
    Grid grid;
    for (const auto& line : LineReader("input")) {
        grid.emplace_back(line.begin(), line.end());
    }
    return grid;
}

// Solution to part one
Int solve_one(const Grid& grid) {
    static constexpr std::array<Position, 4> steps{{{-1, 0}, {+1, 0}, {0, -1}, {0, +1}}};
    const Position shape{static_cast<Int>(grid.size()), static_cast<Int>(grid[0].size())};
    auto connect = [&](Position pos) {
        char type = grid[pos[0]][pos[1]];
        Region region;
        Region visit{pos};
        while (not visit.empty()) {
            const auto& it = visit.begin();
            pos = *it;
            visit.erase(it);
            region.insert(pos);
            for (const auto& step : steps) {
                const Position pos_neighbor{pos[0] + step[0], pos[1] + step[1]};
                if (not(0 <= pos_neighbor[0] && pos_neighbor[0] < shape[0] &&
                        0 <= pos_neighbor[1] && pos_neighbor[1] < shape[1])) {
                    continue;
                }
                if (grid[pos_neighbor[0]][pos_neighbor[1]] != type) {
                    continue;
                }
                if (region.contains(pos_neighbor)) {
                    continue;
                }
                visit.insert(pos_neighbor);
            }
        }
        return region;
    };
    auto get_perimeter = [](const Region& region) {
        Region edges;
        for (const auto& pos : region) {
            for (const auto& step : steps) {
                const Position edge{2 * pos[0] + step[0], 2 * pos[1] + step[1]};
                if (edges.contains(edge)) {
                    edges.erase(edge);
                } else {
                    edges.insert(edge);
                }
            }
        }
        return edges.size();
    };
    Int price_tot = 0;
    Region plots_handled;
    for (const auto& [i, row] : std::views::enumerate(grid)) {
        for (const auto& [j, type] : std::views::enumerate(row)) {
            const Position pos{static_cast<Int>(i), static_cast<Int>(j)};
            if (plots_handled.contains(pos)) {
                continue;
            }
            const auto region = connect(pos);
            plots_handled.insert(region.begin(), region.end());
            price_tot += region.size() * get_perimeter(region);
        }
    }
    return price_tot;
}

// Solution to part two
Int solve_two(const Grid& grid) {
    static constexpr std::array<Position, 4> steps{{{-1, 0}, {+1, 0}, {0, -1}, {0, +1}}};
    const Position shape{static_cast<Int>(grid.size()), static_cast<Int>(grid[0].size())};
    auto connect = [&](Position pos) {
        char type = grid[pos[0]][pos[1]];
        Region region;
        Region visit{pos};
        while (not visit.empty()) {
            const auto& it = visit.begin();
            pos = *it;
            visit.erase(it);
            region.insert(pos);
            for (const auto& step : steps) {
                const Position pos_neighbor{pos[0] + step[0], pos[1] + step[1]};
                if (not(0 <= pos_neighbor[0] && pos_neighbor[0] < shape[0] &&
                        0 <= pos_neighbor[1] && pos_neighbor[1] < shape[1])) {
                    continue;
                }
                if (grid[pos_neighbor[0]][pos_neighbor[1]] != type) {
                    continue;
                }
                if (region.contains(pos_neighbor)) {
                    continue;
                }
                visit.insert(pos_neighbor);
            }
        }
        return region;
    };
    auto get_num_sides = [](const Region& region) {
        Region edges;
        for (const auto& pos : region) {
            for (const auto& step : steps) {
                const Position edge{2 * pos[0] + step[0], 2 * pos[1] + step[1]};
                if (edges.contains(edge)) {
                    edges.erase(edge);
                } else {
                    edges.insert(edge);
                }
            }
        }
        auto does_belong = [&](const Position& edge, int dim) {
            return region.contains(
                {(edge[0] + (dim == 1)) / 2, (edge[1] - (dim == 0)) / 2});
        };
        Int num_sides = 0;
        while (not edges.empty()) {
            const Position edge = *edges.begin();
            num_sides += 1;
            int dim = edge[0] & 1;
            int belongs = does_belong(edge, dim);
            for (const Int step : {-2, +2}) {
                Position edge_neighbor = edge;
                do {
                    edges.erase(edge_neighbor);
                    edge_neighbor[dim] += step;
                } while (edges.contains(edge_neighbor) and
                         does_belong(edge_neighbor, dim) == belongs);
            }
        }
        return num_sides;
    };
    Int price_tot = 0;
    Region plots_handled;
    for (const auto& [i, row] : std::views::enumerate(grid)) {
        for (const auto& [j, type] : std::views::enumerate(row)) {
            const Position pos{static_cast<Int>(i), static_cast<Int>(j)};
            if (plots_handled.contains(pos)) {
                continue;
            }
            const Region region = connect(pos);
            plots_handled.insert(region.begin(), region.end());
            price_tot += region.size() * get_num_sides(region);
        }
    }
    return price_tot;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
