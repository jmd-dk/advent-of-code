#include <cstdint>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../../magic.h"

using Int = std::int32_t;

// Reading in data
std::string read() {
    const std::string filename = "input";
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string memory = oss.str();
    return memory;
}

// Solution to part one
Int solve_one(const std::string& memory) {
    static const std::regex pattern(R"(mul\((\d+),(\d+)\))");
    Int sum = 0;
    for (auto it = std::sregex_iterator(memory.begin(), memory.end(), pattern);
         it != std::sregex_iterator(); ++it) {
        std::smatch match = *it;
        sum += std::stoi(match.str(1)) * std::stoi(match.str(2));
    }
    return sum;
}

// Solution to part two
Int solve_two(const std::string& memory) {
    static const std::regex pattern(R"(mul\((\d+),(\d+)\)|do\(\)|don't\(\))");
    bool enable = true;
    Int sum = 0;
    for (auto it = std::sregex_iterator(memory.begin(), memory.end(), pattern);
         it != std::sregex_iterator(); it++) {
        std::smatch match = *it;
        if (enable and match.size() > 1 and match[1].matched) {
            sum += std::stoi(match.str(1)) * std::stoi(match.str(2));
        } else {
            enable = (match.str() == "do()");
        }
    }
    return sum;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
