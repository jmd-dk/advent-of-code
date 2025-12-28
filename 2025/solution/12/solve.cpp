#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <random>
#include <ranges>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "helper.hpp"

namespace day12 {

using Int = std::int32_t;
using Coord = std::array<Int, 2>;
using Coords = std::unordered_set<Coord, aoc::ArrayHash>;
struct Shape {
    std::array<Int, 2> bbox{};
    Coords coords{};
    explicit Shape(const auto& _coords) {
        // Shift to positive quadrant
        std::array<Int, 2> min{std::numeric_limits<Int>::max(), std::numeric_limits<Int>::max()};
        for (const Coord& coord : _coords) {
            for (std::size_t dim{}; dim < 2; dim++) {
                min[dim] = std::ranges::min(min[dim], coord[dim]);
            }
        }
        for (const Coord& coord_ : _coords) {
            Coord coord{coord_};
            for (std::size_t dim{}; dim < 2; dim++) {
                coord[dim] -= min[dim];
            }
            coords.insert(coord);
        }
        // Compute bounding box
        for (const Coord& coord : coords) {
            for (std::size_t dim{}; dim < 2; dim++) {
                bbox[dim] = std::ranges::max(bbox[dim], coord[dim] + 1);
            }
        }
    }
    constexpr auto operator<=>(Shape const&) const = default;
};
struct Region {
    std::array<Int, 2> bbox{};
    std::vector<std::size_t> quantities{};
};
using Data = std::pair<std::vector<Shape>, std::vector<Region>>;

// Reading in data
Data read() {
    std::vector<Shape> shapes{};
    std::vector<Region> regions{};
    std::ifstream file{"input"};
    std::string line{};
    Coords coords_shape{};
    Int row_shape{};
    while (std::getline(file, line)) {
        if (line.empty()) {
            shapes.emplace_back(std::move(coords_shape));
            coords_shape.clear();
            row_shape = 0;
        } else if (std::ranges::contains(line, '#') || std::ranges::contains(line, '.')) {
            for (const auto [col_shape, chr] : std::views::enumerate(line)) {
                if (chr == '#') {
                    coords_shape.insert({row_shape, static_cast<Int>(col_shape)});
                }
            }
            row_shape++;
        } else if (std::ranges::contains(line, 'x')) {
            auto colon = line.find(':');
            std::string wh_string{line.substr(0, colon)};
            auto x = wh_string.find('x');
            std::string width{wh_string.substr(0, x)};
            std::string hight{wh_string.substr(x + 1)};
            Region region{};
            std::from_chars(width.data(), width.data() + width.size(), region.bbox[0]);
            std::from_chars(hight.data(), hight.data() + hight.size(), region.bbox[1]);
            region.quantities = line.substr(colon + 1) | std::views::split(' ') |
                                std::views::filter([](auto&& part) { return !part.empty(); }) |
                                std::views::transform([](auto&& part) {
                                    std::string str = std::ranges::to<std::string>(part);
                                    std::size_t value{};
                                    std::from_chars(str.data(), str.data() + str.size(), value);
                                    return value;
                                }) |
                                std::ranges::to<std::vector<std::size_t>>();
            regions.push_back(std::move(region));
        }
    }
    return {shapes, regions};
}

// Solution to part one
Int solve_one(const Data& data) {
    const auto& [shapes, regions] = data;
    constexpr std::uint32_t num_attempts{1};
    // Construct all distinct isometries of all shapes
    auto construct_isometries = [](Shape shape) {
        std::vector<Shape> isometries{};
        auto flip = [](const Shape& shp) {
            Coords coords_flipped{};
            for (const Coord& coord : shp.coords) {
                constexpr std::size_t dim{0};  // flip axis does not matter
                Coord coord_flipped{coord};
                coord_flipped[dim] *= -1;
                coords_flipped.insert(coord_flipped);
            }
            return Shape(coords_flipped);
        };
        auto rotate = [](const Shape& shp) {
            Coords coords_rotated{};
            for (const Coord& coord : shp.coords) {
                Coord coord_rotated{-coord[1], coord[0]};  // rotation orientation does not matter
                coords_rotated.insert(coord_rotated);
            }
            return Shape(coords_rotated);
        };
        for (int parity{}; parity < 2; parity++) {
            if (parity) {
                shape = flip(shape);
            }
            for (int rot{}; rot < 4; rot++) {
                if (rot) {
                    shape = rotate(shape);
                }
                if (!std::ranges::contains(isometries, shape)) {
                    isometries.push_back(shape);
                }
            }
        }
        return isometries;
    };
    const auto all_isometries = shapes | std::views::transform(construct_isometries) |
                                std::ranges::to<std::vector<std::vector<Shape>>>();
    // Strategy: For each region, attempt a placement of the shapes, packing the shapes
    // as tightly as possible, starting in one corner and working out way outwards.
    // The order or the shapes are chosen at random, and so are which isometry to use
    // for each shape. We repeat such an attempt a given number of times, assuming
    // the packing to be impossible if no solution is found.
    auto attempt_placement = [&](const Region& region, std::uint32_t seed) {
        // Build sequnce of shape indices required for this region
        std::vector<std::size_t> indices_shapes{};
        for (auto [index, quantity] : std::views::enumerate(region.quantities)) {
            indices_shapes.insert(indices_shapes.end(), quantity, static_cast<std::size_t>(index));
        }
        // Scramble the ordering
        std::seed_seq seq{seed};
        std::mt19937_64 prng{seq};
        std::ranges::shuffle(indices_shapes, prng);
        // Attempt the placement
        Coords board{};
        auto check = [&](const Coords& coords) {
            for (const Coord& coord : coords) {
                if (board.contains(coord)) {
                    return false;
                }
            }
            return true;
        };
        auto place = [&](const Coords& coords) {
            for (const Coord& coord : coords) {
                board.insert(coord);
            }
        };
        for (std::size_t index_shape : indices_shapes) {
            const std::vector<Shape>& isometries = all_isometries[index_shape];
            std::uniform_int_distribution<std::size_t> dist(0, isometries.size() - 1);
            std::size_t index_isometry = dist(prng);
            const Shape& shape = isometries[index_isometry];
            for (Int di{}; di <= region.bbox[0] - shape.bbox[0]; di++) {
                for (Int dj{}; dj <= region.bbox[1] - shape.bbox[1]; dj++) {
                    Coords coords{};
                    for (const Coord& coord : shape.coords) {
                        coords.insert({coord[0] + di, coord[1] + dj});
                    }
                    if (check(coords)) {
                        place(coords);
                        goto placed;
                    }
                }
            }
            return false;
        placed:
        }
        return true;
    };
    auto perform_attempts = [&](const Region& region) {
        for (std::uint32_t seed{}; seed < num_attempts; seed++) {
            if (attempt_placement(region, seed)) {
                return true;
            }
        }
        return false;
    };
    return aoc::sum(regions | std::views::transform(perform_attempts));
}

}  // namespace day12

// Solve
int main() { aoc::analyze(day12::read, day12::solve_one); }
