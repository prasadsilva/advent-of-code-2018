#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>

void read_day1_data(std::vector<int> &outdata, const char* filepath) {
    std::ifstream inputStream(filepath);
    std::istream_iterator<int> inputIterator(inputStream);
    std::copy(inputIterator, std::istream_iterator<int>(), std::back_inserter(outdata));
}

int get_resulting_frequency(std::vector<int> &input) {
    int result = 0;
    for (const auto &value : input) {
        result += value;
    }
    return result;
}

int get_first_repeating_frequency(std::vector<int> &input) {
    std::set<int> seen_frequencies;
    bool found = false;
    int result = 0;
    seen_frequencies.insert(result);    
    do {
        for (const auto &value : input) {
            result += value;
            // Check if result has been seen before
            auto frequency_check = seen_frequencies.find(result);
            found = frequency_check != seen_frequencies.end();
            if (found) break;
            seen_frequencies.insert(result);
        }
    } while(!found);
    return result;
}

namespace day1 {

    void problem1() {
        std::cout << "Day 1 - Problem 1" << std::endl;
        
        std::vector<int> test1;
        read_day1_data(test1, "data/day1/problem1/test1.txt");
        assert(get_resulting_frequency(test1) == 3);

        std::vector<int> test2;
        read_day1_data(test2, "data/day1/problem1/test2.txt");
        assert(get_resulting_frequency(test2) == 0);

        std::vector<int> test3;
        read_day1_data(test3, "data/day1/problem1/test3.txt");
        assert(get_resulting_frequency(test3) == -6);

        std::vector<int> input;
        read_day1_data(input, "data/day1/problem1/input.txt");
        std::cout << "Result : " << get_resulting_frequency(input) << std::endl;
    }

    void problem2() {
        std::cout << "Day 1 - Problem 2" << std::endl;

        std::vector<int> test1;
        read_day1_data(test1, "data/day1/problem2/test1.txt");
        assert(get_first_repeating_frequency(test1) == 0);

        std::vector<int> test2;
        read_day1_data(test2, "data/day1/problem2/test2.txt");
        assert(get_first_repeating_frequency(test2) == 10);

        std::vector<int> test3;
        read_day1_data(test3, "data/day1/problem2/test3.txt");
        assert(get_first_repeating_frequency(test3) == 5);

        std::vector<int> test4;
        read_day1_data(test4, "data/day1/problem2/test4.txt");
        assert(get_first_repeating_frequency(test4) == 14);

        std::vector<int> input;
        read_day1_data(input, "data/day1/problem2/input.txt");
        std::cout << "Result : " << get_first_repeating_frequency(input) << std::endl;
    }

}