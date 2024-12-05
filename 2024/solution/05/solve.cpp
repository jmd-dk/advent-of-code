#include <array>
#include <cstdint>
#include <fstream>
#include <functional>
#include <ranges>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../magic.h"

using Int = std::int32_t;
using Rule = std::array<Int, 2>;
using Update = std::vector<Int>;
using Data = std::pair<std::vector<Rule>, std::vector<Update>>;
struct ArrayHash {
    const std::hash<Int> hasher;
    const std::size_t prime = 37;
    template <std::size_t N> std::size_t operator()(const std::array<Int, N>& arr) const {
        std::size_t hash = 0;
        for (const auto& elem : arr) {
            hash = (hash ^ hasher(elem)) * prime;
        }
        return hash;
    }
};

// Reading in data
Data read() {
    const std::string filename = "input";
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::vector<Rule> rules;
    static const std::regex pattern_rule(R"((\d+)\|(\d+))");
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            break;
        }
        std::smatch match;
        if (std::regex_match(line, match, pattern_rule)) {
            rules.push_back({std::stoi(match.str(1)), std::stoi(match.str(2))});
        }
    }
    std::vector<Update> updates;
    static const std::regex pattern_update(R"((\d+))");
    while (std::getline(file, line)) {
        Update update;
        for (auto it = std::sregex_iterator(line.begin(), line.end(), pattern_update);
             it != std::sregex_iterator(); it++) {
            const std::smatch match = *it;
            update.push_back(std::stoi(match.str()));
        }
        updates.push_back(update);
    }
    return {rules, updates};
}

// Solution to part one
Int solve_one(const Data& data) {
    const auto& [rules, updates] = data;
    std::unordered_map<std::array<Int, 2>, Rule, ArrayHash> rules_map;
    auto get_key = [](const Rule& rule) {
        return std::array<Int, 2>{std::ranges::min(rule), std::ranges::max(rule)};
    };
    for (const Rule& rule : rules) {
        rules_map[get_key(rule)] = rule;
    }
    auto check_order = [&](Int left, Int rght) {
        if (auto it = rules_map.find(get_key({left, rght})); it != rules_map.end()) {
            const auto& [_, rule] = *it;
            if (left != rule[0]) {
                return false;
            }
        }
        return true;
    };
    auto check_update = [&](const Update& update) {
        for (std::size_t index_left = 0; index_left < update.size() - 1; index_left++) {
            const Int& left = update[index_left];
            for (std::size_t index_rght = index_left + 1; index_rght < update.size();
                 index_rght++) {
                const Int& rght = update[index_rght];
                if (not check_order(left, rght)) {
                    return false;
                }
            }
        }
        return true;
    };
    Int score = 0;
    for (const Update& update : updates) {
        if (check_update(update)) {
            score += update[update.size() / 2];
        }
    }
    return score;
}

// Solution to part two
Int solve_two(const Data& data) {
    const auto& [rules, updates] = data;
    std::unordered_map<std::array<Int, 2>, Rule, ArrayHash> rules_map;
    auto get_key = [](const Rule& rule) {
        return std::array<Int, 2>{std::ranges::min(rule), std::ranges::max(rule)};
    };
    for (const Rule& rule : rules) {
        rules_map[get_key(rule)] = rule;
    }
    auto check_order = [&](Int left, Int rght) {
        if (auto it = rules_map.find(get_key({left, rght})); it != rules_map.end()) {
            const auto& [_, rule] = *it;
            if (left != rule[0]) {
                return false;
            }
        }
        return true;
    };
    auto correct_update = [&](Update& update) {
        bool already_correct = true;
        for (std::size_t index_left = 0; index_left < update.size() - 1; index_left++) {
            const Int left = update[index_left];
            for (std::size_t index_rght = index_left + 1; index_rght < update.size();
                 index_rght++) {
                const Int rght = update[index_rght];
                if (not check_order(left, rght)) {
                    update[index_left] = rght;
                    update[index_rght] = left;
                    index_left--;
                    already_correct = false;
                    break;
                }
            }
        }
        return already_correct;
    };
    Int score = 0;
    for (Update update : updates) {
        if (not correct_update(update)) {
            score += update[update.size() / 2];
        }
    }
    return score;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
