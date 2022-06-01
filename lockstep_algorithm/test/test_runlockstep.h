#ifndef TEST_RUNLOCKSTEP_H
#define TEST_RUNLOCKSTEP_H
#include <iostream>
#include <list>
#include <deque>
#include <functional>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "graph_examples.h"
#include "../src/lockstep.h"
#include "../src/bdd_utilities.h"
#include "../src/petriTranslation.h"
#include "../src/benchmark.h"

bool testNoGraph(algorithmType algorithm);
bool testOneNodeGraph(algorithmType algorithm);
bool testOneNodeGraphSelfLoop(algorithmType algorithm);
bool testTwoNodeGraphOneSCC(algorithmType algorithm);
bool testTwoNodeGraphTwoSCCs(algorithmType algorithm);
bool testTwoNodeGraphTwoRelations(algorithmType algorithm);
bool testNonConnectedGraph(algorithmType algorithm);
bool testfourNodesOneRelation(algorithmType algorithm);
bool testGraphExample1oneRel(algorithmType algorithm);
bool testGraphExample1multRel(algorithmType algorithm);
bool testGraphExample2oneRel(algorithmType algorithm);
bool testGraphExample2multRel(algorithmType algorithm);
bool testGraphExample3oneRel(algorithmType algorithm);
bool testGraphExample3multRel(algorithmType algorithm);
int runWithAllAlgorithmTypes(std::string testFunctionString, std::function<bool(algorithmType)> testFunction);
bool runTest(std::string testFunctionString, std::function<bool()> testFunction, bool expectedResult);

bool testGraph(const Graph &graph, const std::list<sylvan::Bdd> expectedSCCs, algorithmType algorithm);

#endif //TEST_RUNLOCKSTEP_H