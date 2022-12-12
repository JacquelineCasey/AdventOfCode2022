
#include <array>
#include <iostream>
#include <vector>


int bfs(const std::vector<std::vector<int>>& heights, int start_row, int start_col, int target_row, int target_col) {
    int iter {0};

    std::vector<std::vector<bool>> visited (heights.size(), std::vector(heights[0].size(), false));
    
    visited[start_row][start_col] = true;

    std::vector<std::pair<int, int>> newly_visited {std::pair(start_row, start_col)};

    while (true) {
        iter++;

        std::vector<std::pair<int, int>> next_newly_visited {};

        for (auto [i, j] : newly_visited) {
            static std::array<std::pair<int, int>, 4> directions {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
            
            for (auto [d_i, d_j] : directions) {
                if (i + d_i < 0 || i + d_i >= static_cast<int>(heights.size()) || j + d_j < 0 || j + d_j >= static_cast<int>(heights[0].size())) {
                    continue;
                }
                if (!visited[i + d_i][j + d_j] && heights[i + d_i][j + d_j] <= heights[i][j] + 1) {
                    visited[i + d_i][j + d_j] = true;
                    next_newly_visited.push_back(std::pair(i + d_i, j + d_j));
                    if (i + d_i == target_row && j + d_j == target_col) {
                        return iter;
                    }
                }
            }
        }

        newly_visited = std::move(next_newly_visited);  // In theory a move here permits newly_visited to avoid copying. It doesn't really matter at this scale.
    }
}


int main() {
    std::vector<std::vector<int>> heights {};

    int count {0};
    int start_row {};
    int start_col {};
    int target_row {};
    int target_col {};
    for (std::string buf; std::cin >> buf; ) {
        heights.push_back({});
        for (char ch : buf) {
            if (ch != 'S' && ch != 'E') {
                heights[count].push_back(ch - 'a');
            }
            else if (ch == 'S') {
                start_row = count;
                start_col = heights[count].size();
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

    std::cout << bfs(heights, start_row, start_col, target_row, target_col) << '\n';
}
