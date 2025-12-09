#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <generator>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "helper.hpp"

using Int = std::int64_t;
using Coord = std::array<Int, 2>;
using Data = std::vector<Coord>;

// Reading in data
Data read() {
    Data data;
    std::ifstream file{"example"};
    std::string line{};
    while (std::getline(file, line)) {
        Coord coord{};
        std::size_t i{};
        for (auto part : std::views::split(line, ',')) {
            std::string_view token{part.begin(), part.end()};
            Int value{};
            std::from_chars(token.data(), token.data() + token.size(), value);
            coord[i++] = value;
        }
        data.push_back(coord);
    }
    return data;
}

// Solution to part one
Int solve_one(Data data) {
    // Find Pareto frontier for each quadrant
    auto cmp_ne = [](const Coord& coord_a, const Coord& coord_b) {
        auto [xa, ya] = coord_a;
        auto [xb, yb] = coord_b;
        return (xa > xb) || (xa == xb && ya > yb);
    };
    auto cmp_nw = [](const Coord& coord_a, const Coord& coord_b) {
        auto [xa, ya] = coord_a;
        auto [xb, yb] = coord_b;
        return (xa < xb) || (xa == xb && ya > yb);
    };
    auto cmp_sw = [](const Coord& coord_a, const Coord& coord_b) {
        auto [xa, ya] = coord_a;
        auto [xb, yb] = coord_b;
        return (xa < xb) || (xa == xb && ya < yb);
    };
    auto cmp_se = [](const Coord& coord_a, const Coord& coord_b) {
        auto [xa, ya] = coord_a;
        auto [xb, yb] = coord_b;
        return (xa > xb) || (xa == xb && ya < yb);
    };
    auto cmp_y_n = [](Int y_a, Int y_b) { return y_a > y_b; };
    auto cmp_y_s = [](Int y_a, Int y_b) { return y_a < y_b; };
    auto build_frontier = [&](auto cmp, auto cmp_y) {
        std::ranges::sort(data, cmp);
        std::vector<Coord> frontier{data.front()};
        Int y_extreme{frontier.back()[1]};
        for (const Coord& coord : data | std::views::drop(1)) {
            if (cmp_y(coord[1], y_extreme)) {
                frontier.push_back(coord);
                y_extreme = coord[1];
            }
        }
        return frontier;
    };
    std::vector<Coord> frontier_ne = build_frontier(cmp_ne, cmp_y_n);
    std::vector<Coord> frontier_nw = build_frontier(cmp_nw, cmp_y_n);
    std::vector<Coord> frontier_sw = build_frontier(cmp_sw, cmp_y_s);
    std::vector<Coord> frontier_se = build_frontier(cmp_se, cmp_y_s);
    // Pair up diagonal quadrants
    auto compute_rect_area = [](const Coord& coord_a, const Coord& coord_b) {
        Int width = std::abs(coord_a[0] - coord_b[0]) + 1;
        Int hight = std::abs(coord_a[1] - coord_b[1]) + 1;
        return width * hight;
    };
    auto find_max_rect_area = [&](std::span<const Coord> frontier_a, std::span<const Coord> frontier_b) {
        return std::ranges::max([&]() -> std::generator<Int> {
            for (const Coord& coord_a : frontier_a) {
                for (const Coord& coord_b : frontier_b) {
                    co_yield compute_rect_area(coord_a, coord_b);
                }
            }
        }());
    };
    Int max_rect_area_ne_sw = find_max_rect_area(frontier_ne, frontier_sw);
    Int max_rect_area_nw_se = find_max_rect_area(frontier_nw, frontier_se);
    return std::max(max_rect_area_ne_sw, max_rect_area_nw_se);
}

// Solve
int main() { helper::analyze(read, solve_one); }
