#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <ranges>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../../magic.h"

using Int = std::int32_t;
using Rule = std::array<Int, 2>;
using Update = std::vector<Int>;
using Data = std::tuple<std::vector<Rule>, std::vector<Update>>;

// Reading in data
Data read() {
    static const std::regex pattern_rule(R"((\d+)\|(\d+))");
    static const std::regex pattern_update(R"((\d+))");
    std::vector<Rule> rules;
    std::vector<Update> updates;
    int section = 0;
    for (const std::string& line : LineReader("input")) {
        if (line.empty()) {
            section++;
            continue;
        }
        std::smatch match;
        switch (section) {
            case 0:
                if (std::regex_match(line, match, pattern_rule)) {
                    Int left, rght;
                    std::istringstream(match.str(1)) >> left;
                    std::istringstream(match.str(2)) >> rght;
                    rules.push_back({left, rght});
                }
                break;
            case 1:
                Update update;
                for (auto it =
                         std::sregex_iterator(line.begin(), line.end(), pattern_update);
                     it != std::sregex_iterator(); it++) {
                    match = *it;
                    Int num;
                    std::istringstream(match.str()) >> num;
                    update.push_back(num);
                }
                updates.push_back(update);
                break;
        }
    }
    return {rules, updates};
}

// Solution to part one
Int solve_one(const Data& data) {
    const auto& [rules, updates] = data;
    std::unordered_map<std::array<Int, 2>, Rule, ArrayHash<Int>> rules_map;
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
            for (std::size_t index_rght = index_left + 1; index_rght < update.size();
                 index_rght++) {
                if (not check_order(update[index_left], update[index_rght])) {
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
    std::unordered_map<std::array<Int, 2>, Rule, ArrayHash<Int>> rules_map;
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
            Int left = update[index_left];
            for (std::size_t index_rght = index_left + 1; index_rght < update.size();
                 index_rght++) {
                Int rght = update[index_rght];
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
