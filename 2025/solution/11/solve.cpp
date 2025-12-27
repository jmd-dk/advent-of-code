#include <array>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "helper.hpp"

namespace day11 {

using Int = std::int64_t;
using Graph = std::unordered_map<std::string, std::unordered_set<std::string>>;

// Reading in data
Graph read() {
    Graph graph{};
    std::ifstream file{"input"};
    std::string line{};
    while (std::getline(file, line)) {
        auto colon = line.find(':');
        std::string device{line.substr(0, colon)};
        graph[device].insert_range(
            line.substr(colon + 1) | std::views::split(' ') |
            std::views::filter([](auto&& part) { return !part.empty(); }) |
            std::ranges::to<std::unordered_set<std::string>>()
        );
    }
    return graph;
}

// Solution to part one
Int solve_one(const Graph& graph) {
    // We assume that the directed graph is acyclic (DAG)
    const std::string bgn{"you"};
    const std::string end{"out"};
    std::unordered_map<std::string, Int> counts{};
    auto count_paths = [&](this const auto& self, const std::string& device) {
        if (device == end) {
            return Int{1};
        }
        if (auto count = aoc::lookup(counts, device)) {
            return *count;
        }
        Int count{};
        if (const auto output_devices = aoc::lookup(graph, device)) {
            for (const std::string& output_device : *output_devices) {
                count += self(output_device);
            }
        }
        return counts[device] = count;
    };
    return count_paths(bgn);
}

// Solution to part two
Int solve_two(const Graph& graph) {
    // We assume that the directed graph is acyclic (DAG)
    const std::string bgn{"svr"};
    const std::string end{"out"};
    const std::array musthaves{"dac", "fft"};
    using Mask = std::uint8_t;
    Mask mask_full{(1 << musthaves.size()) - 1};
    std::unordered_map<std::string, std::unordered_map<Mask, Int>> counts{};
    auto count_paths = [&](this const auto& self, const std::string& device, Mask mask = {}) {
        for (const auto [i, musthave] : std::views::enumerate(musthaves)) {
            if (device == musthave) {
                mask |= 1 << i;
                break;
            }
        }
        if (device == end) {
            return Int{mask == mask_full};
        }
        if (auto mask_map = aoc::lookup(counts, device)) {
            if (auto count = aoc::lookup(*mask_map, mask)) {
                return *count;
            }
        }
        Int count{};
        if (const auto output_devices = aoc::lookup(graph, device)) {
            for (const std::string& output_device : *output_devices) {
                count += self(output_device, mask);
            }
        }
        return counts[device][mask] = count;
    };
    return count_paths(bgn);
}

}  // namespace day11

// Solve
int main() {
    aoc::analyze(day11::read, day11::solve_one);
    aoc::analyze(day11::read, day11::solve_two);
}
