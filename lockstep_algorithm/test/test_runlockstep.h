#ifndef TEST_RUNLOCKSTEP_H
#define TEST_RUNLOCKSTEP_H
#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "graph_examples.h"
#include "../src/lockstep.h"
#include "../src/bdd_utilities.h"
#include "../src/petriTranslation.h"

bool testNoGraph(bool saturation, bool iterative);
bool testOneNodeGraph(bool saturation, bool iterative);
bool testOneNodeGraphSelfLoop(bool saturation, bool iterative);
bool testTwoNodeGraphOneSCC(bool saturation, bool iterative);
bool testTwoNodeGraphTwoSCCs(bool saturation, bool iterative);
bool testTwoNodeGraphTwoRelations(bool saturation, bool iterative);
bool testNonConnectedGraph(bool saturation, bool iterative);
bool testfourNodesOneRelation(bool saturation, bool iterative);
bool testGraphExample1oneRel(bool saturation, bool iterative);
bool testGraphExample1multRel(bool saturation, bool iterative);
bool testGraphExample2oneRel(bool saturation, bool iterative);
bool testGraphExample2multRel(bool saturation, bool iterative);
bool testGraphExample3oneRel(bool saturation, bool iterative);
bool testGraphExample3multRel(bool saturation, bool iterative);

bool testGraph(const Graph &graph, const std::list<sylvan::Bdd> expectedSCCs, bool saturation, bool iterative);

#endif //TEST_RUNLOCKSTEP_H