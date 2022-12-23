
#include <array>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>


/* https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x */
inline void hash_combine([[maybe_unused]] std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, rest...);
}

template<>
struct std::hash<std::pair<int, int>> {
    std::size_t operator()(const std::pair<int, int>& p) const {
        std::size_t seed {0xDEADBEEF};
        hash_combine(seed, p.first, p.second);
        return seed;
    }
};


enum class Direction {
    NORTH,
    SOUTH,
    WEST,
    EAST
};


class direction_info {
    using enum Direction;

public:
    std::array<Direction, 4> order {NORTH, SOUTH, WEST, EAST};

    static constexpr std::array<std::pair<int, int>, 8> adjacent_deltas {{
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}
    }};

    /* Movements in (change in row, change in column) pairs. */
    const std::map<Direction, std::pair<int, int>> deltas;

    /* Required deltas to check before an elf moves. */
    const std::map<Direction, std::array<std::pair<int, int>, 3>> requirements;

    static std::map<Direction, std::pair<int, int>> init_deltas() {
        std::map<Direction, std::pair<int, int>> map {};
        map[NORTH] = {-1, 0}; // -1 is higher in terms of rows.
        map[SOUTH] = {1, 0};
        map[WEST] = {0, -1};
        map[EAST] = {0, 1};
        return map;
    }

    static std::map<Direction, std::array<std::pair<int, int>, 3>> init_requirements() {
        std::map<Direction, std::array<std::pair<int, int>, 3>> map {};
        map[NORTH] = {{{-1, 0}, {-1, -1}, {-1, 1}}};
        map[SOUTH] = {{{1, 0}, {1, -1}, {1, 1}}};
        map[WEST] = {{{0, -1}, {-1, -1}, {1, -1}}};
        map[EAST] = {{{0, 1}, {-1, 1}, {1, 1}}};
        return map;
    }

    direction_info() : deltas {init_deltas()}, requirements {init_requirements()}
    {}

    void cycle_order() {
        Direction first {order[0]};

        for (int i {0}; i < 3; i++) {
            order[i] = order[i+1];
        }

        order[3] = first;
    }
};

using positions = std::unordered_set<std::pair<int, int>>;

std::pair<int, int> next_position(const positions& positions, const direction_info& info, int row, int col) {
    /* Check if elf wants to move */
    bool wants_to_move {false};

    for (auto [d_r, d_c] : info.adjacent_deltas) {
        if (positions.contains({row + d_r, col + d_c})) {
            wants_to_move = true;
            break;
        }
    }

    if (!wants_to_move) {
        return {row, col};
    }

    /* Elf has decided to try move somewhere... */
    for (Direction dir : info.order) {
        bool can_move_in_dir {true};
        for (auto [d_r, d_c] : info.requirements.at(dir)) {
            if (positions.contains({row + d_r, col + d_c})) {
                can_move_in_dir = false;
                break;
            }
        }

        if (can_move_in_dir) {
            auto [d_r, d_c] {info.deltas.at(dir)};
            return {row + d_r, col + d_c};
        }
    }

    /* No proposed movement works, elf stays still. */
    return {row, col};
}

void update_positions(positions& elf_positions, direction_info& info) {
    std::unordered_map<std::pair<int, int>, int> counts {};

    for (auto [old_r, old_c] : elf_positions) {
        auto [new_r, new_c] {next_position(elf_positions, info, old_r, old_c)};

        counts[{new_r, new_c}]++; // On first access a zero is inserted, then incremented.
    }

    positions next_positions {};

    for (auto [old_r, old_c] : elf_positions) {
        auto [new_r, new_c] {next_position(elf_positions, info, old_r, old_c)};

        if (counts[{new_r, new_c}] == 1) {
            next_positions.insert(std::pair {new_r, new_c});
        }
        else {
            next_positions.insert({old_r, old_c}); // There would have been a collision.
        }
    }

    elf_positions = next_positions;
    info.cycle_order();
}


int main() {
    positions positions {};

    int row {0};
    for (std::string buf; std::getline(std::cin, buf); row++) {
        for (int col {0}; col < static_cast<int>(buf.size()); col++) {
            if (buf[col] == '#') {
                positions.insert({row, col});
            }
        }
    }

    direction_info info {};

    for (int i {0}; i < 10; i++) {
        update_positions(positions, info);
    }

    /* Calculate the rectangle */
    int min_r {1000000};
    int min_c {1000000};
    int max_r {-1000000};
    int max_c {-1000000};

    for (auto [row, col] : positions) {
        min_r = std::min(min_r, row); 
        min_c = std::min(min_c, col);
        max_r = std::max(max_r, row); 
        max_c = std::max(max_c, col);
    }

    int height {max_r - min_r + 1};
    int width {max_c - min_c + 1};

    int space {width * height - static_cast<int>(positions.size())};

    std::cout << space << '\n';
}
