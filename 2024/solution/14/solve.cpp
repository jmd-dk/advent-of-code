#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <print>
#include <regex>
#include <sstream>
#include <vector>

#include "../../magic.h"

using Int = std::int32_t;
struct Robot {
    std::array<Int, 2> pos;
    std::array<Int, 2> vel;
};

// Reading in data
std::vector<Robot> read() {
    static const std::regex pattern(R"(p=(\d+),(\d+) v=(-?\d+),(-?\d+))");
    std::vector<Robot> robots;
    for (const std::string& line : LineReader("input")) {
        std::smatch match;
        if (not std::regex_match(line, match, pattern)) {
            continue;
        }
        Robot robot;
        std::istringstream(match[1].str()) >> robot.pos[0];
        std::istringstream(match[2].str()) >> robot.pos[1];
        std::istringstream(match[3].str()) >> robot.vel[0];
        std::istringstream(match[4].str()) >> robot.vel[1];
        robots.push_back(robot);
    }
    return robots;
}

// Solution to part one
constexpr std::array<Int, 2> space{101, 103};
Int solve_one(std::vector<Robot>& robots) {
    constexpr Int time = 100;
    for (Robot& robot : robots) {
        for (auto dim : {0, 1}) {
            robot.pos[dim] = (robot.pos[dim] + time * robot.vel[dim]) % space[dim];
            if (robot.pos[dim] < 0) {
                robot.pos[dim] += space[dim];
            }
        }
    }
    std::array<std::array<Int, 2>, 2> quadrants{};
    for (const Robot& robot : robots) {
        if (robot.pos[0] == space[0] / 2 || robot.pos[1] == space[1] / 2) {
            continue;
        }
        quadrants[robot.pos[0] > space[0] / 2][robot.pos[1] > space[1] / 2] += 1;
    }
    Int safety_factor = 1;
    for (auto i : {0, 1}) {
        for (auto j : {0, 1}) {
            safety_factor *= quadrants[i][j];
        }
    }
    return safety_factor;
}

// Solution to part two
Int solve_two(std::vector<Robot>& robots) {
    constexpr Int time_max = 10'000;
    constexpr Int chunk_size = std::min(space[0], space[1]) / 10;
    using Grid = std::array<std::array<Int, space[0]>, space[1]>;
    auto compute_entropy = [&](const std::vector<Robot>& robots, Grid& grid,
                               const Int chunk_size) {
        for (auto& row : grid) {
            row.fill(0);
        }
        for (const Robot& robot : robots) {
            grid[robot.pos[1]][robot.pos[0]] += 1;
        }
        Int num_chunks = (space[0] / chunk_size) * (space[1] / chunk_size);
        double entropy = 0;
        for (Int j_bgn = 0; j_bgn < space[1]; j_bgn += chunk_size) {
            Int j_end = std::min(j_bgn + chunk_size, space[1]);
            for (Int i_bgn = 0; i_bgn < space[0]; i_bgn += chunk_size) {
                Int i_end = std::min(i_bgn + chunk_size, space[0]);
                Int count = 0;
                for (Int j = j_bgn; j < j_end; j++) {
                    for (Int i = i_bgn; i < i_end; i++) {
                        count += grid[j][i];
                    }
                }
                if (count == 0) {
                    continue;
                }
                double p = static_cast<double>(count) / num_chunks;
                entropy -= p * std::log2(p);
            }
        }
        return entropy;
    };
    auto print_grid = [](const Grid& grid) {
        for (const auto& row : grid) {
            std::ostringstream stream;
            for (Int n : row) {
                if (n == 0) {
                    stream << " ";
                } else if (n >= 10) {
                    stream << "+";
                } else {
                    stream << n;
                }
            }
            std::print("{}\n", stream.str());
        }
    };
    auto evolve = [](std::vector<Robot>& robots) {
        for (Robot& robot : robots) {
            for (auto dim : {0, 1}) {
                robot.pos[dim] = (robot.pos[dim] + robot.vel[dim]) % space[dim];
                if (robot.pos[dim] < 0) {
                    robot.pos[dim] += space[dim];
                }
            }
        }
    };
    Grid grid, grid_special;
    Int time_special = 0;
    double entropy_min = compute_entropy(robots, grid, chunk_size);
    for (Int time = 1; time < time_max; time++) {
        evolve(robots);
        double entropy = compute_entropy(robots, grid, chunk_size);
        if (entropy < entropy_min) {
            time_special = time;
            entropy_min = entropy;
            grid_special = grid;
        }
    }
    print_grid(grid_special);
    return time_special;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
