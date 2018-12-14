#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include <list>
#include <regex>
#include <limits>
#include <array>

const bool trace_read = true;
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

    void move_to(int _x, int _y) {
        x = _x;
        y = _y;
        // Incremenent behavior
        behavior++;
        if (behavior == cart_behavior_e::sentinel) {
            behavior = cart_behavior_e::turn_left;
        }
    }

    bool is_at(int _x, int _y) { return x == _x && y == _y; }
    int get_direction() { return direction; }

};

template <int env_dim>
struct environment_t {
    std::array<std::array<track_type_e, env_dim>, env_dim> tracks{track_type_e::none};
    std::vector<cart_t> carts;

    friend std::ostream & operator << (std::ostream &out, environment_t &env) {
        for (int y = 0; y < env_dim; y++) {
            for (int x = 0; x < env_dim; x++) {
                char out_c = ' ';
                // Check if cart is in position
                for (auto &cart : env.carts) {
                    if (cart.is_at(x, y)) {
                        switch (cart.get_direction()) {
                            case direction_e::up: out_c = '^'; break;
                            case direction_e::right: out_c = '>'; break;
                            case direction_e::down: out_c = 'v'; break;
                            case direction_e::left: out_c = '<'; break;
                        }
                    }
                }
                // If not, render the track
                if (out_c == ' ') {
                    switch (env.tracks[y][x]) {
                        case track_type_e::intersection: out_c = '+'; break;
                        case track_type_e::up_down: out_c = '|'; break;
                        case track_type_e::left_right: out_c = '-'; break;
                        case track_type_e::up_left: out_c = '/'; break;
                        case track_type_e::down_left: out_c = '\\'; break;
                        case track_type_e::down_right: out_c = '/'; break;
                        case track_type_e::up_right: out_c = '\\'; break;
                        case track_type_e::none: out_c = ' '; break;
                    }
                }

                out << out_c;
            }
            out << std::endl;
        }
        out << std::endl;
        return out;
    }
};

template <int env_dim>
void read_day13_data(environment_t<env_dim> &outdata, const char* filepath) {
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
                case '+':   outdata.tracks[y][x] = track_type_e::intersection; break;
                case '|':   outdata.tracks[y][x] = track_type_e::up_down; break;
                case '-':   outdata.tracks[y][x] = track_type_e::left_right; break;
                case '/': {
                    outdata.tracks[y][x] = track_type_e::down_right;
                    int left_x = x - 1;
                    if ((left_x >= 0) && (left_x < env_dim)) {
                        if (processed_input[y][left_x] == '-') {
                            outdata.tracks[y][x] = track_type_e::up_left;
                        }
                    }
                    break;
                }
                case '\\': {
                    outdata.tracks[y][x] = track_type_e::up_right;
                    int left_x = x - 1;
                    if ((left_x >= 0) && (left_x < env_dim)) {
                        if (processed_input[y][left_x] == '-') {
                            outdata.tracks[y][x] = track_type_e::down_left;
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
                        left = processed_input[y][left_x] != ' ';
                    }
                    if ((right_x >= 0) && (right_x < env_dim)) {
                        right = processed_input[y][right_x] != ' ';
                    }
                    if ((up_y >= 0) && (up_y < env_dim)) {
                        up = processed_input[up_y][x] != ' ';
                    }
                    if ((down_y >= 0) && (down_y < env_dim)) {
                        down = processed_input[down_y][x] != ' ';
                    }
                    // Check patterns
                    if (up && down && left && right)    outdata.tracks[y][x] = track_type_e::intersection;
                    else if (down && left)              outdata.tracks[y][x] = track_type_e::down_left;
                    else if (up && left)                outdata.tracks[y][x] = track_type_e::up_left;
                    else if (up && right)               outdata.tracks[y][x] = track_type_e::up_right;
                    else if (down_right)                outdata.tracks[y][x] = track_type_e::down_right;
                    else if (left && right)             outdata.tracks[y][x] = track_type_e::left_right;
                    else if (up && down)                outdata.tracks[y][x] = track_type_e::up_down;
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
                    std::cerr << "Unknown input char '" << pc << "' (" << (int)pc << ")" << " (" << x << ", " << y << ")" << std::endl;
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

namespace day13 {

    void problem1() {
        std::cout << "Day 13 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 13

        environment_t<16> test1;
        read_day13_data(test1, "data/day13/problem1/test1.txt");
        std::cout << test1 << std::endl;

        environment_t<160> input;
        read_day13_data(input, "data/day13/problem1/input.txt");
        std::cout << input << std::endl;

        #endif
    }

    void problem2() {
        std::cout << "Day 13 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 13

        

        #endif
    }

}