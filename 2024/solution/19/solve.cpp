#include <cstdint>
#include <functional>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../../magic.hpp"

using Int = std::int64_t;
using Data = std::tuple<std::vector<std::string>, std::vector<std::string>>;

// Reading in data
Data read() {
    std::vector<std::string> patterns;
    std::vector<std::string> designs;
    bool at_patterns = true;
    for (const std::string& line : LineReader("input")) {
        if (line.empty()) {
            at_patterns = false;
            continue;
        }
        if (at_patterns) {
            std::stringstream stream(line);
            std::string token;
            while (std::getline(stream, token, ',')) {
                token.erase(0, token.find_first_not_of(' '));
                patterns.push_back(token);
            }
        } else {
            designs.push_back(line);
        }
    }
    return {patterns, designs};
}

// Solution to part one
Int solve_one(const Data& data) {
    const auto& [patterns, designs] = data;
    std::function<bool(std::string)> matches;
    std::unordered_map<std::string, bool> cache{{"", true}};
    matches = [&](std::string design) {
        if (auto it = cache.find(design); it != cache.end()) {
            const auto& [_, value] = *it;
            return value;
        }
        for (const auto& pattern : patterns) {
            if (design.starts_with(pattern)) {
                if (matches(design.substr(pattern.size()))) {
                    cache[design] = true;
                    return true;
                }
            }
        }
        cache[design] = false;
        return false;
    };
    Int count = 0;
    for (const auto& design : designs) {
        count += matches(design);
    }
    return count;
}

// Solution to part two
Int solve_two(const Data& data) {
    const auto& [patterns, designs] = data;
    std::function<Int(std::string)> matches;
    std::unordered_map<std::string, Int> cache{{"", 1}};
    matches = [&](std::string design) -> Int {
        if (auto it = cache.find(design); it != cache.end()) {
            const auto& [_, value] = *it;
            return value;
        }
        Int count = 0;
        for (const auto& pattern : patterns) {
            if (design.starts_with(pattern)) {
                count += matches(design.substr(pattern.size()));
            }
        }
        cache[design] = count;
        return count;
    };
    Int count = 0;
    for (const auto& design : designs) {
        count += matches(design);
    }
    return count;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
