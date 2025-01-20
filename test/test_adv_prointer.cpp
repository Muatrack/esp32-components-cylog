#include <memory>
#include <iostream>
#include <thread>

class Store {
public:
    ~Store() {
        std::cout << "~Store()" << std::endl;
    }
};

typedef std::shared_ptr<Store> SPType_t;

void _test1_1(std::shared_ptr<Store> &sp) {
    std::shared_ptr<Store> sp1 = sp;
    std::cout << "count:" << sp1.use_count() << std::endl;
}

void _test2(int a) {
    std::cout << "recv val:" << a << std::endl;
}

void _test1() {
    // SPType_t sp1(new Store());
    SPType_t sp1 = std::make_shared<Store>();
    std::cout << "count:" << sp1.use_count() << std::endl;
    _test1_1(sp1);
    std::cout << "count:" << sp1.use_count() << std::endl;
}

void test_adv_prointer() {
    // _test1();

    std::thread t1(_test2, 100);

    t1.join();

    std::cout << "----------------- Test done -----------------" << std::endl;
}
