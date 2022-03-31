#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "test_runlockstep.h"
#include "test_shift.h"

void runAllTests() {
  bool success = true;
  int fails = 0;
  //Lockstep tests
  if(!testNoGraph()) {
    std::cout << "testNoGraph failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraph()) {
    std::cout << "testOneNodeGraph failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testOneNodeGraphSelfLoop()) {
    std::cout << "testOneNodeGraphSelfLoop failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphOneSCC()) {
    std::cout << "testTwoNodeGraphOneSCC failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoSCCs()) {
    std::cout << "testTwoNodeGraphTwoSCCs failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testTwoNodeGraphTwoRelations()) {
    std::cout << "testTwoNodeGraphTwoRelations failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testNonConnectedGraph()) {
    std::cout << "testNonConnectedGraph failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testfourNodesOneRelation()) {
    std::cout << "testfourNodesOneRelation failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1oneRel()) {
    std::cout << "testGraphExample1oneRel failed" << std::endl;
    success = false;
    fails++;
  }
  if(!testGraphExample1multRel()) {
    std::cout << "testGraphExample1multRel failed" << std::endl;
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