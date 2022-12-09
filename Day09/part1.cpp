
#include <iostream>
#include <set>


int main() {
    std::set<std::pair<int, int>> visited {};

    int head_x {0};
    int head_y {0};
    int tail_x {0};
    int tail_y {0};

    char dir;
    int amount;
    while (std::cin >> dir >> amount) {
        for (int i {0}; i < amount; i++) {
            // Move head
            switch (dir) {
            case 'R':
                head_x++;
                break;
            case 'L':
                head_x--;
                break;
            case 'U':
                head_y++;
                break;
            case 'D':
                head_y--;
                break;
            }

            // Move tail.
            int dif_x {head_x - tail_x};
            int dif_y {head_y - tail_y};

            if (dif_x >= 2) {
                tail_x++;
                tail_y += dif_y;
            }
            else if (dif_x <= -2) {
                tail_x--;
                tail_y += dif_y;
            }
            else if (dif_y >= 2) {
                tail_y++;
                tail_x += dif_x;
            }
            else if (dif_y <= -2) {
                tail_y--;
                tail_x += dif_x;
            }

            visited.insert({tail_x, tail_y});
        }
    }

    std::cout << visited.size() << '\n';
}
