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

  const bool trace_read = true;
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

  void read_day20_data(regex_t &outdata, const char *filepath) {
    std::ifstream input_stream(filepath);
    input_stream >> outdata;
  }

  void problem1() {
    std::cout << "Day 20 - Problem 1" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 20

    if (enable_assertions) {
      regex_t regex;
      read_day20_data(regex, "data/day20/problem1/test1.txt");
      std::cout << regex << std::endl;
    }

    regex_t regex;
    read_day20_data(regex, "data/day20/problem1/input.txt");
    std::cout << regex << std::endl;

#endif
  }

  void problem2() {
    std::cout << "Day 20 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 20


#endif
  }

}
