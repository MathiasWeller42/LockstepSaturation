#ifndef TEST_RUNLOCKSTEP_H
#define TEST_RUNLOCKSTEP_H
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

bool testNoGraph();
bool testOneNodeGraph();
bool testOneNodeGraphSelfLoop();
bool testTwoNodeGraphOneSCC();
bool testTwoNodeGraphTwoSCCs();
bool testTwoNodeGraphTwoRelations();
bool testNonConnectedGraph();
bool testfourNodesOneRelation();
bool testGraphExample1oneRel();
bool testGraphExample1multRel();

bool testGraph(const Graph &graph, const std::list<sylvan::Bdd> expectedSCCs);

#endif //TEST_RUNLOCKSTEP_H