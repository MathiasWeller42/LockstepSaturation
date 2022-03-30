#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "test_runlockstep.h"
#include "test_shift.h"

void runAllTests() {
  if(!testNoGraph()) {
    std::cout << "testNoGraph failed" << std::endl;
  }
  if(!testOneNodeGraph()) {
    std::cout << "testOneNodeGraph failed" << std::endl;
  }
  if(!testOneNodeGraphSelfLoop()) {
    std::cout << "testOneNodeGraphSelfLoop failed" << std::endl;
  }
  if(!testTwoNodeGraphTwoSCCs()) {
    std::cout << "testTwoNodeGraphTwoSCCs failed" << std::endl;
  }


  if(!testShiftOneNode()) {
    std::cout << "testShiftOneNode failed" << std::endl;
  }
  if(!testShiftTwoNodes()) {
    std::cout << "testShiftTwoNodes failed" << std::endl;
  }
  if(!testShiftThreeNodes()) {
    std::cout << "testShiftThreeNodes failed" << std::endl;
  }
  if(!testShiftNineNodes()) {
    std::cout << "testShiftNineNodes failed" << std::endl;
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