
#include <sstream>
#include <iostream>
#include <vector>


struct grid {
    std::vector<std::vector<bool>> matrix {}; // stored in row major order. (rows are y's).

    int x_min;
    int y_min;
    int x_max;
    int y_max;

    void init() { // set up the matrix;
        matrix = std::vector(y_max - y_min + 3, std::vector(x_max - x_min + 3, false));
    }

    // This is the original version with get and set. After I finished the

    bool get(int x, int y) { // accessed in x y form.
        return matrix.at(y - y_min + 1).at(x - x_min + 1); // Can't seem to get reference semantic to work here :(
    } 

    void set(int x, int y, bool val) {
        matrix.at(y - y_min + 1).at(x - x_min + 1) = val;
    }
};


bool add_sand(grid& grid, int x, int y) {
    if (grid.get(x, y)) {
        return false; // sand hole blocked.
    }

    while (y != grid.y_max) {
        if (!grid.get(x, y + 1)) {
            y++;
        }
        else if (!grid.get(x - 1, y + 1)) {
            y++;
            x--;
        }
        else if (!grid.get(x + 1, y + 1)) {
            y++;
            x++;
        }
        else {
            grid.set(x, y, true); // solidify sand to rock.
            return true;
        }
    }

    std::cout << "Sand fell over edge unexpected...\n";
    return false;
}


int main() {
    std::vector<std::vector<std::pair<int, int>>> paths {};

    grid grid {};
    grid.x_min = 1000000;
    grid.y_min = 1000000;
    grid.x_max = -1000000;
    grid.y_max = -1000000;

    for (std::string line; std::getline(std::cin, line); ) {
        std::stringstream sstream {line};

        std::vector<std::pair<int, int>> path {};

        int x;
        int y;
        char ch;
        std::string str;
        while (sstream >> x >> ch >> y) {
            sstream >> str;

            grid.x_min = std::min(grid.x_min, x);
            grid.y_min = std::min(grid.y_min, y);
            grid.x_max = std::max(grid.x_max, x);
            grid.y_max = std::max(grid.y_max, y);

            path.push_back({x, y});
        }

        paths.push_back(path);
    }

    // We can figure out as much space as we need right now. No need for dynamic resize.
    grid.y_min = std::min(grid.y_max - 5, 0);
    grid.x_min = std::min(grid.x_min, 500 - grid.y_max - 10); // -10 for good measure.
    grid.x_max = std::max(grid.x_max, 500 + grid.y_max + 10); // +10 for good measure.
    grid.y_max += 2;
    grid.init();

    for (const auto& path : paths) {
        for (int i {0}; i < static_cast<int>(path.size()) - 1; i++) {
            auto [x_1, y_1] = path[i];
            auto [x_2, y_2] = path[i+1];

            if (x_1 == x_2) {
                for (int y {y_1}; y != y_2; y += (y_2 - y_1) / std::abs(y_2 - y_1)) {
                    grid.set(x_1, y, true); 
                }
                grid.set(x_2, y_2, true);
            }
            else {
                for (int x {x_1}; x != x_2; x += (x_2 - x_1) / std::abs(x_2 - x_1)) {
                    grid.set(x, y_1, true); 
                }
                grid.set(x_2, y_2, true);
            }
        }
    }

    // add the 'infinite' floor
    for (int x {grid.x_min}; x <= grid.x_max; x++) {
        grid.set(x, grid.y_max, true);
    }

    int count = 0;
    while (add_sand(grid, 500, 0)) {
        count++;
    }

    std::cout << count << '\n';
}