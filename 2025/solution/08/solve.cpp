#include <algorithm>
#include <array>
#include <cassert>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "helper.hpp"

using Int = std::int64_t;
using Coord = std::array<Int, 3>;
using Data = std::vector<Coord>;

// Reading in data
Data read() {
    Data data;
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

// Data structure used within solutions
struct CoordPair {
    Int dist2{};
    std::size_t i{};
    std::size_t j{};

    CoordPair() = default;
    CoordPair(std::size_t i_, std::size_t j_, std::span<const Coord> data) : i(i_), j(j_) {
        for (auto [value_i, value_j] : std::views::zip(data[i], data[j])) {
            dist2 += helper::ipow(value_j - value_i, 2);
        }
    }

    constexpr auto operator<=>(CoordPair const&) const = default;
};

// Solution to part one
std::size_t solve_one(const Data& data) {
    constexpr std::size_t num_connections = 1'000;
    constexpr std::size_t num_largest = 3;
    // Heapify all coordinate pairs over their (squared) distances
    helper::Heap<CoordPair> heap(num_connections);  // max-heap
    for (std::size_t i = 0; i < data.size(); i++) {
        for (std::size_t j = i + 1; j < data.size(); j++) {
            heap.emplace(i, j, data);
        }
    }
    // Count disjoint components using DSU
    helper::DSU dsu(data.size());
    for (const CoordPair& coord_pair : heap.data()) {
        dsu.unite(coord_pair.i, coord_pair.j);
    }
    bool largest = true;
    return helper::prod(
        dsu.roots(num_largest, largest) | std::views::transform(std::bind_front(&helper::DSU::component_size, &dsu))
    );
}

// Solution to part two
Int solve_two(const Data& data) {
    // Compute all coordinate pairs, then sort them by (squared) distance
    std::vector<CoordPair> coord_pairs{};
    assert(!data.empty());
    coord_pairs.reserve(data.size() * (data.size() - 1) / 2);
    for (std::size_t i = 0; i < data.size(); i++) {
        for (std::size_t j = i + 1; j < data.size(); j++) {
            coord_pairs.emplace_back(i, j, data);
        }
    }
    std::ranges::sort(coord_pairs);
    // Build up DSU until network is complete
    helper::DSU dsu(data.size());
    CoordPair final_coord_pair{};
    for (const CoordPair& coord_pair : coord_pairs) {
        dsu.unite(coord_pair.i, coord_pair.j);
        if (dsu.component_count() == 1) {
            final_coord_pair = coord_pair;
            break;
        }
    }
    return data[final_coord_pair.i][0] * data[final_coord_pair.j][0];
}

// Solve
int main() {
    helper::analyze(read, solve_one);
    helper::analyze(read, solve_two);
}
