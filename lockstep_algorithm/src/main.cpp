
#include <iostream>

#include <sylvan.h>
#include <sylvan_table.h>
#include <sylvan_obj.hpp>

#include "lockstep.h"
#include "petriTranslation.h"
#include "set_test.h"
#include "lockstep_test.h"
#include "../test/graph_examples.h"

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

  std::cout << "Hello World!" << std::endl;


  Graph bigGraph = PNMLtoStringLists();
  printBddAsString(bigGraph.cube.size(), bigGraph.nodes);
  simpleTestGraph(bigGraph);

  std::cout << "Goodbye :)" << std::endl;

  sylvan::sylvan_quit();
  lace_stop();
  return 0;
}