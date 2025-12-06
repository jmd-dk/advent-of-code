#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <optional>
#include <ranges>
#include <sstream>
#include <utility>
#include <vector>

#include "helper.hpp"

using Int = std::int64_t;
using Operation = std::function<Int(Int, Int)>;
using DigitPlaceholder = std::optional<Int>;
using Digits = std::vector<DigitPlaceholder>;
struct Problem {
    Operation operation{};
    std::vector<Digits> digit_blocks{};
};
using Data = std::vector<Problem>;

// Reading in data
Data read() {
    std::vector<Digits> rows{};
    std::vector<Operation> operations{};
    std::vector<std::size_t> col_indices{};
    std::ifstream file{"input"};
    std::string line{};
    while (std::getline(file, line)) {
        Digits row{};
        for (auto [_i, c] : std::views::enumerate(line)) {
            std::size_t i = static_cast<std::size_t>(_i);
            switch (c) {
            case ' ':
                row.push_back(std::nullopt);
                break;
            case '+':
                operations.push_back(std::plus<>{});
                col_indices.push_back(i);
                break;
            case '*':
                operations.push_back(std::multiplies<>{});
                col_indices.push_back(i);
                break;
            default:
                row.push_back(static_cast<Int>(c - '0'));
            }
        }
        rows.push_back(std::move(row));
    }
    rows.pop_back();
    // Read into columnwise problems
    std::size_t num_problems = operations.size();
    assert(num_problems == col_indices.size());
    col_indices.push_back(rows.front().size() + 1);  // for easier parsing
    Data data{};
    data.reserve(num_problems);
    for (auto [operation, j_pair] : std::views::zip(operations, std::views::pairwise(col_indices))) {
        auto [j_bgn, j_end] = j_pair;
        std::vector<Digits> col{};
        for (const auto& row : rows) {
            col.push_back(
                row | std::views::drop(j_bgn) | std::views::take(j_end - 1 - j_bgn) | std::ranges::to<std::vector>()
            );
        }
        data.emplace_back(operation, std::move(col));
    }
    return data;
}

// Solution to part one
Int solve_one(const Data& data) {
    auto interpret_digits = [](const Digits& digits) {
        Int num{};
        for (const DigitPlaceholder& digit : digits) {
            if (digit) {
                num = num * 10 + *digit;
            }
        }
        return num;
    };
    auto solve_problem = [&](const Problem& problem) {
        auto nums = problem.digit_blocks | std::views::transform(interpret_digits);
        return std::ranges::fold_left(std::views::drop(nums, 1), *std::ranges::begin(nums), problem.operation);
    };
    return std::ranges::fold_left(data | std::views::transform(solve_problem), Int{}, std::plus<>{});
}

// Solution to part two
Int solve_two(Data data) {
    auto transpose = [](Problem& problem) {
        std::size_t num_rows{problem.digit_blocks.size()};
        std::size_t num_cols = std::ranges::max(
            problem.digit_blocks | std::views::transform(&std::vector<DigitPlaceholder>::size)
        );
        std::vector<Digits> digit_blocks_transposed(num_cols);
        for (auto& digits : digit_blocks_transposed) {
            digits.resize(num_rows);
        }
        for (std::size_t i = 0; i < num_rows; i++) {
            const Digits& digits{problem.digit_blocks[i]};
            for (std::size_t j = 0; j < digits.size(); j++) {
                digit_blocks_transposed[j][i] = digits[j];
            }
        }
        problem.digit_blocks = std::move(digit_blocks_transposed);
    };
    for (Problem& problem : data) {
        transpose(problem);
    }
    return solve_one(data);
}

// Solve
int main() {
    helper::analyze(read, solve_one);
    helper::analyze(read, solve_two);
}
