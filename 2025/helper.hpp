#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <filesystem>
#include <format>
#include <print>
#include <source_location>
#include <stdexcept>
#include <string>

namespace helper {

// For taking care of running the solve functions
static unsigned int part{};
template <typename ReadFunc, typename SolveFunc>
auto analyze(
    ReadFunc&& read_func, SolveFunc&& solve_func, std::source_location source = std::source_location::current()
) {
    // Day
    int day{};
    std::filesystem::path path{source.file_name()};
    while (path != path.root_path()) {
        path = path.parent_path();
        std::string dirname{path.filename().string()};
        try {
            day = std::stoi(dirname);
        } catch (const std::invalid_argument&) {
            continue;
        }
        break;
    }
    // Part
    part++;
    // Read in data
    auto data = read_func();
    // Solve and measure execution time
    auto tic = std::chrono::high_resolution_clock::now();
    auto result = solve_func(data);
    auto toc = std::chrono::high_resolution_clock::now();
    // Get pretty time string with three significant digits
    auto duration = toc - tic;
    std::string time_value{};
    std::string time_unit{};
    if (duration >= std::chrono::seconds{1}) {
        time_value = std::format("{:.3g}", std::chrono::duration<double>{duration}.count());
        time_unit = "s";
    } else if (duration >= std::chrono::milliseconds{1}) {
        time_value = std::format("{:.3g}", std::chrono::duration<double, std::milli>{duration}.count());
        time_unit = "ms";
    } else if (duration >= std::chrono::microseconds{1}) {
        time_value = std::format("{:.3g}", std::chrono::duration<double, std::micro>{duration}.count());
        time_unit = "µs";
    } else if (duration >= std::chrono::nanoseconds{1}) {
        time_value = std::format("{}", std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
        time_unit = "ns";
    } else {
        time_value = "no time at all";
    }
    if (time_value.find('.') != std::string::npos) {
        while (time_value.size() < 4) {
            time_value += '0';
        }
    } else if (time_value.size() < 3) {
        time_value += '.';
        while (time_value.size() < 4) {
            time_value += '0';
        }
    }
    // Print out results
    constexpr int width{32};
    std::array output{
        std::format("day {}, part {}: {}", day, part, result),
        std::format("({}{}{})", time_value, time_unit.empty() ? "" : " ", time_unit),
    };
    std::println(
        "{}{}{}",
        output[0],
        std::string(static_cast<std::size_t>(std::max(1, width - static_cast<int>(output[0].size()))), ' '),
        output[1]
    );
    return result;
}

// Hash function for std::array
struct ArrayHash {
    /* Simplified version of Python's tuple hashing,
    itself a simplified version of xxHash. See
    https://github.com/python/cpython/blob/main/Objects/tupleobject.c
    */
    static constexpr bool _64{sizeof(std::size_t) > 4};
    static constexpr std::size_t prime1{_64 ? 11400714785074694791uz : 2654435761uz};
    static constexpr std::size_t prime2{_64 ? 14029467366897019727uz : 2246822519uz};
    static constexpr std::size_t prime5{_64 ? 2870177450012600261uz : 374761393uz};
    template <typename T, std::size_t N>
    std::size_t operator()(const std::array<T, N>& arr) const {
        std::hash<T> hasher{};
        std::size_t hash{prime5};
        for (const T& elem : arr) {
            hash += hasher(elem) * prime2;
            if constexpr (_64) {
                hash = (hash << 31) | (hash >> 33);
            } else {
                hash = (hash << 13) | (hash >> 19);
            }
            hash *= prime1;
        }
        return hash;
    }
};

// Integer power function
template <std::integral Base, std::unsigned_integral Exp>
[[nodiscard]] constexpr Base ipow(Base base, Exp exp) noexcept {
    Base result{1};
    while (exp != 0) {
        if (exp & 1) {
            result *= base;
        }
        base *= base;
        exp >>= 1;
    }
    return result;
}

}  // namespace helper