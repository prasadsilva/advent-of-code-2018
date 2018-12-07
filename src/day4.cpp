#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

const bool trace_read = false;
const bool trace1 = true;

enum class guard_action_e { BEGIN_SHIFT, WAKE_UP, FALL_ASLEEP };

struct guard_event_t {
    int guard_id = -1; // May not be available
    guard_action_e action;
    int year, month, day;
    int hour, minute;

    bool operator < (const guard_event_t& other) const {
        if (year < other.year) return true;
        else if (year == other.year) {
            if (month < other.month) return true;
            else if (month == other.month) {
                if (day < other.day) return true;
                else if (day == other.day) {
                    if (hour < other.hour) return true;
                    else if (hour == other.hour) {
                        return (minute < other.minute);
                    }
                }
            }
        }
        return false;
    }

    friend std::istream & operator >> (std::istream &in, guard_event_t &ge) {
        /** E.g.
            [1518-10-28 23:51] Guard #1283 begins shift
            [1518-10-11 00:29] wakes up
            [1518-07-15 00:37] falls asleep
        **/
        std::string temp_str;
        char temp_char;

        std::string line;
        getline(in, line);

        std::regex timestamp_pattern(R"(\[(\d{4})\-(\d{2})\-(\d{2}) (\d{2}):(\d{2})\] (.*))");
        std::smatch timestamp_matches;

        if (!std::regex_search(line, timestamp_matches, timestamp_pattern)) {
            throw new std::invalid_argument("Cannot parse timestamp!");
        }

        ge.year = std::stoi(timestamp_matches[1].str());
        ge.month = std::stoi(timestamp_matches[2].str());
        ge.day = std::stoi(timestamp_matches[3].str());
        ge.hour = std::stoi(timestamp_matches[4].str());
        ge.minute = std::stoi(timestamp_matches[5].str());

        std::string action = timestamp_matches[6].str();
        std::regex begin_shift_pattern(R"(Guard #(\d+) begins shift)");
        std::regex wake_up_pattern(R"(wakes up)");
        std::regex fall_asleep_pattern(R"(falls asleep)");
        std::smatch action_matches;

        if (std::regex_search(action, action_matches, begin_shift_pattern)) {
            ge.action = guard_action_e::BEGIN_SHIFT;
            ge.guard_id = std::stoi(action_matches[1].str());
        } else if (std::regex_search(action, action_matches, wake_up_pattern)) {
            ge.action = guard_action_e::WAKE_UP;
        } else if (std::regex_search(action, action_matches, fall_asleep_pattern)) {
            ge.action = guard_action_e::FALL_ASLEEP;
        }
         
        if (trace_read) std::cout << "Read " << ge << std::endl;
        return in;
    }

    friend std::ostream & operator << (std::ostream &out, guard_event_t &ge) {
        out << "[" << ge.year << "-" << ge.month << "-" << ge.day << " " << ge.hour << ":" << ge.minute << "] ";
        switch (ge.action) {
            case guard_action_e::BEGIN_SHIFT: {
                out << "Guard #" << ge.guard_id << " begins shift";
                break;
            }
            case guard_action_e::WAKE_UP: {
                out << "wakes up";
                break;
            }
            case guard_action_e::FALL_ASLEEP: {
                out << "falls asleep";
                break;
            }
            default: {
                break;
            }
        }
        return out;
    }
};

void read_day4_data(std::vector<guard_event_t> &outdata, const char* filepath) {
    std::ifstream inputStream(filepath);
    guard_event_t guard_event;
    while (!inputStream.eof()) {
        inputStream >> guard_event;
        outdata.push_back(guard_event);
    }
}

namespace day4 {

    void problem1() {
        std::cout << "Day 4 - Problem 1" << std::endl;

        std::vector<guard_event_t> test1;
        read_day4_data(test1, "data/day4/problem1/test1.txt");
        // Sort the events by timestamp
        std::sort(test1.begin(), test1.end());
        if (trace1) {
            for (auto &ge : test1) {
                std::cout << ge << std::endl;
            }
        }
    }

}