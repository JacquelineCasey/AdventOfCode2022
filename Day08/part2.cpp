
#include <iostream>
#include <vector>


// Handles a single direction
int trees_viewed(const std::vector<std::vector<int>>& trees, int row, int col, int d_r, int d_c) {
    int height = trees[row][col];

    int count {0};
    while (true) {
        row += d_r;
        col += d_c;
        if (row < 0 || row >= static_cast<int>(trees.size()) || col < 0 || col >= static_cast<int>(trees[0].size())) {
            return count;
        }
        count++;

        if (trees[row][col] >= height) {
            return count;
        }
    }
}

int view_score(const std::vector<std::vector<int>>& trees, int row, int col) {
    int score {1};
    score *= trees_viewed(trees, row, col, 1, 0);
    score *= trees_viewed(trees, row, col, -1, 0);
    score *= trees_viewed(trees, row, col, 0, 1);
    score *= trees_viewed(trees, row, col, 0, -1);
    return score;
}

int main() {
    std::vector<std::vector<int>> trees;
    
    for (std::string buf {}; std::cin >> buf; ) {
        std::vector<int> row {};

        for (char ch : buf) {
            row.push_back(ch - '0');
        }

        trees.push_back(row);
    }

    int best_view {-1};
    for (int i {0}; i < static_cast<int>(trees.size()); i++) {
        for (int j {0}; j < static_cast<int>(trees[0].size()); j++) {
            best_view = std::max(best_view, view_score(trees, i, j));
        }
    }

    std::cout << best_view << '\n';
}
