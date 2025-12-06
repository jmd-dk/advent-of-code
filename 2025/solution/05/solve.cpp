#include <algorithm>
#include <charconv>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <utility>
#include <vector>

#include "helper.hpp"

using Int = std::int64_t;
struct Range {
    Int bgn{};
    Int end{};

    constexpr auto operator<=>(Range const&) const = default;
};
using Data = std::pair<std::vector<Range>, std::vector<Int>>;

// Reading in data
Data read() {
    std::ifstream file{"input"};
    std::string line{};
    std::vector<Range> ranges{};
    while (std::getline(file, line)) {
        if (line.empty()) {
            break;
        }
        auto dash = line.find('-');
        Int bgn{};
        Int end{};
        std::from_chars(line.data(), line.data() + dash, bgn);
        std::from_chars(line.data() + dash + 1, line.data() + line.size(), end);
        ranges.push_back({bgn, end + 1});  // exclusive end
    }
    std::vector<Int> ids{};
    while (std::getline(file, line)) {
        Int id{};
        std::from_chars(line.data(), line.data() + line.size(), id);
        ids.push_back(id);
    }
    return {ranges, ids};
}

// Solution to part one
Int solve_one(const Data& data) {
    const auto& [ranges, ids] = data;
    auto is_fresh = [&](Int id) {
        for (const Range& range : ranges) {
            if (range.bgn <= id and id < range.end) {
                return true;
            }
        }
        return false;
    };
    Int num_fresh{};
    for (Int id : ids) {
        num_fresh += is_fresh(id);
    }
    return num_fresh;
}

// Solution to part two
Int solve_two(Data data) {
    auto& [ranges, _] = data;
    std::ranges::sort(ranges);
    Int num_fresh{};
    Range range_prev{};
    for (const Range& range : ranges) {
        if (Int extra{range.end - std::max(range.bgn, range_prev.end)}; extra > 0) {
            num_fresh += extra;
            range_prev = range;
        }
    }
    return num_fresh;
}

// Solve
int main() {
    helper::analyze(read, solve_one);
    helper::analyze(read, solve_two);
}
