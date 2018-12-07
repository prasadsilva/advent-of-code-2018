#include <iostream>
#include <fstream>
#include <cctype>   // islower, toupper, tolower

// Let's use stream reading for this

int get_unit_count_after_reactions(const char* filepath) {
    std::ifstream input_stream(filepath);
    std::string buffer;

    char current_unit;
    while (input_stream.get(current_unit)) {
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

namespace day5 {

    void problem1() {
        std::cout << "Day 5 - Problem 1" << std::endl;

        assert(get_unit_count_after_reactions("data/day5/problem1/test1.txt") == 10);

        std::cout << "Result : " << get_unit_count_after_reactions("data/day5/problem1/input.txt") << std::endl;
    }

}
