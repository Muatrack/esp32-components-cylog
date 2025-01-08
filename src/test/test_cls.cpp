/** 
 *  类相关的各种测试
 */

#include <iostream>

#ifdef USE_SYSTEM_LINUX
#include <stdint.h>
#endif

using namespace std;

class ClsBase{
public:
    int mAge;
};

class ClassA: virtual public ClsBase {
};

class ClassB: virtual public ClsBase {
};

class ClassC: public ClassA, public ClassB {
};

void test_cls1() {
    ClassC cc;    
    // cc.ClassB::mAge = 201;
    // cc.ClassA::mAge = 100;
    // cc.mAge = 34;
    // int a = 102;
    cout << "[ Class A B C ]" << endl;
    cout << "mAge:" << &cc.mAge << endl;
}