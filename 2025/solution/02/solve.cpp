#include <charconv>
#include <cstddef>
#include <cstdint>
#include <format>
#include <fstream>
#include <ranges>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "helper.hpp"

using Int = std::uint64_t;
using Range = std::pair<Int, Int>;
using Data = std::vector<Range>;

// Reading in data
Data read() {
    Data ranges{};
    std::ifstream file{"input"};
    std::string line;
    std::getline(file, line);
    for (auto&& comma_split : std::views::split(line, ',')) {
        std::string token{comma_split.begin(), comma_split.end()};
        auto dash = token.find('-');
        Int bgn{};
        Int end{};
        std::from_chars(token.data(), token.data() + dash, bgn);
        std::from_chars(token.data() + dash + 1, token.data() + token.size(), end);
        ranges.emplace_back(bgn, end);
    }
    return ranges;
}

// Solution to part one
Int solve_one(const Data& ranges) {
    Int sum{};
    for (const Range& range : ranges) {
        for (Int id = range.first; id <= range.second; id++) {
            std::string str = std::to_string(id);
            if (str.substr(0, str.size() / 2) == str.substr(str.size() / 2)) {
                sum += id;
            }
        }
    }
    return sum;
}

// Solution to part two
Int solve_two(const Data& ranges) {
    Int sum{};
    for (const Range& range : ranges) {
        for (Int id = range.first; id <= range.second; id++) {
            std::string str = std::to_string(id);
            for (std::size_t i = 1; i <= str.size() / 2; i++) {
                if (str.size() % i) {
                    continue;
                }
                if (std::regex_match(str, std::regex{std::format("({})+", str.substr(0, i))})) {
                    sum += id;
                    break;
                }
            }
        }
    }
    return sum;
}

// Solve
int main() {
    helper::analyze(read, solve_one);
    helper::analyze(read, solve_two);
}
