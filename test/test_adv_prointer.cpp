#include <memory>
#include <iostream>
#include <thread>
#include <vector>
#include <cstring>

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

void _test3() {
    std::vector<uint8_t> m_bytes;
    uint8_t array[] = {0x0,0x1, 0x2, 0x3,0x4,0x5,0x6, 0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf};

    std::cout << "------------- test3 -------------" << std::endl;
    std::cout << "  bytes size:" << m_bytes.size() << std::endl;
    std::cout << "  array size:" << sizeof(array) << std::endl;

    memcpy((void*)m_bytes.data(), array, sizeof(array));

    std::cout << "  bytes size:" << m_bytes.size() << std::endl;
    std::cout << "  array size:" << sizeof(array) << std::endl;

    std::cout << "---------- test3 Done -------------" << std::endl;
}

void test_adv_prointer() {
    // _test1();

    std::thread t1(_test2, 100);

    t1.join();

    _test3();

    std::cout << "----------------- Test done -----------------" << std::endl;
}
