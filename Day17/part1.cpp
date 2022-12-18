
#include <iostream>
#include <vector>

enum class Direction {
    LEFT,
    RIGHT,
    DOWN,
};

class Rock {
public:
    // These parts should be positioned such that the leftmost x coordinate and the
    // bottom y coordinate are one. coordinates are stored (y, x) for compatibility
    // with the grid below.
    std::vector<std::pair<int, int>> parts; 

    Rock() : parts {} 
    {}
};

class Grid {
public:
    int highest_rock {0};
    // Indexed like (y, x), or (row, col). This makes adding rows easy. The width is 7.
    std::vector<std::vector<bool>> tiles {};

    Grid() {
        tiles.push_back(std::vector(7, true)); // The bottom layer is rock for simplicity.
    }

    // Moves the rock to its starting position. Grows the grid if necessary.
    void init_rock(Rock& rock) {
        int max_row {0};
        for (auto& [r, c] : rock.parts) {
            c += 2;
            r += highest_rock + 4;

            max_row = std::max(max_row, r);
        }

        // Increase grid as needed
        while (max_row >= static_cast<int>(tiles.size())) {
            tiles.push_back(std::vector(7, false));
        }
    }

    // returns true iff (row, col) is empty and in the grid.
    bool is_empty(int row, int col) {
        if (row < 0 || row >= static_cast<int>(tiles.size())) {
            return false;
        }
        if (col < 0 || col >= 7) {
            return false;
        }
        return !tiles[row][col];
    }

    /* Returns true if and only if the rock moved successfully. 
     * grid[i][j] is true iff there is set rock at i or j. */
    bool try_move([[maybe_unused]] Rock& rock, [[maybe_unused]] Direction dir) {
        // Move the rock if possible.

        int d_col = 0;
        int d_row = 0;

        if (dir == Direction::DOWN) {
            d_row = -1;
        }
        else if (dir == Direction::LEFT) {
            d_col = -1;
        }
        else if (dir == Direction::RIGHT) {
            d_col = 1;
        }


        for (auto [r, c] : rock.parts) {
            if (!is_empty(r + d_row, c + d_col)) {
                return false;
            }
        } 

        // The move is safe. Now we make the move;
        for (auto& [r, c] : rock.parts) {
            r += d_row;
            c += d_col;
        } 

        return true;
    }

    // Updates the total number of gusts that were used.
    void drop_rock(Rock& rock, const std::string gusts, int& gusts_used) {
        while (true) {
            // First a gust.
            char gust {gusts[gusts_used % gusts.size()]};
            gusts_used++;
            
            if (gust == '<') {
                try_move(rock, Direction::LEFT);
            }
            else if (gust == '>') {
                try_move(rock, Direction::RIGHT);
            }

            if (!try_move(rock, Direction::DOWN)) {
                break;
            }
        }

        // Now we freeze the rock in place.
        for (auto [r, c] : rock.parts) {
            tiles[r][c] = true;

            highest_rock = std::max(highest_rock, r);
        }
    }

    // Returns the height of the tower after dropping `count` rocks.
    int drop_rocks(const std::vector<Rock>& rocks, const std::string gusts, int count) {
        int gusts_used {0}; // Note: This is captured by reference and modified by drop_rock()
        for (int i {0}; i < count; i++) {
            Rock rock {rocks[i % rocks.size()]};
            
            init_rock(rock);

            drop_rock(rock, gusts, gusts_used);
        }

        return highest_rock;
    }

    void print_tiles() {
        std::cout << "-------\n";
        for (int r {static_cast<int>(tiles.size()) - 1}; r >=0; r--) {
            for (int c {0}; c < 7; c++) {
                std::cout << (tiles[r][c] ? '#' : ' ');
            }
            std::cout << '\n';
        }
    }

};


int main() {
    std::string gusts;
    std::cin >> gusts;

    std::vector<Rock> rocks {};

    // horizontal line rock.
    Rock next_rock {}; 
    next_rock.parts.push_back({0, 0}); // (y, x) :(
    next_rock.parts.push_back({0, 1});
    next_rock.parts.push_back({0, 2});
    next_rock.parts.push_back({0, 3});
    rocks.push_back(next_rock);

    // plus sign rock
    next_rock = {}; 
    next_rock.parts.push_back({1, 1});
    next_rock.parts.push_back({1, 0});
    next_rock.parts.push_back({1, 2});
    next_rock.parts.push_back({0, 1});
    next_rock.parts.push_back({2, 1});
    rocks.push_back(next_rock);

    // backwards L rock
    next_rock = {}; 
    next_rock.parts.push_back({0, 0});
    next_rock.parts.push_back({0, 1});
    next_rock.parts.push_back({0, 2});
    next_rock.parts.push_back({1, 2});
    next_rock.parts.push_back({2, 2});
    rocks.push_back(next_rock);

    // vertical line rock
    next_rock = {}; 
    next_rock.parts.push_back({0, 0});
    next_rock.parts.push_back({1, 0});
    next_rock.parts.push_back({2, 0});
    next_rock.parts.push_back({3, 0});
    rocks.push_back(next_rock);

    // square rock
    next_rock = {}; 
    next_rock.parts.push_back({0, 0});
    next_rock.parts.push_back({1, 0});
    next_rock.parts.push_back({0, 1});
    next_rock.parts.push_back({1, 1});
    rocks.push_back(next_rock);

    Grid grid {};

    std::cout << grid.drop_rocks(rocks, gusts, 2022) << '\n';
}
