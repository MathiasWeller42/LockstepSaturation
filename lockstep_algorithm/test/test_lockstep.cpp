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

  //SCC List Correctness
  if(testSccCorrectnessLen()) {
    std::cout << "testSccCorrectnessLen failed" << std::endl;
    fails++;
  }
  if(testSccCorrectnessDuplicates()) {
    std::cout << "testSccCorrectnessDuplicates failed" << std::endl;
    fails++;
  }
  if(testSccCorrectnessNegSameLenNoDuplicates()) {
    std::cout << "testSccCorrectnessNegSameLenNoDuplicates failed" << std::endl;
    fails++;
  }
  if(!testSccCorrectnessPosPermutation()) {
    std::cout << "testSccCorrectnessPosPermutation failed" << std::endl;
    fails++;
  }
  if(testsccListContainsDifferentSccsWithDuplicateNodesNeg()) {
    std::cout << "testsccListContainsDifferentSccsWithDuplicateNodesNeg failed" << std::endl;
    fails++;
  }
  if(!testsccListContainsDifferentSccsWithDuplicateNodesPos()) {
    std::cout << "testsccListContainsDifferentSccsWithDuplicateNodesPos failed" << std::endl;
    fails++;
  }
  if(!testUnionIsWholeBddPos()) {
    std::cout << "testUnionIsWholeBddPos failed" << std::endl;
    fails++;
  }
  if(testUnionIsWholeBddNeg()) {
    std::cout << "testUnionIsWholeBddNeg failed" << std::endl;
    fails++;
  }

  if(!testNodeCountTerminalTrue()) {
    std::cout << "testNodeCountTerminalTrue failed" << std::endl;
    fails++;
  }

  if(!testNodeCountTerminalFalse()) {
    std::cout << "testNodeCountTerminalTrue failed" << std::endl;
    fails++;
  }

  if(!testNodeCountSingleNode()) {
    std::cout << "testNodeCountSingleNode failed" << std::endl;
    fails++;
  }

  if(!testNodeCountTwoNodes()) {
    std::cout << "testNodeCountTwoNodes failed" << std::endl;
    fails++;
  }

  if(!testNodeCountFiveNodes()) {
    std::cout << "testNodeCountFiveNodes failed" << std::endl;
    fails++;
  }

  if(!testNodeCountTenNodes()) {
    std::cout << "testNodeCountTenNodes failed" << std::endl;
    fails++;
  }

  if(!testNodeCountTwentyNodes()) {
    std::cout << "testNodeCountTwentyNodes failed" << std::endl;
    fails++;
  }

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