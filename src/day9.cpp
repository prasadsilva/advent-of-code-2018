#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include <list>

struct game_t {
    std::list<long> circle;
    std::list<long>::iterator current_marble;
    long num_players;
    std::vector<long> scores;

    game_t(int num_players) : num_players(num_players) {
        circle.push_back(0);
        current_marble = circle.begin();
        scores.resize(num_players, 0);
    }

    void move_iterator_clockwise(std::list<long>::iterator &iter) {
        iter++;
        if (iter == circle.end()) {
            iter = circle.begin();
        }
    }
    void move_iterator_counter_clockwise(std::list<long>::iterator &iter) {
        if (iter == circle.begin()) {
            iter = circle.end();
        }
        iter--;
    }

    void play_marble(int player_id, long value) {
        if (value % 23 == 0) {
            // Special rules
            scores[player_id] += value;
            auto iter = current_marble;
            for (int i = 0; i < 7; i++) {
                move_iterator_counter_clockwise(iter);
            }
            current_marble = iter;
            move_iterator_clockwise(current_marble);
            scores[player_id] += *iter;
            circle.erase(iter);
        } else {
            auto iter = current_marble;
            move_iterator_clockwise(iter); // x2
            move_iterator_clockwise(iter);
            current_marble = circle.insert(iter, value);
            // No score change
        }
    }

    long get_highest_score() {
        return *std::max_element(scores.begin(), scores.end());
    }
};

long compute_high_score(int num_players, int last_marble_value) {
    game_t game(num_players);

    int next_player_id = 0;
    int next_marble_value = 1;
    while (next_marble_value <= last_marble_value) {
        game.play_marble(next_player_id, next_marble_value);

        // Move to next marble value
        next_marble_value++;

        // Move to next player
        next_player_id++;
        if (next_player_id >= num_players) {
            next_player_id = 0;
        }
    }

    // Return winning score
    return game.get_highest_score();
}

namespace day9 {

    void problem1() {
        std::cout << "Day 9 - Problem 1" << std::endl;
        
        assert(compute_high_score(9, 25) == 32);
        assert(compute_high_score(10, 1618) == 8317);
        assert(compute_high_score(13, 7999) == 146373);
        assert(compute_high_score(17, 1104) == 2764);
        assert(compute_high_score(21, 6111) == 54718);
        assert(compute_high_score(30, 5807) == 37305);

        std::cout << "Result : " << compute_high_score(464, 70918) << std::endl;
    }

    void problem2() {
        std::cout << "Day 9 - Problem 2" << std::endl;

        std::cout << "Result : " << compute_high_score(464, 70918 * 100) << std::endl;
    }

}