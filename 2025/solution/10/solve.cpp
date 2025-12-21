#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <generator>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "helper.hpp"

using Int = std::int32_t;
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
    std::ifstream file{"input"};
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
        for (std::span<const std::size_t> button_indices : aoc::powerset(buttons.size())) {
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
        throw std::runtime_error("No solution for this machine!");
    };
    return aoc::sum(data | std::views::transform(find_minimum_button_presses));
}

// Solution to part two
Int solve_two(const Data& data) {
    using Rational = aoc::Rational<Int>;
    using Matrix = aoc::Matrix<Rational>;
    auto matrix_from_buttons = [](const std::vector<Button>& buttons, std::size_t nrows) {
        const std::size_t ncols = buttons.size();
        std::vector<std::vector<Int>> cols;
        cols.reserve(ncols);
        for (std::size_t j = 0; j < ncols; j++) {
            std::vector<Int> col(nrows);
            for (std::size_t i : buttons[j]) {
                col[i] = Int{1};
            }
            cols.push_back(std::move(col));
        }
        return Matrix::from_cols(cols);
    };
    auto get_search_space =
        [](const std::vector<Button>& buttons, const Joltages& joltages, const Matrix::PivotInfo& pivot_info) {
            const std::size_t ncols = buttons.size();
            std::vector<std::size_t> free_cols{};
            std::unordered_map<std::size_t, Int> free_max{};
            for (std::size_t j = 0; j < ncols; j++) {
                if (pivot_info.pivot_row_of_col[j] == Matrix::PivotInfo::npos) {
                    free_cols.push_back(j);
                    Int max_value = std::numeric_limits<Int>::max();
                    for (std::size_t i : buttons[j]) {
                        max_value = std::min(max_value, joltages[i]);
                    }
                    free_max[j] = max_value;
                }
            }
            return std::pair{free_cols, free_max};
        };
    auto find_optimal_solution = [](const Matrix& mat_aug,
                                    const Matrix::PivotInfo& pivot_info,
                                    std::span<const std::size_t> free_cols,
                                    const std::unordered_map<std::size_t, Int>& free_max,
                                    std::size_t ncols) -> std::optional<std::vector<Int>> {
        auto free_assignments = [&]() -> std::generator<std::span<const Int>> {
            std::vector<Int> vals(free_cols.size());
            while (true) {
                co_yield vals;
                std::size_t k{};
                while (k < vals.size() && vals[k] == free_max.at(free_cols[k])) {
                    vals[k] = 0;
                    k++;
                }
                if (k == vals.size()) {
                    co_return;
                }
                vals[k]++;
            }
        };
        // Check whether any general (rational) solution exists
        for (std::size_t i = 0; i < mat_aug.nrows(); i++) {
            std::size_t j = pivot_info.pivot_col_of_row[i];
            if ((j >= ncols || j == Matrix::PivotInfo::npos) && mat_aug(i, ncols) != Rational{}) {
                return std::nullopt;
            }
        }
        // Collect pivots (row, col)
        std::vector<std::pair<std::size_t, std::size_t>> pivots{};
        pivots.reserve(pivot_info.rank);
        for (std::size_t i = 0; i < mat_aug.nrows(); i++) {
            std::size_t j_p = pivot_info.pivot_col_of_row[i];
            if (j_p < ncols && j_p != Matrix::PivotInfo::npos) {
                pivots.emplace_back(i, j_p);
            }
        }
        // Iterate over all possible values of the free variables,
        // compute the pivot variables and reject candidate
        // if any value becomes fractional or negative.
        Int sum_min = std::numeric_limits<Int>::max();
        std::vector<Int> solution(ncols);
        std::vector<Int> solution_optimal{};
        auto eval_candidate = [&](std::span<const Int> free_values) -> std::optional<Int> {
            assert(free_values.size() == free_cols.size());
            Int sum = aoc::sum(free_values);
            if (sum >= sum_min) {
                return std::nullopt;
            }
            std::ranges::fill(solution, 0);
            for (std::size_t k = 0; k < free_cols.size(); k++) {
                solution[free_cols[k]] = free_values[k];
            }
            for (const auto [i, j_p] : pivots) {
                Rational x = mat_aug(i, ncols);
                for (std::size_t k = 0; k < free_cols.size(); k++) {
                    x -= mat_aug(i, free_cols[k]) * free_values[k];
                }
                if (!x.is_integral() || x < Int{}) {
                    return std::nullopt;
                }
                Int x_integral = x.num();
                solution[j_p] = x_integral;
                sum += x_integral;
                if (sum >= sum_min) {
                    return std::nullopt;
                }
            }
            return sum;
        };
        for (std::span<const Int> free_values : free_assignments()) {
            if (auto sum = eval_candidate(free_values)) {
                sum_min = *sum;
                solution_optimal = solution;
            }
        }
        if (solution_optimal.empty()) {
            return std::nullopt;  // no non-negative integer solution exists
        }
        return solution_optimal;
    };
    auto find_minimum_button_presses = [&](const Machine& machine) {
        const std::vector<Button>& buttons = machine.buttons;
        const Joltages& joltages = machine.joltages;
        // Set up augmented matrix [ buttons | joltages ]
        // and bring into RREF.
        const std::size_t nrows = joltages.size();
        const std::size_t ncols = buttons.size();
        Matrix mat_aug = matrix_from_buttons(buttons, nrows).augment_right(Matrix::from_col(joltages));
        const Matrix::PivotInfo pivot_info = mat_aug.rref();
        // Determine seach space for free variables/columns
        const auto [free_cols, free_max] = get_search_space(buttons, joltages, pivot_info);
        // Solve reduced search space by brute force
        const auto solution_optimal = find_optimal_solution(mat_aug, pivot_info, free_cols, free_max, ncols);
        if (!solution_optimal) {
            throw std::runtime_error("No non-negative integer solution for this machine!");
        }
        return aoc::sum(*solution_optimal);
    };
    return aoc::sum(data | std::views::transform(find_minimum_button_presses));
}

// Solve
int main() {
    aoc::analyze(read, solve_one);
    aoc::analyze(read, solve_two);
}
