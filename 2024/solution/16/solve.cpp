#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <limits>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../magic.hpp"

using Int = std::int32_t;
using Position = std::array<Int, 2>;
using Map = std::unordered_set<Position, ArrayHash<Int>>;
using Data = std::tuple<Map, Position, Position>;

// Reading in data
Data read() {
    Map walls;
    Position start{-1, -1};
    Position end{-1, -1};
    Int i = -1;
    for (const auto& line : LineReader("input")) {
        i++;
        Int j = -1;
        for (const char c : line) {
            j++;
            switch (c) {
                case '#':
                    walls.insert({i, j});
                    break;
                case 'S':
                    start = {i, j};
                    break;
                case 'E':
                    end = {i, j};
                    break;
            }
        }
    }
    return {walls, start, end};
}

// Solution to part one
struct State {
    Position pos{};
    std::uint8_t dir{};
    struct Costs {
        static constexpr Int Step = 1;
        static constexpr Int Turn = 1'000;
        Costs() = delete;
    };
    std::array<std::tuple<Int, State>, 3> advance() {
        static constexpr std::array<Position, 4> directions{
            {{0, +1}, {-1, 0}, {0, -1}, {+1, 0}}};
        auto vel = directions[dir];
        return {{{Costs::Step, {{pos[0] + vel[0], pos[1] + vel[1]}, dir}},
                 {Costs::Turn,
                  {pos, static_cast<decltype(dir)>((dir - 1 + directions.size()) %
                                                   directions.size())}},
                 {Costs::Turn,
                  {pos, static_cast<decltype(dir)>((dir + 1) % directions.size())}}}};
    }
    auto operator<=>(const State&) const = default;
};
struct StateHash {
    std::size_t operator()(const State& state) const {
        return ArrayHash<Int>{}(state.pos) ^ std::hash<std::uint8_t>{}(state.dir);
    }
};
Int solve_one(const Data& data) {
    const auto& [walls, start, end] = data;
    std::priority_queue<std::tuple<Int, State>, std::vector<std::tuple<Int, State>>,
                        std::greater<std::tuple<Int, State>>>
        visit;
    visit.push({0, State{start}});
    std::unordered_set<State, StateHash> visited;
    while (not visit.empty()) {
        auto [score, state] = visit.top();
        visit.pop();
        if (state.pos == end) {
            return score;
        }
        visited.insert(state);
        for (auto [cost, state_neighbor] : state.advance()) {
            if (not walls.contains(state_neighbor.pos) and
                not visited.contains(state_neighbor)) {
                visit.push({score + cost, state_neighbor});
            }
        }
    }
    return -1;
}

// Solution to part two
Int solve_two(const Data& data) {
    const auto& [walls, start, end] = data;
    // Find all best path solutions
    std::priority_queue<std::tuple<Int, State>, std::vector<std::tuple<Int, State>>,
                        std::greater<std::tuple<Int, State>>>
        visit;
    visit.push({0, State{start}});
    std::unordered_map<State, std::tuple<Int, std::unordered_set<State, StateHash>>,
                       StateHash>
        visited;
    Int score_end = std::numeric_limits<Int>::max();
    while (not visit.empty()) {
        auto [score, state] = visit.top();
        visit.pop();
        if (state.pos == end) {
            score_end = score;
            break;
        }
        for (auto [cost, state_neighbor] : state.advance()) {
            if (walls.contains(state_neighbor.pos)) {
                continue;
            }
            Int score_next = score + cost;
            if (score_next > score_end) {
                continue;
            }
            auto it = visited.find(state_neighbor);
            if (it != visited.end()) {
                auto& [_, values] = *it;
                auto& [score_prev, states_prev] = values;
                if (score_prev < score_next) {
                    continue;
                } else if (score_prev == score_next) {
                    states_prev.insert(state);
                } else {
                    visited[state_neighbor] = {score_next, {state}};
                }
            } else {
                visited[state_neighbor] = {score_next, {state}};
            }
            visit.push({score_next, state_neighbor});
        }
    }
    // Count number of tiles in best paths
    std::deque<State> backtrack{{end, 0}, {end, 1}, {end, 2}, {end, 3}};
    std::unordered_set<State, StateHash> backtracked;
    std::unordered_set<Position, ArrayHash<Int>> positions;
    while (not backtrack.empty()) {
        State state = backtrack[0];
        backtrack.pop_front();
        backtracked.insert(state);
        positions.insert(state.pos);
        auto it = visited.find(state);
        if (it == visited.end()) {
            continue;
        }
        auto& [_, values] = *it;
        auto& [__, states_prev] = values;
        for (State state_prev : states_prev) {
            if (not backtracked.contains(state_prev)) {
                backtrack.push_back(state_prev);
            }
        }
    }
    return positions.size();
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
