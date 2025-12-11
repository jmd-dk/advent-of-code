#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <filesystem>
#include <format>
#include <functional>
#include <generator>
#include <iostream>
#include <limits>
#include <numeric>
#include <ostream>
#include <print>
#include <ranges>
#include <ratio>
#include <source_location>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

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
    std::cout << std::flush;
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

// Heap
template <typename T, typename Compare>
class Heap {
  public:
    using value_type = T;

    explicit Heap(std::size_t max_size = std::numeric_limits<std::size_t>::max(), Compare comp = Compare{})
        : max_size_(max_size), comp_(std::move(comp)) {}

    explicit Heap(
        std::vector<T> data_in, std::size_t max_size = std::numeric_limits<std::size_t>::max(), Compare comp = Compare{}
    )
        : max_size_(max_size), comp_(std::move(comp)) {
        push_many(std::move(data_in));
    }

    [[nodiscard]] bool empty() const noexcept { return data_.empty(); }
    [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }
    [[nodiscard]] std::size_t max_size() const noexcept { return max_size_; }

    void push(T value) {
        if (data_.size() < max_size_) {
            data_.push_back(std::move(value));
            std::ranges::push_heap(data_, comp_);
        } else if (comp_(value, data_.front())) {
            std::ranges::pop_heap(data_, comp_);
            data_.back() = std::move(value);
            std::ranges::push_heap(data_, comp_);
        }
    }

    void push_many(std::vector<T> data_in) {
        if (data_in.empty()) {
            return;
        }
        if (data_.empty()) {
            // Fast path
            if (data_in.size() > max_size_) {
                auto last = data_in.begin() + static_cast<decltype(data_in)::difference_type>(max_size_);
                std::ranges::nth_element(data_in, last, comp_);
                data_in.resize(max_size_);
            }
            data_ = std::move(data_in);
            std::ranges::make_heap(data_, comp_);
        } else {
            for (T& el : data_in) {
                push(std::move(el));
            }
        }
    }

    template <typename... Args>
    void emplace(Args&&... args) {
        T value(std::forward<Args>(args)...);
        push(std::move(value));
    }

    T pop() {
        std::ranges::pop_heap(data_, comp_);
        T value = std::move(data_.back());
        data_.pop_back();
        return value;
    }

    [[nodiscard]] const T& top() const { return data_.front(); }

    [[nodiscard]] std::vector<T> to_vector() {
        std::vector<T> vec;
        vec.reserve(size());
        while (!empty()) {
            vec.push_back(pop());
        }
        std::ranges::reverse(vec);
        return vec;
    }

    [[nodiscard]] std::vector<T> const& data() const noexcept { return data_; }

  private:
    std::vector<T> data_{};
    const std::size_t max_size_{};
    Compare comp_{};
};

// Named Heap specializations
template <typename T>
using MinHeap = Heap<T, std::greater<T>>;
template <typename T>
using MaxHeap = Heap<T, std::less<T>>;

// Disjoint Set Union
class DSU {
  public:
    explicit DSU(std::size_t element_count)
        : element_count_(element_count), component_count_(element_count), sizes_(element_count, 1) {
        parents_.reserve(element_count_);
        for (std::size_t i = 0; i < element_count_; ++i) {
            parents_.push_back(i);
        }
    }

    std::size_t find(std::size_t i) noexcept {
        if (parents_[i] != i) {
            // Path compression
            parents_[i] = find(parents_[i]);
        }
        return parents_[i];
    }

    bool unite(std::size_t i, std::size_t j) noexcept {
        i = find(i);
        j = find(j);
        if (i == j) {
            return false;
        }
        if (sizes_[i] < sizes_[j]) {
            std::swap(i, j);
        }
        parents_[j] = i;
        sizes_[i] += sizes_[j];
        component_count_--;
        return true;
    }

    [[nodiscard]] std::size_t element_count() const noexcept { return element_count_; }

    [[nodiscard]] std::size_t component_count() const noexcept { return component_count_; }

