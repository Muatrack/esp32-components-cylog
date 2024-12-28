#include "priv/cylog_generic.hpp"
#include "priv/cylog_factory.hpp"

void cylog_test() {
    CYLog *pLogPerformance = CYLogFactory::create('p');
    pLogPerformance->write();
}