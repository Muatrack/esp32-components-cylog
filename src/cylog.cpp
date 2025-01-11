#include "private_include/cylog_generic.hpp"
#include "private_include/cylog_factory.hpp"

void cylog_test() {
    CYLog *pLogPerformance = CYLogFactory::create('p');
    pLogPerformance->write();
}