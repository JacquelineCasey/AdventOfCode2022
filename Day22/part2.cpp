/* 
 * Alright, here's the plan.
 * Reuse as much of the old code as possible by staying on the 2D map. We then 
 * create fold_info objects that tells the system how to process walking off the
 * edges. These will need to be able to figure out which face of the cube you are
 * on, and also how those faces link up. I think I will build these objects by
 * hand.
 */

#include <iostream>
#include <map>
#include <vector>
#include <chrono>
#include <thread> // I did some visualizing for a second, required thread and time libraries.


using grid = std::vector<std::vector<char>>;

// It will be convenient to add a layer of spaces around the input, both for index
// calculation and for preventing index out of bounds.
grid as_grid(std::vector<std::string> lines) {
    /* First, pad the lines vertically, by adding two new lines. */
    lines.insert(lines.begin(), "");
    lines.push_back({});

    /* Next, standardize the width. */
    int max_width {-1};
    for (std::string& line : lines) {
        max_width = std::max(max_width, static_cast<int>(line.size()));
    }

    for (std::string& line : lines) {
        while (static_cast<int>(line.size()) < max_width) {
            line.push_back(' ');
        }
    }

    /* Now we pad the lines horizontally. */
    for (std::string& line : lines) {
        line = " " + line + " ";
    }

    /* Finally, we construct the grid proper. */
    grid grid (lines.size(), std::vector(lines[0].size(), ' '));
    for (int i {0}; i < static_cast<int>(grid.size()); i++) {
        for (int j {0}; j < static_cast<int>(grid[i].size()); j++) {
            grid[i][j] = lines[i][j];
        }
    }

    return grid;
}

void print_grid(const grid& grid) {
    for (auto& row : grid) {
        for (auto ch : row) {
            std::cout << ch;
        }
        std::cout << '\n';
    }
}


using instructions = std::vector<std::variant<int, char>>;

instructions as_instructions(std::string line) {
    int curr_num {0};

    instructions instructions {};

    for (char ch : line) {
        if (!isdigit(ch)) {
            if (curr_num > 0) {
                instructions.push_back(curr_num);
                curr_num = 0;
            }
            instructions.push_back(ch);
        }
        else {
            curr_num *= 10;
            curr_num += ch - '0';
        }
    }

    if (curr_num > 0) {
        instructions.push_back(curr_num);
    }

    return instructions;
}

void print_instructions(const instructions& instructions) {
    for (auto instruction : instructions) {
        if (std::holds_alternative<char>(instruction)) {
            std::cout << std::get<char>(instruction) << ' ';
        }
        else {
            std::cout << std::get<int>(instruction) << ' ';
        }
    }
    std::cout << '\n';
}


enum class direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

direction rotate_right(direction dir) {
    switch (dir) {
    case direction::UP:
        return direction::RIGHT;
    case direction::RIGHT:
        return direction::DOWN;
    case direction::DOWN:
        return direction::LEFT;
    case direction::LEFT:
        return direction::UP;
    }
}

direction rotate_left(direction dir) {
    switch (dir) {
    case direction::UP:
        return direction::LEFT;
    case direction::RIGHT:
        return direction::UP;
    case direction::DOWN:
        return direction::RIGHT;
    case direction::LEFT:
        return direction::DOWN;
    }
}

direction inverse(direction dir) {
    switch (dir) {
    case direction::UP:
        return direction::DOWN;
    case direction::RIGHT:
        return direction::LEFT;
    case direction::DOWN:
        return direction::UP;
    case direction::LEFT:
        return direction::RIGHT;
    }
}

// Returns a pair (change_in_row, change_in_col);
std::pair<int, int> as_delta(direction dir) {
    switch (dir) {
    case direction::UP:
        return {-1, 0};
    case direction::RIGHT:
        return {0, 1};
    case direction::DOWN:
        return {1, 0};
    case direction::LEFT:
        return {0, -1};
    }
}

int as_int(direction dir) {
    switch (dir) {
    case direction::UP:
        return 3;
    case direction::RIGHT:
        return 0;
    case direction::DOWN:
        return 1;
    case direction::LEFT:
        return 2;
    }
}

// location and direction, techincally.
struct location {
    int row;
    int col;
    direction dir;
};

