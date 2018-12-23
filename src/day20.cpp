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

namespace day20 {

  const bool trace_read = false;
  const bool trace1 = true;
  const bool trace2 = false;
  const bool enable_assertions = true;

  enum path_node_e {
    move,
    branch,
  };

  struct path_node_t {
    path_node_e type;

    explicit path_node_t(path_node_e t) : type(t) {}

    friend std::ostream &operator<<(std::ostream &out, const path_node_t *path_node) {
      path_node->print(out);
      return out;
    }

    virtual void print(std::ostream &out) const = 0;
  };

  struct path_t {
    std::list<std::shared_ptr<path_node_t>> nodes;

    friend std::ostream &operator<<(std::ostream &out, const path_t &path) {
      // Print DF order
      for (auto &node : path.nodes) {
        out << node;
      }
      return out;
    }
  };

  struct path_move_node_t : path_node_t {
    char direction{};

    path_move_node_t() : path_node_t(path_node_e::move) {}

    void print(std::ostream &out) const override {
      out << direction;
    }
  };

  struct path_branch_node_t : path_node_t {
    std::list<path_t> sub_paths;

    path_branch_node_t() : path_node_t(path_node_e::branch) {}

    void print(std::ostream &out) const override {
      out << "(";
      bool first = true;
      for (auto &path : sub_paths) {
        if (!first) out << "|";
        out << path;
        first = false;
      }
      out << ")";
    }
  };

  // ^ENWWW(NEEE|SSE(EE|N))$
  std::istream &operator>>(std::istream &in, path_t &path) {
    char c;
    do {
      if (trace_read) std::cout << "Processing " << (char)in.peek() << std::endl;
      switch (in.peek()) {
        case '(': {
          auto branch_node = std::make_shared<path_branch_node_t>();
          // process list of branches
          while (in.peek() != ')') {
            in >> c;  // consume '(', '|'
            path_t sub_path;
            in >> sub_path;
            // finalize branch
            branch_node->sub_paths.push_back(sub_path);
          }
          in >> c; // consume ')' or '|'
          assert(c == ')' || c == '|');
          path.nodes.push_back(branch_node);
          break;
        }
        case ')':
        case '|': {
          // stop path
          // don't consume ')', '|'
          return in;  // return current path
        }
        case 'N':
        case 'E':
        case 'W':
        case 'S': {
          in >> c;
          auto move_node = std::make_shared<path_move_node_t>();
          move_node->direction = c;
          path.nodes.push_back(move_node);
          break;
        }
        default: {
          // Unknown character, bail without consuming
          return in;
        }
      }

    } while (c != '$');
    return in;
  }

  struct regex_t {
    path_t start_path;

    friend std::istream &operator>>(std::istream &in, regex_t &regex) {
      char c;
      in >> c;
      assert(c == '^');
      in >> regex.start_path;
      assert(in.peek() == '$');
      in >> c;
      return in;
    }

    friend std::ostream &operator<<(std::ostream &out, const regex_t &regex) {
      out << '^' << regex.start_path << '$';
      return out;
    }
  };

  struct doors_t {
    bool valid = false;
    bool north = false, east = false, west = false, south = false;
  };

  template <int map_dim>
  struct map_t {
    std::vector<std::vector<doors_t>> cells;
    int start_x = map_dim / 2, start_y = map_dim / 2;
    int bx0 = start_x, bx1 = start_x, by0 = start_y, by1 = start_y;

    void initialize() {
      cells.resize(static_cast<unsigned long>(map_dim));
      for (auto &row : cells) {
        row.resize(static_cast<unsigned long>(map_dim));
      }
    }

    bool is_valid(int x, int y) const {
      return ((x >= 0) && (x < map_dim - 1) && (y >= 0) && (y < map_dim - 1));
    }

    void modify_bounds_for(int x, int y) {
      if (x < bx0) bx0 = x;
      else if (x > bx1) bx1 = x;
      if (y < by0) by0 = y;
      else if (y > by1) by1 = y;
    }

