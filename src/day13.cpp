#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include <list>
#include <regex>
#include <limits>
#include <array>

namespace day13 {

  const bool trace_read = false;
  const bool trace_carts_collisions = true;
  const bool trace1 = true;
  const bool trace2 = false;
  const bool enable_assertions = false;

  enum direction_e {
    up,
    right,
    down,
    left,
  };

  enum track_type_e {
    none,
    up_down,
    left_right,
    up_left,
    up_right,
    down_left,
    down_right,
    intersection,
  };

  enum cart_behavior_e {
    turn_left = 0,
    go_straight,
    turn_right,

    sentinel,
  };

  struct track_t {
    track_type_e type = track_type_e::none;
  };

  class cart_t {
    int x;
    int y;
    int direction;
    int behavior;

  public:
    void initialize(int _x, int _y, int _direction) {
      x = _x;
      y = _y;
      direction = _direction;
      behavior = cart_behavior_e::turn_left;
    }

    void move_to(int _x, int _y, int _direction, bool turned_at_intersection) {
      x = _x;
      y = _y;
      direction = _direction;

      if (turned_at_intersection) {
        // If turned, incremenent behavior
        behavior++;
        if (behavior == cart_behavior_e::sentinel) {
          behavior = cart_behavior_e::turn_left;
        }
      }
    }

    bool is_at(int _x, int _y) const { return x == _x && y == _y; }

    int get_direction() const { return direction; }

    int get_behavior() const { return behavior; }

    int get_x() const { return x; }

    int get_y() const { return y; }

    bool operator<(const cart_t &other) const {
      if (y < other.y) return true;
      else if (y == other.y) {
        return x < other.x;
      }
      return false;
    }
  };

  template<int env_dim>
  struct environment_t {
    std::array<std::array<track_t, env_dim>, env_dim> tracks;
    std::vector<cart_t> carts;

    std::pair<int, int> get_first_collision() {
      std::sort(carts.begin(), carts.end());

      for (int i = 1; i < carts.size(); i++) {
        auto &cart0 = carts[i - 1];
        auto &cart1 = carts[i];
        if ((cart0.get_x() == cart1.get_x()) && (cart0.get_y() == cart1.get_y())) {
          return {cart0.get_x(), cart0.get_y()};
        }
      }

      return {-1, -1};
    }

    bool has_cart_collision() {
      auto[x, y] = get_first_collision();
      return x != -1 && y != -1;
    }

    void remove_colliding_carts() {
      std::sort(carts.begin(), carts.end());

      std::vector<cart_t> next_carts;
      int i = 0;
      while (i < (carts.size() - 1)) {
        auto &cart0 = carts[i];
        auto &cart1 = carts[i + 1];
        if ((cart0.get_x() == cart1.get_x()) && (cart0.get_y() == cart1.get_y())) {
          // Don't copy the current cart and the next cart
          i++;
        } else {
          next_carts.push_back(carts[i]);
        }
        i++;
      }
      if (i < carts.size()) {
        next_carts.push_back(carts[i]);
      }

      carts = next_carts;
    }

    void tick() {
      std::sort(carts.begin(), carts.end());

      for (auto &cart : carts) {
        int x = cart.get_x();
        int y = cart.get_y();
        int direction = cart.get_direction();
        int next_x = x, next_y = y;
        switch (direction) {
          case direction_e::up: {
            next_y = y - 1;
            assert(y >= 0 && y < env_dim);
            break;
          }
          case direction_e::down: {
            next_y = y + 1;
            assert(y >= 0 && y < env_dim);
            break;
          }
          case direction_e::left: {
            next_x = x - 1;
            assert(x >= 0 && x < env_dim);
            break;
          }
          case direction_e::right: {
            next_x = x + 1;
            assert(x >= 0 && x < env_dim);
            break;
          }
        }
        // Next track cell needs to have a track
        assert(tracks[y][x].type != track_type_e::none);
        // What's the next direction?
        int next_direction = direction;
        bool turned_at_intersection = false;
        auto &next_track = tracks[next_y][next_x];
        if (next_track.type == track_type_e::intersection) {
          // Turning logic
          switch (cart.get_behavior()) {
            case cart_behavior_e::turn_left: {
              if (direction == direction_e::up) next_direction = direction_e::left;
              else if (direction == direction_e::left) next_direction = direction_e::down;
              else if (direction == direction_e::down) next_direction = direction_e::right;
              else if (direction == direction_e::right) next_direction = direction_e::up;
              break;
            }
            case cart_behavior_e::turn_right: {
              if (direction == direction_e::up) next_direction = direction_e::right;
              else if (direction == direction_e::left) next_direction = direction_e::up;
              else if (direction == direction_e::down) next_direction = direction_e::left;
              else if (direction == direction_e::right) next_direction = direction_e::down;
              break;
            }
            case cart_behavior_e::go_straight: {
              // No turning
              break;
            }
          }
          turned_at_intersection = true;
        } else {
          switch (direction) {
            case direction_e::up: {
              if (next_track.type == track_type_e::down_right) {
                next_direction = direction_e::right;
              } else if (next_track.type == track_type_e::down_left) {
                next_direction = direction_e::left;
              }
              break;
            }
            case direction_e::down: {
              if (next_track.type == track_type_e::up_right) {
                next_direction = direction_e::right;
              } else if (next_track.type == track_type_e::up_left) {
                next_direction = direction_e::left;
              }
              break;
            }
            case direction_e::left: {
              if (next_track.type == track_type_e::down_right) {
                next_direction = direction_e::down;
              } else if (next_track.type == track_type_e::up_right) {
                next_direction = direction_e::up;
              }
              break;
            }
            case direction_e::right: {
              if (next_track.type == track_type_e::down_left) {
                next_direction = direction_e::down;
              } else if (next_track.type == track_type_e::up_left) {
                next_direction = direction_e::up;
              }
              break;
            }
          }
        }

        cart.move_to(next_x, next_y, next_direction, turned_at_intersection);
      }
    }

