#ifndef TEST_SHIFT_H
#define TEST_SHIFT_H
#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "../src/lockstep.h"
#include "../src/utilities.h"
#include "../src/graph_examples.h"
#include "../src/petriTranslation.h"

bool testShiftOneNode();
bool testShiftTwoNodes();
bool testShiftFourNodes();
bool testShiftNineNodes();

#endif //TEST_SHIFT_H