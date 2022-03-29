#include <iostream>
#include <list>
#include <deque>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "../src/lockstep.h"
#include "../src/utilities.h"
#include "../src/graph_examples.h"

bool testNoGraph() {
  sylvan::Bdd expected_scc1 = leaf_false();

  std::list<sylvan::Bdd> result = lockstepSaturation(noGraph());
  sylvan::Bdd result_scc1 = result.front();

  return expected_scc1 == result_scc1;
}

void runAllTests() {
  if(!testNoGraph()) {
    std::cout << "testNoGraph failed" << std::endl;
  } else {
    std::cout << "testNoGraph passed" << std::endl;
  }
}

int main() {
  runAllTests();
  return 0;
}