    friend std::ostream &operator<<(std::ostream &out, environment_t &env) {
      std::array<std::array<char, env_dim>, env_dim> output;
      for (int y = 0; y < env_dim; y++) {
        for (int x = 0; x < env_dim; x++) {
          char out_c = ' ';
          switch (env.tracks[y][x].type) {
            case track_type_e::intersection:
              out_c = '+';
              break;
            case track_type_e::up_down:
              out_c = '|';
              break;
            case track_type_e::left_right:
              out_c = '-';
              break;
            case track_type_e::up_left:
              out_c = '%';
              break;
            case track_type_e::down_left:
              out_c = '\\';
              break;
            case track_type_e::down_right:
              out_c = '/';
              break;
            case track_type_e::up_right:
              out_c = '#';
              break;
            case track_type_e::none:
              out_c = ' ';
              break;
          }
          output[y][x] = out_c;
        }
      }
      if (trace_carts_collisions) {
        // Add Carts and collisions
        for (auto &cart : env.carts) {
          char out_c = 0;
          char in = output[cart.get_y()][cart.get_x()];
          if (in == '^' || in == '>' || in == 'v' || in == '<' || in == 'X') {
            out_c = 'X';
          } else {
            switch (cart.get_direction()) {
              case direction_e::up:
                out_c = '^';
                break;
              case direction_e::right:
                out_c = '>';
                break;
              case direction_e::down:
                out_c = 'v';
                break;
              case direction_e::left:
                out_c = '<';
                break;
            }
          }
          output[cart.get_y()][cart.get_x()] = out_c;
        }
      }

      for (int y = 0; y < env_dim; y++) {
        for (int x = 0; x < env_dim; x++) {
          out << output[y][x];
        }
        out << std::endl;
      }
      out << std::endl;

      return out;
    }
  };

