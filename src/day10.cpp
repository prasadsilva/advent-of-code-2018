#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include <list>
#include <regex>
#include <limits>

namespace day10 {
    
    const bool trace_read = false;
    const bool trace1 = true;

    struct point_t {
        long x, y;

        point_t& operator += (const point_t &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        point_t operator * (long value) {
            point_t result = *this;
            result.x *= value;
            result.y *= value;
            return result;
        }

        friend std::ostream & operator << (std::ostream &out, point_t &pl) {
            out << "(" << pl.x << ", " << pl.y << ")";
            return out;
        }
    };

    struct point_of_light_t {
        point_t initial_position;
        point_t velocity;
        point_t current_position;

        point_of_light_t() {}
        point_of_light_t(point_t initial_position, point_t velocity) : initial_position(initial_position), velocity(velocity), current_position(initial_position) {}

        friend std::istream & operator >> (std::istream &in, point_of_light_t &pl) {
            // E.g.: position=< 9,  1> velocity=< 0,  2>
            std::string line;
            getline(in, line);

            std::regex light_pattern(R"(position=<\s*(-?\d+),\s*(-?\d+)> velocity=<\s*(-?\d+),\s*(-?\d+)>)");
            std::smatch light_matches;

            if (!std::regex_search(line, light_matches, light_pattern)) {
                throw new std::invalid_argument("Cannot parse step rule!");
            }

            pl.initial_position.x = std::stoi(light_matches[1].str());
            pl.initial_position.y = std::stoi(light_matches[2].str());
            pl.velocity.x = std::stoi(light_matches[3].str());
            pl.velocity.y = std::stoi(light_matches[4].str());
            pl.current_position = pl.initial_position;

            if (trace_read) std::cout << "Read " << pl << std::endl;
            return in;
        }

        friend std::ostream & operator << (std::ostream &out, point_of_light_t &pl) {
            out << "position=<" << pl.initial_position.x << ", " << pl.initial_position.y << "> velocity=<" << pl.velocity.x << ", " << pl.velocity.y << ">, current=<" << pl.current_position.x << ", " << pl.current_position.y << ">";
            return out;
        }
    };

    void read_day10_data(std::vector<point_of_light_t> &outdata, const char* filepath) {
        std::ifstream input_stream(filepath);
        point_of_light_t point_of_light;
        while (!input_stream.eof()) {
            input_stream >> point_of_light;
            outdata.push_back(point_of_light);
        }
    }

    std::pair<point_t, point_t> compute_enclosing_area_coords(const std::vector<point_of_light_t> &points) {
        long min_x = std::numeric_limits<long>::max();
        long max_x = std::numeric_limits<long>::min();
        long min_y = std::numeric_limits<long>::max();
        long max_y = std::numeric_limits<long>::min();
        for (auto &point : points) {
            if (point.current_position.x < min_x) min_x = point.current_position.x;
            if (point.current_position.x > max_x) max_x = point.current_position.x;
            if (point.current_position.y < min_y) min_y = point.current_position.y;
            if (point.current_position.y > max_y) max_y = point.current_position.y;
        }
        auto tl = point_t{min_x, min_y};
        auto br = point_t{max_x, max_y};
        // std::cout << tl << ", " << br << std::endl;
        return { tl, br };
    }

    long compute_enclosing_area(const std::vector<point_of_light_t> &points) {
        auto [tl, br] = compute_enclosing_area_coords(points);
        auto width = abs(br.x - tl.x);
        auto height = abs(br.y - tl.y);
        // std::cout << "Width: " << width << ", Height: " << height << std::endl;
        return width * height;
    }

    void move_points(std::vector<point_of_light_t> &points, int time_steps) {
        long velocity_coeff = 1;
        if (time_steps < 0) {
            velocity_coeff = -1;
        }
        while (time_steps > 0) {
            for (auto &point : points) {
                point.current_position += (point.velocity * velocity_coeff);
            }
            time_steps--;
        }
    }

    void move_points_one_timestamp(std::vector<point_of_light_t> &points) {
        move_points(points, 1);
    }

    int find_time_step_for_smallest_dimension(const std::vector<point_of_light_t> &input) {
        // Loop until enclosing area stop going down
        std::vector<point_of_light_t> points = input;
        long last_area = std::numeric_limits<long>::max();
        int time_step = 0;
        while (1) {
            int area = compute_enclosing_area(points);
            // if (trace1) std::cout << "(" << time_step << ")" << "Area: " << area << std::endl;
            if (area >= last_area) {
                break;
            }
            last_area = area;

            move_points_one_timestamp(points);

            time_step++;
        }
        return time_step - 1;
    }

    void render_output(const std::vector<point_of_light_t> &input, int swidth, int sheight) {
        auto [tl, br] = compute_enclosing_area_coords(input);
        int width = abs(br.x - tl.x) + 1;
        int height = abs(br.y - tl.y) + 1;
        
        int output_width = std::min(width, swidth);
        int output_height = std::min(height, sheight);
        float wcoeff = output_width / (float)width;
        float hcoeff = output_height / (float)height;

        std::cout << "Width : " << width << ", Height: " << height << std::endl;
        
        std::vector<std::vector<char>> output;
        output.resize(output_height, std::vector<char>(output_width, '.'));
        for (auto& point : input) {
            int x = point.current_position.x - tl.x;
            int y = point.current_position.y - tl.y;
            int sx = x * wcoeff;
            int sy = y * hcoeff;
            output[sy][sx] = '#';
        }

        for (auto& row : output) {
            for (auto& value : row) {
                std::cout << value;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void problem1() {
        std::cout << "Day 10 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 10

        std::vector<point_of_light_t> test1;
        read_day10_data(test1, "data/day10/problem1/test1.txt");
        auto time_steps = find_time_step_for_smallest_dimension(test1);
        std::cout << "Smallest area timestemp: " << time_steps << std::endl;
        move_points(test1, time_steps);
        render_output(test1, 160, 160);


        std::vector<point_of_light_t> input;
        read_day10_data(input, "data/day10/problem1/input.txt");
        int total_time_steps = 0;

        time_steps = find_time_step_for_smallest_dimension(input);
        total_time_steps += time_steps;
        std::cout << "Smallest area timestemp: " << total_time_steps << std::endl;
        move_points(input, time_steps);

        char command = 0;
        while (command != 'q') {            
            render_output(input, 80, 80);
            std::cout << "Next command (q:quit, c:force +1 timestep and continue): ";
            std::cin >> command;

            switch (command) {
                case 'c': {
                    total_time_steps++;
                    move_points(input, 1); // Skip 1 to get out of potential local minima
                    time_steps = find_time_step_for_smallest_dimension(input);
                    total_time_steps += time_steps;
                    std::cout << "Smallest area timestemp: " << total_time_steps << std::endl;
                    move_points(input, time_steps);
                    break;
                }
            }
        }

        #endif
    }

    void problem2() {
        std::cout << "Day 10 - Problem 2 (NOOP)" << std::endl;

        // Problem2 just uses the output from 1
    }

}