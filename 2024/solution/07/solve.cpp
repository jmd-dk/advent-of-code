#include <cstddef>
#include <cstdint>
#include <format>
#include <regex>
#include <sstream>
#include <utility>
#include <vector>

#include "../../magic.h"

using Int = std::uint64_t;
using Equation = std::pair<Int, std::vector<Int>>;

// Reading in data
std::vector<Equation> read() {
    static const std::regex pattern(R"(\d+)");
    std::vector<Equation> equations;
    for (const std::string& line : LineReader("input")) {
        Equation equation;
        std::size_t index = 0;
        for (auto it = std::sregex_iterator(line.begin(), line.end(), pattern);
             it != std::sregex_iterator(); index++, it++) {
            std::smatch match = *it;
            Int num;
            std::istringstream(match.str()) >> num;
            if (index == 0) {
                equation.first = num;
            } else {
                equation.second.push_back(num);
            }
        }
        equations.push_back(equation);
    }
    return equations;
}

// Solution to part one
Int solve_one(const std::vector<Equation>& equations) {
    std::function<Int(Int, const Equation&, std::size_t)> evaluate;
    evaluate = [&](Int left, const Equation& equation, std::size_t index) -> Int {
        if (equation.first < left) {
            return 0;
        }
        if (index == equation.second.size()) {
            return equation.first == left;
        }
        Int rght = equation.second[index++];
        return evaluate(left + rght, equation, index) +
               evaluate(left * rght, equation, index);
    };
    Int result = 0;
    for (const Equation& equation : equations) {
        if (evaluate(equation.second[0], equation, 1)) {
            result += equation.first;
        }
    }
    return result;
}

// Solution to part two
Int solve_two(const std::vector<Equation>& equations) {
    auto concat = [](Int left, Int rght) {
        Int num;
        std::istringstream(std::format("{}{}", left, rght)) >> num;
        return num;
    };
    std::function<Int(Int, const Equation&, std::size_t)> evaluate;
    evaluate = [&](Int left, const Equation& equation, std::size_t index) -> Int {
        if (equation.first < left) {
            return 0;
        }
        if (index == equation.second.size()) {
            return equation.first == left;
        }
        Int rght = equation.second[index++];
        return evaluate(left + rght, equation, index) +
               evaluate(left * rght, equation, index) +
               evaluate(concat(left, rght), equation, index);
    };
    Int result = 0;
    for (const Equation& equation : equations) {
        if (evaluate(equation.second[0], equation, 1)) {
            result += equation.first;
        }
    }
    return result;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
