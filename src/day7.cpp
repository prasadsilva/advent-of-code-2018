#include <iostream>
#include <fstream>
#include <cctype>   // islower, toupper, tolower
#include <limits>
#include <set>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>
#include <regex>
#include <map>
#include <set>
#include <list>

const bool trace_read = false;
const bool trace1 = false;

struct step_rule_t {
    char dependancy_step;
    char step;

    friend std::istream & operator >> (std::istream &in, step_rule_t &sr) {
        // E.g.: Step C must be finished before step A can begin.
        std::string line;
        getline(in, line);

        std::regex step_pattern(R"(Step ([A-Z]{1}) must be finished before step ([A-Z]{1}) can begin.)");
        std::smatch step_matches;

        if (!std::regex_search(line, step_matches, step_pattern)) {
            throw new std::invalid_argument("Cannot parse step rule!");
        }

        sr.dependancy_step = step_matches[1].str()[0];
        sr.step = step_matches[2].str()[0];

        if (trace_read) std::cout << "Read " << sr << std::endl;
        return in;
    }

    friend std::ostream & operator << (std::ostream &out, step_rule_t &sr) {
        out << "Step " << sr.dependancy_step << " must be finished before step " << sr.step << " can begin.";
        return out;
    }
};

struct step_t {
    char id;
    std::set<char> dependency_steps;

    bool operator < (const step_t& other) const {
        if (dependency_steps.size() < other.dependency_steps.size()) return true;
        else if (dependency_steps.size() == other.dependency_steps.size()) {
            return id < other.id;
        }
        return false;
    }

    friend std::ostream & operator << (std::ostream &out, step_t &step) {
        out << step.id << "[";
        for (auto& dep : step.dependency_steps) {
            out << dep;
        }
        out << "] ";
        return out;
    }
};

void read_day7_data(std::vector<step_rule_t> &outdata, const char* filepath) {
    std::ifstream inputStream(filepath);
    step_rule_t step_rule;
    while (!inputStream.eof()) {
        inputStream >> step_rule;
        outdata.push_back(step_rule);
    }
}

std::string find_step_sequence(std::vector<step_rule_t> &input) {
    std::map<char, std::set<char>> dependencies;
    for (auto& step_rule : input) {
        // Ensure dependency step is also added
        if (dependencies.find(step_rule.dependancy_step) == dependencies.end()) {
            dependencies[step_rule.dependancy_step] = std::set<char>();
        }
        dependencies[step_rule.step].insert(step_rule.dependancy_step);
    }

    // Flatten map to a list of steps
    std::list<step_t> steps;
    for (auto& [id, dependency_steps] : dependencies) {
        step_t step;
        step.id = id;
        step.dependency_steps = dependency_steps;
        if (trace1) std::cout << "Creating step: " << step.id << " (" << step.dependency_steps.size() << ")" << std::endl;
        steps.push_back(step);
    }

    std::string step_sequence;
    while (!steps.empty()) {
        // Sort list with special sort rule
        steps.sort();
        if (trace1) {
            std::cout << ">> ";
            for (auto& step : steps) {
                std::cout << step << " ";
            }
            std::cout << std::endl;
        }

        // Process first step in list
        auto& active_step = steps.front();
        auto active_step_id = active_step.id;
        if (trace1) std::cout << "Active step : " << active_step << std::endl;
        assert(active_step.dependency_steps.empty());
        steps.pop_front();

        // Add first step to sequence
        step_sequence += active_step_id;

        // Remove active step from all other steps' dependencies
        for (auto &step : steps) {
            step.dependency_steps.erase(active_step_id);
        }
    }

    std::cout << "Sequence: " << step_sequence << std::endl;
    return step_sequence;
}

namespace day7 {

    void problem1() {
        std::cout << "Day 7 - Problem 1" << std::endl;

        std::vector<step_rule_t> test1;
        read_day7_data(test1, "data/day7/problem1/test1.txt");
        assert(find_step_sequence(test1) == "CABDFE");

        std::vector<step_rule_t> input;
        read_day7_data(input, "data/day7/problem1/input.txt");
        std::cout << "Result : " << find_step_sequence(input) << std::endl;
    }

    void problem2() {
        std::cout << "Day 7 - Problem 2" << std::endl;
    }

}
