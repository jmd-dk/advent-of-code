#include <array>
#include <ranges>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../magic.hpp"

using Int = std::int32_t;
using Position = std::array<Int, 2>;
using Map = std::unordered_map<Position, char, ArrayHash<Int>>;
using Movements = std::vector<char>;
using Data = std::tuple<Position, Map, Movements>;

namespace Values {
constexpr char Robot = '@';
constexpr char Space = '.';
constexpr char Wall = '#';
constexpr char Box = 'O';
constexpr char Up = '^';
constexpr char Down = 'v';
constexpr char Left = '<';
constexpr char Right = '>';
constexpr char BoxLeft = '[';
constexpr char BoxRight = ']';
}  // namespace Values

// Reading in data
Data read() {
    Position robot{};
    Map map;
    Movements movements;
    bool at_map = true;
    Int i = -1;
    for (const auto& line : LineReader("input")) {
        if (line.empty()) {
            at_map = false;
            continue;
        }
        if (at_map) {
            i++;
            for (auto [_j, c] : std::views::enumerate(line)) {
                Int j = static_cast<Int>(_j);
                if (c == Values::Wall or c == Values::Box) {
                    map[{i, j}] = c;
                } else if (c == Values::Robot) {
                    robot[0] = i;
                    robot[1] = j;
                }
            }
        } else {
            movements.insert(movements.end(), line.begin(), line.end());
        }
    }
    return {robot, map, movements};
}

// Solution to part one
Int solve_one(Data& data) {
    auto& [robot, map, movements] = data;
    auto lookup = [](const Map& map, const Position& position) {
        if (auto it = map.find(position); it != map.end()) {
            auto [_, obj] = *it;
            return obj;
        } else {
            return Values::Space;
        }
    };
    auto get_position_next = [](Position position, char movement) {
        switch (movement) {
            case Values::Up:
                position[0] -= 1;
                break;
            case Values::Down:
                position[0] += 1;
                break;
            case Values::Left:
                position[1] -= 1;
                break;
            case Values::Right:
                position[1] += 1;
                break;
        }
        return position;
    };
    auto move = [&](Map& map, Position robot, char movement) {
        Position robot_next = get_position_next(robot, movement);
        char tile_next = lookup(map, robot_next);
        if (tile_next == Values::Space) {
            // Free path
            return robot_next;
        }
        if (tile_next == Values::Wall) {
            // Wall in the way
            return robot;
        }
        // Box in the way
        Position box = robot_next;
        char tile_box = tile_next;
        while (tile_box == Values::Box) {
            box = get_position_next(box, movement);
            tile_box = lookup(map, box);
        }
        if (tile_box == Values::Space) {
            // Push line of boxes (teleport first box to the end)
            map.erase(robot_next);
            map[box] = Values::Box;
            return robot_next;
        } else {
            // Wall at end of line of boxes
            return robot;
        }
    };
    auto compute_gps = [](const Position& position) {
        constexpr Int factor = 100;
        return factor * position[0] + position[1];
    };
    for (char movement : movements) {
        robot = move(map, robot, movement);
    }
    Int gps_tot = 0;
    for (const auto& [position, obj] : map) {
        if (obj == Values::Box) {
            gps_tot += compute_gps(position);
        }
    }
    return gps_tot;
}

// Solution to part two
Int solve_two(Data& data) {
    auto& [robot, map, movements] = data;
    auto widen = [](Position& robot, Map& map) {
        // Widen robot position
        robot[1] *= 2;
        // Widen map
        Map map_wide;
        for (const auto& [position, obj] : map) {
            Position left = {position[0], 2 * position[1] + 0};
            Position rght = {position[0], 2 * position[1] + 1};
            if (obj == Values::Box) {
                map_wide[left] = Values::BoxLeft;
                map_wide[rght] = Values::BoxRight;
            } else {
                map_wide[left] = obj;
                map_wide[rght] = obj;
            }
        }
        map = std::move(map_wide);
    };
    auto lookup = [](const Map& map, const Position& position) {
        if (auto it = map.find(position); it != map.end()) {
            auto [_, obj] = *it;
            return obj;
        } else {
            return Values::Space;
        }
    };
    auto get_position_next = [](Position position, char movement) {
        switch (movement) {
            case Values::Up:
                position[0] -= 1;
                break;
            case Values::Down:
                position[0] += 1;
                break;
            case Values::Left:
                position[1] -= 1;
                break;
            case Values::Right:
                position[1] += 1;
                break;
        }
        return position;
    };
    std::function<bool(const Map& map, const Position& box, char movement, Map& moved)>
        can_move;
    can_move = [&](const Map& map, const Position& box, char movement, Map& moved) {
        if (moved.contains(box)) {
            return true;
        }
        char tile = lookup(map, box);
        if (tile == Values::Space) {
            return true;
        } else if (tile == Values::Wall) {
            return false;
        }
        moved[box] = tile;
        Position box_next = get_position_next(box, movement);
        if (movement == Values::Left or movement == Values::Right) {
            // Check horizontal move
            return can_move(map, box_next, movement, moved);
        } else {
            // Check vertical move
            if (tile == Values::BoxLeft) {
                Position box_rght{box[0], box[1] + 1};
                Position box_next_rght{box_next[0], box_next[1] + 1};
                return can_move(map, box_rght, movement, moved) and
                       can_move(map, box_next, movement, moved) and
                       can_move(map, box_next_rght, movement, moved);
            } else if (tile == Values::BoxRight) {
                Position box_left{box[0], box[1] - 1};
                Position box_next_left{box_next[0], box_next[1] - 1};
                return can_move(map, box_left, movement, moved) and
                       can_move(map, box_next, movement, moved) and
                       can_move(map, box_next_left, movement, moved);
            }
        }
        throw std::runtime_error("Control flow should never get here");
    };
    auto move = [&](Map& map, Position robot, char movement) {
        Position robot_next = get_position_next(robot, movement);
        char tile_next = lookup(map, robot_next);
        if (tile_next == Values::Space) {
            // Free path
            return robot_next;
        }
        if (tile_next == Values::Wall) {
            // Wall in the way
            return robot;
        }
        // Box in the way
        Map moved;
        if (can_move(map, robot_next, movement, moved)) {
            for (const auto& [box, tile] : moved) {
                map.erase(box);
            }
            for (const auto& [box, tile] : moved) {
                Position box_next = get_position_next(box, movement);
                map[box_next] = tile;
            }
            return robot_next;
        }
        return robot;
    };
    auto compute_gps = [](const Position& position) {
        constexpr Int factor = 100;
        return factor * position[0] + position[1];
    };
    widen(robot, map);
    for (char movement : movements) {
        robot = move(map, robot, movement);
    }
    Int gps_tot = 0;
    for (const auto& [position, obj] : map) {
        if (obj == Values::BoxLeft) {
            gps_tot += compute_gps(position);
        }
    }
    return gps_tot;
}

// Solve
int main() {
    analyze(read, solve_one);
    analyze(read, solve_two);
    return 0;
}
