#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <print>
#include <ranges>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using Int = std::int32_t;

// Structure holding the data
struct Data {
    std::array<std::vector<Int>, 2> both;
    std::vector<Int>& left = both[0];
    std::vector<Int>& rght = both[1];
    Data() = default;
    Data(const Data& other) {
        both[0] = other.both[0];
        both[1] = other.both[1];
    }
    auto begin() {
        return both.begin();
    }
    auto end() {
        return both.end();
    }
};

// Reading in data
Data read() {
    const std::string filename = "input";
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    Data data;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream stream(line);
        for (auto& list : data) {
            Int n;
            stream >> n;
            list.push_back(n);
        }
    }
    return data;
}

// Solution to part one
Int solve_one(Data data) {
    for (auto& list : data) {
        std::ranges::sort(list);
    }
    Int distance = 0;
    for (const auto [left, rght] : std::views::zip(data.left, data.rght)) {
        distance += std::abs(left - rght);
    }
    return distance;
}

// Solution to part two
Int solve_two(Data data) {
    auto count = [](std::span<const Int> list) {
        std::unordered_map<Int, Int> counter;
        for (const Int n : list) {
            counter[n] += 1;
        }
        return counter;
    };
    const auto counter_left = count(data.left);
    const auto counter_rght = count(data.rght);
    Int score = 0;
    for (const auto& [key, val_left] : counter_left) {
        if (auto it = counter_rght.find(key); it != counter_rght.end()) {
            const auto& [_, val_rght] = *it;
            score += key * val_left * val_rght;
        }
    }
    return score;
}

// Solve
int main() {
    const int day = 1;
    Data data = read();
    std::print("day {}, part {}: {}\n", day, 1, solve_one(data));
    std::print("day {}, part {}: {}\n", day, 2, solve_two(data));
    return 0;
}
