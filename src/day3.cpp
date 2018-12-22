#include <iostream>
#include <fstream>
#include <vector>
#include <array>

namespace day3 {

  const bool trace1 = false;

  static const int FABRIC_MAX_DIM = 1000;

  struct fabric_rect_t {
    int id;
    int x, y;
    int width, height;

    friend std::istream &operator>>(std::istream &in, fabric_rect_t &fr) {
      // E.g.: #1 @ 1,3: 4x4
      std::string temp_str;
      char temp_char;

      in >> temp_char >> fr.id >> temp_str >> fr.x >> temp_char >> fr.y >> temp_char >> fr.width >> temp_char
         >> fr.height;

      if (trace1) std::cout << "Read " << fr << std::endl;
      return in;
    }

    friend std::ostream &operator<<(std::ostream &out, fabric_rect_t &fr) {
      out << "#" << fr.id << " (" << fr.x << ", " << fr.y << ") - " << fr.width << ", " << fr.height;
      return out;
    }
  };

  struct fabric_t {
    std::array<std::array<int, FABRIC_MAX_DIM>, FABRIC_MAX_DIM> square_inch_claims = {};

    void claim(const fabric_rect_t &fabric_rect) {
      assert((fabric_rect.x + fabric_rect.width) <= FABRIC_MAX_DIM);
      assert((fabric_rect.y + fabric_rect.height) <= FABRIC_MAX_DIM);

      for (int y = fabric_rect.y; y < fabric_rect.y + fabric_rect.height; y++) {
        for (int x = fabric_rect.x; x < fabric_rect.x + fabric_rect.width; x++) {
          square_inch_claims[y - 1][x - 1]++;
        }
      }
    }

    bool has_overlapping_claims(const fabric_rect_t &fabric_rect) {
      assert((fabric_rect.x + fabric_rect.width) <= FABRIC_MAX_DIM);
      assert((fabric_rect.y + fabric_rect.height) <= FABRIC_MAX_DIM);

      for (int y = fabric_rect.y; y < fabric_rect.y + fabric_rect.height; y++) {
        for (int x = fabric_rect.x; x < fabric_rect.x + fabric_rect.width; x++) {
          if (square_inch_claims[y - 1][x - 1] > 1) return true;
        }
      }

      return false;
    }

    int get_num_square_inches_with_overlapping_claims() {
      int overlapping_claims = 0;
      for (auto &claims_row : square_inch_claims) {
        for (auto &claims : claims_row) {
          if (claims >= 2) overlapping_claims++;
        }
      }
      return overlapping_claims;
    }
  };

  void read_day3_data(std::vector<fabric_rect_t> &outdata, const char *filepath) {
    std::ifstream inputStream(filepath);
    fabric_rect_t fabric_rect;
    while (!inputStream.eof()) {
      inputStream >> fabric_rect;
      outdata.push_back(fabric_rect);
    }
  }

  int get_num_square_inches_overlapping(std::vector<fabric_rect_t> &input) {
    // For each fabric rect, increment the count in the fabric
    fabric_t fabric;
    for (auto &fabric_rect : input) {
      fabric.claim(fabric_rect);
    }
    // Scan the fabric and track square inches with > 2 claims
    return fabric.get_num_square_inches_with_overlapping_claims();
  }

  int find_claim_id_not_overlapping(std::vector<fabric_rect_t> &input) {
    // For each fabric rect, increment the count in the fabric
    fabric_t fabric;
    for (auto &fabric_rect : input) {
      fabric.claim(fabric_rect);
    }
    // Go through all fabric rects and check if rect has overlapping claims
    for (auto &fabric_rect : input) {
      if (!fabric.has_overlapping_claims(fabric_rect)) {
        return fabric_rect.id;
      }
    }

    return -1;
  }

  void problem1() {
    std::cout << "Day 3 - Problem 1" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 3

    std::vector<fabric_rect_t> test1;
    read_day3_data(test1, "data/day3/problem1/test1.txt");
    assert(get_num_square_inches_overlapping(test1) == 4);

    std::vector<fabric_rect_t> input;
    read_day3_data(input, "data/day3/problem1/input.txt");
    std::cout << "Result : " << get_num_square_inches_overlapping(input) << std::endl;

#endif
  }

  void problem2() {
    std::cout << "Day 3 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 3

    std::vector<fabric_rect_t> test1;
    read_day3_data(test1, "data/day3/problem2/test1.txt");
    assert(find_claim_id_not_overlapping(test1) == 3);

    std::vector<fabric_rect_t> input;
    read_day3_data(input, "data/day3/problem2/input.txt");
    std::cout << "Result : " << find_claim_id_not_overlapping(input) << std::endl;

#endif
  }
}
