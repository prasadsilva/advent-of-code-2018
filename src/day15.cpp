#include <iostream>
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
const bool trace_elves = false;
const bool trace1 = true;
const bool trace2 = false;
const bool enable_assertions = true;

enum cell_type_e {
    none = 0,
    wall,

    unit = 100,
    goblin,
    elf,
};

struct cell_t {
    int x;
    int y;
    cell_type_e type = cell_type_e::none;
    int unit_id = -1;

    // Comparator based on reading order
    bool operator < (const cell_t &other) const {
        if (y < other.y) return true;
        else if ( y == other.y) {
            return x < other.x;
        }
        return false;
    }

    friend std::ostream & operator << (std::ostream &out, cell_t &cell) {
        char output;
        switch (cell.type) {
            case cell_type_e::wall: output = '#'; break;
            case cell_type_e::goblin: output = 'G'; break;
            case cell_type_e::elf: output = 'E'; break;
            default: output = '.'; break;
        }
        out << output;
        return out;
    }
};

struct unit_t {
    int x;
    int y;  
    int attack_power = 3;
    int hit_points = 200;
};

struct map_t {
    std::vector<std::vector<cell_t>> cells;
    std::vector<unit_t> goblins, elves;

    void tick() {
        // Move if not in range of enemy (single step, first in reading order)
        // Attack if in range of enemy
    }

    friend std::istream & operator >> (std::istream &in, map_t &env) {
        std::string line;
        while (!in.eof()) {
            int y = env.cells.size();
            getline(in, line);
            std::vector<cell_t> new_row;
            for (int x = 0; x < line.size(); x++) {
                cell_t new_cell;
                new_cell.x = x;
                new_cell.y = y;
                switch (line[x]) {
                    case '#': {
                        new_cell.type = cell_type_e::wall;
                        break;
                    }
                    case 'G': {
                        new_cell.type = cell_type_e::goblin;
                        new_cell.unit_id = env.goblins.size();
                        env.goblins.push_back({ x, y });
                        break;
                    }
                    case 'E': {
                        new_cell.type = cell_type_e::elf;
                        new_cell.unit_id = env.elves.size();
                        env.elves.push_back({ x, y });
                        break;
                    }
                }
                new_row.push_back(new_cell);
            }
            env.cells.push_back(new_row);
        }
        return in;
    }

    friend std::ostream & operator << (std::ostream &out, map_t &env) {
        std::vector<int> unit_cells_col;
        for (int y = 0; y < env.cells.size(); y++) {
            auto& row = env.cells[y];
            for (int x = 0; x < env.cells.size(); x++) {
                auto& cell = row[x];
                if (cell.type > cell_type_e::unit) unit_cells_col.push_back(x);
                out << cell;
            }
            // Render hps
            if (!unit_cells_col.empty()) out << "    ";
            for (auto &unit_cell_col : unit_cells_col) {
                auto& cell = row[unit_cell_col];
                if (cell.type == cell_type_e::goblin) {
                    out << "G(" << env.goblins[cell.unit_id].hit_points << "), ";
                } else {
                    out << "E(" << env.elves[cell.unit_id].hit_points << "), ";
                }
            }
            out << std::endl;
            unit_cells_col.clear();
        }
        out << std::endl;
        return out;
    }
};

void read_day15_data(map_t &outdata, const char* filepath) {
    std::ifstream input_stream(filepath);
    input_stream >> outdata;
}

namespace day15 {

    void problem1() {
        std::cout << "Day 15 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 15

        map_t test1;
        read_day15_data(test1, "data/day15/problem1/test1.txt");
        std::cout << test1 << std::endl;

        #endif
    }

    void problem2() {
        std::cout << "Day 15 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 15

        #endif
    }

}