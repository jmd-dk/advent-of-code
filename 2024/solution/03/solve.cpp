#include <cstdint>
#include <regex>
#include <sstream>
#include <string>

#include "../../magic.h"

using Int = std::int32_t;

// Reading in data
std::string read() {
    std::string memory;
    for (const std::string& line : LineReader("input")) {
        memory += line;
    }
    return memory;
}

// Solution to part one
Int solve_one(const std::string& memory) {
    static const std::regex pattern(R"(mul\((\d+),(\d+)\))");
    Int sum = 0;
    for (auto it = std::sregex_iterator(memory.begin(), memory.end(), pattern);
         it != std::sregex_iterator(); it++) {
        std::smatch match = *it;
        Int left, rght;
        std::istringstream(match.str(1)) >> left;
        std::istringstream(match.str(2)) >> rght;
        sum += left * rght;
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
            Int left, rght;
            std::istringstream(match.str(1)) >> left;
            std::istringstream(match.str(2)) >> rght;
            sum += left * rght;
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
