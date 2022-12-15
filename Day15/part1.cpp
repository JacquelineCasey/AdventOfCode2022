
#include <iostream>
#include <vector>
#include <set>


struct interval_edge {
    // Intervals are half open [) for better processing.
    bool is_start_edge; // whether or not the interval is a start interval or an end one.
    int pos; // x position.

    friend bool operator<(const interval_edge& lhs, const interval_edge& rhs) {
        return lhs.pos < rhs.pos;
    }
};

// Note that this does not consider beacons literally on the row. You need to subtract.
int beacon_exclusion_on_row(const std::vector<std::pair<int, int>>& sensor_pos, const std::vector<int>& dists, int row) {
    // row is a y position.

    // Construct a vector of interval edge objects representing overlapping 
    // intervals of beacon exclusion on y = `row`.

    std::vector<interval_edge> edges {}; // We will sort later.

    for (int i {0}; i < static_cast<int>(sensor_pos.size()); i++) {
        auto [x, y] {sensor_pos[i]};
        int dist {dists[i]};

        int d_y {std::abs(y - row)};

        if (d_y > dist)
            continue;

        int d_x {dist - d_y}; // distance from x that the interval spans on y = row. d_x = 0 means it spans a single grid square.

        interval_edge left {};
        left.is_start_edge = true;
        left.pos = x - d_x; // left is inclusive.
        edges.push_back(left);

        interval_edge right {};
        right.is_start_edge = false;
        right.pos = x + d_x + 1; // +1 since right is exclusive
        edges.push_back(right);
    }

    std::sort(edges.begin(), edges.end()); // Sorts ascending by edge.pos

    // Now process the edges in order
    int enclosing_intervals {0};
    int exclusion_zone_size {0};

    int curr_x = {-10000000};
    for (interval_edge edge : edges) {
        int new_x {edge.pos};
        
        if (enclosing_intervals > 0) {
            exclusion_zone_size += new_x - curr_x;
        }

        curr_x = new_x;

        if (edge.is_start_edge) {
            enclosing_intervals++;
        }
        else {
            enclosing_intervals--;
        }
    }

    return exclusion_zone_size;    
}

int main() {
    std::vector<std::pair<int, int>> sensor_pos {};
    std::set<std::pair<int, int>> beacon_pos {}; // Only known beacons included...

    std::vector<int> sensor_beacon_dist {}; // index i stores the manhattan distance of sensor i's closest beacon;

    int x;
    int y;
    char ch;
    std::string buf;
    while (std::cin >> buf >> buf >> ch >> ch >> x >> ch >> ch >> ch >> y >> ch) {
        sensor_pos.push_back({x,y});
        int x_1;
        int y_1;
        std::cin >> buf >> buf >> buf >> buf >> ch >> ch >> x_1 >> ch >> ch >> ch >> y_1 >> ch;
        beacon_pos.insert({x_1,y_1}); 

        sensor_beacon_dist.push_back(std::abs(x - x_1) + std::abs(y - y_1));
    }

    int row {2000000}; // switch to 10 for example.txt

    int exclusion {beacon_exclusion_on_row(sensor_pos, sensor_beacon_dist, row)};

    // manually exclude beacons litterally on the row.
    for (auto [x, y] : beacon_pos) {
        if (y == row) {
            exclusion--;
        }
    }
    
    std::cout << exclusion << '\n'; // switch to different row for input.
}