void print_grid_with_loc(const grid& grid, const location& loc) {
    for (int r {0}; r < static_cast<int>(grid.size()); r++) {
        for (int c {0}; c < static_cast<int>(grid[0].size()); c++) {
            if (r == loc.row && c == loc.col) {
                std::cout << "😀";
            }
            else {
                std::cout << grid[r][c];
            }
        }
        std::cout << '\n';
    }
}

class fold_info {
public:
    // Chop the grid up into large squares the size of a cube face, and consider
    // this grid to be 0 based. This map tells you which cube face you are one
    // [1...6]. 
    // Keys are (row, col) on the big grid.
    std::map<std::pair<int, int>, int> cube_locations {};
    std::map<int, std::pair<int, int>> reverse_cube_locations {}; // Reverse of previous map.

    // These map face direction pairs to each other. Pre finalize(), this will hold
    // 7 pairs, one per pair of faces that meet. Post finalize(), it will hold
    // 14, 2 per pair of faces that meet (one in each direction).
    std::map<std::pair<int, direction>, std::pair<int, direction>> folds;

    int side_length {};

    // I will encode some information, and finalize() will infer the rest.
    void finalize() {
        /* Build reverse_cube_locations out of cube_locations. */
        for (auto [key, val] : cube_locations) {
            reverse_cube_locations[val] = key;
        }

        decltype(folds) new_folds {};

        for (auto [key, val] : folds) {
            new_folds[key] = val;
            new_folds[val] = key;
        }

        folds = new_folds;
    }

    int get_face(int row, int col) const {
        // Let's go back to 0 based.
        row--;
        col--; 

        // Now its a simple lookup.
        return cube_locations.at({row / side_length, col / side_length});
    }

    int get_face(const location& loc) const {
        return get_face(loc.row, loc.col); 
    }

    std::pair<int, int> get_local_coords(int row, int col) const {
        row--;
        col--;

        return {row % side_length, col % side_length};
    }

    std::pair<int, int> get_local_coords(const location& loc) const {
        return get_local_coords(loc.row, loc.col); 
    }

    std::pair<int, int> get_map_coords(int face, int row, int col) const {
        auto [big_grid_r, big_grid_c] {reverse_cube_locations.at(face)};

        return {row + side_length * big_grid_r + 1, col + side_length * big_grid_c + 1}; // Back to 1 based.
    }

    // Strictly in terms of local coordinates, steps across a fold.
    std::pair<int, int> step_across(direction dir1, direction dir2, int row, int col) const {
        using enum direction;

        if (dir2 == inverse(dir1)) {
            // No reorienting. Just take the step.
            auto [d_r, d_c] = as_delta(dir1);
            row += d_r;
            col += d_c;
            row = (row + side_length) % side_length;
            col = (col + side_length) % side_length;
            
            return {row, col};
        }
        if (dir2 == dir1) {
            if (dir1 == UP || dir1 == DOWN) {
                return {row, (side_length - 1) - col};
            }
            else {
                return {(side_length - 1) - row, col};
            }
        }
        else if (dir1 == UP && dir2 == RIGHT) {
            return {(side_length - 1) - col, (side_length - 1) - row};
        }
        else if (dir1 == RIGHT && dir2 == UP) {
            return {(side_length - 1) - col, (side_length - 1) - row};
        }
        else if (dir1 == UP && dir2 == LEFT) {
            return {col, row};
        }
        else if (dir1 == LEFT && dir2 == UP) {
            return {col, row};
        }
        else if (dir1 == DOWN && dir2 == RIGHT) {
            return {col, row};
        }
        else if (dir1 == RIGHT && dir2 == DOWN) {
            return {col, row};
        }
        else if (dir1 == DOWN && dir2 == LEFT) {
            return {(side_length - 1) - col, (side_length - 1) - row};
        }
        else if (dir1 == LEFT && dir2 == DOWN) {
            return {(side_length - 1) - col, (side_length - 1) - row};
        }

        std::cout << "Something went wrong...\n";
        return {-100, -100};
    }
};


location init_location(const grid& grid) {
    location loc {.row = 1, .col = 0, .dir = direction::RIGHT};

    // We move right until we are on a .
    while (grid[1][loc.col] != '.') {
        loc.col++;
    }

    return loc;
}

