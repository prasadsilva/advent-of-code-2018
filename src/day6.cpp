#include <iostream>
#include <fstream>
#include <cctype>   // islower, toupper, tolower
#include <limits>
#include <set>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

const bool trace_read = false;
const bool trace1 = false;

struct point_t {
    int index;
    int x, y;

    point_t(int index) : index(index) {}

    friend std::istream & operator >> (std::istream &in, point_t &point) {
        // E.g. 3, 4
        char temp_char;
        in >> point.x >> temp_char >> point.y;
         
        if (trace_read) std::cout << "Read " << point << std::endl;
        return in;
    }

    friend std::ostream & operator << (std::ostream &out, point_t &point) {
        out << "[" << point.index << "] (" << point.x << ", " << point.y << ")";
        return out;
    }
};

struct point_distance_t {
    std::shared_ptr<point_t> point;
    int distance;
    bool shared;

    point_distance_t() : point(nullptr), distance(-1), shared(false) {}
};

int compute_manhattan_distance(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

struct grid_t {
    int width, height;
    std::vector<std::vector<point_distance_t>> point_distances;

    grid_t(int width, int height) : width(width), height(height) {
        if (trace_read) std::cout << "Creating grid " << width << "x" << height << std::endl;
        point_distances.resize(height, std::vector<point_distance_t>(width));
    }

    void add_point(std::shared_ptr<point_t> point) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                auto &pd = point_distances[y][x];
                auto new_distance = compute_manhattan_distance(point->x, point->y, x, y);
                point_distance_t new_pd;
                new_pd.point = point;
                new_pd.distance = new_distance;
                new_pd.shared = false;

                if (!pd.point) {
                    // No points in grid cell; add cell regardless of distance
                    point_distances[y][x] = new_pd;
                } else {
                    // Point exists in grid
                    // If registered distance >= incoming distance then replace with incoming point
                    if (pd.distance >= new_distance) {
                        // And if registered distance == incoming distance set the shared to true
                        if (pd.distance == new_distance) {
                            new_pd.shared = true;
                        }
                        point_distances[y][x] = new_pd;
                    }
                }
            }
        }
    }

    void visit_all(std::function<void(int, int, const point_distance_t&)> visitor) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                visitor(x, y, point_distances[y][x]);
            }
        }
    }

    void visit_perimeter(std::function<void(int, int, const point_distance_t&)> visitor) {
        // Top
        for (int x = 0; x < width; x++) {
            visitor(x, 0, point_distances[0][x]);   
        }
        // Right
        for (int y = 1; y < height; y++) {
            visitor(width - 1, y, point_distances[y][width - 1]);
        }
        // Bottom
        for (int x = width - 2; x > -1; x--) {
            visitor(x, height - 1, point_distances[height - 1][x]);
        }
        // Left
        for (int y = height - 2; y > 0; y--) {
            visitor(0, y, point_distances[y][0]);
        }
    }
};

void read_day6_data(std::vector<std::shared_ptr<point_t>> &outdata, const char* filepath) {
    std::ifstream inputStream(filepath);
    int index = 0;
    while (!inputStream.eof()) {
        point_t point = { index++ };
        inputStream >> point;
        outdata.push_back(std::make_shared<point_t>(point));
    }
}

int find_largest_finite_area(std::vector<std::shared_ptr<point_t>> &input) {
    int max_x = 0, max_y = 0;
    // Find the largest coordinate in list of points
    for (auto &pt : input) {
        if (pt->x > max_x) {
            max_x = pt->x;
        }
        if (pt->y > max_y) {
            max_y = pt->y;
        }
    }

    grid_t grid(max_x + 1, max_y + 1);
    for (auto &pt : input) {
        grid.add_point(pt);
    }
    
    // For each point, count how many cells are registered to that point
    // NOTE: The incoming points have sequential indices
    std::vector<int> cell_counts;
    cell_counts.resize(input.size(), 0);
    int last_y = 0;
    if (trace1) std::cout << std::endl;
    grid.visit_all([&last_y, &cell_counts](int x, int y, const point_distance_t &pd) {
        if (trace1) {
            // Trace a map
            if (last_y != y) {
                std::cout << std::endl;
                last_y = y;
            }
            if (pd.shared) std::cout << ".";
            else if (pd.distance == 0) std::cout << char(65 + pd.point->index);
            else std::cout << char(97 + pd.point->index);
        }
        int point_index = pd.point->index;
        if (!pd.shared) cell_counts[point_index]++;
    });
    if (trace1) std::cout << std::endl << std::endl;

    // For points registered in perimeter, zero the cell counts (these are infinite areas)
    grid.visit_perimeter([&cell_counts](int x, int y, const point_distance_t &pd) {
        // std::cout << "Perimeter visiting " << x << ", " << y << std::endl;
        int point_index = pd.point->index;
        if (!pd.shared) cell_counts[point_index] = -1;
    });

    if (trace1) {
        for (int i = 0; i < cell_counts.size(); i++) {
            std::cout << "Point " << i << ": " << cell_counts[i] << std::endl;
        }
    }

    // Return max value in cell counts
    return *std::max_element(cell_counts.begin(), cell_counts.end());
}

int find_area_size_within_limit(std::vector<std::shared_ptr<point_t>> &input, int limit) {
    int max_x = 0, max_y = 0;
    // Find the largest coordinate in list of points
    for (auto &pt : input) {
        if (pt->x > max_x) {
            max_x = pt->x;
        }
        if (pt->y > max_y) {
            max_y = pt->y;
        }
    }

    int cells_within_limit = 0;
    for (int x = 0; x < max_x + 1; x++) {
        for (int y = 0; y < max_y + 1; y++) {
            int distance_sum = 0;
            for (auto &pt : input) {
                distance_sum += compute_manhattan_distance(x, y, pt->x, pt->y);
            }
            if (distance_sum < limit) {
                cells_within_limit++;
            }
        }
    }

    return cells_within_limit;
}

namespace day6 {

    void problem1() {
        std::cout << "Day 6 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 6

        std::vector<std::shared_ptr<point_t>> test1;
        read_day6_data(test1, "data/day6/problem1/test1.txt");
        assert(find_largest_finite_area(test1) == 17);

        std::vector<std::shared_ptr<point_t>> input;
        read_day6_data(input, "data/day6/problem1/input.txt");
        std::cout << "Result : " << find_largest_finite_area(input) << std::endl;

        #endif
    }

    void problem2() {
        std::cout << "Day 6 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 6

        std::vector<std::shared_ptr<point_t>> test1;
        read_day6_data(test1, "data/day6/problem2/test1.txt");
        assert(find_area_size_within_limit(test1, 32) == 16);

        std::vector<std::shared_ptr<point_t>> input;
        read_day6_data(input, "data/day6/problem2/input.txt");
        std::cout << "Result : " << find_area_size_within_limit(input, 10000) << std::endl;

        #endif
    }

}