    void process_path(int x, int y, const path_t &path) {
      int sx = x, sy = y;
      for (auto &node : path.nodes) {
        if (node->type == path_node_e::move) {
          auto move_node = dynamic_cast<path_move_node_t*>(node.get());
          int tx = sx, ty = sy;
          switch (move_node->direction) {
            case 'N': { ty--; break; }
            case 'S': { ty++; break; }
            case 'E': { tx++; break; }
            case 'W': { tx--; break; }
            default:  { assert(0); }
          }
          if (!is_valid(tx, ty)) {
            std::cerr << "Out of bounds! (" << tx << "," << ty << ")" << std::endl;
            assert(0);
          }
          // Mark cells' doors
          switch (move_node->direction) {
            case 'N': { cells[sy][sx].north = true; cells[ty][tx].south = true; break; }
            case 'S': { cells[sy][sx].south = true; cells[ty][tx].north = true; break; }
            case 'E': { cells[sy][sx].east = true;  cells[ty][tx].west = true;  break; }
            case 'W': { cells[sy][sx].west = true;  cells[ty][tx].east = true;  break; }
            default: { assert(0); }
          }
          // if (trace1) std::cout << "Marking (" << sx << "," << sy << ") and (" << tx << "," << ty << ")" << std::endl;
          cells[sy][sx].valid = true;
          cells[ty][tx].valid = true;
          sx = tx;
          sy = ty;
        } else {
          auto branch_node = dynamic_cast<path_branch_node_t*>(node.get());
          for (auto &sub_path : branch_node->sub_paths) {
            process_path(sx, sy, sub_path);
          }
        }
        modify_bounds_for(sx, sy);
      }
    }

    void process(const regex_t &regex) {
      process_path(start_x, start_y, regex.start_path);
    }

    friend std::ostream &operator<<(std::ostream &out, const map_t &regex) {
      // Top border
      std::cout << "#";
      for (int x = regex.bx0; x <= regex.bx1; x++) {
        std::cout << "##";
      }
      std::cout << std::endl;
      // cells
      for (int y = regex.by0; y <= regex.by1; y++) {
        // cell row
        std::cout << '#';
        for (int x = regex.bx0; x <= regex.bx1; x++) {
          auto &doors = regex.cells[y][x];
          if (doors.valid) {
            if (regex.start_x == x && regex.start_y == y) {
              std::cout << 'X';
            } else {
              std::cout << '.';
            }
            if (doors.east) {
              std::cout << '|';
            } else {
              std::cout << '#';
            }
          } else {
            std::cout << "##";
          }
        }
        std::cout << std::endl;
        // bottom connection row
        std::cout << '#';
        for (int x = regex.bx0; x <= regex.bx1; x++) {
          auto &doors = regex.cells[y][x];
          if (doors.south) {
            std::cout << '-';
          } else {
            std::cout << '#';
          }
          std::cout << '#';
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
      return out;
    }
  };

  void read_day20_data(regex_t &outdata, const char *filepath) {
    std::ifstream input_stream(filepath);
    input_stream >> outdata;
  }

  void problem1() {
    std::cout << "Day 20 - Problem 1" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 20

    if (enable_assertions) {
      {
        regex_t regex;
        read_day20_data(regex, "data/day20/problem1/test1.txt");
        std::cout << regex << std::endl;
        map_t<100> map;
        map.initialize();
        map.process(regex);
        std::cout << map << std::endl;
      }
      {
        regex_t regex;
        read_day20_data(regex, "data/day20/problem1/test2.txt");
        std::cout << regex << std::endl;
        map_t<100> map;
        map.initialize();
        map.process(regex);
        std::cout << map << std::endl;
      }
    }

    regex_t regex;
    read_day20_data(regex, "data/day20/problem1/input.txt");
//    std::cout << regex << std::endl;
    map_t<200> map;
    map.initialize();
    map.process(regex);
    std::cout << map << std::endl;

#endif
  }

  void problem2() {
    std::cout << "Day 20 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 20


#endif
  }

}
