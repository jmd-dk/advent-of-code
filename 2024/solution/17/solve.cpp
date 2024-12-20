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

#include "../../magic.hpp"

using Int = std::int64_t;
using Data = std::tuple<std::unordered_map<char, Int>, std::vector<Int>>;

// Reading in data
Data read() {
    static const std::regex pattern_register(R"(Register (.): (\d+))");
    static const std::regex pattern_program(R"(Program: (.+))");
    std::unordered_map<char, Int> registers;
    std::vector<Int> instructions;
    for (const std::string& line : LineReader("input")) {
        std::smatch match;
        if (std::regex_match(line, match, pattern_register)) {
            Int value;
            std::istringstream(match[2].str()) >> value;
            registers[match[1].str()[0]] = value;
        } else if (std::regex_match(line, match, pattern_program)) {
            std::stringstream stream(match[1].str());
            std::string token;
            while (std::getline(stream, token, ',')) {
                Int value;
                std::istringstream(token) >> value;
                instructions.push_back(value);
            }
        }
    }
    return {registers, instructions};
}

// Solution to part one
struct Machine {
    std::array<Int, 3> registers{};
    std::vector<Int> instructions{};
    std::size_t ip{};
    std::vector<Int> output{};
    Int combo(Int operand) {
        if (operand < 4) {
            return operand;
        } else {
            return registers[operand - 4];
        }
    }
    void execute(Int num = 1) {
        if (num == 0) {
            return;
        }
        auto opcode = instructions[ip++];
        auto operand = instructions[ip++];
        switch (opcode) {
            case (0):
                registers[0] /= (1 << combo(operand));
                break;
            case (1):
                registers[1] ^= operand;
                break;
            case (2):
                registers[1] = combo(operand) % 8;
                break;
            case (3):
                if (registers[0]) {
                    ip = operand;
                }
                break;
            case (4):
                registers[1] ^= registers[2];
                break;
            case (5):
                output.push_back(combo(operand) % 8);
                break;
            case (6):
                registers[1] = registers[0] / (1 << combo(operand));
                break;
            case (7):
                registers[2] = registers[0] / (1 << combo(operand));
                break;
        }
        execute(num - 1);
    }
    bool halted() { return ip == instructions.size(); }
    std::string get_output() {
        if (output.empty()) {
            return "";
        }
        std::ostringstream stream;
        stream << output[0];
        for (auto value : output | std::views::drop(1)) {
            stream << ",";
            stream << value;
        }
        return stream.str();
    }
};
std::string solve_one(const Data& data) {
    auto [registers, instructions] = data;
    Machine machine{{registers['A'], registers['B'], registers['C']}, instructions};
    while (not machine.halted()) {
        machine.execute();
    }
    return machine.get_output();
}

// Solution to part two
Int solve_two(const Data& data) {
    auto [registers, instructions] = data;
    std::function<Int(Int, std::size_t)> backtrace;
    backtrace = [&](Int target_register, std::size_t instruction_index) -> Int {
        Int target_output = instructions[instruction_index];
        std::vector<Int> possibilities;
        for (Int a = target_register * 8; a < (target_register + 1) * 8; a++) {
            Int b = 0;  // does not matter
            Int c = 0;  // does not matter
            Machine machine{{a, b, c}, instructions};
            machine.execute(instructions.size() / 2);
            if (machine.registers[0] == target_register and
                machine.output == std::vector<Int>{target_output}) {
                if (instruction_index == 0) {
                    return a;
                }
                possibilities.push_back(a);
            }
        }
        for (Int a : possibilities) {
            if (Int a_init = backtrace(a, instruction_index - 1); a_init != -1) {
                return a_init;
            }
        }
        return -1;
    };
    return backtrace(0, instructions.size() - 1);  // A = 0 implies halt
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
