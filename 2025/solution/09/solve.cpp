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
    Data data{};
    std::ifstream file{"input"};
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

// Solution to part two
Int solve_two(const Data& data) {
    struct Stripe {
        Int fixed;
        Int min;
        Int max;  // inclusive
    };
    std::array<std::vector<Stripe>, 2> invalid_stripes;  // vertical and horizontal
    // Iterate over the polygon perimeter.
    // It is assumed that the data is provided ordered
    // and in counterclockwise direction.
    auto edges = std::views::concat(data, data | std::views::take(2)) |
                 std::views::adjacent<3>;  // loop in triplets, for turn information
    for (const auto& [coord0, coord1, coord2] : edges) {
        // Previous edge
        Int dx_prev = coord1[0] - coord0[0];
        Int dy_prev = coord1[1] - coord0[1];
        // Current edge
        Int dx_next = coord2[0] - coord1[0];
        Int dy_next = coord2[1] - coord1[1];
        if (dx_next != 0 && dy_next != 0) {
            throw std::runtime_error("Input data not axis-aligned");
        }
        if (dx_next == 0 && dy_next == 0) {
            continue;
        }
        // Perpendicular direction, pointing away from polygon (CCW assumes)
        std::array<Int, 2> perp{
            +aoc::sign(dy_next),
            -aoc::sign(dx_next),
        };
        // Determine which dimension is fixed
        std::size_t fixed_dim = (perp[0] == 0);
        std::size_t range_dim = !fixed_dim;
        Int fixed = coord1[fixed_dim] + perp[fixed_dim];
        // Insert invalid stripe
        invalid_stripes[fixed_dim].emplace_back(fixed, coord1[range_dim], coord2[range_dim]);
        // Fixup for internal corners (right turns)
        Int cross = dx_prev * dy_next - dy_prev * dx_next;
        if (cross < 0) {
            Stripe& curr = invalid_stripes[fixed_dim].back();
            curr.min += aoc::sign(curr.max - curr.min);
            if (!invalid_stripes[range_dim].empty()) {
                Stripe& prev = invalid_stripes[range_dim].back();
                prev.max += aoc::sign(prev.min - prev.max);
            }
        }
    }
    // Ensure all stripes have min <= max
    auto normalize_stripes = [](std::vector<Stripe>& stripes) {
        for (Stripe& stripe : stripes) {
            if (stripe.max < stripe.min) {
                std::swap(stripe.min, stripe.max);
            }
        }
    };
    for (auto& invalid_stripe : invalid_stripes) {
        normalize_stripes(invalid_stripe);
    }
    // Check all possible rectangles
    auto intersects_stripes = [](const std::vector<Stripe>& stripes, Int fixed, Int min, Int max) {
        for (const Stripe& stripe : stripes) {
            if (stripe.fixed < min || stripe.fixed > max) {
                continue;
            }
            if (stripe.min <= fixed && fixed <= stripe.max) {
                return true;
            }
        }
        return false;
    };
    return std::ranges::max([&]() -> std::generator<Int> {
        for (std::size_t i{}; i < data.size(); i++) {
            auto [xi, yi] = data[i];
            for (std::size_t j{i + 1}; j < data.size(); j++) {
                auto [xj, yj] = data[j];
                Int x_min = std::min(xi, xj);
                Int x_max = std::max(xi, xj);
                Int y_min = std::min(yi, yj);
                Int y_max = std::max(yi, yj);
                bool invalid = (intersects_stripes(invalid_stripes[0], y_min, x_min, x_max)     // bottom side
                                || intersects_stripes(invalid_stripes[0], y_max, x_min, x_max)  // top side
                                || intersects_stripes(invalid_stripes[1], x_min, y_min, y_max)  // left side
                                || intersects_stripes(invalid_stripes[1], x_max, y_min, y_max)  // right side
                );
                if (!invalid) {
                    Int width = (x_max - x_min) + 1;
                    Int hight = (y_max - y_min) + 1;
                    co_yield width* hight;
                }
            }
        }
    }());
}

// Solve
int main() {
    aoc::analyze(read, solve_one);
    aoc::analyze(read, solve_two);
}
