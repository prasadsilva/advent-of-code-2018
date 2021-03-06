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
#include "micropather_1_2_0/micropather.h"

namespace day15 {

  const bool trace_read = false;
  const bool trace_elves = false;
  const bool trace1 = false;
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

    char get_type_char() const {
      if (type == unit_type_e::goblin) {
        return 'G';
      } else {
        return 'E';
      }
    }

    friend std::ostream &operator<<(std::ostream &out, unit_t &unit) {
      if (unit.type == unit_type_e::goblin) {
        out << "G(" << unit.x << "," << unit.y << ")(" << unit.hit_points << ")";
      } else {
        out << "E(" << unit.x << "," << unit.y << ")(" << unit.hit_points << ")";
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
    std::shared_ptr<unit_t> unit = nullptr;

    bool is_empty() const {
      return type == cell_type_e::none;
    }

    bool has_unit() const {
      return (bool) unit;
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

    // // Compartor based on (x, y)
    // bool operator == (const cell_t &other) const {
    //     return (x == other.x) && (y == other.y);
    // }

    // Comparator based on (x, y) reading order
    bool operator<(const cell_t &other) const {
      if (y < other.y) return true;
      else if (y == other.y) {
        return x < other.x;
      }
      return false;
    }

    friend std::ostream &operator<<(std::ostream &out, cell_t &cell) {
      char output;
      switch (cell.type) {
        case cell_type_e::wall:
          output = '#';
          break;
        case cell_type_e::unit: {
          output = cell.unit->get_type_char();
          break;
        }
        default:
          output = '.';
          break;
      }
      out << output;
      return out;
    }
  };

  struct reachability_t {
    cell_t *source;
    cell_t *destination;
    float cost;
    std::vector<cell_t *> path;

    // Cost first, then reading order of the destination, then reading order of the source
    bool operator<(const reachability_t &other) const {
      if (cost < other.cost) return true;
      else if (cost == other.cost) {
        if (destination->y < other.destination->y) return true;
        else if (destination->y == other.destination->y) {
          if (destination->x < other.destination->x) return true;
          else if (destination->x == other.destination->x) {
            if (source->y < other.source->y) return true;
            else if (source->y == other.source->y) {
              return (source->x < other.source->x);
            }
          }
        }
      }
      return false;
    }

    friend std::ostream &operator<<(std::ostream &out, const reachability_t &reachability) {
      for (auto cell : reachability.path) {
        out << "(" << cell->x << "," << cell->y << ") ";
      }
      return out;
    }
  };

  struct map_t : public micropather::Graph {
    std::vector<std::vector<std::shared_ptr<cell_t>>> cells;
    std::list<std::shared_ptr<unit_t>> goblins, elves;
    int map_width, map_height;
    int current_tick = 0;
    std::shared_ptr<micropather::MicroPather> pather = nullptr;
    int elf_ap, goblin_ap;

    map_t(int elf_attack_power = 3, int goblin_attack_power = 3) : elf_ap(elf_attack_power),
                                                                   goblin_ap(goblin_attack_power) {}

    // micropath::Graph interface
    float LeastCostEstimate(void *stateStart, void *stateEnd) {
      cell_t *start = (cell_t *) stateStart;
      cell_t *end = (cell_t *) stateEnd;
      return (float) std::abs(start->x - end->x) + std::abs(start->y - end->y);
    }

    void AdjacentCost(void *state, std::vector<micropather::StateCost> *adjacent) {
      cell_t *start = (cell_t *) state;
      int x = start->x;
      int y = start->y;
      constexpr std::pair<int, int> valid_deltas[4] = {
        {0,  -1},
        {-1, 0},
        {1,  0},
        {0,  1}
      };
      for (auto[dx, dy] : valid_deltas) {
        auto end = cells[y + dy][x + dx].get();
        micropather::StateCost state_cost;
        state_cost.state = (void *) end;
        if (end->is_empty()) {
          state_cost.cost = 1;
          if (end->has_unit()) {
            state_cost.cost = std::numeric_limits<float>::max();
          }
          adjacent->push_back(state_cost);
        }
      }
    }

    void PrintStateInfo(void *state) {
      std::cout << *((cell_t *) state);
    }

    bool game_over() const {
      return goblins.empty() || elves.empty();
    }

    int elapsed_rounds() const { return current_tick; }

    int total_remaining_hit_points() const {
      int total_hp = 0;
      for (auto &goblin : goblins) {
        total_hp += goblin->hit_points;
      }
      for (auto &elf : elves) {
        total_hp += elf->hit_points;
      }
      return total_hp;
    }

    void get_enemy_cells_in_range(std::vector<cell_t *> &out, unit_t *unit) {
      out.clear();
      int x = unit->x;
      int y = unit->y;
      // In read order
      constexpr std::pair<int, int> valid_deltas[4] = {
        {0,  -1},
        {-1, 0},
        {1,  0},
        {0,  1}
      };
      for (auto[dx, dy] : valid_deltas) {
        auto cell = cells[y + dy][x + dx].get();
        if (cell->has_unit() && cell->unit->is_enemy_of(unit)) {
          out.push_back(cell);
        }
      }

      // Sort is unnecessary. Traversal above is in read order
      // std::sort(out.begin(), out.end(), [](cell_t *a, cell_t *b) { return *a < *b; });
    }

    bool attack_enemies_in_range(std::shared_ptr<unit_t> unit) {
      std::vector<cell_t *> adj_enemy_cells;
      get_enemy_cells_in_range(adj_enemy_cells, unit.get());
      auto &enemies = unit->type == unit_type_e::goblin ? elves : goblins;
      if (!adj_enemy_cells.empty()) {
        // Find enemy with lowest hit points
        std::shared_ptr<unit_t> enemy_with_lowest_hp = nullptr;
        int lowest_hp = std::numeric_limits<int>::max();
        for (auto &adj_enemy_cell : adj_enemy_cells) {
          if (adj_enemy_cell->unit->hit_points < lowest_hp) {
            enemy_with_lowest_hp = adj_enemy_cell->unit;
            lowest_hp = enemy_with_lowest_hp->hit_points;
          }
        }
        // Deal damage equal to attack power
        if (trace1)
          std::cout << "\t\tAttacking " << enemy_with_lowest_hp->get_type_char() << "(" << enemy_with_lowest_hp->x
                    << "," << enemy_with_lowest_hp->y << ")" << std::endl;
        enemy_with_lowest_hp->hit_points -= unit->attack_power;
        if (enemy_with_lowest_hp->hit_points < 0) {
          enemy_with_lowest_hp->hit_points = 0;
        }
        // Is enemy dead?
        if (enemy_with_lowest_hp->hit_points == 0) {
          // Enemy is dead
          // - Mark cell as empty
          cells[enemy_with_lowest_hp->y][enemy_with_lowest_hp->x]->clear();
          // - Remove from unit list
          int prev_sz = enemies.size();
          enemies.remove(enemy_with_lowest_hp);
          assert(prev_sz > enemies.size());
        }
        return true;
      }
      return false;
    }

    std::vector<reachability_t> find_reachable_attack_cells_for(std::shared_ptr<unit_t> unit) const {
      cell_t *unit_cell = cells[unit->y][unit->x].get();
      constexpr std::pair<int, int> valid_deltas[4] = {
        {0,  -1},
        {-1, 0},
        {1,  0},
        {0,  1}
      };
      // Get all start positions
      std::vector<cell_t *> start_cells;
      for (auto[dx, dy] : valid_deltas) {
        auto start_cell = cells[unit->y + dy][unit->x + dx].get();
        if (start_cell->is_empty()) {
          start_cells.push_back(start_cell);
        }
      }
      // Get all attack positions
      std::vector<cell_t *> attack_cells;
      auto &enemies = unit->type == unit_type_e::goblin ? elves : goblins;
      for (auto enemy : enemies) {
        for (auto[dx, dy] : valid_deltas) {
          auto cell = cells[enemy->y + dy][enemy->x + dx].get();
          if (cell->is_empty()) {
            attack_cells.push_back(cell);
          }
        }
      }
      // For each attack position compute reachability/path
      std::vector<reachability_t> reachability;
      for (auto start_cell : start_cells) {
        for (auto attack_cell : attack_cells) {
          std::vector<void *> path;
          float totalCost;
          int result = pather->Solve(start_cell, attack_cell, &path, &totalCost);
          if (result != micropather::MicroPather::NO_SOLUTION) {
            reachability_t r;
            r.source = start_cell;
            r.destination = attack_cell;
            if (result == micropather::MicroPather::START_END_SAME) {
              r.cost = 0;
              r.path.push_back(start_cell);
            } else {
              r.cost = totalCost;
              for (auto p : path) {
                r.path.push_back((cell_t *) p);
              }
            }
            reachability.push_back(r);
          }
        }
      }
      // Sort to get the shortest path to an attack cell
      std::sort(reachability.begin(), reachability.end());
      for (auto &r : reachability) {
        if (trace1)
          std::cout << "\t\t\tRcost (" << r.source->x << "," << r.source->y << ")->(" << r.destination->x << ","
                    << r.destination->y << ") : " << r.cost << std::endl;
      }

      return reachability;
    }

    void move_unit_towards(std::shared_ptr<unit_t> unit, const reachability_t &target) {
      // Mark unit cell empty
      cells[unit->y][unit->x]->clear();
      // Move unit one step in path
      if (trace1) std::cout << "\t\tMoving on " << target << std::endl;
      unit->x = target.path[0]->x;
      unit->y = target.path[0]->y;
      // Mark new unit cell
      cells[unit->y][unit->x]->set_unit(unit);
    }

    void move_toward_closest_enemy(std::shared_ptr<unit_t> unit) {
      std::vector<reachability_t> reachable_attack_cells = find_reachable_attack_cells_for(unit);
      if (reachable_attack_cells.empty()) return;

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
      if (!pather) {
        pather = std::make_shared<micropather::MicroPather>(this);
      }

      if (trace1) std::cout << "Starting round" << std::endl;
      std::vector<cell_t *> unit_cells;
      for (auto &goblin : goblins) {
        unit_cells.push_back(cells[goblin->y][goblin->x].get());
      }
      for (auto &elf : elves) {
        unit_cells.push_back(cells[elf->y][elf->x].get());
      }
      // Sort all units
      std::sort(unit_cells.begin(), unit_cells.end(), [](cell_t *a, cell_t *b) { return *a < *b; });
      // Cells will change. Iterate over units directly
      std::vector<std::shared_ptr<unit_t>> units;
      for (auto &unit_cell : unit_cells) {
        units.push_back(unit_cell->unit);
      }
      // Units are now sorted by read order
      for (auto &unit : units) {
        // No targets left! Early exit if game over (whithout incrementing ticks)
        if (game_over()) {
          return;
        }

        // Unit may be dead. Don't tick dead units
        if (unit->hit_points > 0) {
          if (trace1)
            std::cout << "\tTicking " << unit->get_type_char() << "(" << unit->x << "," << unit->y << ")" << std::endl;
          tick_unit(unit);
        }

        // ??? Reset after each unit moves?
        pather->Reset();
      }

      current_tick++;
    }

    friend std::istream &operator>>(std::istream &in, map_t &env) {
      std::string line;
      while (!in.eof()) {
        int y = env.cells.size();
        getline(in, line);
        std::vector<std::shared_ptr<cell_t>> new_row;
        for (int x = 0; x < line.size(); x++) {
          std::shared_ptr<cell_t> new_cell = std::make_shared<cell_t>();
          new_cell->x = x;
          new_cell->y = y;
          switch (line[x]) {
            case '#': {
              new_cell->type = cell_type_e::wall;
              break;
            }
            case 'G': {
              new_cell->type = cell_type_e::unit;
              auto goblin = std::make_shared<unit_t>();
              goblin->type = unit_type_e::goblin;
              goblin->attack_power = env.goblin_ap;
              goblin->x = x;
              goblin->y = y;
              new_cell->unit = goblin;
              env.goblins.push_back(goblin);
              break;
            }
            case 'E': {
              new_cell->type = cell_type_e::unit;
              auto elf = std::make_shared<unit_t>();
              elf->type = unit_type_e::elf;
              elf->attack_power = env.elf_ap;
              elf->x = x;
              elf->y = y;
              new_cell->unit = elf;
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

    friend std::ostream &operator<<(std::ostream &out, map_t &env) {
      out << "After " << env.current_tick << " rounds:" << std::endl;
      std::vector<cell_t *> unit_cells;
      for (int y = 0; y < env.cells.size(); y++) {
        auto &row = env.cells[y];
        unit_cells.clear();
        for (int x = 0; x < env.cells.size(); x++) {
          auto &cell = row[x];
          if (cell->type == cell_type_e::unit) unit_cells.push_back(cell.get());
          out << *cell;
        }
        // Render hps
        if (!unit_cells.empty()) out << "    ";
        for (auto unit_cell : unit_cells) {
          out << *(unit_cell->unit) << ", ";
        }
        out << std::endl;
      }
      out << std::endl;
      return out;
    }
  };

  void read_day15_data(map_t &outdata, const char *filepath) {
    std::ifstream input_stream(filepath);
    input_stream >> outdata;
  }

  void problem1() {
    std::cout << "Day 15 - Problem 1" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 15

    if (enable_assertions) {
      {
        map_t test;
        read_day15_data(test, "data/day15/problem1/move.txt");
        std::cout << test << std::endl;
        for (int i = 0; i < 3; i++) {
          test.tick();
          std::cout << test << std::endl;
        }
        assert(test.cells[4][1]->type == cell_type_e::unit && test.cells[4][1]->unit->type == unit_type_e::goblin);
      }
      {
        map_t test;
        read_day15_data(test, "data/day15/problem1/test1.txt");
        std::cout << test << std::endl;
        while (!test.game_over()) {
          test.tick();
          std::cout << test << std::endl;
        }
        assert(test.elapsed_rounds() * test.total_remaining_hit_points() == 27730);
      }
      {
        map_t test;
        read_day15_data(test, "data/day15/problem1/test2.txt");
        std::cout << test << std::endl;
        while (!test.game_over()) {
          test.tick();
          std::cout << test << std::endl;
        }
        assert(test.elapsed_rounds() * test.total_remaining_hit_points() == 27755);
      }
    }

    map_t input;
    read_day15_data(input, "data/day15/problem1/input.txt");
    while (!input.game_over()) {
      input.tick();
    }
    std::cout << "Result: " << (input.elapsed_rounds() * input.total_remaining_hit_points()) << std::endl;

#endif
  }

  void problem2() {
    std::cout << "Day 15 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 15

    if (enable_assertions) {
      {
        int deaths = std::numeric_limits<int>::max();
        int elf_ap = 0;
        int last_outcome = 0;
        while (deaths > 0) {
          std::cout << "Trying elf attack power: " << elf_ap << std::endl;
          map_t test(elf_ap);
          read_day15_data(test, "data/day15/problem2/test1.txt");
          int initial_elves = test.elves.size();
          while (!test.game_over()) {
            test.tick();
          }
          deaths = initial_elves - test.elves.size();
          last_outcome = (test.elapsed_rounds() * test.total_remaining_hit_points());
          elf_ap++;
        }
        assert(last_outcome == 4988);
      }
    }

    int deaths = std::numeric_limits<int>::max();
    int elf_ap = 0;
    int last_outcome = 0;
    while (deaths > 0) {
      std::cout << "Trying elf attack power: " << elf_ap << std::endl;
      map_t input(elf_ap);
      read_day15_data(input, "data/day15/problem2/input.txt");
      int initial_elves = input.elves.size();
      while (!input.game_over()) {
        input.tick();
      }
      deaths = initial_elves - input.elves.size();
      last_outcome = (input.elapsed_rounds() * input.total_remaining_hit_points());
      elf_ap++;
    }
    std::cout << "Result: " << last_outcome << std::endl;

#endif
  }

}
