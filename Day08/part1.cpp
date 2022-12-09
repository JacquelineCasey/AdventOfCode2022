
#include <iostream>
#include <vector>

int main() {
    std::vector<std::vector<int>> trees;
    
    for (std::string buf {}; std::cin >> buf; ) {
        std::vector<int> row {};

        for (char ch : buf) {
            row.push_back(ch - '0');
        }

        trees.push_back(row);
    }

    std::vector<std::vector<bool>> visible (trees.size(), std::vector(trees[0].size(), false));

    // Right

    for (int i = 0; i < static_cast<int>(trees.size()); i++) {
        int curr_heighest {-1};
        for (int j {0}; j < static_cast<int>(trees[0].size()); j++) {
            if (trees[i][j] > curr_heighest) {
                curr_heighest = trees[i][j];
                visible[i][j] = true;
            }
        }
    }

    // Left

    for (int i = 0; i < static_cast<int>(trees.size()); i++) {
        int curr_heighest {-1};
        for (int j {static_cast<int>(trees[0].size()) - 1}; j >= 0; j--) {
            if (trees[i][j] > curr_heighest) {
                curr_heighest = trees[i][j];
                visible[i][j] = true;
            }
        }
    }

    // Down

    for (int j {0}; j < static_cast<int>(trees[0].size()); j++) {
        int curr_heighest {-1};
        for (int i = 0; i < static_cast<int>(trees.size()); i++) {
            if (trees[i][j] > curr_heighest) {
                curr_heighest = trees[i][j];
                visible[i][j] = true;
            }
        }
    }

    // Up

    for (int j {0}; j < static_cast<int>(trees[0].size()); j++) {
        int curr_heighest {-1};
        for (int i = static_cast<int>(trees.size() - 1); i >= 0; i--) {
            if (trees[i][j] > curr_heighest) {
                curr_heighest = trees[i][j];
                visible[i][j] = true;
            }
        }
    }

    int total {0};
    for (auto& row : visible) {
        for (bool b : row) {
            total += b;
        }
    }

    std::cout << total << '\n';
}
