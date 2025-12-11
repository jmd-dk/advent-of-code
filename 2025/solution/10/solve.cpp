#include <algorithm>
// #include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <fstream>
// #include <generator>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "helper.hpp"

using Int = std::int64_t;
using Lights = std::vector<std::uint8_t>;
using Button = std::vector<std::size_t>;
using Joltages = std::vector<Int>;
struct Machine {
    Lights lights_diagram{};
    std::vector<Button> buttons{};
    Joltages joltages{};
};
using Data = std::vector<Machine>;

// Reading in data
Data read() {
    Data data{};
    std::ifstream file{"example"};
    std::string line{};
    while (std::getline(file, line)) {
        Machine machine{};
        for (auto part : std::views::split(line, ' ')) {
            std::string_view token{part.begin() + 1, part.end() - 1};
            switch (part.front()) {
                case '[': {
                    machine.lights_diagram = token | std::views::transform([](char c) { return c == '#'; }) |
                                             std::ranges::to<Lights>();
                    break;
                }
                case '(': {
                    Button button{};
                    for (auto index : std::views::split(token, ',')) {
                        std::size_t value{};
                        std::from_chars(index.data(), index.data() + index.size(), value);
                        button.push_back(value);
                    }
                    machine.buttons.push_back(std::move(button));
                    break;
                }
                case '{': {
                    for (auto joltage : std::views::split(token, ',')) {
                        Int value{};
                        std::from_chars(joltage.data(), joltage.data() + joltage.size(), value);
                        machine.joltages.push_back(value);
                    }
                    break;
                }
            }
        }
        data.push_back(std::move(machine));
    }
    return data;
}

// Solution to part one
Int solve_one(const Data& data) {
    auto find_minimum_button_presses = [](const Machine& machine) {
        const Lights& lights_diagram = machine.lights_diagram;
        const std::vector<Button>& buttons = machine.buttons;
        // Key insights:
        // * Each button press is involutory,
        //   so each button needs to be pressed at most once.
        // * All button presses commute.
        Lights lights(lights_diagram.size());
        for (std::span<const std::size_t> button_indices : helper::powerset(buttons.size())) {
            std::ranges::fill(lights, 0);
            for (std::size_t button_index : button_indices) {
                for (std::size_t light_index : buttons[button_index]) {
                    lights[light_index] ^= 1;  // bit-flip
                }
            }
            if (lights == lights_diagram) {
                return static_cast<Int>(button_indices.size());
            }
        }
        return Int{-1};  // no solution
    };
    return helper::sum(data | std::views::transform(find_minimum_button_presses));
}

// Solve
int main() {
    helper::analyze(read, solve_one);
}
