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
const bool trace1 = false;
const bool trace2 = false;
const bool enable_assertions = false;

const int dim = 1000;

struct next_gen_rule_t {
    std::array<bool, 5> pattern;
    bool result;

    friend std::istream & operator >> (std::istream &in, next_gen_rule_t &ngr) {
        // E.g.: ####. => .
        // E.g.: ##.#. => #
        std::string line;
        getline(in, line);

        std::regex rule_pattern(R"(([#.])([#.])([#.])([#.])([#.]) => ([#.]))");
        std::smatch rule_matches;
        if (!std::regex_search(line, rule_matches, rule_pattern)) {
            throw new std::invalid_argument("Cannot parse rule!");
        }

        for (int i = 0; i < 5; i++) {
            char value = rule_matches[i + 1].str()[0];
            ngr.pattern[i] = (value == '#');
        }

        ngr.result = rule_matches[5].str()[0] == '#';

        if (trace_read) std::cout << "Rule: " << ngr << std::endl;
        return in;
    }

    friend std::ostream & operator << (std::ostream &out, next_gen_rule_t &ngr) {
        for (auto& p : ngr.pattern) {
            out << (p ? '#' : '.');
        }
        out << " => " << (ngr.result ? '#' : '.');
        return out;
    }
};

struct cave_t {
    // -dim, ..., -1, 0, 1, ..., dim
    std::array<bool, (dim * 2) + 1> pots;
    int zero_index = dim + 1;
    std::vector<next_gen_rule_t> next_gen_rules;

    void initialize(const std::vector<bool> &initial_state) {
        assert(initial_state.size() < dim);
        for (int i = 0; i < initial_state.size(); i++) {
            pots[zero_index + i] = initial_state[i];
        }
    }

    friend std::istream & operator >> (std::istream &in, cave_t &cave) {
        // First line is initial state
        // E.g.: initial state: #..#.#..##......###...###
        std::string line;
        getline(in, line);

        std::regex state_pattern(R"(initial state: ([#.]+))");
        std::smatch state_matches;
        if (!std::regex_search(line, state_matches, state_pattern)) {
            throw new std::invalid_argument("Cannot parse initial state!");
        }
        std::vector<bool> initial_state;
        for (auto& c : state_matches[1].str()) {
            if (c == '#') initial_state.push_back(true);
            else if (c == '.') initial_state.push_back(false);
            else throw new std::invalid_argument("Unknown initial state value!");
        }
        if (trace_read) {
            std::cout << "initial_state: ";
            for (auto s : initial_state) {
                std::cout << (s ? '#' : '.');
            }
            std::cout << std::endl;
        }
        cave.initialize(initial_state);

        // Skip next line
        getline(in, line);

        // Process the gen rules
        next_gen_rule_t next_gen_rule;
        while (!in.eof()) {
            in >> next_gen_rule;
            cave.next_gen_rules.push_back(next_gen_rule);
        }

        return in;
    }

    friend std::ostream & operator << (std::ostream &out, cave_t &cave) {
        // TODO

        return out;
    }
};

void read_day12_data(cave_t &outdata, const char* filepath) {
    std::ifstream input_stream(filepath);
    input_stream >> outdata;
}

namespace day12 {

    void problem1() {
        std::cout << "Day 12 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 12

        cave_t test1;
        read_day12_data(test1, "data/day12/problem1/test1.txt");
        // TODO

        cave_t input;
        read_day12_data(input, "data/day12/problem1/input.txt");
        // TODO

        #endif
    }

    void problem2() {
        std::cout << "Day 12 - Problem 2" << std::endl;

        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 12

        // TODO

        #endif
    }

}