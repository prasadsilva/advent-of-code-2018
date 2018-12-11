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

template <int serial_number>
struct fuel_cell_t {
    int x;
    int y;
    int power_level;

    void initialize(int _x, int _y) {
        x = _x;
        y = _y;
        power_level = compute_power_level();
    }

    int get_rack_id() { return x + 10; }

    int compute_power_level() {
        // std::cout << "Compute " << x << ", " << y << std::endl;
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

template <int serial_number>
struct fuel_cell_grid_t {
    std::array<std::array<fuel_cell_t<serial_number>, 300>, 300> fuel_cells;

    std::vector<std::array<std::array<int, 300>, 300>> power_total_cache;
    std::vector<std::array<std::array<bool, 300>, 300>> power_total_cache_calc_bit;

    void initialize() {
        for (int y = 0; y < 300; y++) {
            for (int x = 0; x < 300; x++) {
                fuel_cells[y][x].initialize(x + 1, y + 1);
            }
        }
        
        power_total_cache.resize(300);
        power_total_cache_calc_bit.resize(300);
    }

    max_power_coord_t find_coord_of_largest_total_power(int window_sz) {
        max_power_coord_t result;
        int max_power = std::numeric_limits<int>::min();
        int max_d = 300 - window_sz + 1;
        int window_sz_idx = window_sz - 1;

        for (int y = 0; y < max_d; y++) {
            for (int x = 0; x < max_d; x++) {

                int power = 0;

                if (power_total_cache_calc_bit[window_sz_idx][y][x]) {
                    power = power_total_cache[window_sz_idx][y][x];
                } else {
                    // If we have cached value for window_sz_idx - 1
                    if (window_sz_idx > 0 && power_total_cache_calc_bit[window_sz_idx - 1][y][x]) {
                        power += power_total_cache[window_sz_idx - 1][y][x];
                        // Add totals from right and bottom perimeter
                        for (int j = 0; j < window_sz; j++) {
                            power += fuel_cells[y + j][window_sz_idx].power_level;
                        }
                        for (int i = 0; i < window_sz - 1; i++) { // Skip last cell (already added in vertical scan above)
                            power += fuel_cells[window_sz_idx][x + i].power_level;
                        }

                    } else {
                        // Do full computation
                        for (int j = 0; j < window_sz; j++) {
                            for (int i = 0; i < window_sz; i++) {
                                power += fuel_cells[y + j][x + i].power_level;
                            }
                        }    
                    }

                    power_total_cache[window_sz_idx][y][x] = power;
                    // power_total_cache_calc_bit[window_sz_idx][y][x] = true;
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

    max_window_and_coord_t find_coord_and_window_of_largest_total_power() {
        max_window_and_coord_t result;
        int max_power = std::numeric_limits<int>::min();
        for (int window_sz = 1; window_sz <= 300; window_sz++) {
            auto max_coord = find_coord_of_largest_total_power(window_sz);
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

        fuel_cell_t<8> cell_8; cell_8.initialize(3, 5); assert( cell_8.power_level == 4 );
        fuel_cell_t<57> cell_57; cell_57.initialize(122, 79); assert( cell_57.power_level == -5 );
        fuel_cell_t<39> cell_39; cell_39.initialize(217, 196); assert( cell_39.power_level == 0 );
        fuel_cell_t<71> cell_71; cell_71.initialize(101, 153); assert( cell_71.power_level == 4 );

        fuel_cell_grid_t<18> grid_18; grid_18.initialize();
        auto max_coord_18 = grid_18.find_coord_of_largest_total_power(3);
        assert(max_coord_18.x == 33 && max_coord_18.y == 45);

        fuel_cell_grid_t<2866> grid_result; grid_result.initialize();
        auto max_coord = grid_result.find_coord_of_largest_total_power(3);
        std::cout << "Result: " << max_coord.x << "," << max_coord.y << std::endl;
    }

    void problem2() {
        std::cout << "Day 11 - Problem 2" << std::endl;

        fuel_cell_grid_t<18> grid_18; grid_18.initialize();
        auto max_coord_and_window_18 = grid_18.find_coord_and_window_of_largest_total_power();
        assert(max_coord_and_window_18.x == 90 && max_coord_and_window_18.y == 269 && max_coord_and_window_18.window_sz == 16);
        
        fuel_cell_grid_t<42> grid_42; grid_42.initialize();
        auto max_coord_and_window_42 = grid_42.find_coord_and_window_of_largest_total_power();
        assert(max_coord_and_window_42.x == 232 && max_coord_and_window_42.y == 251 && max_coord_and_window_42.window_sz == 12);
    }

}