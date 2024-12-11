#include <array>
#include <cstdint>
#include <queue>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "../../magic.h"

using Int = std::int32_t;
using Grid = std::vector<std::vector<std::uint8_t>>;

// Reading in data
Grid read() {
    Grid grid;
    for (const auto& line : LineReader("input")) {
        grid.emplace_back(line.begin(), line.end());
        for (auto& c : grid.back()) {
            c -= '0';
        }
    }
    return grid;
}

// Solution to part one
struct Values {
    static constexpr Int Sentinel = -1;
    static constexpr Int Begin = 0;
    static constexpr Int End = 9;
};
Int solve_one(const Grid& grid) {
    const std::array<Int, 2> shape{static_cast<Int>(grid.size()),
                                   static_cast<Int>(grid[0].size())};
    auto next_trailhead = [&](const Int& i_bgn = 0,
                              const Int& j_bgn = -1) -> std::array<Int, 2> {
        for (Int i = i_bgn; i < shape[0]; i++) {
            for (Int j = (i == i_bgn ? j_bgn + 1 : 0); j < shape[1]; j++) {
                if (grid[i][j] == Values::Begin) {
                    return {i, j};
                }
            }
        }
        return {Values::Sentinel, Values::Sentinel};
    };
    auto look_around = [&](const Int& i_bgn, const Int& j_bgn, auto& visited,
                           auto& reachable) {
        static constexpr std::array<std::array<Int, 2>, 4> steps{
            {{-1, 0}, {+1, 0}, {0, -1}, {0, +1}}};
        const auto height = grid[i_bgn][j_bgn];
        for (const auto& [di, dj] : steps) {
            const Int i = i_bgn + di;
            const Int j = j_bgn + dj;
            if (not(0 <= i and i < shape[0] and 0 <= j and j < shape[1])) {
                continue;
            }
            if (grid[i][j] != height + 1) {
                continue;
            }
            if (visited.contains({i, j})) {
                continue;
            }
            reachable.insert({i, j});
        }
    };
    auto explore = [&](const Int& i_bgn, const Int& j_bgn) {
        std::unordered_set<std::array<Int, 2>, ArrayHash<Int>> visited;
        std::unordered_set<std::array<Int, 2>, ArrayHash<Int>> reachable{{i_bgn, j_bgn}};
        Int score = 0;
        while (not reachable.empty()) {
            const auto& it = reachable.begin();
            const auto [i, j] = *it;
            reachable.erase(it);
            visited.insert({i, j});
            if (grid[i][j] == Values::End) {
                score += 1;
                continue;
            }
            look_around(i, j, visited, reachable);
        }
        return score;
    };
    Int score_tot = 0;
    auto [i_bgn, j_bgn] = next_trailhead();
    while (i_bgn != Values::Sentinel) {
        score_tot += explore(i_bgn, j_bgn);
        std::tie(i_bgn, j_bgn) = next_trailhead(i_bgn, j_bgn);
    }
    return score_tot;
}

// Solution to part two
Int solve_two(const Grid& grid) {
    const std::array<Int, 2> shape{static_cast<Int>(grid.size()),
                                   static_cast<Int>(grid[0].size())};
    auto next_trailhead = [&](const Int& i_bgn = 0,
                              const Int& j_bgn = -1) -> std::array<Int, 2> {
        for (Int i = i_bgn; i < shape[0]; i++) {
            for (Int j = (i == i_bgn ? j_bgn + 1 : 0); j < shape[1]; j++) {
                if (grid[i][j] == Values::Begin) {
                    return {i, j};
                }
            }
        }
        return {Values::Sentinel, Values::Sentinel};
    };
    auto look_around = [&](const Int& i_bgn, const Int& j_bgn, auto& counts, auto& tovisit,
                           auto& tovisit_ever) {
        static constexpr std::array<std::array<Int, 2>, 4> steps{
            {{-1, 0}, {+1, 0}, {0, -1}, {0, +1}}};
        const auto height = grid[i_bgn][j_bgn];
        for (const auto& [di, dj] : steps) {
            const Int i = i_bgn + di;
            const Int j = j_bgn + dj;
            if (not(0 <= i and i < shape[0] and 0 <= j and j < shape[1])) {
                continue;
            }
            if (grid[i][j] != height + 1) {
                continue;
            }
            counts[i][j] += counts[i_bgn][j_bgn];
            if (tovisit_ever.contains({i, j})) {
                continue;
            }
            tovisit.push({i, j});
            tovisit_ever.insert({i, j});
        }
    };
    auto explore = [&](const Int& i_bgn, const Int& j_bgn) {
        std::unordered_set<std::array<Int, 2>, ArrayHash<Int>> tovisit_ever{{i_bgn, j_bgn}};
        std::vector<std::vector<Int>> counts(shape[0], std::vector<Int>(shape[1], 0));
        counts[i_bgn][j_bgn] = 1;
        std::queue<std::array<Int, 2>> tovisit{{{i_bgn, j_bgn}}};
        std::unordered_set<std::array<Int, 2>, ArrayHash<Int>> ends;
        while (not tovisit.empty()) {
            const auto [i, j] = tovisit.front();
            tovisit.pop();
            if (grid[i][j] == Values::End) {
                ends.insert({i, j});
                continue;
            }
            look_around(i, j, counts, tovisit, tovisit_ever);
        }
        Int rating = 0;
        for (const auto& [i, j] : ends) {
            rating += counts[i][j];
        }
        return rating;
    };
    Int rating_tot = 0;
    auto [i_bgn, j_bgn] = next_trailhead();
    while (i_bgn != Values::Sentinel) {
        rating_tot += explore(i_bgn, j_bgn);
        std::tie(i_bgn, j_bgn) = next_trailhead(i_bgn, j_bgn);
    }
    return rating_tot;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
