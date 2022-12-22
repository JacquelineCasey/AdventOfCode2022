
#include <iostream>
#include <vector>


using grid = std::vector<std::vector<char>>;

// It will be convenient to add a layer of spaces around the input, both for index
// calculation and for preventing index out of bounds.
grid as_grid(std::vector<std::string> lines) {
    /* First, pad the lines vertically, by adding too new lines. */
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

location init_location(const grid& grid) {
    location loc {.row = 1, .col = 0, .dir = direction::RIGHT};

    // We move right until we are on a .
    while (grid[1][loc.col] != '.') {
        loc.col++;
    }

    return loc;
}

// Returns the coordinates on the map of the next would be move. Handles the
// complicated wrap around logic.
std::pair<int, int> next_position(const grid& grid, const location& loc) {
    int row {loc.row};
    int col {loc.col};
    auto [d_r, d_c] {as_delta(loc.dir)};

    if (grid[row + d_r][col + d_c] != ' ') {
        return {row + d_r, col + d_c};
    }

    /* Invert the direction, and begin moving that way. */
    d_r *= -1;
    d_c *= -1;

    while (grid[row][col] != ' ') {
        row += d_r;
        col += d_c;
    }

    // Now we are on a ' ', so pull back one.
    return {row - d_r, col - d_c};
}

void move_forward(const grid& grid, location& loc, int steps) {
    for (int i {0}; i < steps; i++) {
        // std::cout << "(row=" << loc.row << ", col=" << loc.col << ") -> ";
        auto [row, col] = next_position(grid, loc);

        if (grid[row][col] != '.') {
            break;
        }

        loc.row = row;
        loc.col = col;
        // std::cout << "(row=" << loc.row << ", col=" << loc.col << ")\n";
    }
    // std::cout << "stop\n";
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

    for (auto& instr : instructions) {
        if (std::holds_alternative<int>(instr)) {
            move_forward(grid, loc, std::get<int>(instr));
        }
        else {
            turn(loc, std::get<char>(instr));
        }
    }
    
    std::cout << as_password(loc) << '\n';
}
