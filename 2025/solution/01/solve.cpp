#include <charconv>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "helper.hpp"

using Int = std::int32_t;
using Data = std::vector<Int>;

// Reading in data
Data read() {
    Data rotations{};
    std::ifstream file{"input"};
    for (std::string& line : std::ranges::istream_view<std::string>{file}) {
        Int rotation{};
        std::from_chars(line.data() + 1, line.data() + line.size(), rotation);
        rotation *= (line[0] == 'L' ? -1 : +1);
        rotations.push_back(rotation);
    }
    return rotations;
}

// Solution to part one
Int solve_one(const Data& rotations) {
    constexpr Int dial_size{100};
    constexpr Int dial_init{50};
    auto mod = [](Int a, Int b) {
        Int r{a % b};
        return r + (r < 0) * b;
    };
    Int zeros{};
    Int dial{dial_init};
    for (Int rotation : rotations) {
        dial = mod(dial + rotation, dial_size);
        zeros += (dial == 0);
    }
    return zeros;
}

// Solution to part two
Int solve_two(const Data& rotations) {
    constexpr Int dial_size{100};
    constexpr Int dial_init{50};
    auto divmod = [](Int a, Int b) {
        Int q{a / b};
        Int r{a % b};
        Int fix{r < 0};
        r += fix * b;
        q -= fix;
        return std::pair{q, r};
    };
    Int zeros{};
    Int dial{dial_init};
    for (Int rotation : rotations) {
        auto [q, r] = divmod(dial + rotation, dial_size);
        if (q < 0) {
            zeros += -q - (dial == 0);
            zeros += (r == 0);
        } else if (q > 0) {
            zeros += q;
        } else {
            zeros += (r == 0);
        }
        dial = r;
    }
    return zeros;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
}
