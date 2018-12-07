#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <regex>
#include <map>
#include <memory>

const bool trace_read = false;
const bool trace1 = false;
const bool trace2 = false;

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

struct guard_schedule_t {
    std::array<bool, 60> minutes_asleep{false};

    void sleeping_from(int minute) {
        for (int i = minute; i < 60; i++) {
            minutes_asleep[i] = true;
        }
    }

    void awake_from(int minute) {
        for (int i = minute; i < 60; i++) {
            minutes_asleep[i] = false;
        }
    }

    int get_minutes_slept() {
        int result = 0;
        for (auto& slept : minutes_asleep) {
            result += (slept ? 1 : 0);
        }
        return result;
    }
};

struct guard_t {
    int guard_id;
    std::vector<guard_schedule_t> schedules;

    guard_t(int gid = -1) : guard_id(gid) {}

    void add_schedule(const guard_schedule_t &schedule) {
        schedules.push_back(schedule);
    }

    int get_minutes_slept() {
        int minutes_slept = 0;
        for (auto& schedule : schedules) {
            minutes_slept += schedule.get_minutes_slept();
        }
        return minutes_slept;
    }

    std::pair<int, int> find_minute_slept_the_most() {
        std::array<int, 60> slept_instances{0};
        for (auto& schedule : schedules) {
            for (int i = 0; i < 60; i++) {
                slept_instances[i] += (schedule.minutes_asleep[i] ? 1 : 0);
            }
        }
        int minute = std::distance(slept_instances.begin(), std::max_element(slept_instances.begin(), slept_instances.end()));
        int times_slept = slept_instances[minute];
        return { minute, times_slept };
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

std::pair<int, int> find_guard_and_minute1(std::vector<guard_event_t> &input) {
    // Sort the events by timestamp
    std::sort(input.begin(), input.end());
    if (trace1) {
        for (auto &ge : input) {
            std::cout << ge << std::endl;
        }
    }

    std::map<int, std::shared_ptr<guard_t>> guards;
    guard_t guard_data;
    std::shared_ptr<guard_t> current_guard;
    guard_schedule_t new_schedule;

    for (auto &ge : input) {
        switch (ge.action) {
            case guard_action_e::BEGIN_SHIFT: {
                // Finalize last guard
                if (current_guard) {
                    current_guard->add_schedule(new_schedule);
                }
                // Reset tracking schedule
                new_schedule = guard_schedule_t();
                // Find new guard in map
                auto guard_search = guards.find(ge.guard_id);
                if (guard_search == guards.end()) {
                    guards[ge.guard_id] = std::make_shared<guard_t>(ge.guard_id);
                }
                current_guard = guards[ge.guard_id];
                break;
            }
            case guard_action_e::FALL_ASLEEP: {
                assert(current_guard);
                new_schedule.sleeping_from(ge.minute);
                break;
            }
            case guard_action_e::WAKE_UP: {
                assert(current_guard);
                new_schedule.awake_from(ge.minute);
                break;
            }
        }
    }
    // Finalize last guard
    if (current_guard) {
        current_guard->add_schedule(new_schedule);
    }
    
    // Find the guard that sleeps the most
    int guard_id_who_sleeps_the_most = -1;
    int max_minutes_slept = 0;
    for (auto& [guard_id, guard] : guards) {
        auto guard_minutes_slept = guard->get_minutes_slept();
        if (guard_minutes_slept > max_minutes_slept) {
            guard_id_who_sleeps_the_most = guard->guard_id;
            max_minutes_slept = guard_minutes_slept;
        }
    }
    assert(guard_id_who_sleeps_the_most != -1);

    // Found the guard. Now get the minute that the guard slept the most
    auto guard = guards[guard_id_who_sleeps_the_most];
    auto [ minute_slept_the_most, _ ] = guard->find_minute_slept_the_most();

    return { guard_id_who_sleeps_the_most, minute_slept_the_most };
}

std::pair<int, int> find_guard_and_minute2(std::vector<guard_event_t> &input) {
    // Sort the events by timestamp
    std::sort(input.begin(), input.end());
    if (trace1) {
        for (auto &ge : input) {
            std::cout << ge << std::endl;
        }
    }

    std::map<int, std::shared_ptr<guard_t>> guards;
    guard_t guard_data;
    std::shared_ptr<guard_t> current_guard;
    guard_schedule_t new_schedule;

    for (auto &ge : input) {
        switch (ge.action) {
            case guard_action_e::BEGIN_SHIFT: {
                // Finalize last guard
                if (current_guard) {
                    current_guard->add_schedule(new_schedule);
                }
                // Reset tracking schedule
                new_schedule = guard_schedule_t();
                // Find new guard in map
                auto guard_search = guards.find(ge.guard_id);
                if (guard_search == guards.end()) {
                    guards[ge.guard_id] = std::make_shared<guard_t>(ge.guard_id);
                }
                current_guard = guards[ge.guard_id];
                break;
            }
            case guard_action_e::FALL_ASLEEP: {
                assert(current_guard);
                new_schedule.sleeping_from(ge.minute);
                break;
            }
            case guard_action_e::WAKE_UP: {
                assert(current_guard);
                new_schedule.awake_from(ge.minute);
                break;
            }
        }
    }
    // Finalize last guard
    if (current_guard) {
        current_guard->add_schedule(new_schedule);
    }

    int guard_id_who_sleeps_the_most_in_single_minute = -1;
    int max_times_slept = 0;
    int minute_slept_the_most = -1;
    for (auto& [guard_id, guard] : guards) {
        auto [ minute, times_slept ] = guard->find_minute_slept_the_most();
        if (trace2) std::cout << "#" << guard->guard_id << " slept " << times_slept << " times at minute " << minute << std::endl;
        if (times_slept > max_times_slept) {
            max_times_slept = times_slept;
            guard_id_who_sleeps_the_most_in_single_minute = guard->guard_id;
            minute_slept_the_most = minute;
        }
    }
    
    if (trace2) std::cout << "Returning " << guard_id_who_sleeps_the_most_in_single_minute << ", " << minute_slept_the_most << std::endl;
    return { guard_id_who_sleeps_the_most_in_single_minute, minute_slept_the_most };
}

namespace day4 {

    void problem1() {
        std::cout << "Day 4 - Problem 1" << std::endl;

        std::vector<guard_event_t> test1;
        read_day4_data(test1, "data/day4/problem1/test1.txt");
        auto [guard_id, minute] = find_guard_and_minute1(test1);
        assert((guard_id * minute) == 240);

        std::vector<guard_event_t> input;
        read_day4_data(input, "data/day4/problem1/input.txt");
        auto [guard_id2, minute2] = find_guard_and_minute1(input);
        std::cout << "Result : " << (guard_id2 * minute2) << std::endl;
    }

    void problem2() {
        std::cout << "Day 4 - Problem 2" << std::endl;

        std::vector<guard_event_t> test1;
        read_day4_data(test1, "data/day4/problem2/test1.txt");
        auto [guard_id, minute] = find_guard_and_minute2(test1);
        assert((guard_id * minute) == 4455);

        std::vector<guard_event_t> input;
        read_day4_data(input, "data/day4/problem2/input.txt");
        auto [guard_id2, minute2] = find_guard_and_minute2(input);
        std::cout << "Result : " << (guard_id2 * minute2) << std::endl;
    }
}