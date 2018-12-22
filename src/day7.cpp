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
#include <stack>

namespace day7 {

  const bool trace_read = false;
  const bool trace1 = false;
  const bool trace2 = true;

  struct step_rule_t {
    char dependancy_step;
    char step;

    friend std::istream &operator>>(std::istream &in, step_rule_t &sr) {
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

    friend std::ostream &operator<<(std::ostream &out, step_rule_t &sr) {
      out << "Step " << sr.dependancy_step << " must be finished before step " << sr.step << " can begin.";
      return out;
    }
  };

  struct step_t {
    char id;
    std::set<char> dependency_steps;

    bool operator<(const step_t &other) const {
      if (dependency_steps.size() < other.dependency_steps.size()) return true;
      else if (dependency_steps.size() == other.dependency_steps.size()) {
        return id < other.id;
      }
      return false;
    }

    friend std::ostream &operator<<(std::ostream &out, step_t &step) {
      out << step.id << "[";
      for (auto &dep : step.dependency_steps) {
        out << dep;
      }
      out << "] ";
      return out;
    }
  };

  void read_day7_data(std::vector<step_rule_t> &outdata, const char *filepath) {
    std::ifstream inputStream(filepath);
    step_rule_t step_rule;
    while (!inputStream.eof()) {
      inputStream >> step_rule;
      outdata.push_back(step_rule);
    }
  }

  std::string find_step_sequence(std::vector<step_rule_t> &input) {
    std::map<char, std::set<char>> dependencies;
    for (auto &step_rule : input) {
      // Ensure dependency step is also added
      if (dependencies.find(step_rule.dependancy_step) == dependencies.end()) {
        dependencies[step_rule.dependancy_step] = std::set<char>();
      }
      dependencies[step_rule.step].insert(step_rule.dependancy_step);
    }

    // Flatten map to a list of steps
    std::list<step_t> steps;
    for (auto&[id, dependency_steps] : dependencies) {
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
        for (auto &step : steps) {
          std::cout << step << " ";
        }
        std::cout << std::endl;
      }

      // Process first step in list
      auto &active_step = steps.front();
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

  struct worker_t {
    bool is_idle;
    int seconds_to_go;
    char processing_step;

    worker_t() : is_idle(true), seconds_to_go(0), processing_step(0) {}
  };

  int find_step_sequence_runtime(std::vector<step_rule_t> &input, int num_workers, int runtime_shift) {
    std::map<char, std::set<char>> dependencies;
    for (auto &step_rule : input) {
      // Ensure dependency step is also added
      if (dependencies.find(step_rule.dependancy_step) == dependencies.end()) {
        dependencies[step_rule.dependancy_step] = std::set<char>();
      }
      dependencies[step_rule.step].insert(step_rule.dependancy_step);
    }

    // Flatten map to a list of steps
    std::list<step_t> steps;
    for (auto&[id, dependency_steps] : dependencies) {
      step_t step;
      step.id = id;
      step.dependency_steps = dependency_steps;
      if (trace1) std::cout << "Creating step: " << step.id << " (" << step.dependency_steps.size() << ")" << std::endl;
      steps.push_back(step);
    }
    // Sort list with special sort rule
    steps.sort();

    int seconds_elapsed = 0;
    std::vector<worker_t> workers(num_workers);
    std::stack<int> idle_worker_ids;
    for (int i = 0; i < num_workers; i++) {
      idle_worker_ids.push(i);
    }
    while (!steps.empty() || idle_worker_ids.size() != workers.size()) {
      if (trace2)
        std::cout << "Second " << seconds_elapsed << " (" << idle_worker_ids.size() << " idle workers)" << std::endl;
      // Tick down active workers
      for (int i = 0; i < num_workers; i++) {
        auto &worker = workers[i];
        if (!worker.is_idle) {
          worker.seconds_to_go--;
          if (worker.seconds_to_go == 0) {
            worker.is_idle = true;

            // Remove processing step from all other steps' dependencies
            for (auto &step : steps) {
              step.dependency_steps.erase(worker.processing_step);
            }
            steps.sort();

            idle_worker_ids.push(i);
          }
        }
      }

      // We have idle workers. Let's try to assign some work
      while (!steps.empty() && !idle_worker_ids.empty()) {
        if (trace2) std::cout << "\tWe have idle workers" << std::endl;
        // Check if first step in list is clear of dependencies
        auto &next_step = steps.front();
        if (!next_step.dependency_steps.empty()) {
          // Can't assign any work yet. Bail
          if (trace2) std::cout << "\t" << next_step << " is not dependency free. Bail.." << std::endl;
          break;
        }
        // Step is ready to be assigned
        auto active_step_id = next_step.id;
        steps.pop_front();

        // Spool work to idle worker
        int next_idle_worker_id = idle_worker_ids.top();
        idle_worker_ids.pop();
        workers[next_idle_worker_id].seconds_to_go = runtime_shift + (active_step_id - 65 + 1);
        workers[next_idle_worker_id].is_idle = false;
        workers[next_idle_worker_id].processing_step = active_step_id;
        if (trace2)
          std::cout << "\tAssigning worker(" << next_idle_worker_id << ") to " << active_step_id << " for "
                    << workers[next_idle_worker_id].seconds_to_go << " secs" << std::endl;
      }

      seconds_elapsed++;
    }

    if (trace2) std::cout << "Seconds elapsed: " << (seconds_elapsed - 1) << std::endl;
    return (seconds_elapsed - 1);
  }

  void problem1() {
    std::cout << "Day 7 - Problem 1" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 7

    std::vector<step_rule_t> test1;
    read_day7_data(test1, "data/day7/problem1/test1.txt");
    assert(find_step_sequence(test1) == "CABDFE");

    std::vector<step_rule_t> input;
    read_day7_data(input, "data/day7/problem1/input.txt");
    std::cout << "Result : " << find_step_sequence(input) << std::endl;

#endif
  }

  void problem2() {
    std::cout << "Day 7 - Problem 2" << std::endl;
#if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 7

    std::vector<step_rule_t> test1;
    read_day7_data(test1, "data/day7/problem2/test1.txt");
    assert(find_step_sequence_runtime(test1, 2, 0) == 15);

    std::vector<step_rule_t> input;
    read_day7_data(input, "data/day7/problem2/input.txt");
    std::cout << "Result : " << find_step_sequence_runtime(input, 5, 60) << std::endl;

#endif
  }

}
