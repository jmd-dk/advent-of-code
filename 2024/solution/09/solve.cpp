#include <algorithm>
#include <list>

#include "../../magic.hpp"

using Int = std::int64_t;
struct Block {
    static constexpr Int id_space = -1;
    Int size;
    Int id = id_space;
    bool is_file() const { return id != id_space; }
    void free() { id = id_space; }
};

// Reading in data
std::list<Block> read() {
    std::list<Block> diskmap;
    for (const auto& line : LineReader("input")) {
        for (auto [twice_id, c] : std::views::enumerate(line)) {
            Int size = c - '0';
            if (size == 0) {
                continue;
            }
            if (twice_id & 1) {
                diskmap.push_back({size});
            } else {
                diskmap.push_back({size, static_cast<Int>(twice_id / 2)});
            }
        }
    }
    return diskmap;
}

// Solution to part one
Int solve_one(std::list<Block>& diskmap) {
    for (auto it_space = diskmap.begin(); it_space != diskmap.end(); it_space++) {
        Block space = *it_space;
        if (space.is_file()) {
            continue;
        }
        for (auto it_file = --diskmap.end(); it_file != it_space; it_file--) {
            Block file = *it_file;
            if (not file.is_file()) {
                continue;
            }
            if (it_space == --it_file) {
                // Final move
                it_space = diskmap.erase(it_space, diskmap.end());
                it_space = diskmap.insert(it_space, file);
                break;
            }
            ++it_file;
            // Pop space and file block
            it_space = diskmap.erase(it_space);
            it_file = diskmap.erase(it_file);
            // Insert file blocks
            Int size = std::min(space.size, file.size);
            it_space = diskmap.insert(it_space, {size, file.id});
            if (size < file.size) {
                it_file = diskmap.insert(it_file, {file.size - size, file.id});
            }
            // Insert space block
            if (size < space.size) {
                it_space = --diskmap.insert(++it_space, {space.size - size});
            }
            break;
        }
    }
    Int checksum = 0;
    Int index = 0;
    for (const auto& block : diskmap) {
        checksum += block.id * block.size * (2 * index + block.size - 1) / 2;
        index += block.size;
    }
    return checksum;
}

// Solution to part two
Int solve_two(std::list<Block>& diskmap) {
    for (auto it_file = --diskmap.end(); it_file != diskmap.begin(); it_file--) {
        Block file = *it_file;
        if (not file.is_file()) {
            continue;
        }
        for (auto it_space = diskmap.begin(); it_space != it_file; it_space++) {
            Block space = *it_space;
            if (space.is_file()) {
                continue;
            }
            if (space.size < file.size) {
                continue;
            }
            it_space = diskmap.erase(it_space);
            it_space = diskmap.insert(it_space, file);
            Int size = space.size - file.size;
            if (size > 0) {
                it_space = --diskmap.insert(++it_space, {size});
            }
            it_file->free();
            break;
        }
    }
    Int checksum = 0;
    Int index = 0;
    for (const auto& block : diskmap) {
        if (block.is_file()) {
            checksum += block.id * block.size * (2 * index + block.size - 1) / 2;
        }
        index += block.size;
    }
    return checksum;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
