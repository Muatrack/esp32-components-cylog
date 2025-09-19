#include <iostream>

extern void test_alarm_log();
extern void test_dir_del( std::string );

int main(void) {
    std::cout << "Hi, CYLOG" << std::endl;

    std::cout << "-------- dir removed testing ---------" << std::endl;
    test_dir_del("excp/b");

    // test_alarm_log();
}