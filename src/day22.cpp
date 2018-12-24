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

namespace day22 {

  const bool trace_read = false;
  const bool trace1 = false;
  const bool trace2 = false;
  const bool enable_assertions = true;

  using val_t = unsigned long;

  enum class region_type_e {
    unknown,
    mouth,
    target,
    rocky,
    narrow,
    wet
  };

  struct region_t {
    val_t x, y;
    region_type_e type;
    bool has_geologic_index = false;
    val_t geologic_index;
    bool has_erosion_level = false;
    val_t erosion_level;
  };

  struct cave_t {
    val_t depth;
    val_t target_x, target_y;
    val_t width, height;
    std::vector<std::vector<region_t>> regions;

    cave_t(val_t _depth, val_t _target_x, val_t _target_y) :
      depth(_depth), target_x(_target_x), target_y(_target_y), width(_target_x + 1), height(_target_y + 1) {
      regions.resize(height);
      for (val_t y = 0; y < height; y++) {
        auto &row = regions[y];
        row.resize(width);
        for (val_t x = 0; x < width; x++) {
          auto &region = row[x];
          region.x = x;
          region.y = y;
          region.type = region_type_e ::unknown;
          if (x == 0 && y == 0) {
            region.type = region_type_e::mouth;
          } else if (x == target_x && y == target_y) {
            region.type = region_type_e::target;
          }
        }
      }
    }

    val_t get_geologic_index(val_t x, val_t y) {
      auto &region = regions[y][x];
      if (!region.has_geologic_index) {
        val_t geologic_index;
        // The region at 0,0 (the mouth of the cave) has a geologic index of 0.
        if (x == 0 && y == 0) geologic_index = 0;
          // The region at the coordinates of the target has a geologic index of 0.
        else if (x == target_x && y == target_y) geologic_index = 0;
          // If the region's Y coordinate is 0, the geologic index is its X coordinate times 16807.
        else if (y == 0) geologic_index = x * 16807;
          // If the region's X coordinate is 0, the geologic index is its Y coordinate times 48271.
        else if (x == 0) geologic_index = y * 48271;
          // Otherwise, the region's geologic index is the result of multiplying the erosion levels of the regions at X-1,Y and X,Y-1.
        else {
          geologic_index = get_erosion_level(x - 1, y) * get_erosion_level(x, y - 1);
        }
        // Cache value
        region.geologic_index = geologic_index;
        region.has_geologic_index = true;
      }
      return region.geologic_index;
    }

    val_t get_erosion_level(val_t x, val_t y) {
      auto &region = regions[y][x];
      if (region.type == region_type_e::unknown) {
        // A region's erosion level is its geologic index plus the cave system's depth, all modulo 20183. Then:
        auto erosion_level = (get_geologic_index(x, y) + depth) % 20183;
        auto type = region_type_e::unknown;
        auto elm = erosion_level % 3;
        // If the erosion level modulo 3 is 0, the region's type is rocky.
        if (elm == 0) type = region_type_e::rocky;
        // If the erosion level modulo 3 is 1, the region's type is wet.
        else if (elm == 1) type = region_type_e::wet;
        // If the erosion level modulo 3 is 2, the region's type is narrow.
        else if (elm == 2) type = region_type_e::narrow;
        // Cache value
        region.erosion_level = erosion_level;
        region.type = type;
      }
      return region.erosion_level;
    }

    void process_regions() {
      for (auto &row : regions) {
        for (auto &region : row) {
          // This computes and caches the type, erosion level and geologic index of region
          get_erosion_level(region.x, region.y);
        }
      }
    }

    val_t get_risk_level() {
      val_t risk_level = 0;
      for (auto &row : regions) {
        for (auto &region : row) {
          switch (region.type) {
            case region_type_e::rocky:    risk_level += 0;  break;
            case region_type_e::wet:      risk_level += 1;  break;
            case region_type_e::narrow:   risk_level += 2;  break;
            default:                      assert(0);
          }
        }
      }
      return risk_level;
    }

    friend std::ostream &operator<<(std::ostream &out, const cave_t &cave) {
      for (auto &row : cave.regions) {
        for (auto &region : row) {
          switch (region.type) {
            case region_type_e::unknown:  out << "?"; break;
            case region_type_e::mouth:    out << "M"; break;
            case region_type_e::target:   out << "T"; break;
            case region_type_e::rocky:    out << "."; break;
            case region_type_e::wet:      out << "="; break;
            case region_type_e::narrow:   out << "|"; break;
            default:                      assert(0);
          }
        }
        out << std::endl;
      }
      out << std::endl;
      return out;
    }
  };

  void problem1() {
    std::cout << "Day 22 - Problem 1" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 22

  if (enable_assertions) {
    {
      cave_t cave(510, 10, 10);
      cave.process_regions();
      std::cout << cave << std::endl;
      assert(cave.get_risk_level() == 114);
    }
  }

  cave_t cave(5355, 14, 796);
  cave.process_regions();
  std::cout << cave << std::endl;
  std::cout << "Result: " << cave.get_risk_level() << std::endl;

#endif
  }

  void problem2() {
    std::cout << "Day 22 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 22


#endif
  }

}
