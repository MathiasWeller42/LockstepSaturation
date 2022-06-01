#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "test_runlockstep.h"
#include "test_sccListCorrectness.h"

void runAllTests() {
  int fails = 0;

  fails += runWithAllAlgorithmTypes("testNoGraph", testNoGraph);
  fails += runWithAllAlgorithmTypes("testOneNodeGraph", testOneNodeGraph);
  fails += runWithAllAlgorithmTypes("testOneNodeGraphSelfLoop", testOneNodeGraphSelfLoop);
  fails += runWithAllAlgorithmTypes("testTwoNodeGraphOneSCC", testTwoNodeGraphOneSCC);
  fails += runWithAllAlgorithmTypes("testTwoNodeGraphTwoSCCs", testTwoNodeGraphTwoSCCs);
  fails += runWithAllAlgorithmTypes("testTwoNodeGraphTwoRelations", testTwoNodeGraphTwoRelations);
  fails += runWithAllAlgorithmTypes("testNonConnectedGraph", testNonConnectedGraph);
  fails += runWithAllAlgorithmTypes("testfourNodesOneRelation", testfourNodesOneRelation);
  fails += runWithAllAlgorithmTypes("testGraphExample1oneRel", testGraphExample1oneRel);
  fails += runWithAllAlgorithmTypes("testGraphExample1multRel", testGraphExample1multRel);
  fails += runWithAllAlgorithmTypes("testGraphExample2oneRel", testGraphExample2oneRel);
  fails += runWithAllAlgorithmTypes("testGraphExample2multRel", testGraphExample2multRel);
  fails += runWithAllAlgorithmTypes("testGraphExample3oneRel", testGraphExample3oneRel);
  fails += runWithAllAlgorithmTypes("testGraphExample3multRel", testGraphExample3multRel);

  //Test SCC list correctness
  fails += !runTest("testSccCorrectnessLen", testSccCorrectnessLen, false);
  fails += !runTest("testSccCorrectnessDuplicates", testSccCorrectnessDuplicates, false);
  fails += !runTest("testSccCorrectnessNegSameLenNoDuplicates", testSccCorrectnessNegSameLenNoDuplicates, false);
  fails += !runTest("testSccCorrectnessPosPermutation", testSccCorrectnessPosPermutation, true);
  fails += !runTest("testsccListContainsDifferentSccsWithDuplicateNodesNeg", testsccListContainsDifferentSccsWithDuplicateNodesNeg, false);
  fails += !runTest("testsccListContainsDifferentSccsWithDuplicateNodesPos", testsccListContainsDifferentSccsWithDuplicateNodesPos, true);
  fails += !runTest("testUnionIsWholeBddPos", testUnionIsWholeBddPos, true);
  fails += !runTest("testUnionIsWholeBddNeg", testUnionIsWholeBddNeg, false);


  //Test nodeCount
  fails += !runTest("testNodeCountTerminalTrue", testNodeCountTerminalTrue, true);
  fails += !runTest("testNodeCountTerminalFalse", testNodeCountTerminalFalse, true);
  fails += !runTest("testNodeCountSingleNode", testNodeCountSingleNode, true);
  fails += !runTest("testNodeCountTwoNodes", testNodeCountTwoNodes, true);
  fails += !runTest("testNodeCountFiveNodes", testNodeCountFiveNodes, true);
  fails += !runTest("testNodeCountTenNodes", testNodeCountTenNodes, true);
  fails += !runTest("testNodeCountTwentyNodes", testNodeCountTwentyNodes, true);

bool success = (fails == 0);

  if(success) {
    std::cout << "All tests passed!" << std::endl;
  } else {
    std::cout << "Number of fails: " << fails << std::endl;
  }
}

int main() {
  // Init LACE
  lace_start(1, 1000000);

  const size_t memory_bytes = 128u * 1024u * 1024u;

  // Init Sylvan
  sylvan::sylvan_set_limits(memory_bytes, // Set memory limit
                            6,            // Set (exponent) of cache ratio
                            0);           // Initialise unique node table to full size
  sylvan::sylvan_set_granularity(1);
  sylvan::sylvan_init_package();
  sylvan::sylvan_init_bdd();

  std::cout << "Hello Test World!" << std::endl;

  runAllTests();

  std::cout << "Goodbye :)" << std::endl;

  sylvan::sylvan_quit();
  lace_stop();
  return 0;
}