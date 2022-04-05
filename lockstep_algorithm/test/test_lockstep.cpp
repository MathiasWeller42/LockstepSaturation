#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "test_runlockstep.h"
#include "test_shift.h"

void runAllTests() {
  bool saturation = true;
  bool noSaturation = false;

  bool success = true;
  int fails = 0;
  //Lockstep tests
  if(!testNoGraph(saturation)) {
    std::cout << "testNoGraph with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testNoGraph(noSaturation)) {
    std::cout << "testNoGraph without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraph(saturation)) {
    std::cout << "testOneNodeGraph with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraph(noSaturation)) {
    std::cout << "testOneNodeGraph without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraphSelfLoop(saturation)) {
    std::cout << "testOneNodeGraphSelfLoop with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraphSelfLoop(noSaturation)) {
    std::cout << "testOneNodeGraphSelfLoop without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphOneSCC(saturation)) {
    std::cout << "testTwoNodeGraphOneSCC with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphOneSCC(noSaturation)) {
    std::cout << "testTwoNodeGraphOneSCC without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoSCCs(saturation)) {
    std::cout << "testTwoNodeGraphTwoSCCs with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoSCCs(noSaturation)) {
    std::cout << "testTwoNodeGraphTwoSCCs without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoRelations(saturation)) {
    std::cout << "testTwoNodeGraphTwoRelations with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoRelations(noSaturation)) {
    std::cout << "testTwoNodeGraphTwoRelations without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testNonConnectedGraph(saturation)) {
    std::cout << "testNonConnectedGraph with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testNonConnectedGraph(noSaturation)) {
    std::cout << "testNonConnectedGraph without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testfourNodesOneRelation(saturation)) {
    std::cout << "testfourNodesOneRelation with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testfourNodesOneRelation(noSaturation)) {
    std::cout << "testfourNodesOneRelation without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1oneRel(saturation)) {
    std::cout << "testGraphExample1oneRel with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1oneRel(noSaturation)) {
    std::cout << "testGraphExample1oneRel without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1multRel(saturation)) {
    std::cout << "testGraphExample1multRel with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1multRel(noSaturation)) {
    std::cout << "testGraphExample1multRel without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample2oneRel(saturation)) {
    std::cout << "testGraphExample2oneRel with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample2oneRel(noSaturation)) {
    std::cout << "testGraphExample2oneRel without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample2multRel(saturation)) {
    std::cout << "testGraphExample2multRel with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample2multRel(noSaturation)) {
    std::cout << "testGraphExample2multRel without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample3oneRel(saturation)) {
    std::cout << "testGraphExample3oneRel with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample3oneRel(noSaturation)) {
    std::cout << "testGraphExample3oneRel without saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample3multRel(saturation)) {
    std::cout << "testGraphExample3multRel with saturation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample3multRel(noSaturation)) {
    std::cout << "testGraphExample3multRel without saturation failed" << std::endl;
    success = false;
    fails++;
  }

  //Shift tests
  if(!testShiftOneNode()) {
    std::cout << "testShiftOneNode failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testShiftTwoNodes()) {
    std::cout << "testShiftTwoNodes failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testShiftFourNodes()) {
    std::cout << "testShiftFourNodes failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testShiftNineNodes()) {
    std::cout << "testShiftNineNodes failed" << std::endl;
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