    [[nodiscard]] std::size_t component_size(std::size_t i) noexcept { return sizes_[find(i)]; }

    [[nodiscard]] bool same_component(std::size_t i, std::size_t j) noexcept { return find(i) == find(j); }

    // Get all root indices
    [[nodiscard]] std::vector<std::size_t> roots() {
        std::vector<std::size_t> root_indices;
        root_indices.reserve(component_count_);
        // Ensure every element points directly to a root
        for (std::size_t i = 0; i < element_count_; i++) {
            find(i);
        }
        // Collect all roots
        for (std::size_t i = 0; i < element_count_; i++) {
            if (parents_[i] == i) {
                root_indices.push_back(i);
            }
        }
        return root_indices;
    }

    // Get `num` root indices with smallest/largest component sizes
    [[nodiscard]] std::vector<std::size_t> roots(std::size_t num, bool largest = false) {
        if (num == 0) {
            return {};
        }
        // Get all root indices
        std::vector<std::size_t> root_indices{roots()};
        if (num >= root_indices.size()) {
            return root_indices;
        }
        // Get component information
        struct Component {
            std::size_t size{};
            std::size_t root{};
            constexpr auto operator<=>(Component const&) const = default;
        };
        std::vector<Component> components;
        components.reserve(root_indices.size());
        for (std::size_t root_index : root_indices) {
            components.emplace_back(component_size(root_index), root_index);
        }
        // Filter out the `num` smallest/largest components
        auto nth = components.begin() + static_cast<decltype(components)::difference_type>(num);
        if (largest) {
            std::ranges::nth_element(components, nth, std::ranges::greater{});
        } else {
            std::ranges::nth_element(components, nth, std::ranges::less{});
        }
        components.resize(num);
        // Return roots only
        return components | std::views::transform([](Component const& component) { return component.root; }) |
               std::ranges::to<std::vector>();
    }

  private:
    const std::size_t element_count_{};
    std::size_t component_count_{};
    std::vector<std::size_t> sizes_{};
    std::vector<std::size_t> parents_{};
};

// Coroutine for generating all subsets of indices from a size
// in lexicographic order.
inline std::generator<std::span<const std::size_t>> powerset(std::size_t n) {
    std::vector<std::size_t> indices;
    indices.reserve(n);
    for (std::size_t k = 0; k <= n; ++k) {
        indices.resize(k);
        std::iota(indices.begin(), indices.end(), std::size_t{});
        auto next_combination = [&]() {
            for (std::size_t i = k; i-- > 0;) {
                if (indices[i] != i + n - k) {
                    indices[i]++;
                    for (std::size_t j = i + 1; j < k; j++) {
                        indices[j] = indices[j - 1] + 1;
                    }
                    return true;
                }
            }
            return false;  // exhausted all subsets
        };
        do {
            co_yield indices;
        } while (next_combination());
    }
}

// General sum and producct functions
auto sum(std::ranges::input_range auto&& r) {
    using R = decltype(r);
    using T = std::ranges::range_value_t<R>;
    return std::ranges::fold_left(std::forward<R>(r), T{}, std::plus{});
}
auto prod(std::ranges::input_range auto&& r) {
    using R = decltype(r);
    using T = std::ranges::range_value_t<R>;
    return std::ranges::fold_left(std::forward<R>(r), T{1}, std::multiplies{});
}

// Integer power function
template <std::integral Base, std::integral Exp>
[[nodiscard]] constexpr Base ipow(Base base, Exp exp) noexcept {
    assert(exp >= 0);
    Base result{1};
    while (exp > 0) {
        if (exp & 1) {
            result *= base;
        }
        base *= base;
        exp >>= 1;
    }
    return result;
}

// Combined concept for integral and floating-point
template <typename T>
concept arithmetic = std::integral<T> || std::floating_point<T>;

// Mathematical sign function, with possible return values {-1, 0, +1}
template <arithmetic T>
constexpr T sign(T x) {
    return (x > T{0}) - (x < T{0});
}

}  // namespace helper