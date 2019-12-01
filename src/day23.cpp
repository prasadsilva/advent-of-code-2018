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
#include <cmath>

namespace day23 {

  const bool trace_read = false;
  const bool trace1 = true;
  const bool trace2 = false;
  const bool enable_assertions = true;

  using val_t = long;

  val_t compute_manhattan_distance(val_t x1, val_t y1, val_t z1, val_t x2, val_t y2, val_t z2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2) + std::abs(z1 - z2);
  }

  struct point3 {
    val_t x, y, z;

    val_t distance_from_origin() {
      return compute_manhattan_distance(0, 0, 0, x, y, z);
    }

    friend std::ostream &operator<<(std::ostream &out, const point3 &pt) {
      out << "(" << pt.x << "," << pt.y << "," << pt.z << ")";
      return out;
    }
  };

  struct nanobot_t {
    point3 coord;
    val_t radius;

    bool contains(const point3 &pt) {
      auto dist = compute_manhattan_distance(pt.x, pt.y, pt.z, coord.x, coord.y, coord.z);
      return dist <= radius;
    }

    friend std::istream &operator>>(std::istream &in, nanobot_t &nanobot) {
      std::string line;
      getline(in, line);
      assert(!line.empty());

      // pos=<0,0,0>, r=4
      std::regex scan_pattern(R"(pos=<([-\d]+),([-\d]+),([-\d]+)>, r=(\d+))");
      std::smatch scan_matches;
      if (!std::regex_search(line, scan_matches, scan_pattern)) {
        throw new std::invalid_argument("Cannot parse nanobot!");
      }

      nanobot.coord.x = static_cast<val_t>(std::stoi(scan_matches[1].str()));
      nanobot.coord.y = static_cast<val_t>(std::stoi(scan_matches[2].str()));
      nanobot.coord.z = static_cast<val_t>(std::stoi(scan_matches[3].str()));
      nanobot.radius = static_cast<val_t>(std::stoi(scan_matches[4].str()));

      if (trace_read) {
        std::cout << nanobot << std::endl;
      }

      return in;
    }

    friend std::ostream &operator<<(std::ostream &out, const nanobot_t &nanobot) {
      std::cout << "pos=<" << nanobot.coord.x << "," << nanobot.coord.y << "," << nanobot.coord.z << ">, r=" << nanobot.radius;
      return out;
    }
  };

  struct env_t {
    std::vector<std::shared_ptr<nanobot_t>> nanobots;

    env_t(const std::vector<nanobot_t> &_nanobots) {
      for (auto &_nanobot : _nanobots) {
        auto nanobot = std::make_shared<nanobot_t>();
        *nanobot = _nanobot;
        nanobots.push_back(nanobot);
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
          nanobot->coord.x, nanobot->coord.y, nanobot->coord.z,
          other_nanobot->coord.x, other_nanobot->coord.y, other_nanobot->coord.z
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

  std::vector<nanobot_t> nanobots;
  read_day23_data(nanobots, "data/day23/problem1/input.txt");
  env_t env(nanobots);
  auto nanobot = env.find_nanobot_with_largest_radius();
  std::cout << "Nanobot with largest radius: " << *nanobot << std::endl;
  std::cout << "Result: " << env.find_nanobots_in_range_of(nanobot.get()) << std::endl;

#endif
  }

  void problem2() {
    std::cout << "Day 23 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 23

    if (enable_assertions) {
      {
        nanobot_t n{};
        n.coord = {16, 12, 12};
        n.radius = 4;
        point3 pt{12, 12, 12};
        assert(n.contains(pt) == true);
      }

      {
        std::vector<nanobot_t> nanobots;
        read_day23_data(nanobots, "data/day23/problem2/test1.txt");
        env_t env(nanobots);

        // For every nanobot range
        int max_intersections = 0;
        val_t distance_with_max_intersections = std::numeric_limits<val_t>::max();
        for (auto &nanobot : env.nanobots) {
          // Go through all extreme points integer distances away on the line from origin to center of range
          auto &c = nanobot->coord;
          auto r = nanobot->radius;
          std::array<point3, 27> points = {{
                                            { c.x - r,      c.y - r,      c.z - r   },
                                            { c.x - r,      c.y - r,      c.z       },
                                            { c.x - r,      c.y - r,      c.z + r   },
                                            { c.x - r,      c.y,          c.z - r   },
                                            { c.x - r,      c.y,          c.z       },
                                            { c.x - r,      c.y,          c.z + r   },
                                            { c.x - r,      c.y + r,      c.z - r   },
                                            { c.x - r,      c.y + r,      c.z       },
                                            { c.x - r,      c.y + r,      c.z + r   },

                                            { c.x,          c.y - r,      c.z - r   },
                                            { c.x,          c.y - r,      c.z       },
                                            { c.x,          c.y - r,      c.z + r   },
                                            { c.x,          c.y,          c.z - r   },
                                            { c.x,          c.y,          c.z       },
                                            { c.x,          c.y,          c.z + r   },
                                            { c.x,          c.y + r,      c.z - r   },
                                            { c.x,          c.y + r,      c.z       },
                                            { c.x,          c.y + r,      c.z + r   },

                                            { c.x + r,      c.y - r,      c.z - r   },
                                            { c.x + r,      c.y - r,      c.z       },
                                            { c.x + r,      c.y - r,      c.z + r   },
                                            { c.x + r,      c.y,          c.z - r   },
                                            { c.x + r,      c.y,          c.z       },
                                            { c.x + r,      c.y,          c.z + r   },
                                            { c.x + r,      c.y + r,      c.z - r   },
                                            { c.x + r,      c.y + r,      c.z       },
                                            { c.x + r,      c.y + r,      c.z + r   },
          }};
          // If point is in range, check against other ranges to determine maximal coverage
          for (auto &pt : points) {
            int intersections = 0;
            for (auto &tbot : env.nanobots) {
              if (tbot->contains(pt)) {
                intersections++;
              }
            }
            if (intersections >= max_intersections) {
              max_intersections = intersections;
              val_t distance = pt.distance_from_origin();
              std::cout << "Max intersections: " << intersections << " at " << pt << " (" << distance << ")" << std::endl;
              if (intersections > max_intersections) {
                distance_with_max_intersections = distance;
              } else if (distance <= distance_with_max_intersections) {
                distance_with_max_intersections = distance;
              }
            }
          }
        }
        std::cout << "M : " << distance_with_max_intersections << std::endl;
        assert(distance_with_max_intersections == 36);
      }
    }

    std::vector<nanobot_t> nanobots;
    read_day23_data(nanobots, "data/day23/problem2/input.txt");
    env_t env(nanobots);
    // For every nanobot range
    int max_intersections = 0;
    val_t distance_with_max_intersections = std::numeric_limits<val_t>::max();
    for (auto &nanobot : env.nanobots) {
      // Go through all extreme points integer distances away on the line from origin to center of range
      auto &c = nanobot->coord;
      auto r = nanobot->radius;
      std::array<point3, 27> points = {{
                                         { c.x - r,      c.y - r,      c.z - r   },
                                         { c.x - r,      c.y - r,      c.z       },
                                         { c.x - r,      c.y - r,      c.z + r   },
                                         { c.x - r,      c.y,          c.z - r   },
                                         { c.x - r,      c.y,          c.z       },
                                         { c.x - r,      c.y,          c.z + r   },
                                         { c.x - r,      c.y + r,      c.z - r   },
                                         { c.x - r,      c.y + r,      c.z       },
                                         { c.x - r,      c.y + r,      c.z + r   },

                                         { c.x,          c.y - r,      c.z - r   },
                                         { c.x,          c.y - r,      c.z       },
                                         { c.x,          c.y - r,      c.z + r   },
                                         { c.x,          c.y,          c.z - r   },
                                         { c.x,          c.y,          c.z       },
                                         { c.x,          c.y,          c.z + r   },
                                         { c.x,          c.y + r,      c.z - r   },
                                         { c.x,          c.y + r,      c.z       },
                                         { c.x,          c.y + r,      c.z + r   },

                                         { c.x + r,      c.y - r,      c.z - r   },
                                         { c.x + r,      c.y - r,      c.z       },
                                         { c.x + r,      c.y - r,      c.z + r   },
                                         { c.x + r,      c.y,          c.z - r   },
                                         { c.x + r,      c.y,          c.z       },
                                         { c.x + r,      c.y,          c.z + r   },
                                         { c.x + r,      c.y + r,      c.z - r   },
                                         { c.x + r,      c.y + r,      c.z       },
                                         { c.x + r,      c.y + r,      c.z + r   },
                                       }};
      // If point is in range, check against other ranges to determine maximal coverage
      for (auto &pt : points) {
        int intersections = 0;
        for (auto &tbot : env.nanobots) {
          if (tbot->contains(pt)) {
            intersections++;
          }
        }
        if (intersections >= max_intersections) {
          max_intersections = intersections;
          val_t distance = pt.distance_from_origin();
          std::cout << "Max intersections: " << intersections << " at " << pt << " (" << distance << ")" << std::endl;
          if (intersections > max_intersections) {
            distance_with_max_intersections = distance;
          } else if (distance < distance_with_max_intersections) {
            distance_with_max_intersections = distance;
          }
        }
      }
    }
    std::cout << "M : " << distance_with_max_intersections << std::endl;
    std::cout << "Result: " << distance_with_max_intersections << std::endl;
    // 129475294 is too low
    //  90055549 is too low
    // 154678717 not the right answer

#endif
  }

}
