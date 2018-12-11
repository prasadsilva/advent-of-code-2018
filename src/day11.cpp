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

    std::pair<int, int> find_coord_of_largest_total_power(int serial_number) {
        std::pair<int, int> max_power_coord;
        int max_power = std::numeric_limits<int>::min();
        for (int y = 0; y < (300 - 2); y++) {
            for (int x = 0; x < (300 - 2); x++) {
                int power = 0;
                for (int j = 0; j < 3; j++) {
                    for (int i = 0; i < 3; i++) {
                        power += fuel_cells[y + j][x + i].get_power_level(serial_number);
                    }
                }
                if (power > max_power) {
                    max_power = power;
                    max_power_coord = { x + 1, y + 1 };
                }
            }
        }
        return max_power_coord;
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
        auto [x_18, y_18] = grid.find_coord_of_largest_total_power(18);
        assert(x_18 == 33 && y_18 == 45);

        auto [x_result, y_result] = grid.find_coord_of_largest_total_power(2866);
        std::cout << "Result: " << x_result << "," << y_result << std::endl;
    }

    void problem2() {
        std::cout << "Day 11 - Problem 2" << std::endl;
    }

}