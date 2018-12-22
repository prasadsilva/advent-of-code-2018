#include <iostream>
#include <vector>
#include <fstream>
#include <map>

namespace day2 {
    
    const bool trace1 = false;
    const bool trace2 = false;

    void read_day2_data(std::vector<std::string> &outdata, const char* filepath) {
        std::ifstream inputStream(filepath);
        std::string line;
        while (std::getline(inputStream, line)) {
            outdata.push_back(line);
        }
    }

    int get_box_ids_checksum(std::vector<std::string> &input) {
        int num_two_char_ids = 0, num_three_char_ids = 0;

        for (auto &box_id : input) {
            if (trace1) std::cout << "Processing '" << box_id << "'.." << std::endl;
            std::map<char, int> character_counts;
            for (auto &character : box_id) {
                auto is_tracking_character = character_counts.count(character) > 0;
                if (is_tracking_character) {
                    character_counts[character] = character_counts[character] + 1;
                }
                else {
                    character_counts[character] = 1;
                }
            }

            // C++ 17 structured binding
            bool two_char_id_found = false, three_char_id_found = false;
            for (const auto& [ character, count ] : character_counts) {
                if (trace1) std::cout << "\tCount for '" << character << "' : " << count << std::endl;
                // Early out if both cases found
                if (two_char_id_found && three_char_id_found) break;
                switch (count) {
                    case 2: {
                        if (!two_char_id_found) {
                            num_two_char_ids++;
                            two_char_id_found = true;
                        }
                        break;
                    }
                    case 3: {
                        if (!three_char_id_found) {
                            num_three_char_ids++;
                            three_char_id_found = true;
                        }
                        break;
                    }
                    default: {
                        // Do nothing
                    }
                }    
            }
        }

        if (trace1) std::cout << "2 char: " << num_two_char_ids << ", 3 char: " << num_three_char_ids << std::endl;
        return num_two_char_ids * num_three_char_ids;
    }

    // Returns hamming distance and the last character index that was different
    std::pair<int, int> hamming_distance(const std::string &s1, const std::string &s2) {
        assert(s1.size() == s2.size());

        int last_different_index = -1;
        int num_different_chars = 0;
        for (std::string::size_type idx = 0; idx < s1.size(); idx++) {
            auto is_char_different = s1[idx] != s2[idx];
            if (is_char_different) {
                ++num_different_chars;
                last_different_index = idx;
            }
        }
        
        return { num_different_chars, last_different_index };
    }

    std::string get_prototype_fabric_box_common_letters(std::vector<std::string> &input) {
        for (auto s1 : input) {
            for (auto s2 : input) {
                auto [ num_different_chars, last_different_index ] = hamming_distance(s1, s2);
                if (trace2) std::cout << "Checking '" << s1 << "' and '" << s2 << "' : " << num_different_chars << ", " << last_different_index << std::endl;
                if (num_different_chars == 1) {
                    if (trace2) std::cout << "Found '" << s1 << "' and '" << s2 << "'" << std::endl;
                    auto output = s1;
                    output.erase(last_different_index, 1);
                    return output;
                }
            }
        }

        return "";
    }

    void problem1() {
        std::cout << "Day 2 - Problem 1" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 2

        std::vector<std::string> test1;
        read_day2_data(test1, "data/day2/problem1/test1.txt");
        assert(get_box_ids_checksum(test1) == 12);

        std::vector<std::string> input;
        read_day2_data(input, "data/day2/problem1/input.txt");
        std::cout << "Result : " << get_box_ids_checksum(input) << std::endl;

        #endif
    }

    void problem2() {
        std::cout << "Day 2 - Problem 2" << std::endl;
        #if !defined(ONLY_ACTIVATE) || ONLY_ACTIVATE == 2

        std::vector<std::string> test1;
        read_day2_data(test1, "data/day2/problem2/test1.txt");
        assert(get_prototype_fabric_box_common_letters(test1) == "fgij");

        std::vector<std::string> input;
        read_day2_data(input, "data/day2/problem2/input.txt");
        std::cout << "Result : " << get_prototype_fabric_box_common_letters(input) << std::endl;

        #endif
    }
}