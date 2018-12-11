#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include <list>
#include <regex>
#include <limits>
#include <array>

const bool trace_read = false;
const bool trace1 = true;

struct max_power_coord_t {
    int x, y;
    int power_level;
};

struct max_window_and_coord_t {
    int x, y;
    int window_sz;
};

struct fuel_cell_t {
    int x;
    int y;

    int get_rack_id() { return x + 10; }

    int get_power_level(int serial_number) {
        int power_level = get_rack_id() * y;
        power_level += serial_number;
        power_level *= get_rack_id();

        int div_100 = power_level / 100;
        int hundreds_digit = div_100 % 10;
        power_level = hundreds_digit;

        power_level -= 5;

        return power_level;
    }
};

struct fuel_cell_grid_t {
    std::array<std::array<fuel_cell_t, 300>, 300> fuel_cells;

    fuel_cell_grid_t() {
        for (int y = 0; y < 300; y++) {
            for (int x = 0; x < 300; x++) {
                fuel_cells[y][x] = fuel_cell_t{x + 1, y + 1};
            }
        }
    }

    max_power_coord_t find_coord_of_largest_total_power(int serial_number, int window_sz) {
        max_power_coord_t result;
        int max_power = std::numeric_limits<int>::min();
        int max_d = 300 - window_sz + 1;
        for (int y = 0; y < max_d; y++) {
            for (int x = 0; x < max_d; x++) {
                int power = 0;
                for (int j = 0; j < window_sz; j++) {
                    for (int i = 0; i < window_sz; i++) {
                        power += fuel_cells[y + j][x + i].get_power_level(serial_number);
                    }
                }
                if (power > max_power) {
                    max_power = power;
                    result = { x + 1, y + 1, power };
                }
            }
        }
        std::cout << "Coord for window " << window_sz << ": " << result.x << "," << result.y << " (" << result.power_level << ")" << std::endl;
        return result;
    }

    max_window_and_coord_t find_coord_and_window_of_largest_total_power(int serial_number) {
        max_window_and_coord_t result;
        int max_power = std::numeric_limits<int>::min();
        for (int window_sz = 1; window_sz <= 300; window_sz++) {
            auto max_coord = find_coord_of_largest_total_power(serial_number, window_sz);
            if (max_coord.power_level > max_power) {
                max_power = max_coord.power_level;
                result.window_sz = window_sz;
                result.x = max_coord.x;
                result.y = max_coord.y;
            }
        }
        std::cout << "Max window and coord: " << result.x << "," << result.y << "," << result.window_sz << std::endl;
        return result;
    }
};

namespace day11 {

    void problem1() {
        std::cout << "Day 11 - Problem 1" << std::endl;

        assert( (fuel_cell_t{3, 5}).get_power_level(8) == 4 );
        assert( (fuel_cell_t{122, 79}).get_power_level(57) == -5 );
        assert( (fuel_cell_t{217, 196}).get_power_level(39) == 0 );
        assert( (fuel_cell_t{101, 153}).get_power_level(71) == 4 );

        fuel_cell_grid_t grid;
        auto max_coord_18 = grid.find_coord_of_largest_total_power(18, 3);
        assert(max_coord_18.x == 33 && max_coord_18.y == 45);

        auto max_coord = grid.find_coord_of_largest_total_power(2866, 3);
        std::cout << "Result: " << max_coord.x << "," << max_coord.y << std::endl;
    }

    void problem2() {
        std::cout << "Day 11 - Problem 2" << std::endl;

        fuel_cell_grid_t grid;
        auto max_coord_and_window_18 = grid.find_coord_and_window_of_largest_total_power(18);
        assert(max_coord_and_window_18.x == 90 && max_coord_and_window_18.y == 269 && max_coord_and_window_18.window_sz == 16);
        auto max_coord_and_window_42 = grid.find_coord_and_window_of_largest_total_power(18);
        assert(max_coord_and_window_42.x == 232 && max_coord_and_window_42.y == 251 && max_coord_and_window_42.window_sz == 12);
    }

}