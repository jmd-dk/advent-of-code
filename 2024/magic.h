#ifndef MAGIC_H
#define MAGIC_H

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <format>
#include <functional>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

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
    // Get pretty time string
    auto duration = tock - tick;
    std::ostringstream time_stream;
    if (duration >= std::chrono::seconds(1)) {
        time_stream << std::chrono::duration_cast<std::chrono::seconds>(duration).count()
                    << " s";
    } else if (duration >= std::chrono::milliseconds(1)) {
        time_stream
            << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
            << " ms";
    } else if (duration >= std::chrono::microseconds(1)) {
        time_stream
            << std::chrono::duration_cast<std::chrono::microseconds>(duration).count()
            << " µs";
    } else if (duration >= std::chrono::nanoseconds(1)) {
        time_stream
            << std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()
            << " ns";
    } else {
        time_stream << "0";
    }
    // Print out results
    const int width = 34;
    std::vector<std::string> output = {
        std::format("day {}, part {}: {}", day, part, result),
        std::format("({})", time_stream.str())};
    std::print(
        "{}{}{}\n", output[0],
        std::string(std::max<int>(1, width - output[0].size() - output[1].size()), ' '),
        output[1]);
    return result;
}

#endif // MAGIC_H
