#include <iostream>

// Forward declarations
namespace day1 {
    void problem1();
    void problem2();
}
namespace day2 {
    void problem1();
    void problem2();
}

int main(int argc, char const *argv[])
{
    day1::problem1();
    day1::problem2();

    day2::problem1();
    day2::problem2();

    return 0;
}
