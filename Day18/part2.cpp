
#include <array>
#include <iostream>
#include <vector>


using vec3 = std::tuple<int, int, int>;

std::istream& operator>>(std::istream& in, vec3& vec) {
    char ch;
    auto& [x, y, z] {vec};
    return in >> x >> ch >> y >> ch >> z;
}

constexpr std::array<vec3, 6> directions {{
    {-1,  0,  0},
    { 1,  0,  0},
    { 0, -1,  0},
    { 0,  1,  0},
    { 0,  0, -1},
    { 0,  0,  1}
}};

enum class material {
    LAVA,
    AIR,
    WATER
};

using grid = std::vector<std::vector<std::vector<material>>>;


void fill_grid_from(grid& grid, int x, int y, int z) {
    int size {static_cast<int>(grid.size())};

    if (x < 0 || x >= size || y < 0 || y >= size || z < 0 || z >= size) {
        return;
    }

    if (grid[x][y][z] == material::AIR) {
        grid[x][y][z] = material::WATER;

        for (auto [dx, dy, dz] : directions) {
            fill_grid_from(grid, x + dx, y + dy, z + dz);
        }
    }
}

int main() {
    std::vector<vec3> cubes {};

    int min {1000000};
    int max {-1000000};
    for (vec3 cube; std::cin >> cube; ) {
        auto [x, y, z] {cube};
        min = std::min(min, x);
        min = std::min(min, y);
        min = std::min(min, z);
        max = std::max(max, x);
        max = std::max(max, y);
        max = std::max(max, z);
        cubes.push_back(cube);
    }

    // It will be helpful to move all rocks into a cube, with at least a single
    // layer around the edge. We will simulate filling this with water.

    for (auto& [x, y, z] : cubes) {
        x = x - min + 1; // so if x was the min, it is now at 1.
        y = y - min + 1;
        z = z - min + 1;
    }

    int size {max - min + 3}; // So this is 2 greater than the new max coordinate. This provides one layer of buffer.

    grid grid(size, std::vector(size, std::vector(size, material::AIR)));

    for (auto [x, y, z] : cubes) {
        grid[x][y][z] = material::LAVA;
    }

    // Now we fill the cube with water.

    fill_grid_from(grid, 0, 0, 0);

    // Now we perform the same surface area calculation as before, but only counting water.

    int surface_area {0}; 

    for (auto [x, y, z] : cubes) {
        for (auto [dx, dy, dz] : directions) {
            if (grid[x + dx][y + dy][z + dz] == material::WATER) {
                surface_area++;
            }
        }
    }

    std::cout << surface_area << '\n';
}