  template<int env_dim>
  void read_day13_data(environment_t<env_dim> &outdata, const char *filepath) {
    std::ifstream input_stream(filepath);
    std::string line;
    std::array<std::array<char, env_dim>, env_dim> processed_input{0};
    int x = 0, y = 0;
    // Process the input first
    while (!input_stream.eof()) {
      getline(input_stream, line);
      if (trace_read) std::cout << "Read: " << line << std::endl;
      int x = 0;
      for (auto c : line) {
        processed_input[y][x] = c;
        x++;
      }
      y++;
    }
    if (trace_read) std::cout << "Processed input.." << std::endl;

    // Load processed input into environment
    for (int y = 0; y < env_dim; y++) {
      for (int x = 0; x < env_dim; x++) {
        // Tracks
        auto pc = processed_input[y][x];
        switch (pc) {
          case '+':
            outdata.tracks[y][x].type = track_type_e::intersection;
            break;
          case '|':
            outdata.tracks[y][x].type = track_type_e::up_down;
            break;
          case '-':
            outdata.tracks[y][x].type = track_type_e::left_right;
            break;
          case '/': {
            outdata.tracks[y][x].type = track_type_e::down_right;
            int left_x = x - 1;
            if ((left_x >= 0) && (left_x < env_dim)) {
              auto p = processed_input[y][left_x];
              if (p == '-' || p == '+' || p == '>' || p == '<') {
                outdata.tracks[y][x].type = track_type_e::up_left;
              }
            }
            break;
          }
          case '\\': {
            outdata.tracks[y][x].type = track_type_e::up_right;
            int left_x = x - 1;
            if ((left_x >= 0) && (left_x < env_dim)) {
              auto p = processed_input[y][left_x];
              if (p == '-' || p == '+' || p == '>' || p == '<') {
                outdata.tracks[y][x].type = track_type_e::down_left;
              }
            }
            break;
          }
          case '^':
          case '>':
          case 'v':
          case '<': {
            // Cart is in cell. Figure out the track underneath the cart
            int left_x = x - 1;
            int right_x = x + 1;
            int up_y = y - 1;
            int down_y = y + 1;
            bool left = false, right = false, up = false, down = false;
            if ((left_x >= 0) && (left_x < env_dim)) {
              auto p = processed_input[y][left_x];
              left = p == '-' || p == '\\' || p == '/' || p == '+';
            }
            if ((right_x >= 0) && (right_x < env_dim)) {
              auto p = processed_input[y][right_x];
              right = p == '-' || p == '\\' || p == '/' || p == '+';
            }
            if ((up_y >= 0) && (up_y < env_dim)) {
              auto p = processed_input[up_y][x];
              up = p == '|' || p == '\\' || p == '/' || p == '+';
            }
            if ((down_y >= 0) && (down_y < env_dim)) {
              auto p = processed_input[down_y][x];
              down = p == '|' || p == '\\' || p == '/' || p == '+';
            }
            // Check patterns
            if (up && down && left && right) outdata.tracks[y][x].type = track_type_e::intersection;
            else if (down && left) outdata.tracks[y][x].type = track_type_e::down_left;
            else if (up && left) outdata.tracks[y][x].type = track_type_e::up_left;
            else if (up && right) outdata.tracks[y][x].type = track_type_e::up_right;
            else if (down && right) outdata.tracks[y][x].type = track_type_e::down_right;
            else if (left && right) outdata.tracks[y][x].type = track_type_e::left_right;
            else if (up && down) outdata.tracks[y][x].type = track_type_e::up_down;

            break;
          }
          case ' ': {
            // Empty
            break;
          }
          case 0: {
            // End of row
            break;
          }
          default: {
            std::cerr << "Unknown input char '" << pc << "' (" << (int) pc << ")" << " (" << x << ", " << y << ")"
                      << std::endl;
            throw std::invalid_argument("Unknown input character!");
          }
        }

        // Carts
        if (pc == '^' || pc == '>' || pc == 'v' || pc == '<') {
          int direction;
          switch (pc) {
            case '^': {
              direction = direction_e::up;
              break;
            }
            case '>': {
              direction = direction_e::right;
              break;
            }
            case 'v': {
              direction = direction_e::down;
              break;
            }
            case '<': {
              direction = direction_e::left;
              break;
            }
          }
          cart_t cart;
          cart.initialize(x, y, direction);
          outdata.carts.push_back(cart);
        }
      }
    }
  }

  void problem1() {
    std::cout << "Day 13 - Problem 1" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 13

    environment_t<16> test1;
    read_day13_data(test1, "data/day13/problem1/test1.txt");
    int ticks = 0;
    while (!test1.has_cart_collision()) {
      std::cout << "Ticks: " << ticks++ << std::endl;
      std::cout << test1 << std::endl;
      test1.tick();
    }
    std::cout << "Ticks: " << ticks++ << " - COLLISION" << std::endl;
    std::cout << test1 << std::endl;
    auto[x_test, y_test] = test1.get_first_collision();
    assert(x_test == 7 && y_test == 3);

    environment_t<160> input;
    read_day13_data(input, "data/day13/problem1/input.txt");
    ticks = 0;
    std::cout << input << std::endl;
    while (!input.has_cart_collision()) {
      // std::cout << "Ticks: " << ticks++ << std::endl;
      // std::cout << input << std::endl;
      input.tick();
    }
    auto[x, y] = input.get_first_collision();
    // std::cout << "Ticks: " << ticks++ << std::endl;
    std::cout << input << std::endl;
    std::cout << "Result: " << x << "," << y << std::endl;

#endif
  }

  void problem2() {
    std::cout << "Day 13 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 13

    environment_t<10> test1;
    read_day13_data(test1, "data/day13/problem2/test1.txt");
    int ticks = 0;
    while (test1.carts.size() > 1) {
      while (!test1.has_cart_collision()) {
        std::cout << "Ticks: " << ticks++ << std::endl;
        std::cout << test1 << std::endl;
        test1.tick();
      }
      std::cout << test1 << std::endl;
      test1.remove_colliding_carts();
    }
    assert(test1.carts.size() == 1);
    assert(test1.carts[0].get_x() == 6 && test1.carts[0].get_y() == 4);

    environment_t<160> input;
    read_day13_data(input, "data/day13/problem2/input.txt");
    ticks = 0;
    while (input.carts.size() > 1) {
      while (!input.has_cart_collision()) {
        // std::cout << "Ticks: " << ticks++ << std::endl;
        // std::cout << test1 << std::endl;
        input.tick();
      }
      // std::cout << test1 << std::endl;
      input.remove_colliding_carts();
    }
    assert(input.carts.size() == 1);
    std::cout << "Result: " << input.carts[0].get_x() << "," << input.carts[0].get_y() << std::endl;

#endif
  }

}
