#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>

#include "../../magic.h"

using Int = std::int32_t;
using Data = std::vector<std::vector<char>>;

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
        data.emplace_back(line.begin(), line.end());
    }
    return data;
}

// Solution to part one
Int solve_one(const Data& data) {
    static constexpr std::string target = "XMAS";
    const std::array<std::size_t, 2> shape{data.size(), data[0].size()};
    auto check = [&](std::size_t i, std::size_t j, int di, int dj) {
        for (const char& c : target) {
            if (i >= shape[0] or j >= shape[1] or data[i][j] != c) {
                return false;
            }
            i += di;
            j += dj;
        }
        return true;
    };
    Int count = 0;
    for (std::size_t i = 0; i < shape[0]; i++) {
        for (std::size_t j = 0; j < shape[1]; j++) {
            for (int di : {-1, 0, +1}) {
                for (int dj : {-1, 0, +1}) {
                    count += (di != 0 or dj != 0) and check(i, j, di, dj);
                }
            }
        }
    }
    return count;
}

// Solution to part two
Int solve_two(const Data& data) {
    static constexpr std::string target = "MAS";
    static_assert(target.size() & 1);
    const std::array<std::size_t, 2> shape{data.size(), data[0].size()};
    auto check = [&](std::size_t i, std::size_t j, int di, int dj) {
        i -= di * (target.size() / 2);
        j -= dj * (target.size() / 2);
        for (const char& c : target) {
            if (i >= shape[0] or j >= shape[1] or data[i][j] != c) {
                return false;
            }
            i += di;
            j += dj;
        }
        return true;
    };
    Int count = 0;
    for (std::size_t i = 0; i < shape[0]; i++) {
        for (std::size_t j = 0; j < shape[1]; j++) {
            count += (check(i, j, +1, +1) or check(i, j, -1, -1)) and
                     (check(i, j, -1, +1) or check(i, j, +1, -1));
        }
    }
    return count;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
