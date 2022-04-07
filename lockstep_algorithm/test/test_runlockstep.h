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

bool testNoGraph(bool saturation);
bool testOneNodeGraph(bool saturation);
bool testOneNodeGraphSelfLoop(bool saturation);
bool testTwoNodeGraphOneSCC(bool saturation);
bool testTwoNodeGraphTwoSCCs(bool saturation);
bool testTwoNodeGraphTwoRelations(bool saturation);
bool testNonConnectedGraph(bool saturation);
bool testfourNodesOneRelation(bool saturation);
bool testGraphExample1oneRel(bool saturation);
bool testGraphExample1multRel(bool saturation);
bool testGraphExample2oneRel(bool saturation);
bool testGraphExample2multRel(bool saturation);
bool testGraphExample3oneRel(bool saturation);
bool testGraphExample3multRel(bool saturation);
bool testSccCorrectnessNeg1Len();


bool testGraph(const Graph &graph, const std::list<sylvan::Bdd> expectedSCCs);

#endif //TEST_RUNLOCKSTEP_H