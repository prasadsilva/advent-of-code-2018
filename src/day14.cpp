#include <iostream>
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
const bool trace_elves = false;
const bool trace1 = true;
const bool trace2 = false;
const bool enable_assertions = true;

struct recipe_score_t {
    int value;

    recipe_score_t() = default;
    recipe_score_t(int _value) : value(_value) {}

    bool operator == (const recipe_score_t& other) const {
        return value == other.value;
    }
};

struct elf_t {
    int current_recipe;

    elf_t() = default;
    elf_t(int _current_recipe) : current_recipe(_current_recipe) {}
};

struct environment_t {
    std::vector<recipe_score_t> recipe_scores;
    std::vector<elf_t> elves;

    environment_t(const std::vector<recipe_score_t> &_recipe_scores) {
        recipe_scores = _recipe_scores;
        for (auto i = 0; i < recipe_scores.size(); i++) {
            elves.push_back(i);
        }
    }

    void tick() {
        long next_recipe_score = 0;
        for (auto &elf : elves) {
            next_recipe_score += recipe_scores[elf.current_recipe].value;
        }
        // Iterate over digits and add digits as new recipes
        std::ostringstream os;
        os << next_recipe_score;
        std::string digits = os.str();
        for (auto digit : digits) {
            int value = static_cast<int>(digit - '0');
            recipe_scores.push_back({value});
        }
        // Choose new recipes for elves (do this before adding new recipe scores)
        for (auto &elf : elves) {
            int steps = 1 + recipe_scores[elf.current_recipe].value;
            elf.current_recipe += steps;
            elf.current_recipe %= recipe_scores.size();
        }
    }

    std::vector<recipe_score_t> get_ten_recipes_after(int recipe_num) {
        std::vector<recipe_score_t> result;
        for (int i = 0; i < 10; i++) {
            result.push_back(recipe_scores[recipe_num + i]);
        }
        return result;
    }

    friend std::ostream & operator << (std::ostream &out, environment_t &env) {
        for (auto& recipe_score : env.recipe_scores) {
            out << recipe_score.value << ", ";
        }
        if (trace_elves) {
            out << std::endl;
            for (auto i = 0; i < env.elves.size(); i++) {
                out << "\te" << i << ": " << env.elves[i].current_recipe << " (" << env.recipe_scores[env.elves[i].current_recipe].value << ")" << std::endl;
            }       
        }
        return out;
    }
};

void make_recipes(environment_t& env, int limit) {
    if (trace_read) std::cout << env << std::endl;
    while (env.recipe_scores.size() < limit) {
        env.tick();
        if (trace_read) std::cout << env << std::endl;
    }
}

namespace day14 {

    void problem1() {
        std::cout << "Day 14 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 14

        if (enable_assertions) {
            {
                environment_t env({3, 7});
                make_recipes(env, 9 + 10);
                assert(env.get_ten_recipes_after(9) == (std::vector<recipe_score_t>{5,1,5,8,9,1,6,7,7,9}));
            }
            {
                environment_t env({3, 7});
                make_recipes(env, 5 + 10);
                assert(env.get_ten_recipes_after(5) == (std::vector<recipe_score_t>{0,1,2,4,5,1,5,8,9,1}));
            }
            {
                environment_t env({3, 7});
                make_recipes(env, 18 + 10);
                assert(env.get_ten_recipes_after(18) == (std::vector<recipe_score_t>{9,2,5,1,0,7,1,0,8,5}));
            }
            {
                environment_t env({3, 7});
                make_recipes(env, 2018 + 10);
                assert(env.get_ten_recipes_after(2018) == (std::vector<recipe_score_t>{5,9,4,1,4,2,9,8,8,2}));
            }
        }        

        environment_t env({3, 7});
        make_recipes(env, 110201 + 10);        
        auto result = env.get_ten_recipes_after(110201);
        std::cout << "Result: ";
        for (auto& rs : result) {
            std::cout << rs.value;
        }
        std::cout << std::endl;

        #endif
    }

    void problem2() {
        std::cout << "Day 14 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 14


        #endif
    }

}