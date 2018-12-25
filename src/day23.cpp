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

namespace day23 {

  const bool trace_read = true;
  const bool trace1 = true;
  const bool trace2 = false;
  const bool enable_assertions = true;

  using val_t = long;

  struct nanobot_t {
    val_t x, y, z;
    val_t radius;

    friend std::istream &operator>>(std::istream &in, nanobot_t &nanobot) {
      std::string line;
      getline(in, line);
      assert(!line.empty());

      // pos=<0,0,0>, r=4
      std::regex scan_pattern(R"(pos=<(\d+),(\d+),(\d+)>, r=(\d+))");
      std::smatch scan_matches;
      if (!std::regex_search(line, scan_matches, scan_pattern)) {
        throw new std::invalid_argument("Cannot parse nanobot!");
      }

      nanobot.x = static_cast<val_t>(std::stoi(scan_matches[1].str()));
      nanobot.y = static_cast<val_t>(std::stoi(scan_matches[2].str()));
      nanobot.z = static_cast<val_t>(std::stoi(scan_matches[3].str()));
      nanobot.radius = static_cast<val_t>(std::stoi(scan_matches[4].str()));

      if (trace_read) {
        std::cout << nanobot << std::endl;
      }

      return in;
    }

    friend std::ostream &operator<<(std::ostream &out, const nanobot_t &nanobot) {
      std::cout << "pos=<" << nanobot.x << "," << nanobot.y << "," << nanobot.z << ">, r=" << nanobot.radius;
      return out;
    }
  };

  struct cell_t {
    val_t x, y, z;
    std::shared_ptr<nanobot_t> nanobot = nullptr;
  };

  val_t compute_manhattan_distance(val_t x1, val_t y1, val_t z1, val_t x2, val_t y2, val_t z2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2) + std::abs(z1 - z2);
  }

  struct env_t {
    val_t max_x = 0, max_y = 0, max_z = 0;
    std::vector<std::shared_ptr<nanobot_t>> nanobots;
    std::vector<std::vector<std::vector<cell_t>>> cells;

    env_t(const std::vector<nanobot_t> &_nanobots) {
      // Find max dims
      for (auto &nanobot : _nanobots) {
        if (nanobot.x > max_x) max_x = nanobot.x;
        if (nanobot.y > max_y) max_y = nanobot.y;
        if (nanobot.z > max_z) max_z = nanobot.z;
      }
      // Reserve cells
      cells.resize(static_cast<unsigned long>(max_z + 1));
      for (val_t z = 0; z <= max_z; z++) {
        auto &slice = cells[z];
        slice.resize(static_cast<unsigned long>(max_y + 1));
        for (val_t y = 0; y <= max_y; y++) {
          auto &row = slice[y];
          row.resize(static_cast<unsigned long>(max_x + 1));
          for (val_t x = 0; x <= max_x; x++) {
            auto &cell = row[x];
            cell.x = x;
            cell.y = y;
            cell.z = z;
          }
        }
      }
      // Add nanobots to cells
      for (auto &_nanobot : _nanobots) {
        auto nanobot = std::make_shared<nanobot_t>();
        *nanobot = _nanobot;
        nanobots.push_back(nanobot);
        cells[_nanobot.z][_nanobot.y][_nanobot.x].nanobot = nanobot;
      }
    }

    std::shared_ptr<nanobot_t> find_nanobot_with_largest_radius() {
      val_t max_radius = 0;
      std::shared_ptr<nanobot_t> nanobot_with_largest_radius = nullptr;
      for (const auto &nanobot : nanobots) {
        if (nanobot->radius > max_radius) {
          max_radius = nanobot->radius;
          nanobot_with_largest_radius = nanobot;
        }
      }
      return nanobot_with_largest_radius;
    }

    val_t find_nanobots_in_range_of(nanobot_t *nanobot) {
      int in_range_count = 0;
      for (const auto &other_nanobot : nanobots) {
        auto dist = compute_manhattan_distance(
          nanobot->x, nanobot->y, nanobot->z,
          other_nanobot->x, other_nanobot->y, other_nanobot->z
        );
        if (dist <= nanobot->radius) {
          in_range_count++;
        }
      }
      return in_range_count;
    }
  };

  void read_day23_data(std::vector<nanobot_t> &outdata, const char *filepath) {
    std::ifstream input_stream(filepath);
    while (!input_stream.eof()) {
      nanobot_t nanobot{};
      input_stream >> nanobot;
      outdata.push_back(nanobot);
    }
  }

  void problem1() {
    std::cout << "Day 23 - Problem 1" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 23

  if (enable_assertions) {
    {
      std::vector<nanobot_t> nanobots;
      read_day23_data(nanobots, "data/day23/problem1/test1.txt");
      env_t env(nanobots);
      auto nanobot = env.find_nanobot_with_largest_radius();
      assert(env.find_nanobots_in_range_of(nanobot.get()) == 7);
    }
  }


#endif
  }

  void problem2() {
    std::cout << "Day 23 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 23


#endif
  }

}
