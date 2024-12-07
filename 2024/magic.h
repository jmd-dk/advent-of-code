#ifndef MAGIC_H
#define MAGIC_H

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string>

// For rading in a text file line by line
class LineReader {
  public:
    explicit LineReader(const std::string& filename) : file(filename) {
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }
    class Iterator {
      public:
        explicit Iterator(std::ifstream& file, bool is_end)
            : file(file), is_end(is_end) {
            if (!is_end) {
                ++(*this);
            }
        }
        std::string operator*() const { return line; }
        Iterator& operator++() {
            is_end = !std::getline(file, line);
            return *this;
        }
        bool operator!=(const Iterator& other) const {
            return is_end != other.is_end;
        }

      private:
        std::ifstream& file;
        std::string line;
        bool is_end;
    };
    Iterator begin() { return Iterator(file, false); }
    Iterator end() { return Iterator(file, true); }

  private:
    std::ifstream file;
};

// For taking care of running the solve functions
template <typename ReadFunc, typename SolveFunc>
auto analyze(ReadFunc&& read_func, SolveFunc&& solve_func) {
    // Day
    int day = 0;
    std::filesystem::path path(__FILE__);
    while (path != path.root_path()) {
        path = path.parent_path();
        std::string dirname = path.filename().string();
        try {
            day = std::stoi(dirname);
        } catch (const std::invalid_argument&) {
            continue;
        }
        break;
    }
    // Part
    static int part = 2;
    part = 3 - part;
    // Read in data
    auto data = read_func();
    // Solve and measure execution time
    auto tick = std::chrono::high_resolution_clock::now();
    auto result = solve_func(data);
    auto tock = std::chrono::high_resolution_clock::now();
    // Get pretty time string with three significant digits
    auto duration = tock - tick;
    std::string time_value;
    std::string time_unit;
    if (duration >= std::chrono::seconds(1)) {
        time_value = std::format(
            "{:.3g}",
            std::chrono::duration_cast<std::chrono::milliseconds>(duration)
                    .count() /
                1e+3);
        time_unit = "s";
    } else if (duration >= std::chrono::milliseconds(1)) {
        time_value = std::format(
            "{:.3g}",
            std::chrono::duration_cast<std::chrono::microseconds>(duration)
                    .count() /
                1e+3);
        time_unit = "ms";
    } else if (duration >= std::chrono::microseconds(1)) {
        time_value = std::format(
            "{:.3g}",
            std::chrono::duration_cast<std::chrono::nanoseconds>(duration)
                    .count() /
                1e+3);
        time_unit = "µs";
    } else if (duration >= std::chrono::nanoseconds(1)) {
        time_value = std::format(
            "{}", std::chrono::duration_cast<std::chrono::nanoseconds>(duration)
                      .count());
        time_unit = "ns";
    } else {
        time_value = "no time at all";
    }
    if (std::ranges::find(time_value, '.') != time_value.end()) {
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
    const int width = 32;
    std::array<std::string, 2> output = {
        std::format("day {}, part {}: {}", day, part, result),
        std::format("({} {})", time_value, time_unit)};
    std::print("{}{}{}\n", output[0],
               std::string(std::max<int>(1, width - output[0].size()), ' '),
               output[1]);
    return result;
}

// General hash function for std::array
template <typename T>
struct ArrayHash {
    /* Simplified version of Python's tuple hashing,
    itself a simplified version of xxHash. See
    https://github.com/python/cpython/blob/main/Objects/tupleobject.c
    */
    static constexpr bool _64 = (sizeof(std::size_t) > 4);
    static constexpr std::size_t prime1 =
        static_cast<std::size_t>(_64 ? 11400714785074694791ULL : 2654435761UL);
    static constexpr std::size_t prime2 =
        static_cast<std::size_t>(_64 ? 14029467366897019727ULL : 2246822519UL);
    static constexpr std::size_t prime5 =
        static_cast<std::size_t>(_64 ? 2870177450012600261ULL : 374761393UL);
    static constexpr std::hash<T> hasher{};
    template <std::size_t N>
    std::size_t operator()(const std::array<T, N>& arr) const {
        std::size_t hash = prime5;
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

#endif  // MAGIC_H
