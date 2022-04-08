#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "test_runlockstep.h"
#include "test_sccListCorrectness.h"

void runAllTests() {
  bool saturation = true;
  bool noSaturation = false;
  bool iterative = true;
  bool notIterative = false;

  bool success = true;
  int fails = 0;
  //Lockstep tests
  //testNoGraph
  if(!testNoGraph(saturation, iterative)) {
    std::cout << "testNoGraph iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testNoGraph(noSaturation, iterative)) {
    std::cout << "testNoGraph iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testNoGraph(saturation, notIterative)) {
    std::cout << "testNoGraph non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testNoGraph(noSaturation, notIterative)) {
    std::cout << "testNoGraph non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testOneNodeGraph
  if(!testOneNodeGraph(saturation, iterative)) {
    std::cout << "testOneNodeGraph iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraph(noSaturation, iterative)) {
    std::cout << "testOneNodeGraph iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraph(saturation, notIterative)) {
    std::cout << "testOneNodeGraph non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraph(noSaturation, notIterative)) {
    std::cout << "testOneNodeGraph non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testOneNodeGraphSelfLoop
  if(!testOneNodeGraphSelfLoop(saturation, iterative)) {
    std::cout << "testOneNodeGraphSelfLoop iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraphSelfLoop(noSaturation, iterative)) {
    std::cout << "testOneNodeGraphSelfLoop iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraphSelfLoop(saturation, notIterative)) {
    std::cout << "testOneNodeGraphSelfLoop non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraphSelfLoop(noSaturation, notIterative)) {
    std::cout << "testOneNodeGraphSelfLoop non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testTwoNodeGraphOneSCC
  if(!testTwoNodeGraphOneSCC(saturation, iterative)) {
    std::cout << "testTwoNodeGraphOneSCC iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphOneSCC(noSaturation, iterative)) {
    std::cout << "testTwoNodeGraphOneSCC iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphOneSCC(saturation, notIterative)) {
    std::cout << "testTwoNodeGraphOneSCC non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphOneSCC(noSaturation, notIterative)) {
    std::cout << "testTwoNodeGraphOneSCC non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testTwoNodeGraphTwoSCCs
  if(!testTwoNodeGraphTwoSCCs(saturation, iterative)) {
    std::cout << "testTwoNodeGraphTwoSCCs iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoSCCs(noSaturation, iterative)) {
    std::cout << "testTwoNodeGraphTwoSCCs iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoSCCs(saturation, notIterative)) {
    std::cout << "testTwoNodeGraphTwoSCCs non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoSCCs(noSaturation, notIterative)) {
    std::cout << "testTwoNodeGraphTwoSCCs non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testTwoNodeGraphTwoRelations
  if(!testTwoNodeGraphTwoRelations(saturation, iterative)) {
    std::cout << "testTwoNodeGraphTwoRelations iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoRelations(noSaturation, iterative)) {
    std::cout << "testTwoNodeGraphTwoRelations iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoRelations(saturation, notIterative)) {
    std::cout << "testTwoNodeGraphTwoRelations non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoRelations(noSaturation, notIterative)) {
    std::cout << "testTwoNodeGraphTwoRelations non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testNonConnectedGraph
  if(!testNonConnectedGraph(saturation, iterative)) {
    std::cout << "testNonConnectedGraph iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testNonConnectedGraph(noSaturation, iterative)) {
    std::cout << "testNonConnectedGraph iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testNonConnectedGraph(saturation, notIterative)) {
    std::cout << "testNonConnectedGraph non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testNonConnectedGraph(noSaturation, notIterative)) {
    std::cout << "testNonConnectedGraph non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testfourNodesOneRelation
  if(!testfourNodesOneRelation(saturation, iterative)) {
    std::cout << "testfourNodesOneRelation iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testfourNodesOneRelation(noSaturation, iterative)) {
    std::cout << "testfourNodesOneRelation iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testfourNodesOneRelation(saturation, notIterative)) {
    std::cout << "testfourNodesOneRelation non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testfourNodesOneRelation(noSaturation, notIterative)) {
    std::cout << "testfourNodesOneRelation non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testGraphExample1oneRel
  if(!testGraphExample1oneRel(saturation, iterative)) {
    std::cout << "testGraphExample1oneRel iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1oneRel(noSaturation, iterative)) {
    std::cout << "testGraphExample1oneRel iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1oneRel(saturation, notIterative)) {
    std::cout << "testGraphExample1oneRel non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1oneRel(noSaturation, notIterative)) {
    std::cout << "testGraphExample1oneRel non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testGraphExample1multRel
  if(!testGraphExample1multRel(saturation, iterative)) {
    std::cout << "testGraphExample1multRel iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1multRel(noSaturation, iterative)) {
    std::cout << "testGraphExample1multRel iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1multRel(saturation, notIterative)) {
    std::cout << "testGraphExample1multRel non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1multRel(noSaturation, notIterative)) {
    std::cout << "testGraphExample1multRel non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testGraphExample2oneRel
  if(!testGraphExample2oneRel(saturation, iterative)) {
    std::cout << "testGraphExample2oneRel iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample2oneRel(noSaturation, iterative)) {
    std::cout << "testGraphExample2oneRel iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample2oneRel(saturation, notIterative)) {
    std::cout << "testGraphExample2oneRel non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample2oneRel(noSaturation, notIterative)) {
    std::cout << "testGraphExample2oneRel non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testGraphExample2multRel
  if(!testGraphExample2multRel(saturation, iterative)) {
    std::cout << "testGraphExample2multRel iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample2multRel(noSaturation, iterative)) {
    std::cout << "testGraphExample2multRel iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample2multRel(saturation, notIterative)) {
    std::cout << "testGraphExample2multRel non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample2multRel(noSaturation, notIterative)) {
    std::cout << "testGraphExample2multRel non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testGraphExample3oneRel
  if(!testGraphExample3oneRel(saturation, iterative)) {
    std::cout << "testGraphExample3oneRel iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample3oneRel(noSaturation, iterative)) {
    std::cout << "testGraphExample3oneRel iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample3oneRel(saturation, notIterative)) {
    std::cout << "testGraphExample3oneRel non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample3oneRel(noSaturation, notIterative)) {
    std::cout << "testGraphExample3oneRel non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //testGraphExample3multRel
  if(!testGraphExample3multRel(saturation, iterative)) {
    std::cout << "testGraphExample3multRel iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample3multRel(noSaturation, iterative)) {
    std::cout << "testGraphExample3multRel iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample3multRel(saturation, notIterative)) {
    std::cout << "testGraphExample3multRel non-iterative with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample3multRel(noSaturation, notIterative)) {
    std::cout << "testGraphExample3multRel non-iterative without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //SCC List Correctness
  if(testSccCorrectnessLen()) {
    std::cout << "testSccCorrectnessLen failed" << std::endl;
    success = false;
    fails++;
  }
  if(testSccCorrectnessDuplicates()) {
    std::cout << "testSccCorrectnessDuplicates failed" << std::endl;
    success = false;
    fails++;
  }
  if(testSccCorrectnessNegSameLenNoDuplicates()) {
    std::cout << "testSccCorrectnessNegSameLenNoDuplicates failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testSccCorrectnessPosPermutation()) {
    std::cout << "testSccCorrectnessPosPermutation failed" << std::endl;
    success = false;
    fails++;
  }
  if(testsccListContainsDifferentSccsWithDuplicateNodesNeg()) {
    std::cout << "testsccListContainsDifferentSccsWithDuplicateNodesNeg failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testsccListContainsDifferentSccsWithDuplicateNodesPos()) {
    std::cout << "testsccListContainsDifferentSccsWithDuplicateNodesPos failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testUnionIsWholeBddPos()) {
    std::cout << "testUnionIsWholeBddPos failed" << std::endl;
    success = false;
    fails++;
  }
  if(testUnionIsWholeBddNeg()) {
    std::cout << "testUnionIsWholeBddNeg failed" << std::endl;
    success = false;
    fails++;
  }


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