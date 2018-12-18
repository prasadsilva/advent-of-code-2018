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

enum unit_type_e {
    goblin = 0,
    elf,
};

struct unit_t {
    unit_type_e type;
    int x;
    int y;  
    int attack_power = 3;
    int hit_points = 200;

    bool is_enemy_of(unit_t *other) const {
        return type != other->type;
    }

    friend std::ostream & operator << (std::ostream &out, unit_t &unit) {
        if (unit.type == unit_type_e::goblin) {
            out << "G(" << unit.hit_points << ")";
        } else {
            out << "E(" << unit.hit_points << ")";
        }
        return out;
    }
};

enum cell_type_e {
    none = 0,
    wall,
    unit,
};

struct cell_t {
    int x = -1;
    int y = -1;
    cell_type_e type = cell_type_e::none;
    std::shared_ptr<unit_t> unit;

    bool has_unit() const {
        return (bool)unit;
    }

    void set_unit(std::shared_ptr<unit_t> _unit) {
        assert(type == cell_type_e::none);
        unit = _unit;
        type = cell_type_e::unit;
    }

    void clear() {
        type = cell_type_e::none;
        unit = nullptr;
    }

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
            case cell_type_e::unit: {
                if (cell.unit->type == unit_type_e::goblin) {
                    output = 'G';
                } else {
                    output = 'E';
                }
                break;
            }
            default: output = '.'; break;
        }
        out << output;
        return out;
    }
};

struct map_t {
    std::vector<std::vector<cell_t>> cells;
    std::list<std::shared_ptr<unit_t>> goblins, elves;
    int map_width, map_height;

    void get_enemy_cells_in_range(std::vector<cell_t> &out, unit_t *unit) {
        out.clear();
        int x = unit->x;
        int y = unit->y;
        std::vector<cell_t> attack_cells;
        // In read order
        for (int ay = y - 1; ay < y + 1; ay++) {
            for (int ax = x - 1; ax < x + 1; ax++) {
                // Cell is valid?
                if (ax >= 0 && ay >= 0 && ay < map_height && ax < map_width) {
                    if (cells[ay][ax].has_unit() && cells[ay][ax].unit->is_enemy_of(unit)) {
                        attack_cells.push_back(cells[ay][ax]);
                    }
                }
            }
        }
        // Sort is unnecessary. Traversal above is in read order
        // std::sort(out.begin(), out.end());
    }

    bool attack_enemies_in_range(std::shared_ptr<unit_t> unit) {
        std::vector<cell_t> adj_enemy_cells;
        get_enemy_cells_in_range(adj_enemy_cells, unit.get());
        auto& enemies = unit->type == unit_type_e::goblin ? goblins : elves;
        if (!adj_enemy_cells.empty()) {
            // Find enemy with lowest hit points
            std::shared_ptr<unit_t> enemy_with_lowest_hp = nullptr;
            int lowest_hp = 1000;
            for (int ac = 0; ac < adj_enemy_cells.size(); ac++) {
                if (adj_enemy_cells[ac].unit->hit_points < lowest_hp) {
                    enemy_with_lowest_hp = adj_enemy_cells[ac].unit;
                }
            }
            // Deal damage equal to attack power
            enemy_with_lowest_hp->hit_points -= unit->attack_power;
            // Is enemy dead?
            if (enemy_with_lowest_hp->hit_points <= 0) {
                // Enemy is dead
                // - Mark cell as empty
                cells[enemy_with_lowest_hp->y][enemy_with_lowest_hp->x].clear();
                // - Remove from unit list
                enemies.remove(enemy_with_lowest_hp);   
            }
            return true;
        }
        return false;
    }

    struct reachability_t {
        cell_t destination;
        int cost;
        std::vector<std::pair<int, int>> path;

        // Cost first, then reading order
        bool operator < (const reachability_t &other) const {
            if (cost < other.cost) return true;
            else if (cost == other.cost) {
                if (destination.y < other.destination.y) return true;
                else if (destination.y == other.destination.y) {
                    return destination.x < other.destination.x;
                }
            }
            return false;
        }
    };

    std::vector<reachability_t> find_reachable_attack_cells_for(std::shared_ptr<unit_t> unit) const {
        // TODO
        return {};
    }

    void move_unit_towards(std::shared_ptr<unit_t> unit, const reachability_t &target) {
        // Mark unit cell empty
        cells[unit->y][unit->x].clear();
        // Move unit one step in path
        unit->x = target.path[0].first;
        unit->y = target.path[0].second;
        // Mark new unit cell
        cells[unit->y][unit->x].set_unit(unit);
    }

    void move_toward_closest_enemy(std::shared_ptr<unit_t> unit) {
        std::vector<reachability_t> reachable_attack_cells = find_reachable_attack_cells_for(unit);
        if (reachable_attack_cells.empty()) return;

        // Sort nearest by cost and reading order 
        std::sort(reachable_attack_cells.begin(), reachable_attack_cells.end());
        // Choose the first reachable cell
        move_unit_towards(unit, *reachable_attack_cells.begin());
    }

    void tick_unit(std::shared_ptr<unit_t> unit) {
        if (!attack_enemies_in_range(unit)) {
            move_toward_closest_enemy(unit);
            attack_enemies_in_range(unit);
        }
    }

    void tick() {
        std::vector<cell_t> unit_cells;
        for (auto& goblin : goblins) {
            unit_cells.push_back(cells[goblin->y][goblin->x]);
        }
        for (auto& elf : elves) {
            unit_cells.push_back(cells[elf->y][elf->x]);
        }
        // Sort all units
        std::sort(unit_cells.begin(), unit_cells.end());
        // Units are now sorted by read order
        for (auto& unit_cell : unit_cells) {
            tick_unit(unit_cell.unit);
        }
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
                        new_cell.type = cell_type_e::unit;
                        auto goblin = std::make_shared<unit_t>();
                        goblin->type = unit_type_e::goblin;
                        goblin->x = x;
                        goblin->y = y;
                        new_cell.unit = goblin;
                        env.goblins.push_back(goblin);
                        break;
                    }
                    case 'E': {
                        new_cell.type = cell_type_e::unit;
                        auto elf = std::make_shared<unit_t>();
                        elf->type = unit_type_e::elf;
                        elf->x = x;
                        elf->y = y;
                        new_cell.unit = elf;
                        env.elves.push_back(elf);
                        break;
                    }
                }
                new_row.push_back(new_cell);
            }
            env.cells.push_back(new_row);
        }
        env.map_width = env.cells[0].size();
        env.map_height = env.cells.size();
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
                if (cell.type == cell_type_e::unit) {
                    out << unit << ", ";
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
        for (int i = 0; i < 2; i++) {
            test1.tick();
            std::cout << test1 << std::endl;
        }

        #endif
    }

    void problem2() {
        std::cout << "Day 15 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 15

        #endif
    }

}