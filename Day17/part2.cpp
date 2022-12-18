// This one is special. It only works on my input. READ THE COMMENTS - they show
// you how to discover the length of cycles in the tower behavior, and the height
// added. Once you know those, you can update the magic numbers for your input.
//
// I contend that this is faster than writing code to analyze for cycles itself,
// as that could take quite a while and is probably error prone (and computationally expensive).

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
    std::vector<std::pair<long long, long long>> parts; 

    Rock() : parts {} 
    {}
};

class Grid {
public:
    long long highest_rock {0};
    // Indexed like (y, x), or (row, col). This makes adding rows easy. The width is 7.
    std::vector<std::vector<bool>> tiles {};

    Grid() {
        tiles.push_back(std::vector(7, true)); // The bottom layer is rock for simplicity.
    }

    // Moves the rock to its starting position. Grows the grid if necessary.
    void init_rock(Rock& rock) {
        long long max_row {0};
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
    void drop_rock(Rock& rock, const std::string& gusts, long long& gusts_used) {
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

    // Returns the heights after different time steps.
    long long drop_rocks(const std::vector<Rock>& rocks, const std::string& gusts, long long count) {
        long long gusts_used {0};

        long long secret_height {0}; // We are gonna skip some cycles. The simulation won't know, but we will track some extra height.
        for (long long i {0}; i < count; i++) {
            Rock rock {rocks[i % rocks.size()]};

            // This will reveal a cycle. To find 6, print out gust_num between 0 and 100. Try millions of iters.
            //
            // int gust_num {static_cast<int>(gusts_used % gusts.size())};
            
            // if (gust_num == 6) { // A visual check is a good idea.
            //     std::cout << "-------\n";
            //     for (int r {static_cast<int>(tiles.size()) - 1}; r >= static_cast<int>(tiles.size()) - 14; r--) {
            //         for (int c {0}; c < 7; c++) {
            //             std::cout << (tiles[r][c] ? '#' : ' ');
            //         }
            //         std::cout << '\n';
            //     }

            //     std::cout << i << '\n'; // This lets you know the cycle length.
            // }
            
            init_rock(rock);

            drop_rock(rock, gusts, gusts_used);
            
            // Adding this will let you know the height per cycle.
            // if (gust_num == 6) {
            //     std::cout << " height = " << highest_rock << '\n';
            // }



            // We are well within the cylce now. The cylce has length 1700 and each cycle adds height 2623.
            // Lets skip some iterations.

            // In retrospect this was needlessly silly. We could choose the i we
            // were interested in so that we don't need to restart the simulation.
            if (i == 999596) {
                while (i + 1700 < count) {
                    i += 1700;
                    secret_height += 2623;
                }
            } // We now let the simulation continue in peace.
        }

        return highest_rock + secret_height;
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

    long long iterations {1000000000000};
    // std::cout << grid.drop_rocks(rocks, gusts, iterations) << '\n';

    // [[maybe_unused]] std::vector<long long> heights {grid.drop_rocks(rocks, gusts, 1000000000000)};
    std::cout << grid.drop_rocks(rocks, gusts, iterations) << '\n';

    // We will scan for lines in this, which we can interpret as patterns.
}
