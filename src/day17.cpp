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

namespace day17 {
    
    const bool trace_read = false;
    const bool trace1 = false;
    const bool trace2 = false;
    const bool enable_assertions = true;

    enum tile_type_e {
        sand,
        wet_sand,
        clay,
        water,    
        spring,
    };

    struct tile_t {
        tile_type_e type;
        int x;
        int y;

        friend std::ostream & operator << (std::ostream &out, const tile_t &tile) {
            char type_c;
            switch (tile.type) {
                case tile_type_e::sand: type_c = '.'; break;
                case tile_type_e::clay: type_c = '#'; break;
                case tile_type_e::water: type_c = '~'; break;
                case tile_type_e::spring: type_c = '+'; break;
                case tile_type_e::wet_sand: type_c = '|'; break;
            }
            out << type_c << "(" << tile.x << "," << tile.y << ")";
            return out;
        }
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

    template <int terrain_dimx, int terrain_dimy>
    struct terrain_t {
        std::vector<std::vector<tile_t>> tiles;
        int minScanX = std::numeric_limits<int>::max(), minScanY = std::numeric_limits<int>::max();
        int maxScanX = std::numeric_limits<int>::min(), maxScanY = std::numeric_limits<int>::min();

        void initialize() {        
            tiles.resize(terrain_dimy);
            for (int y = 0; y < terrain_dimy; y++) {
                tiles[y].resize(terrain_dimx);
                for (int x = 0; x < terrain_dimx; x++) {
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

        int get_water_reach() {
            int result = 0;
            for (int y = minScanY; y <= maxScanY; y++) {
                auto &row = tiles[y];
                for (int x = minScanX - 1; x <= maxScanX + 1; x++) {
                    auto &current_tile = row[x];
                    if (current_tile.type == tile_type_e::water || current_tile.type == tile_type_e::wet_sand) {
                        result++;
                    }
                }
            }
            return result;
        }

        int get_standing_water() {
            int result = 0;
            for (int y = minScanY; y <= maxScanY; y++) {
                auto &row = tiles[y];
                for (int x = minScanX - 1; x <= maxScanX + 1; x++) {
                    auto &current_tile = row[x];
                    if (current_tile.type == tile_type_e::water) {
                        result++;
                    }
                }
            }
            return result;
        }

        void simulate() {        
            if (trace1) std::cout << "Simulating.." << std::endl;
            // Start scanning up from maxScanY + 1
            for (int y = maxScanY + 1; y >= 0; y--) {
                const auto& row = tiles[y];
                auto write_row = row;
                // Left to right, see if there are water tiles that can flow
                for (int x = minScanX - 1; x <= maxScanX + 1; x++) {
                    const auto& current_tile = row[x];
                    if (trace1) std::cout << "\tProcessing " << current_tile << std::endl;
                    auto &tile_below = tiles[y + 1][x];
                    if (current_tile.type == tile_type_e::spring) {
                        // We have a spring. Generate water
                        assert(tile_below.type == tile_type_e::sand || tile_below.type == tile_type_e::wet_sand);
                        if (trace1) std::cout << "\t\tGenerated water at " << tile_below << std::endl;                    
                        if (tile_below.type == tile_type_e::sand) tile_below.type = tile_type_e::wet_sand;
                    } 
                    else if (current_tile.type == tile_type_e::water) {
                        if (trace1) std::cout << "\t\tSTANDING WATER" << std::endl;
                    }
                    else if (current_tile.type == tile_type_e::wet_sand) {
                        if (trace1) std::cout << "\t\tWET SAND" << std::endl;
                        // Can flow down?
                        if (tile_below.type == tile_type_e::sand || tile_below.type == tile_type_e::wet_sand) {
                            if (trace1) std::cout << "\t\tMoving water DOWN to " << tile_below << std::endl;
                            tile_below.type = tile_type_e::wet_sand;
                            // Check if tiles below represent standing water
                            if (tile_below.type == tile_type_e::wet_sand) {
                                auto &row_below = tiles[y + 1];
                                int wx = x;
                                while (row_below[wx].type == tile_type_e::wet_sand) {
                                    wx--;
                                }
                                bool clay_to_the_left = row_below[wx].type == tile_type_e::clay;
                                int wx2 = x;
                                while (row_below[wx2].type == tile_type_e::wet_sand) {
                                    wx2++;
                                }
                                bool clay_to_the_right = row_below[wx2].type == tile_type_e::clay;
                                if (clay_to_the_left && clay_to_the_right) {
                                    // Check if tiles below on clay or water
                                    bool change_to_standing_water = true;
                                    for (int sx = wx + 1; sx < wx2; sx++) {
                                        if (tiles[y + 2][sx].type != tile_type_e::clay && tiles[y + 2][sx].type != tile_type_e::water) {
                                            change_to_standing_water = false;
                                            break;
                                        }
                                    }
                                    if (change_to_standing_water) {
                                        for (int sx = wx + 1; sx < wx2; sx++) {
                                            row_below[sx].type = tile_type_e::water;
                                        }
                                    }                                
                                }
                            }                        
                        }
                        else { // water or clay
                            bool go_left = false;
                            if (row[x - 1].type == tile_type_e::sand || row[x - 1].type == tile_type_e::wet_sand) {
                                go_left = true;
                            }
                            bool go_right = false;
                            if (row[x + 1].type == tile_type_e::sand || row[x + 1].type == tile_type_e::wet_sand) {
                                go_right = true;
                            }

                            assert(go_left || go_right);
                            if (go_left) {
                                if (trace1) std::cout << "\t\tMoving water LEFT" << std::endl;
                                write_row[x - 1].type = tile_type_e::wet_sand;
                                // Stay wet sand
                            }
                            if (go_right) {
                                if (trace1) std::cout << "\t\tMoving water RIGHT" << std::endl;
                                write_row[x + 1].type = tile_type_e::wet_sand;
                                // Stay wet sand
                            }
                        }
                    }
                }  
                tiles[y] = write_row;        
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
                        case tile_type_e::wet_sand: out << '|'; break;
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

    void problem1() {
        std::cout << "Day 17 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 17

        if (enable_assertions) {
            {
                std::vector<scan_t> scans;
                read_day17_data(scans, "data/day17/problem1/test1.txt");
                terrain_t<600, 16> terrain;
                terrain.configure(scans);
                std::cout << "Terrain x=(" << terrain.minScanX << ", " << terrain.maxScanX << ") y=(" << terrain.minScanY << ", " << terrain.maxScanY << ")" << std::endl;
                int old_water_reach = -1;
                while (1) {
                    std::cout << terrain << std::endl;
                    terrain.simulate();
                    if (terrain.get_water_reach() == old_water_reach) {
                        break;
                    }
                    old_water_reach = terrain.get_water_reach();
                }
                std::cout << terrain << std::endl;
                assert(terrain.get_water_reach() == 57);
            }
            {
                std::vector<scan_t> scans;
                read_day17_data(scans, "data/day17/problem1/test2.txt");
                terrain_t<600, 16> terrain;
                terrain.configure(scans);
                std::cout << "Terrain x=(" << terrain.minScanX << ", " << terrain.maxScanX << ") y=(" << terrain.minScanY << ", " << terrain.maxScanY << ")" << std::endl;
                int old_water_reach = -1;
                while (1) {
                    std::cout << terrain << std::endl;
                    terrain.simulate();
                    if (terrain.get_water_reach() == old_water_reach) {
                        break;
                    }
                    old_water_reach = terrain.get_water_reach();
                }
                std::cout << terrain << std::endl;
                assert(terrain.get_water_reach() == 54);
            }
            
        }        
        
        std::vector<scan_t> scans;
        read_day17_data(scans, "data/day17/problem1/input.txt");
        terrain_t<600, 2000> terrain;
        terrain.configure(scans);
        std::cout << "Terrain x=(" << terrain.minScanX << ", " << terrain.maxScanX << ") y=(" << terrain.minScanY << ", " << terrain.maxScanY << ")" << std::endl;
        int old_water_reach = -1;
        while (1) {
            terrain.simulate();
            if (terrain.get_water_reach() != 0 && terrain.get_water_reach() == old_water_reach) {
                break;
            }
            std::cout << "Water Reach: " << terrain.get_water_reach() << std::endl;
            old_water_reach = terrain.get_water_reach();
        }
        std::cout << "Result: " << terrain.get_water_reach() << std::endl;
        std::ofstream output_stream("day17_result.txt");
        output_stream << terrain;

        // Problem 2 result here
        std::cout << "Standing water: " << terrain.get_standing_water() << std::endl;

        #endif
    }

    void problem2() {
        std::cout << "Day 17 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 17

        // Problem2 solution is implemented in problem1

        #endif
    }

}