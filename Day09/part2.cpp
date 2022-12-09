
#include <iostream>
#include <set>
#include <vector>


int main() {
    std::set<std::pair<int, int>> visited {};

    std::vector<int> x_pos (10, 0);
    std::vector<int> y_pos (10, 0);
    
    char dir;
    int amount;
    while (std::cin >> dir >> amount) {
        for (int i {0}; i < amount; i++) {
            // Move head
            switch (dir) {
            case 'R':
                x_pos[0]++;
                break;
            case 'L':
                x_pos[0]--;
                break;
            case 'U':
                y_pos[0]++;
                break;
            case 'D':
                y_pos[0]--;
                break;
            }

            for (int i {1}; i < static_cast<int>(x_pos.size()); i++) {
                int dif_x {x_pos[i-1] - x_pos[i]};
                int dif_y {y_pos[i-1] - y_pos[i]};

                // Did some playing around after the submit to make this part super short.
                if (std::abs(dif_x) >= 2 || std::abs(dif_y) >= 2) {
                    y_pos[i] += std::clamp(dif_y, -1, 1);
                    x_pos[i] += std::clamp(dif_x, -1, 1);
                }
            }
            
            visited.insert({x_pos.back(), y_pos.back()});
        }
    }

    std::cout << visited.size() << '\n';
}
