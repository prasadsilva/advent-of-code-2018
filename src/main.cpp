#include <iostream>
#include <array>
#include <vector>
#include <functional>

// Forward declarations
namespace day1 {
    void problem1();
    void problem2();
}
namespace day2 {
    void problem1();
    void problem2();
}
namespace day3 {
    void problem1();
    void problem2();
}
namespace day4 {
    void problem1();
    void problem2();
}
namespace day5 {
    void problem1();
    void problem2();
}
namespace day6 {
    void problem1();
    void problem2();
}
namespace day7 {
    void problem1();
    void problem2();
}
namespace day8 {
    void problem1();
    void problem2();
}
namespace day9 {
    void problem1();
    void problem2();
}
namespace day10 {
    void problem1();
    void problem2();
}
namespace day11 {
    void problem1();
    void problem2();
}
namespace day12 {
    void problem1();
    void problem2();
}
namespace day13 {
    void problem1();
    void problem2();
}
namespace day14 {
    void problem1();
    void problem2();
}
namespace day15 {
    void problem1();
    void problem2();
}
namespace day16 {
    void problem1();
    void problem2();
}
namespace day17 {
    void problem1();
    void problem2();
}
namespace day18 {
    void problem1();
    void problem2();
}

int main(int argc, char const *argv[])
{
    std::vector<std::vector<std::function<void(void)>>> days = {
        { day1::problem1, day1::problem2 },
        { day2::problem1, day2::problem2 },
        { day3::problem1, day3::problem2 },
        { day4::problem1, day4::problem2 },
        { day5::problem1, day5::problem2 },
        { day6::problem1, day6::problem2 },
        { day7::problem1, day7::problem2 },
        { day8::problem1, day8::problem2 },
        { day9::problem1, day9::problem2 },
        { day10::problem1, day10::problem2 },
        { day11::problem1, day11::problem2 },
        { day12::problem1, day12::problem2 },
        { day13::problem1, day13::problem2 },
        { day14::problem1, day14::problem2 },
        { day15::problem1, day15::problem2 },
        { day16::problem1, day16::problem2 },
        { day17::problem1, day17::problem2 },
        { day18::problem1, day18::problem2 }
    };

    if (argc > 2) {
        std::cerr << "ERROR: Specify no params or a day # to run a specific day" << std::endl;
        return -1;
    }

    int dayToRun = -1;
    if (argc == 2) {
        try {
            dayToRun = std::stoi(argv[1]);
        } catch (...) {
            std::cerr << "ERROR: Invalid parameter!" << std::endl;
            return -2;
        }
        if (dayToRun < 1 || dayToRun > days.size()) {
            std::cerr << "ERROR: Day parameter is invalid!" << std::endl;
            return -3;
        }
    }

    if (dayToRun != -1) {
        // Run a specific day
        auto& problems = days[dayToRun - 1];
        std::cout << "Running day " << dayToRun << std::endl;
        for (auto& problem : problems) {
            problem();
            std::cout << std::endl;
        }
    } else {
        // Run all days
        for (auto& problems : days) {
            for (auto& problem : problems) {
                problem();
                std::cout << std::endl;
            }
        }
    }

    return 0;
}