// Returns the coordinates on the map of the next would be move. Handles the
// complicated wrap around logic. The next position may be a wall.
location next_position(const grid& grid, const fold_info& info, const location& loc) {
    int row {loc.row};
    int col {loc.col};
    auto [d_r, d_c] {as_delta(loc.dir)};

    /* This is still the happy case: No folding shenanigans. */
    if (grid[row + d_r][col + d_c] != ' ') {
        return {.row = row + d_r, .col = col + d_c, .dir = loc.dir};
    }

    /* Let the folding commence. */
    int face_num {info.get_face(loc)};
    auto [new_face, face_dir] {info.folds.at({face_num, loc.dir})};
    auto [local_row, local_col] {info.get_local_coords(loc)};
    auto [new_local_row, new_local_col] {info.step_across(loc.dir, face_dir, local_row, local_col)};
    auto [new_row, new_col] {info.get_map_coords(new_face, new_local_row, new_local_col)};

    return {.row = new_row, .col = new_col, .dir = inverse(face_dir)};
}

void move_forward(const grid& grid, const fold_info& info, location& loc, int steps) {
    for (int i {0}; i < steps; i++) {
        location next_loc = next_position(grid, info, loc);

        if (grid[next_loc.row][next_loc.col] != '.') {
            break;
        }

        loc = next_loc;
    }
}

void turn(location& loc, char ch) {
    if (ch == 'R') {
        loc.dir = rotate_right(loc.dir);
    }
    else if (ch == 'L') {
        loc.dir = rotate_left(loc.dir);
    }
};

int as_password(const location& loc) {
    return 1000 * loc.row + 4 * loc.col + as_int(loc.dir);
}


// I hardcoded this.
fold_info get_example_fold() {
    fold_info info {};

    info.cube_locations[{0, 2}] = 1;
    info.cube_locations[{1, 0}] = 2;
    info.cube_locations[{1, 1}] = 3;
    info.cube_locations[{1, 2}] = 4;
    info.cube_locations[{2, 2}] = 5;
    info.cube_locations[{2, 3}] = 6;

    // Write down every fold, exactly once. There should be 7.
    // Travelling around clockwise gives this:
    using enum direction;

    info.folds[{1, LEFT}] = {3, UP};
    info.folds[{1, UP}] = {2, UP};
    info.folds[{1, RIGHT}] = {6, RIGHT};
    info.folds[{4, RIGHT}] = {6, UP};
    info.folds[{6, DOWN}] = {2, LEFT};
    info.folds[{5, DOWN}] = {2, DOWN};
    info.folds[{5, LEFT}] = {3, DOWN};

    info.side_length = 4;

    info.finalize();
    return info;
}

// Also hardcoded this
fold_info get_input_fold() {
    fold_info info {};

    info.cube_locations[{0, 1}] = 1;
    info.cube_locations[{0, 2}] = 2;
    info.cube_locations[{1, 1}] = 3;
    info.cube_locations[{2, 0}] = 4;
    info.cube_locations[{2, 1}] = 5;
    info.cube_locations[{3, 0}] = 6;

    // Write down every fold, exactly once. There should be 7.
    // Travelling around clockwise gives this:
    using enum direction;

    info.folds[{1, LEFT}] = {4, LEFT};
    info.folds[{1, UP}] = {6, LEFT};
    info.folds[{2, UP}] = {6, DOWN};
    info.folds[{2, RIGHT}] = {5, RIGHT};
    info.folds[{2, DOWN}] = {3, RIGHT};
    info.folds[{5, DOWN}] = {6, RIGHT};
    info.folds[{4, UP}] = {3, LEFT};

    info.side_length = 50;

    info.finalize();
    return info;
}


int main() {
    std::vector<std::string> lines {};
    for (std::string buf; std::getline(std::cin, buf); ) {
        if (buf == "")
            break;
        lines.push_back(buf);
    }

    grid grid {as_grid(lines)};

    std::string line;
    std::cin >> line;

    instructions instructions {as_instructions(line)};
    
    location loc {init_location(grid)};

    fold_info info {};
    if (grid.size() == 14) {
        info = get_example_fold();
    }
    else {
        info = get_input_fold();
    }

    for (auto& instr : instructions) {
        if (std::holds_alternative<int>(instr)) {
            move_forward(grid, info, loc, std::get<int>(instr));
        }
        else {
            turn(loc, std::get<char>(instr));
        }
    }
    
    std::cout << as_password(loc) << '\n';
}
