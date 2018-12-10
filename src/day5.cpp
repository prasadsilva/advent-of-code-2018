#include <iostream>
#include <fstream>
#include <cctype>   // islower, toupper, tolower
#include <limits>
#include <set>
#include <vector>

// Do the processing while reading from stream

std::vector<char> get_unique_units(const char* filepath) {
    std::ifstream input_stream(filepath);
    std::set<char> unique_units;
    char unit;
    while (input_stream.get(unit)) {
        unique_units.insert(unit);
    }
    std::vector<char> unique_units_arr;
    unique_units_arr.assign(unique_units.begin(), unique_units.end());
    return unique_units_arr;
}

int get_polymer_length_after_reactions(const char* filepath, char remove_unit = 0) {
    std::ifstream input_stream(filepath);
    std::string buffer;

    char remove_unit_upper = toupper(remove_unit);
    char remove_unit_lower = tolower(remove_unit);

    char current_unit;
    while (input_stream.get(current_unit)) {
        if (current_unit == remove_unit_lower || current_unit == remove_unit_upper) {
            continue;
        }

        bool nuke_last_unit = false;

        if (buffer.size() > 0) {
            char last_unit = buffer.back();
            if (toupper(last_unit) == toupper(current_unit)) {
                // Units are same type, check polarity
                if (last_unit != current_unit) {
                    // Nuke the last unit
                    nuke_last_unit = true;
                }
            }
        }

        if (nuke_last_unit) {
            buffer.pop_back();
        } else {
            buffer += current_unit;
        }
    }

    return buffer.size();
}

int get_shortest_polymer_length_after_removing_single_unit(const char* filepath) {
    // Get unique units from polymer
    std::vector<char> unique_units = get_unique_units(filepath);

    int shortest_polymer_length = std::numeric_limits<int>::max();
    for (auto& unit : unique_units) {
        int polymer_length = get_polymer_length_after_reactions(filepath, unit);
        if (polymer_length < shortest_polymer_length) {
            shortest_polymer_length = polymer_length;
        }
    }

    return shortest_polymer_length;
}

namespace day5 {

    void problem1() {
        std::cout << "Day 5 - Problem 1" << std::endl;

        assert(get_polymer_length_after_reactions("data/day5/problem1/test1.txt") == 10);

        std::cout << "Result : " << get_polymer_length_after_reactions("data/day5/problem1/input.txt") << std::endl;
    }

    void problem2() {
        std::cout << "Day 5 - Problem 2" << std::endl;

        assert(get_shortest_polymer_length_after_removing_single_unit("data/day5/problem2/test1.txt") == 4);

        std::cout << "Result : " << get_shortest_polymer_length_after_removing_single_unit("data/day5/problem2/input.txt") << std::endl;
    }

}
