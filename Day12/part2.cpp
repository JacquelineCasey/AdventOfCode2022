
#include <array>
#include <iostream>
#include <vector>


int bfs(const std::vector<std::vector<int>>& heights, int target_row, int target_col) {
    int iter {0};

    std::vector<std::vector<bool>> visited (heights.size(), std::vector(heights[0].size(), false));
    
    visited[target_row][target_col] = true; // Now we bfs from the end!

    std::vector<std::pair<int, int>> newly_visited {std::pair(target_row, target_col)};

    while (true) {
        iter++;

        std::vector<std::pair<int, int>> next_newly_visited {};

        for (auto [i, j] : newly_visited) {
            static std::array<std::pair<int, int>, 4> directions {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
            
            for (auto [d_i, d_j] : directions) {
                if (i + d_i < 0 || i + d_i >= static_cast<int>(heights.size()) || j + d_j < 0 || j + d_j >= static_cast<int>(heights[0].size())) {
                    continue;
                }
                if (!visited[i + d_i][j + d_j] && heights[i + d_i][j + d_j] >= heights[i][j] - 1) { // is current position climbable from new position.
                    visited[i + d_i][j + d_j] = true;
                    next_newly_visited.push_back(std::pair(i + d_i, j + d_j));
                    if (heights[i + d_i][j + d_j] == 'a' - 'a') { // Is this an 'a'.
                        return iter;
                    }
                }
            }
        }

        newly_visited = std::move(next_newly_visited);
    }
}


int main() {
    std::vector<std::vector<int>> heights {};

    int count {0};
    int target_row {};
    int target_col {};
    for (std::string buf; std::cin >> buf; ) {
        heights.push_back({});
        for (char ch : buf) {
            if (ch != 'S' && ch != 'E') {
                heights[count].push_back(ch - 'a');
            }
            else if (ch == 'S') {
                heights[count].push_back('a' - 'a');
            }
            else if (ch == 'E') {
                target_row = count;
                target_col = heights[count].size();
                heights[count].push_back('z' - 'a');
            }
        }
        count++;
    }

    std::cout << bfs(heights, target_row, target_col) << '\n';
}
