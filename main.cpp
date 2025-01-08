#include <iostream>
#include "inc/cylog_c_api.hpp"

extern void test_cls1();

int main(void) {
    std::cout << "Hi, CYLOG" << std::endl;
    cylog_test();
    test_cls1();
}