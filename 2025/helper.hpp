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
#include <memory>
#include <numeric>
#include <ostream>
#include <print>
#include <ranges>
#include <ratio>
#include <source_location>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace aoc {

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
    // Read in answer file if present
    auto read_answers = []() -> std::vector<std::string> {
        std::filesystem::path path_answer{"answer"};
        if (!std::filesystem::is_regular_file(path_answer)) {
            return {};
        }
        std::string line_answer{};
        std::ifstream file{path_answer};
        std::getline(file, line_answer);
        auto tokens = line_answer | std::views::split(' ') |
                      std::views::filter([](auto&& token_part) { return !std::ranges::empty(token_part); }) |
                      std::views::transform([](auto&& token_part) { return std::ranges::to<std::string>(token_part); });
        return std::ranges::to<std::vector<std::string>>(tokens);
    };
    std::vector<std::string> answers = read_answers();
    assert(answers.size() <= 2);
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
    constexpr int width{33};
    std::string checkmark{" "};
    if (0 < part && part - 1 < answers.size()) {
        if (std::format("{}", result) == answers[part - 1]) {
            checkmark = "\x1b[32m✓\x1b[0m";
        } else {
            checkmark = "\x1b[31m✗\x1b[0m";
        }
    }
    std::array output{
        std::format("day {:>2}, part {}: {}", day, part, result),
        std::format("{} \x1b[2m{}{}{}\x1b[0m", checkmark, time_value, time_unit.empty() ? "" : " ", time_unit),
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

// Lookup functions for std::unordered_map and std::map
template <typename Map, typename Key>
auto lookup(Map& map, const Key& key) -> decltype(std::addressof(map.begin()->second)) {
    if (auto it = map.find(key); it != map.end()) {
        return std::addressof(it->second);
    }
    return nullptr;
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

// Rational (integer fraction) number type
template <std::integral T>
class Rational {
  public:
    using number_tag = void;  // for number concept

    // Default ctor: constructs 0/1
    Rational() = default;

    // 1-param ctor: constructs n/1
    constexpr Rational(T num) : num_(num) {}

    // 2-param ctor: constructs num/den (normalizes, throws if den == 0)
    constexpr Rational(T num, T den) : num_(num), den_(den) {
        if (den_ == T{}) {
            throw std::invalid_argument("Rational: denominator cannot be zero");
        }
        normalize();
    }

    // 2-param ctor for other types
    template <std::integral NUM, std::integral DEN>
    constexpr Rational(NUM num, DEN den) : Rational(T{num}, T{den}) {}

    // Cross-type ctor
    template <std::integral U>
    explicit constexpr Rational(Rational<U> other) : Rational(T{other.num()}, T{other.den()}) {}

    // Explicit conversion from Rational to integral or floating-point
    template <typename U>
        requires(std::integral<U> || std::floating_point<U>)
    [[nodiscard]] explicit constexpr operator U() const noexcept {
        return static_cast<U>(num_) / static_cast<U>(den_);
    }

    // Assignment operator from Rational<U> to Rational<T>
    template <std::integral U>
        requires(!std::same_as<U, T>)
    constexpr Rational& operator=(Rational<U> other) {
        *this = Rational<T>(other);
        return *this;
    }

    [[nodiscard]] constexpr T num() const noexcept { return num_; }
    [[nodiscard]] constexpr T den() const noexcept { return den_; }

    // --------------------------------
    // (Rational, Rational) arithmetics
    // --------------------------------

    template <std::integral U>
    [[nodiscard]] constexpr auto operator+(Rational<U> other) const {
        return Rational(num_ * other.den() + other.num() * den_, den_ * other.den());
    }

    [[nodiscard]] constexpr Rational operator+() const noexcept { return Rational(num_, den_); }

    template <std::integral U>
    [[nodiscard]] constexpr auto operator-(Rational<U> other) const {
        return Rational(num_ * other.den() - other.num() * den_, den_ * other.den());
    }

    [[nodiscard]] constexpr Rational operator-() const noexcept { return Rational(-num_, den_); }

    template <std::integral U>
    [[nodiscard]] constexpr auto operator*(Rational<U> other) const {
        return Rational(num_ * other.num(), den_ * other.den());
    }

    template <std::integral U>
    [[nodiscard]] constexpr auto operator/(Rational<U> other) const {
        if (other.num() == U{}) {
            throw std::invalid_argument("Rational: division by zero");
        }
        return Rational(num_ * other.den(), den_ * other.num());
    }

    // -------------------------------------
    // (Rational, std::integral) arithmetics
    // -------------------------------------

    template <std::integral U>
    [[nodiscard]] constexpr auto operator+(U value) const {
        return *this + Rational<U>(value);
    }

    template <std::integral U>
    [[nodiscard]] friend constexpr auto operator+(U lhs, Rational rhs) {
        return Rational<U>(lhs) + rhs;
    }

    template <std::integral U>
    [[nodiscard]] constexpr auto operator-(U value) const {
        return *this - Rational<U>(value);
    }

    template <std::integral U>
    [[nodiscard]] friend constexpr auto operator-(U lhs, Rational rhs) {
        // (lhs) - (rhs)
        return Rational<U>(lhs) - rhs;
    }

    template <std::integral U>
    [[nodiscard]] constexpr auto operator*(U value) const {
        return *this * Rational<U>(value);
    }

    template <std::integral U>
    [[nodiscard]] friend constexpr auto operator*(U lhs, Rational rhs) {
        return rhs * Rational<U>(lhs);
    }

    template <std::integral U>
    [[nodiscard]] constexpr auto operator/(U value) const {
        if (value == U{}) {
            throw std::invalid_argument("Rational: division by zero");
        }
        return *this / Rational<U>(value);
    }

    template <std::integral U>
    [[nodiscard]] friend constexpr auto operator/(U lhs, Rational rhs) {
        return Rational<U>(lhs) / rhs;
    }

    // -------------------
    // Compound assignment
    // -------------------

    template <std::integral U>
    constexpr Rational& operator+=(Rational<U> other) {
        *this = *this + other;
        return *this;
    }

    template <std::integral U>
    constexpr Rational& operator-=(Rational<U> other) {
        *this = *this - other;
        return *this;
    }

    template <std::integral U>
    constexpr Rational& operator*=(Rational<U> other) {
        *this = *this * other;
        return *this;
    }

    template <std::integral U>
    constexpr Rational& operator/=(Rational<U> other) {
        *this = *this / other;
        return *this;
    }

    // -----------
    // Comparisons
    // -----------

    template <std::integral U>
    [[nodiscard]] constexpr bool operator==(Rational<U> other) const noexcept {
        return num_ == other.num() && den_ == other.den();
    }

    template <std::integral U>
    [[nodiscard]] constexpr bool operator==(U value) const noexcept {
        return *this == Rational<U>(value);
    }

    template <std::integral U>
    constexpr std::strong_ordering operator<=>(Rational<U> other) const noexcept {
        // Beware of overflow here!
        return num_ * other.den() <=> other.num() * den_;
    }

    template <std::integral U>
    constexpr std::strong_ordering operator<=>(U value) const noexcept {
        return *this <=> Rational<U>(value);
    }

    [[nodiscard]] explicit constexpr operator bool() const noexcept { return num_ != T{}; }

    // --------------------------
    // Additional math operations
    // --------------------------

    [[nodiscard]] constexpr bool is_integral() const noexcept { return den_ == T{1}; }

    [[nodiscard]] constexpr Rational reciprocal() const { return Rational(den_, num_); }

    [[nodiscard]] constexpr Rational abs() const noexcept {
        if constexpr (std::signed_integral<T>) {
            return Rational(std::abs(num_), den_);
        } else {
            return *this;
        }
    }

    template <std::integral U>
    [[nodiscard]] constexpr Rational pow(U exponent) const {
        if (exponent == U{}) {
            if (*this == T{}) {
                throw std::invalid_argument("Rational::pow: 0^0 is undefined");
            }
            return Rational(T{1});
        }
        if (exponent < U{}) {
            return reciprocal().pow(-exponent);
        }
        return Rational(ipow(num_, exponent), ipow(den_, exponent));
    }

  private:
    constexpr void normalize() noexcept {
        if (num_ == T{}) {
            den_ = T{1};
            return;
        }
        if (den_ < T{}) {
            num_ = -num_;
            den_ = -den_;
        }
        const T gcd = std::gcd(num_, den_);
        num_ /= gcd;
        den_ /= gcd;
    }

    T num_{};
    T den_{1};
};

// CTAD: allow constructing Rational objects from mixed types
template <std::integral NUM, std::integral DEN>
Rational(NUM, DEN) -> Rational<std::common_type_t<NUM, DEN>>;

// Combined concept for number-like types
template <typename T>
concept number = std::integral<T> || std::floating_point<T> || requires { typename T::number_tag; };

// Mathematical sign function, with possible return values {-1, 0, +1}
template <number T>
constexpr T sign(T x) {
    return (x > T{}) - (x < T{});
}

// Matrix type in row-major order, dynamically sized
template <number T>
class Matrix {
  public:
    using value_type = T;

    // Default ctor: constructs an empty 0x0 matrix
    Matrix() = default;

    // 3-param ctor: constructs nrows x ncols matrix filled with `value`
    explicit Matrix(std::size_t nrows, std::size_t ncols, T value = T{})
        : nrows_(nrows), ncols_(ncols), data_(nrows * ncols, value) {}

    // Cross-type ctor
    template <number U>
    explicit Matrix(Matrix<U> const& other) : Matrix(other.nrows(), other.ncols()) {
        auto& mat = *this;
        for (std::size_t i = 0; i < nrows_; i++) {
            auto dst = mat.row(i);
            auto src = other.row(i);
            for (std::size_t j = 0; j < ncols_; j++) {
                dst[j] = src[j];
            }
        }
    }

    // Construct from columns
    template <typename Cols>
    static Matrix from_cols(Cols const& cols) {
        std::span outer{cols};
        if (outer.empty()) {
            return Matrix{};
        }
        auto first = std::span{outer[0]};
        using U = typename decltype(first)::value_type;
        const std::size_t ncols = outer.size();
        const std::size_t nrows = first.size();
        std::vector<std::span<const U>> colspans;
        colspans.reserve(ncols);
        for (std::size_t j = 0; j < ncols; j++) {
            std::span col{outer[j]};
            if (col.size() != nrows) {
                throw std::invalid_argument(
                    std::format("Matrix::from_cols: column {} has size {}, expected {}", j, col.size(), nrows)
                );
            }
            colspans.push_back(col);
        }
        Matrix mat(nrows, ncols);
        for (std::size_t i = 0; i < nrows; i++) {
            auto rr = mat.row(i);
            for (std::size_t j = 0; j < ncols; j++) {
                rr[j] = colspans[j][i];  // T ← U conversion
            }
        }
        return mat;
    }

    // Construct from single column
    template <typename Col>
    static Matrix from_col(Col const& col) {
        const std::array cols{std::span{col}};
        return from_cols(cols);
    }

    // Construct from rows
    template <typename Rows>
    static Matrix from_rows(Rows const& rows) {
        std::span outer{rows};
        if (outer.empty()) {
            return Matrix{};
        }
        auto first = std::span{outer[0]};
        using U = typename decltype(first)::value_type;
        const std::size_t nrows = outer.size();
        const std::size_t ncols = first.size();
        std::vector<std::span<const U>> rowspans;
        rowspans.reserve(nrows);
        for (std::size_t i = 0; i < nrows; i++) {
            std::span row{outer[i]};
            if (row.size() != ncols) {
                throw std::invalid_argument(
                    std::format("Matrix::from_rows: row {} has size {}, expected {}", i, row.size(), ncols)
                );
            }
            rowspans.push_back(row);
        }
        Matrix mat(nrows, ncols);
        for (std::size_t i = 0; i < nrows; i++) {
            auto rr = mat.row(i);
            auto src = rowspans[i];
            for (std::size_t j = 0; j < ncols; j++) {
                rr[j] = src[j];  // T ← U conversion
            }
        }
        return mat;
    }

    // Construct from single row
    template <typename Row>
    static Matrix from_row(Row const& row) {
        const std::array rows{std::span{row}};
        return from_rows(rows);
    }

    [[nodiscard]] constexpr std::size_t nrows() const noexcept { return nrows_; }
    [[nodiscard]] constexpr std::size_t ncols() const noexcept { return ncols_; }
    [[nodiscard]] constexpr bool empty() const noexcept { return nrows_ == 0 || ncols_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }

    // Element access: (i, j) in row-major order
    [[nodiscard]] T& operator()(std::size_t i, std::size_t j) noexcept {
        assert(i < nrows_ && j < ncols_);
        return data_[i * ncols_ + j];
    }
    [[nodiscard]] T const& operator()(std::size_t i, std::size_t j) const noexcept {
        assert(i < nrows_ && j < ncols_);
        return data_[i * ncols_ + j];
    }

    // Row view
    [[nodiscard]] std::span<T> row(std::size_t i) noexcept {
        assert(i < nrows_);
        return std::span<T>(data_).subspan(i * ncols_, ncols_);
    }
    [[nodiscard]] std::span<const T> row(std::size_t i) const noexcept {
        assert(i < nrows_);
        return std::span<const T>(data_).subspan(i * ncols_, ncols_);
    }

    // Raw data access
    [[nodiscard]] std::vector<T> const& data() const noexcept { return data_; }
    [[nodiscard]] std::vector<T>& data() noexcept { return data_; }

    // ---------------
    // Math operations
    // ---------------

    // Matrix-matrix multiplication
    template <number U>
    [[nodiscard]] auto operator*(Matrix<U> const& rhs) const {
        using V = decltype(T{} * U{});
        const auto& lhs = *this;
        if (lhs.ncols_ != rhs.nrows()) {
            throw std::invalid_argument(
                std::format(
                    "Matrix::operator*: dimension mismatch ({}x{}) * ({}x{})",
                    lhs.nrows_,
                    lhs.ncols_,
                    rhs.nrows(),
                    rhs.ncols()
                )
            );
        }
        Matrix<V> out(lhs.nrows_, rhs.ncols(), V{});
        const std::size_t rhs_ncols = rhs.ncols();
        for (std::size_t i = 0; i < lhs.nrows_; i++) {
            auto out_row = out.row(i);
            auto lhs_row = lhs.row(i);
            for (std::size_t k = 0; k < lhs.ncols_; k++) {
                const T lhsik = lhs_row[k];
                if (lhsik == T{}) {
                    continue;
                }
                auto rhs_row = rhs.row(k);
                for (std::size_t j = 0; j < rhs_ncols; j++) {
                    out_row[j] += lhsik * rhs_row[j];
                }
            }
        }
        return out;
    }

    // In-place matrix-matrix multiplication
    template <number U>
    Matrix& operator*=(Matrix<U> const& rhs) {
        const auto& lhs = *this;
        *this = Matrix<T>(lhs * rhs);
        return *this;
    }

    // Matrix-scalar multiplication
    template <number S>
    [[nodiscard]] auto operator*(S scalar) const {
        using V = decltype(T{} * S{});
        const auto& lhs = *this;
        Matrix<V> out(lhs.nrows_, lhs.ncols_, V{});
        for (std::size_t i = 0; i < lhs.nrows_; i++) {
            auto out_row = out.row(i);
            auto lhs_row = lhs.row(i);
            for (std::size_t j = 0; j < lhs.ncols_; j++) {
                out_row[j] = lhs_row[j] * scalar;
            }
        }
        return out;
    }

    // Scalar-matrix multiplication
    template <number S>
    [[nodiscard]] friend auto operator*(S scalar, Matrix const& rhs) {
        return rhs * scalar;
    }

    // In-place multiplication by scalar
    template <number S>
    Matrix& operator*=(S scalar) {
        const T factor{scalar};
        for (T& x : data_) {
            x *= factor;
        }
        return *this;
    }

    // --------------------------------
    // Other matrix specific operations
    // -------------------------------

    // Return autmented matrix [*this | other]
    template <number U>
    [[nodiscard]] Matrix augment_right(Matrix<U> const& other) const {
        const auto& mat = *this;
        if (mat.nrows_ != other.nrows()) {
            throw std::invalid_argument(
                std::format("Matrix::augment_right: nrows mismatch ({} vs {})", mat.nrows_, other.nrows())
            );
        }
        Matrix out(mat.nrows_, mat.ncols_ + other.ncols(), T{});
        const std::size_t lhs_ncols = mat.ncols_;
        const std::size_t rhs_ncols = other.ncols();
        for (std::size_t i = 0; i < mat.nrows_; i++) {
            auto dst = out.row(i);
            auto lhs = mat.row(i);
            auto rhs = other.row(i);
            for (std::size_t j = 0; j < lhs_ncols; j++) {
                dst[j] = lhs[j];
            }
            for (std::size_t j = 0; j < rhs_ncols; j++) {
                dst[lhs_ncols + j] = rhs[j];  // T ← U conversion
            }
        }
        return out;
    }

    // ------------------------
    // Reduced row echelon form
    // ------------------------

    struct PivotInfo {
        static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();
        std::size_t rank{};
        std::vector<std::size_t> pivot_col_of_row{};  // size = nrows, npos if no pivot in row
        std::vector<std::size_t> pivot_row_of_col{};  // size = ncols, npos if column is free
        PivotInfo() = default;
        explicit PivotInfo(std::size_t nrows, std::size_t ncols)
            : pivot_col_of_row(nrows, npos), pivot_row_of_col(ncols, npos) {}
    };

    // Swap two rows
    void swap_rows(std::size_t i_a, std::size_t i_b) noexcept {
        assert(i_a < nrows_ && i_b < nrows_);
        if (i_a == i_b) {
            return;
        }
        auto row_a = row(i_a);
        auto row_b = row(i_b);
        for (std::size_t j = 0; j < ncols_; j++) {
            std::swap(row_a[j], row_b[j]);
        }
    }

    // Multiply row by factor (throws if factor == 0)
    void scale_row(std::size_t i, T factor) {
        assert(i < nrows_);
        if (factor == T{}) {
            throw std::invalid_argument("Matrix::scale_row: factor cannot be zero");
        }
        for (T& x : row(i)) {
            x *= factor;
        }
    }

    // Add scaled row to another row
    void add_scaled_row(std::size_t dst, std::size_t src, T factor) noexcept {
        assert(dst < nrows_ && src < nrows_);
        if (factor == T{}) {
            return;
        }
        auto d = row(dst);
        auto s = row(src);
        for (std::size_t j = 0; j < ncols_; j++) {
            d[j] += s[j] * factor;
        }
    }

    // In-place RREF (Gauss–Jordan), returns pivot metadata
    [[nodiscard]] PivotInfo rref() {
        static_assert(!std::integral<T>, "Matrix::rref requires non-truncating division; prefer Matrix<Rational<...>>");
        PivotInfo info(nrows_, ncols_);
        if (empty()) {
            return info;
        }
        auto& mat = *this;
        std::size_t i{};
        for (std::size_t j = 0; j < ncols_ && i < nrows_; j++) {
            // Find pivot row i_p >= i with mat(i_p, j) != 0
            std::size_t i_p = i;
            while (i_p < nrows_ && mat(i_p, j) == T{}) {
                i_p++;
            }
            if (i_p == nrows_) {
                continue;  // free column
            }
            swap_rows(i, i_p);
            // Normalize pivot to 1
            const T pivot = mat(i, j);
            scale_row(i, T{1} / pivot);
            assert(mat(i, j) == T{1});
            // Eliminate column j in all other rows
            for (std::size_t ii = 0; ii < nrows_; ii++) {
                if (ii == i) {
                    continue;
                }
                const T f = mat(ii, j);
                if (f != T{}) {
                    add_scaled_row(ii, i, -f);
                    assert(mat(ii, j) == T{});
                }
            }
            info.pivot_col_of_row[i] = j;
            info.pivot_row_of_col[j] = i;
            info.rank++;
            i++;
        }
        return info;
    }

  private:
    std::size_t nrows_{};
    std::size_t ncols_{};
    std::vector<T> data_{};
};

}  // namespace aoc

// Specializations of std templates
namespace std {

// Hashing of Rational objects
template <std::integral T>
struct hash<aoc::Rational<T>> {
    [[nodiscard]] constexpr std::size_t operator()(aoc::Rational<T> const& rational) const noexcept {
        return aoc::ArrayHash{}({rational.num(), rational.den()});
    }
};

// Formatting of Rational objects
template <std::integral T>
struct formatter<aoc::Rational<T>> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(aoc::Rational<T> const& r, FormatContext& ctx) const {
        if (r.den() == T{1}) {
            return std::format_to(ctx.out(), "{}", r.num());
        }
        return std::format_to(ctx.out(), "{}/{}", r.num(), r.den());
    }
};

template <aoc::number T>
struct formatter<aoc::Matrix<T>> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    template <typename FormatContext>
    auto format(aoc::Matrix<T> const& mat, FormatContext& ctx) const {
        const std::size_t nrows = mat.nrows();
        const std::size_t ncols = mat.ncols();
        if (nrows == 0 || ncols == 0) {
            return std::format_to(ctx.out(), "[]");
        }
        auto out = ctx.out();
        out = std::format_to(out, "[");
        for (std::size_t i = 0; i < nrows; i++) {
            if (i != 0) {
                out = std::format_to(out, "\n ");
            }
            out = std::format_to(out, "[");
            for (std::size_t j = 0; j < ncols; j++) {
                if (j != 0) {
                    out = std::format_to(out, ", ");
                }
                out = std::format_to(out, "{}", mat(i, j));
            }
            out = std::format_to(out, "]");
            if (i + 1 != nrows) {
                out = std::format_to(out, ",");
            }
        }
        out = std::format_to(out, "]");
        return out;
    }
};

}  // namespace std
