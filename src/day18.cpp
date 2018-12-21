#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include <list>
#include <regex>
#include <limits>
#include <array>
#include <sstream>

const bool trace_read = false;
const bool trace1 = false;
const bool trace2 = false;
const bool enable_assertions = true;

enum acre_type_e {
    ground,
    trees,
    lumberyard,
};

struct surrounding_area_t {
    int num_trees = 0;
    int num_lumberyards = 0;
    int num_ground = 0;

    surrounding_area_t(std::vector<std::vector<acre_type_e>> &acres, int x, int y) {
        bool check_left = false, check_right = false;
        // Row above
        if (y > 0) {
            auto &row_above = acres[y - 1];
            if (x > 0) count_acre(row_above[x - 1]);
            count_acre(row_above[x]);
            if (x < row_above.size() - 1) count_acre(row_above[x + 1]);
        }        
        // Same row
        auto &row = acres[y];
        if (x > 0) count_acre(row[x - 1]);
        if (x < row.size() - 1) count_acre(row[x + 1]);
        // Row below
        if (y < acres.size() - 1) {
            auto &row_below = acres[y + 1];
            if (x > 0) count_acre(row_below[x - 1]);
            count_acre(row_below[x]);
            if (x < row_below.size() - 1) count_acre(row_below[x + 1]);
        }
    }
    
    friend std::ostream & operator << (std::ostream &out, const surrounding_area_t &surround_area) {
        out << ".(" << surround_area.num_ground << ") |(" << surround_area.num_trees << ") #(" << surround_area.num_lumberyards << ")";
        return out;
    }

private:
    void count_acre(acre_type_e type) {
        switch (type) {
            case acre_type_e::ground: num_ground++; break;
            case acre_type_e::lumberyard: num_lumberyards++; break;
            case acre_type_e::trees: num_trees++; break;
        }
    }
};

struct area_t {
    std::vector<std::vector<acre_type_e>> acres;

    void tick_one_minute() {
        auto write_state = acres;
        // An open acre will become filled with trees if three or more adjacent acres contained trees. Otherwise, nothing happens.
        // An acre filled with trees will become a lumberyard if three or more adjacent acres were lumberyards. Otherwise, nothing happens.
        // An acre containing a lumberyard will remain a lumberyard if it was adjacent to at least one other lumberyard and at least one acre containing trees. Otherwise, it becomes open.
        for (int y = 0; y < acres.size(); y++) {
            auto &read_row = acres[y];
            auto &write_row = write_state[y];
            for (int x = 0; x < read_row.size(); x++) {
                surrounding_area_t surrounding_area(acres, x, y);
                switch (read_row[x]) {
                    case acre_type_e::ground: {
                        if (surrounding_area.num_trees >= 3) {
                            if (trace1) std::cout << "\t" << surrounding_area << " Changing (" << x << "," << y << ") to trees " << std::endl;
                            write_row[x] = acre_type_e::trees;
                        }
                        break;
                    }
                    case acre_type_e::trees: {
                        if (surrounding_area.num_lumberyards >= 3) {
                            if (trace1) std::cout << "\t" << surrounding_area << " Changing (" << x << "," << y << ") to lumberyard " << std::endl;
                            write_row[x] = acre_type_e::lumberyard;
                        }
                        break;
                    }
                    case acre_type_e::lumberyard: {
                        if (surrounding_area.num_lumberyards >= 1 && surrounding_area.num_trees >= 1) {
                            // Stay a lumberyard
                        } else {
                            if (trace1) std::cout << "\t" << surrounding_area << " Changing (" << x << "," << y << ") to ground " << std::endl;
                            write_row[x] = acre_type_e::ground;
                        }
                        break;
                    }
                }
            }
        }
        acres = write_state;
    }

    int get_resource_value() {
        int num_wooded_areas = 0, num_lumberyards = 0;
        for (auto &row : acres) {
            for (auto type : row) {
                if (type == acre_type_e::trees) {
                    num_wooded_areas++;
                } else if (type == acre_type_e::lumberyard) {
                    num_lumberyards++;
                }
            }
        }
        return num_wooded_areas * num_lumberyards;
    }

    friend std::ostream & operator << (std::ostream &out, const area_t &area) {
        for (auto &row : area.acres) {
            for (auto type : row) {
                switch (type) {
                    case acre_type_e::ground: out << "."; break;
                    case acre_type_e::lumberyard: out << "#"; break;
                    case acre_type_e::trees: out << "|"; break;
                }                
            }
            out << std::endl;
        }
        out << std::endl;
        return out;
    }
};

void read_day18_data(area_t &outdata, const char* filepath) {
    std::ifstream input_stream(filepath);
    std::string line;
    while (!input_stream.eof()) {
        std::vector<acre_type_e> row;
        getline(input_stream, line);
        for (int i = 0; i < line.size(); i++) {
            acre_type_e type = acre_type_e::ground;
            switch (line[i]) {
                case '|': type = acre_type_e::trees; break;
                case '#': type = acre_type_e::lumberyard; break;
            }
            row.push_back(type);
        }
        outdata.acres.push_back(row);
    }
}

namespace day18 {

    void problem1() {
        std::cout << "Day 18 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 18

        if (enable_assertions) {
            area_t area;
            read_day18_data(area, "data/day18/problem1/test1.txt");
            std::cout << area << std::endl;
            for (int i = 0; i < 10; i++) {
                std::cout << "After " << (i + 1) << " minutes.." << std::endl;
                area.tick_one_minute();
                std::cout << area << std::endl;
            }
            assert(area.get_resource_value() == 1147);
        }        

        area_t area;
        read_day18_data(area, "data/day18/problem1/input.txt");
        std::cout << area << std::endl;
        for (int i = 0; i < 10; i++) {
            area.tick_one_minute();
        }
        std::cout << area << std::endl;        
        std::cout << "Result: " << area.get_resource_value() << std::endl;

        #endif
    }

    void problem2() {
        std::cout << "Day 18 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 18

        area_t area;
        read_day18_data(area, "data/day18/problem2/input.txt");
        for (int i = 0; i < 700; i++) {
            area.tick_one_minute();
            // if ((i + 1) % 100 == 0) {
                std::cout << std::setw(10) << (i + 1) << ": " << area.get_resource_value() << std::endl;
            // }
        }   

        // This one requires looking at the repeating pattern in the output
        // The following pattern repeats indefinitely after (including) 672
        std::vector<int> pattern = {
            176782,
            175212,
            173290,
            173658,
            173922,
            177815,
            182358,
            186042,
            192504,
            195308,
            200646,
            205120,
            208650,
            210588,
            212833,
            212688,
            212443,
            208278,
            200466,
            196680,
            195290,
            189980,
            186795,
            184392,
            180560,
            181383,
            182700,
            180942
        };
        
        int result = pattern[(1000000000 - 672) % pattern.size()];
        std::cout << "Result: " << result << std::endl;

        #endif
    }

}