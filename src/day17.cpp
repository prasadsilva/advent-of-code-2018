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

const bool trace_read = false;
const bool trace1 = true;
const bool trace2 = false;
const bool enable_assertions = true;

enum tile_type_e {
    sand,
    clay,
    water,
    spring,
};

struct tile_t {
    tile_type_e type;
    int x;
    int y;
};

struct scan_t {
    bool vertical;
    int anchorPos;
    int rangeMin;
    int rangeMax;

    friend std::istream & operator >> (std::istream &in, scan_t &scan) {
        std::string line;
        getline(in, line);
        assert(!line.empty());

        std::regex scan_pattern(R"(([xy]{1})=(\d+), ([xy]{1})=(\d+)\.\.(\d+))");
        std::smatch scan_matches;
        if (!std::regex_search(line, scan_matches, scan_pattern)) {
            throw new std::invalid_argument("Cannot parse before state!");
        }

        char anchorDim = scan_matches[1].str()[0];
        scan.vertical = (anchorDim == 'x');
        scan.anchorPos = std::stoi(scan_matches[2].str());
        scan.rangeMin = std::stoi(scan_matches[4].str());
        scan.rangeMax = std::stoi(scan_matches[5].str());
        
        if (trace_read) std::cout << "Read: " << scan << std::endl;
        return in;
    }

    friend std::ostream & operator << (std::ostream &out, const scan_t &scan) {
        out << (scan.vertical ? 'x' : 'y') << "=" << scan.anchorPos << ", " << (!scan.vertical ? 'x' : 'y') << "=" << scan.rangeMin << ".." << scan.rangeMax;
        return out;
    }
};

template <int terrain_dim>
struct terrain_t {
    std::vector<std::vector<tile_t>> tiles;
    int minScanX = std::numeric_limits<int>::max(), minScanY = std::numeric_limits<int>::max();
    int maxScanX = std::numeric_limits<int>::min(), maxScanY = std::numeric_limits<int>::min();

    void initialize() {        
        tiles.resize(terrain_dim);
        for (int y = 0; y < terrain_dim; y++) {
            tiles[y].resize(terrain_dim);
            for (int x = 0; x < terrain_dim; x++) {
                tiles[y][x] = { tile_type_e::sand, x, y };
            }
        }
        tiles[0][500].type = tile_type_e::spring;
    }

    void configure(const std::vector<scan_t> &scans) {
        initialize();
        for (auto &scan : scans) {
            if (scan.vertical) {
                int x = scan.anchorPos;                
                for (int y = scan.rangeMin; y <= scan.rangeMax; y++) {
                    if (x < minScanX) minScanX = x;
                    else if (x > maxScanX) maxScanX = x;
                    if (y < minScanY) minScanY = y;
                    else if (y > maxScanY) maxScanY = y;
                    tiles[y][x].type = tile_type_e::clay;
                }
            } else {
                int y = scan.anchorPos;
                if (y < minScanY) minScanY = y;
                else if (y > maxScanY) maxScanY = y;
                for (int x = scan.rangeMin; x <= scan.rangeMax; x++) {
                    if (x < minScanX) minScanX = x;
                    else if (x > maxScanX) maxScanX = x;
                    if (y < minScanY) minScanY = y;
                    else if (y > maxScanY) maxScanY = y;
                    tiles[y][x].type = tile_type_e::clay;
                }
            }
        }
    }

    friend std::ostream & operator << (std::ostream &out, const terrain_t &terrain) {
        for (int y = 0; y <= terrain.maxScanY; y++) {
            for (int x = terrain.minScanX - 1; x <= terrain.maxScanX + 1; x++) {
                auto &tile = terrain.tiles[y][x];
                switch (tile.type) {
                    case tile_type_e::sand: out << '.'; break;
                    case tile_type_e::clay: out << '#'; break;
                    case tile_type_e::water: out << '~'; break;
                    case tile_type_e::spring: out << '+'; break;
                }
            }
            out << std::endl;
        }
        out << std::endl;
        return out;
    }
};

void read_day17_data(std::vector<scan_t> &outdata, const char* filepath) {
    std::ifstream input_stream(filepath);
    while (!input_stream.eof()) {
        scan_t scan;
        input_stream >> scan;
        outdata.push_back(scan);
    }
}

namespace day17 {

    void problem1() {
        std::cout << "Day 17 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 17

        // NOTES
        // -----
        // Read data in. Maintain min and max values to generate 2d vector of terrain type and to use y coordinates to prevent counting forever
        // Spring is at 500,0
        // Spring produces water indefinitely
        // Water moves through sand, blocked by clay.
        // Water always moves down
        // How many tiles can the water reach within the range of y values in your scan?

        std::vector<scan_t> scans;
        read_day17_data(scans, "data/day17/problem1/test1.txt");
        terrain_t<2000> terrain;
        terrain.configure(scans);
        std::cout << "Terrain x=(" << terrain.minScanX << ", " << terrain.maxScanX << ") y=(" << terrain.minScanY << ", " << terrain.maxScanY << ")" << std::endl;
        std::cout << terrain << std::endl;

        #endif
    }

    void problem2() {
        std::cout << "Day 17 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 17

        #endif
    }

}